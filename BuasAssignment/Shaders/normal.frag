#version 120

uniform sampler2D diffuse;
uniform sampler2D normal;

void main(){

	if(texture2D(diffuse, gl_TexCoord[0].xy).a > 0){
		vec4 normalColor = texture2D(normal, gl_TexCoord[0].xy);
		gl_FragColor = vec4(normalColor.rgb, 1.0 - normalColor.a + gl_Color.a);
	}else{ discard; }

}