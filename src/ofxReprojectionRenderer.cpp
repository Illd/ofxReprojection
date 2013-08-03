#include "ofxReprojectionRenderer.h"

#define STRINGIFY(A) #A

ofxReprojectionRenderer::ofxReprojectionRenderer()
{

	useTransform = true;
	useDepthImage = true;
	pointsize = 4.0;

	drawX = 0;
	drawY = 0;
	drawWidth = 0;
	drawHeight = 0;

	drawMethod = OFX_REPROJECTION_RENDERER_DRAW_METHOD_POINTS;

	string vertexshader = 	"#version 120\n"
				"#extension GL_ARB_texture_rectangle : enable\n" 
				STRINGIFY(

		uniform sampler2DRect depth_map;
		uniform sampler2DRect color_image;
		uniform mat4 kinproj_transform;
		uniform float ps;

		void main() {
			gl_TexCoord[0] = gl_MultiTexCoord0;

			vec4 pos;

			// Extract color
			pos = gl_Vertex;
			gl_FrontColor.rgb = texture2DRect(color_image, pos.xy).rgb;

			// Combine pixel XY (gl_vertex) with depth data.
			vec4 dp = texture2DRect(depth_map, pos.xy);
			//pos.z  = 100;

			pos.z = 256*(256*256*dp.r + 256*dp.g + dp.b);

			if(abs(pos.z) < 1e-5) {
				gl_FrontColor.rgb = vec3(0,0,0);
			}


			pos = pos*kinproj_transform;

			// Still do depth sorting
			pos.z = 256*(256*256*dp.r + 256*dp.g + dp.b);

			gl_Position = gl_ModelViewProjectionMatrix * pos;

			gl_PointSize = ps;
		}
	);

	ofLogVerbose("ofxReprojection") << "Renderer shader string: " << vertexshader;

}


ofxReprojectionRenderer::~ofxReprojectionRenderer()
{

}

bool ofxReprojectionRenderer::init(ofxBase3DVideo *cam) {
	if(cam == NULL) {
		ofLogWarning("ofxReprojection") << "Valid ofxBase3DVideo providing both color and depth image must be passed to constructor ofxReprojectionRenderer";
		return false;
	} else {
		this->cam = cam;
	}

	cam_height = cam->getPixelsRef().getHeight();
	cam_width = cam->getPixelsRef().getWidth();

	return true;
}

void ofxReprojectionRenderer::begin() {
    shader.begin();
}

void ofxReprojectionRenderer::end() {
    shader.end();

}
void ofxReprojectionRenderer::drawImage(ofTexture tex) 
{


    // Setup orthographic projection.
    // Without tranformation, the space is [-1,1]x[-1,1]x[0,1], WRONG
    // with transformation, the space is [0,1024]x[0,768]x[0].

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ofScale(1,1,1); // Pixel/image format -> GL coord.sys.

    if(useTransform)
    {

        glOrtho(0, projectorWidth, 0, projectorHeight,1,-100*ref_max_depth);
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

    float ps = max(projectionMatrix(0,0), projectionMatrix(1,1));

    shader.setUniform1f("ps",pointsize);

    ofTexture depthTexture = cam->getDepthTextureReference();

    if(useTransform)
    {
        shader.setUniformMatrix4f("kinproj_transform", projectionMatrix);
    }
    else
    {
        shader.setUniformMatrix4f("kinproj_transform", identityMatrix);
    }

    shader.setUniformTexture("depth_map", depthTexture, 0);
    if(useDepthImage)
    {
        shader.setUniformTexture("color_image", depthTexture, 1);
    }
    else
    {
        shader.setUniformTexture("color_image", tex, 1);
    }

    outputgrid.draw();



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
//             outputgrid.addVertex(ofVec3f(x+skip,y+skip,2));
//             outputgrid.addVertex(ofVec3f(x,y+skip,2));
//
//             outputgrid.addVertex(ofVec3f(x,y,2));
//             outputgrid.addVertex(ofVec3f(x+skip,y,2));
//             outputgrid.addVertex(ofVec3f(x+skip,y+skip,2));


            outputgrid.addTexCoord(ofVec2f(x*2.0,y*2.0));

            // Points for adding 2 triangles:
//             outputgrid.addTexCoord(ofVec2f(x+skip,y+skip));
//             outputgrid.addTexCoord(ofVec2f(x,y+skip));
//
//             outputgrid.addTexCoord(ofVec2f(x,y));
//             outputgrid.addTexCoord(ofVec2f(x+skip,y));
//             outputgrid.addTexCoord(ofVec2f(x+skip,y+skip));
        }
    }


}

void ofxReprojectionRenderer::setProjectionMatrix(ofMatrix4x4 m)
{
    projectionMatrix = m;
    identityMatrix.makeIdentityMatrix();

}

void ofxReprojectionRenderer::setProjectorInfo(int projectorWidth, int projectorHeight, ofxDirection projectorPosition) {
	this->projectorWidth = projectorWidth;
	this->projectorHeight = projectorHeight;
	this->projectorPosition = projectorPosition;
	
	if(drawWidth == 0) {
	       drawWidth = projectorWidth;
	}
	if(drawHeight == 0) {
		drawHeight = projectorHeight;
	}
}

void ofxReprojectionRenderer::setDrawArea(float x, float y, float w, float h) {
	drawX = x;
	drawY = y;
	drawWidth = w;
	drawHeight = h;
}
