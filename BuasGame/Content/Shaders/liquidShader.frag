#version 120

uniform sampler2D heightMap;
uniform vec4 dropColor;

void main(){

	if(texture2D(heightMap, gl_TexCoord[0].xy).a > 0){
		gl_FragColor = dropColor;
	}else{ discard; }

}