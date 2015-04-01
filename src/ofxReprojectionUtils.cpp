#include "ofxReprojectionUtils.h"

//
//
// Setup screen for rendering (orthographic projection
// mulitiplied by the camera/projector projection matrix)
//
//

void ofxReprojectionUtils::setupScreen(ofxReprojectionCalibrationData& data) {
	setupScreen(data.getMatrix());
}

void ofxReprojectionUtils::setupScreen(ofMatrix4x4 m) {
	ofMatrix4x4 ortho = ofMatrix4x4::newOrthoMatrix(0, 1, 0, 1, -1, 1);
	ofMatrix4x4 glprojectionmatrix = ofMatrix4x4::getTransposedOf(m) * ortho;

	ofSetMatrixMode(OF_MATRIX_PROJECTION);
	ofLoadMatrix(glprojectionmatrix);

	ofSetMatrixMode(OF_MATRIX_MODELVIEW);
	ofLoadIdentityMatrix();
}

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



const string ofxReprojectionUtils::stringVertexShader2DTriangles = 	"#version 150\n"
			STRINGIFY(

	// depth_map: R32F format, 32 bit floats in red channel 
	// (real z values, not normalized)
	uniform sampler2DRect depth_map;

	// color_image: RBG format
	uniform sampler2DRect color_image;

	uniform mat4 transform;
                      
    uniform mat4 modelViewProjectionMatrix;

    in vec4 position;

    out vec3 frontColorVertex;

	void main() {
		vec4 pos = position;
		frontColorVertex.rgb = texture(color_image, pos.xy).rgb;
		float z = texture(depth_map, pos.xy).r;
		pos.z = z;
		pos = pos*transform; 
		pos.z = z;
		gl_Position = modelViewProjectionMatrix * pos;
		if(abs(pos.z) < 1e-5) {
			frontColorVertex.rgb = vec3(0,0,0);
		}
	}
);

const string ofxReprojectionUtils::stringFragmentShader2DTriangles = "#version 150\n"
			STRINGIFY(
    in vec3 frontColorGeometry;
    out vec4 outputColor;
                      
	void main() {
		outputColor = vec4(frontColorGeometry, 1);
	}
			);

// TODO: Clean up this shader a little?
const string ofxReprojectionUtils::stringGeometryShader2DTriangles = "#version 150\n"
			STRINGIFY(
	// Pointsize variable used as maximum length of triangle side.
	// 0.2 seems like a good value here, but a better measure
	// for distortion should be used (TODO)
                      
    layout(triangles) in;
    layout(triangle_strip, max_vertices = 3) out;

    in vec3 frontColorVertex[];
    out vec3 frontColorGeometry;

	uniform float pointsize;
	void main() {
		vec3 sumcolor = vec3(1,1,1);
		for (int i = 0; i < gl_in.length(); i++) {
			if(frontColorVertex[i].rgb == vec3(0,0,0)) {
				sumcolor = vec3(0,0,0);
			}
		}

        float lena = length((gl_in[1].gl_Position - gl_in[0].gl_Position).xy);
        float lenb = length((gl_in[2].gl_Position - gl_in[0].gl_Position).xy);
        float lenc = length((gl_in[2].gl_Position - gl_in[1].gl_Position).xy);

		if(sumcolor != vec3(0,0,0) && lena < pointsize && lenb < pointsize && lenc < pointsize) {
			for (int i = 0; i < gl_in.length(); i++) {
				gl_Position = gl_in[i].gl_Position;
				frontColorGeometry = frontColorVertex[i];
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



