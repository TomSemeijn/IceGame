#version 120

//https://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map

uniform sampler2D unit_wave;
uniform vec2 resolution;

const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);

void main(){

	vec4 wave = texture2D(unit_wave, gl_TexCoord[0].xy);

	if(wave.a == 0){discard;}

	float left = texture2D(unit_wave, (gl_TexCoord[0].xy * resolution + off.xy) / resolution).x * 255.0;
   	float right = texture2D(unit_wave, (gl_TexCoord[0].xy * resolution + off.zy) / resolution).x * 255.0;
   	float top = texture2D(unit_wave, (gl_TexCoord[0].xy * resolution + off.yx) / resolution).x * 255.0;
    	float bottom = texture2D(unit_wave, (gl_TexCoord[0].xy * resolution + off.yz) / resolution).x * 255.0;

	vec3 va = vec3(size.xy,right-left);
    	vec3 vb = vec3(size.yx,bottom-top);
    	vec3 normal = normalize(cross(va,vb));

	normal += 1.0;
	normal /= 2.0;
	gl_FragColor = vec4(normal.x, 1.0 - normal.y, normal.z, wave.x);

}