#include "ofxReprojectionRenderer.h"

#define STRINGIFY(A) #A

ofxReprojectionRenderer::ofxReprojectionRenderer()
{

    string shaderstring = STRINGIFY(

                              \#version 120
                          \#extension GL_ARB_texture_rectangle : enable

                              uniform sampler2DRect depth_map;
                              uniform sampler2DRect color_image;
                              uniform mat4 kinproj_transform;
                              uniform float ps;
                              // uniform float kw;
                              // uniform float kh;
                              // uniform float kz;
                              //
                              // vec3 pixel_to_world(vec3 p) {
                              // 	vec3 r;
                              // 	r.x = 2.0*p.x/kw - 1.0;
                              // 	r.y = 2.0*p.y/kh - 1.0;
                              // 	r.z = p.z/kz;
                              //
                              // 	return p;
                              // }

                              void main()
    {
        gl_TexCoord[0] = gl_MultiTexCoord0;

        vec4 pos;

        // Extract color
        pos = gl_Vertex;
        gl_FrontColor.rgb = texture2DRect(color_image, pos.xy).rgb;

        // Combine pixel XY (gl_vertex) with depth data.
        vec4 dp = texture2DRect(depth_map, pos.xy);
        //pos.z  = 100;

        pos.z = 256*(256*256*dp.r + 256*dp.g + dp.b);

        if(abs(pos.z) < 1e-5)
        {
            gl_FrontColor.rgb = vec3(0,0,0);
        }


        pos = pos*kinproj_transform;

        // Still do depth sorting
        pos.z = 256*(256*256*dp.r + 256*dp.g + dp.b);

        gl_Position = gl_ModelViewProjectionMatrix * pos;

        gl_PointSize = ps;
    }
                          );

}

ofxReprojectionRenderer::~ofxReprojectionRenderer()
{

}

void ofxReprojectionRenderer::draw(ofTexture depthTexture, ofTexture userTexture, bool use_transform, bool use_depthimage)
{


    // Setup orthographic projection.
    // Without tranformation, the space is [-1,1]x[-1,1]x[0,1], WRONG
    // with transformation, the space is [0,1024]x[0,768]x[0].

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ofScale(1,-1,1); // Pixel/image format -> GL coord.sys.

    if(use_transform)
    {

        glOrtho(0, projector_width, 0, projector_height,1,-100*ref_max_depth);
    }
    else
    {
        glOrtho(0, cam_width,0, cam_height,1,-100*ref_max_depth);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    shader.begin();

    float ps = 2*max(projectionMatrix(0,0), projectionMatrix(1,1));

    shader.setUniform1f("ps",ps);

    if(use_transform)
    {
        shader.setUniformMatrix4f("kinproj_transform", projectionMatrix);
    }
    else
    {
        shader.setUniformMatrix4f("kinproj_transform", identityMatrix);
    }

    shader.setUniformTexture("depth_map", depthTexture, 0);
    if(use_depthimage)
    {
        shader.setUniformTexture("color_image", depthTexture, 1);
    }
    else
    {
        shader.setUniformTexture("color_image", userTexture, 1);
    }

    outputgrid.draw();
    shader.end();

}


void ofxReprojectionRenderer::generate_grid()
{
    outputgrid.clear();
    outputgrid.setMode(OF_PRIMITIVE_POINTS);

    int skip = 1;
    for(int y = 0; y < cam_height - skip; y += skip)
    {
        for(int x = 0; x < cam_width - skip; x += skip)
        {
            outputgrid.addVertex(ofVec3f(x,y,2));


            // Points for adding 2 triangles:
            // kinect_grid.addVertex(ofVec3f(x+skip,y+skip,2));
            // kinect_grid.addVertex(ofVec3f(x,y+skip,2));

            // kinect_grid.addVertex(ofVec3f(x,y,2));
            // kinect_grid.addVertex(ofVec3f(x+skip,y,2));
            // kinect_grid.addVertex(ofVec3f(x+skip,y+skip,2));


            outputgrid.addTexCoord(ofVec2f(x,y));

            // Points for adding 2 triangles:
            // kinect_grid.addTexCoord(ofVec2f(x+skip,y+skip));
            // kinect_grid.addTexCoord(ofVec2f(x,y+skip));

            // kinect_grid.addTexCoord(ofVec2f(x,y));
            // kinect_grid.addTexCoord(ofVec2f(x+skip,y));
            // kinect_grid.addTexCoord(ofVec2f(x+skip,y+skip));
        }
    }


}

void ofxReprojectionRenderer::setProjectionMatrix(ofMatrix4x4 m)
{
    projectionMatrix = m;
    identityMatrix.makeIdentityMatrix();

}

void ofxReprojectionRenderer::setProjectionInfo(int proj_w, int proj_h, int cam_w, int cam_h, float max_depth) {
    projector_width = proj_w;
    projector_height = proj_w;
    cam_width = cam_w;
    cam_height = cam_h;
    ref_max_depth = max_depth;
    shader.load("shader.vert","shader.frag");
}
