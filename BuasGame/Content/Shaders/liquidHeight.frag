#version 120

uniform vec2 dropCenters[${DROPAMOUNT}];
uniform float minDistance[${DROPAMOUNT}];

uniform int dropCount;

uniform vec4 viewBox;

void main(){

	vec2 screenPos = vec2(gl_FragCoord.xy / viewBox.ba);
	vec2 worldPos = vec2(viewBox.xy + viewBox.ba * screenPos);

	float power = 0;

	for(int k = 0; k < dropCount; k++){
		float distance = length(dropCenters[k] - worldPos);
		if(distance <= minDistance[k]){
			power += pow(clamp(1.0 - pow(distance, 2) / pow(minDistance[k], 2), 0.0, 1.0), 2.0);
		}
	}

	if(power >= 0.5){
		power /= pow(dropCount, 0.5);
		gl_FragColor = vec4(power, power, power, 1.0);
	}else{discard;}

}