#version 120

void main() {
	if(gl_Color.rgb == vec3(0,0,0)) discard;
	gl_FragColor = gl_Color;
}
