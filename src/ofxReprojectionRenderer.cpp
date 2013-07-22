#include "ofxReprojectionRenderer.h"

#define STRINGIFY(A) #A

ofxReprojectionRenderer::ofxReprojectionRenderer() {

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

}

ofxReprojectionRenderer::~ofxReprojectionRenderer() {

}

void ofxReprojectionRenderer::setProjectionMatrix(ofMatrix4x4& m) {
	projectionMatrix = m;
}
