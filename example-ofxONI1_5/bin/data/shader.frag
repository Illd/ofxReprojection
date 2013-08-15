#version 120
#extension GL_ARB_texture_rectangle : enable
uniform sampler2DRect color_image;

void main() {
	if(gl_Color.rgb == vec3(0,0,0)) discard;
	//gl_FragColor = gl_Color;

	vec4 color_map = texture2DRect(color_image, gl_TexCoord[0].st);
	
	gl_FragColor = color_map;
}
