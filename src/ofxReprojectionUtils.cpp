#include "ofxReprojectionUtils.h"



//
//
// GPU Shader programs as strings
//
//

const string ofxReprojectionUtils::stringVertexShader2DPoints = 	"#version 120\n"
			"#extension GL_ARB_texture_rectangle : enable\n"
			STRINGIFY(

	// depth_map: R32F format, 32 bit floats in red channel
	// (real z values, not normalized)
	uniform sampler2DRect depth_map;

	// color_image: RBG format
	uniform sampler2DRect color_image;

	uniform mat4 transform;
	uniform float pointsize;

	void main() {
		vec4 pos = gl_Vertex;
		gl_FrontColor.rgb = texture2DRect(color_image, pos.xy).rgb;
		float z = texture2DRect(depth_map, pos.xy).r;
		pos.z = z;
		pos = pos*transform;
		pos.z = z;
		gl_Position = gl_ModelViewProjectionMatrix * pos;
		if(abs(pos.z) < 1e-5) {
			gl_FrontColor.rgb = vec3(0,0,0);
		}

		gl_PointSize = pointsize;
	}
);

const string ofxReprojectionUtils::stringFragmentShader2DPoints = "#version 120\n"
		STRINGIFY(
	void main() {
		if(gl_Color.rgb == vec3(0,0,0)) discard;
		gl_FragColor = gl_Color;
	}
);

const string ofxReprojectionUtils::stringGeometryShader2DPoints = "";



const string ofxReprojectionUtils::stringVertexShader2DTriangles = 	"#version 120\n"
			"#extension GL_ARB_texture_rectangle : enable\n" 
			STRINGIFY(

	// depth_map: R32F format, 32 bit floats in red channel 
	// (real z values, not normalized)
	uniform sampler2DRect depth_map;

	// color_image: RBG format
	uniform sampler2DRect color_image;

	uniform mat4 transform;

	void main() {
		vec4 pos = gl_Vertex;
		gl_FrontColor.rgb = texture2DRect(color_image, pos.xy).rgb;
		float z = texture2DRect(depth_map, pos.xy).r;
		pos.z = z;
		pos = pos*transform; 
		pos.z = z;
		gl_Position = gl_ModelViewProjectionMatrix * pos;
		if(abs(pos.z) < 1e-5) {
			gl_FrontColor.rgb = vec3(0,0,0);
		}
	}
);

const string ofxReprojectionUtils::stringFragmentShader2DTriangles = "#version 120\n"
			STRINGIFY(
	void main() {
		gl_FragColor = gl_Color;
	}
			);

// TODO: Clean up this shader a little?
const string ofxReprojectionUtils::stringGeometryShader2DTriangles = "#version 120\n"
				"#extension GL_EXT_geometry_shader4 : enable\n"
			STRINGIFY(
	// Pointsize variable used as maximum length of triangle side.
	// 0.2 seems like a good value here, but a better measure
	// for distortion should be used (TODO)
	uniform float pointsize;
	void main() {
		vec3 sumcolor = vec3(1,1,1);
		for (int i = 0; i < gl_VerticesIn; i++) {
			if(gl_FrontColorIn[i].rgb == vec3(0,0,0)) {
				sumcolor = vec3(0,0,0);
			}
		}

		float lena = length((gl_PositionIn[1] - gl_PositionIn[0]).xy);
		float lenb = length((gl_PositionIn[2] - gl_PositionIn[0]).xy);
		float lenc = length((gl_PositionIn[2] - gl_PositionIn[1]).xy);

		if(sumcolor != vec3(0,0,0) && lena < pointsize && lenb < pointsize && lenc < pointsize) {
			for (int i = 0; i < gl_VerticesIn; i++) {
				gl_Position = gl_PositionIn[i];
				gl_FrontColor = gl_FrontColorIn[i];
				EmitVertex();
			}

			EndPrimitive();
		}
	}

			);


const string ofxReprojectionUtils::stringVertexShaderCalibration3DView = 	"#version 120\n"
			"#extension GL_ARB_texture_rectangle : enable\n" 
			STRINGIFY(

	// depth_map: R32F format, 32 bit floats in red channel 
	// (real z values, not normalized)
	uniform sampler2DRect depth_map;

	// color_image: RBG format
	uniform sampler2DRect color_image;

	void main() {
		vec4 pos = gl_Vertex;
		gl_FrontColor.rgb = texture2DRect(color_image, pos.xy).rgb;
		float z = texture2DRect(depth_map, pos.xy).r / 20.0;
		pos.z = z;
		gl_Position = gl_ModelViewProjectionMatrix * pos;
		if(abs(pos.z) < 1e-5) {
			gl_FrontColor.rgb = vec3(0,0,0);
		}
	}
);

const string ofxReprojectionUtils::stringFragmentShaderCalibration3DView = "#version 120\n"
			STRINGIFY(
	void main() {
		gl_FragColor = gl_Color;
	}
			);

// TODO: Clean up this shader a little?
const string ofxReprojectionUtils::stringGeometryShaderCalibration3DView = "#version 120\n"
				"#extension GL_EXT_geometry_shader4 : enable\n"
			STRINGIFY(
	void main() {
		vec3 sumcolor = vec3(1,1,1);
		for (int i = 0; i < gl_VerticesIn; i++) {
			if(gl_FrontColorIn[i].rgb == vec3(0,0,0)) {
				sumcolor = vec3(0,0,0);
			}
		}
		if(sumcolor != vec3(0,0,0)) {
			for (int i = 0; i < gl_VerticesIn; i++) {
				gl_Position = gl_PositionIn[i];
				gl_FrontColor = gl_FrontColorIn[i];
				EmitVertex();
			}

			EndPrimitive();
		}
	}

			);



