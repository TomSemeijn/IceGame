#version 120

//Puts the given shading color info at the current pixel
//The shading color is arranged as follows:
//
//	R:	alpha [0 => 255]
//	G:	secondary alpha [0 => 255] (this makes the total alpha range [0 => 65025])
//	B:	normalRotation [0 => 2*PI] (including the horizontal and vertical flip)
//	A:	cells [0 => 255]

uniform sampler2D texture;

void main(){

	if(texture2D(texture, gl_TexCoord[0].xy).a > 0){

		gl_FragColor = gl_Color;

	}else{ discard; }

}