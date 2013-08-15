#version 120
#extension GL_ARB_texture_rectangle : enable

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
	pos.z = 256*(256*256*dp.r + 256*dp.g + dp.b);

	if(abs(pos.z) < 1e-5) {
		gl_FrontColor.rgb = vec3(0,0,0);
	}

	// Transform (kinproj_transform is on transposed form from 
	// openFrameworks' ofMatrix4x4, so post-multiplying).
	pos = pos*kinproj_transform;

	// Still do depth sorting
	pos.z = 256*(256*256*dp.r + 256*dp.g + dp.b);

	gl_Position = gl_ModelViewProjectionMatrix * pos;

	gl_PointSize = ps;
}
