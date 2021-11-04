#version 120

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform sampler2D spriteMap;
uniform sampler2D normalMap;
uniform sampler2D infoMap;

uniform vec4 viewBox;
uniform vec2 resolution;
uniform vec4 viewport;

const float PI = 3.14159;

int alpha;
bool useSpecular;
int cells;
float normalRotation;
bool flipX;
bool flipY;
float z;

vec2 getRotatedAround(vec2 point, vec2 center, float radians){
	float sine = sin(radians);
	float cosine = cos(radians);
	float x = point.x - center.x;
	float y = point.y - center.y;
	return center + 
		vec2(
			x * cosine - y * sine, 
			x * sine + y * cosine 
		);
}

vec2 getWorldNormal(vec2 localNormal){
	if(flipX){ localNormal.x *= -1.0; }
	if(flipY){ localNormal.y *= -1.0; }
	return getRotatedAround(localNormal, vec2(0, 0), normalRotation);
}

vec3 mainSpecular(vec3 diffuseColor, vec3 lightColor, vec3 direction, vec3 normal, float attenuation){
	vec3 reflected = reflect(-direction, normal);
	return pow(clamp(dot(reflected, vec3(0.0, 0.0, 1.0)), 0.0, 1.0), alpha) * diffuseColor * 0.5 * lightColor * attenuation;
}

vec3 mainLighting(vec3 diffuseColor, vec3 lightColor, float intensity, vec3 direction, vec3 normal, float attenuation){
	direction = normalize(direction);
	float diffuse = max(dot(direction, normal), 0.0);
	vec3 endColor = lightColor * diffuse;
	endColor = vec3(endColor.x * diffuseColor.x, endColor.y * diffuseColor.y, endColor.z * diffuseColor.z);
	endColor = (endColor / 255.0) * intensity;
	endColor = vec3(min(endColor.x, 1.0), min(endColor.y, 1.0), min(endColor.z, 1.0));
	if(useSpecular){ endColor += mainSpecular(diffuseColor, lightColor, direction, normal, attenuation); }
	return endColor;
}

uniform vec3 rebSunLightPos[1];
uniform vec3 rebSunLightColor[1];
uniform float rebSunLightIntensity[1];
uniform int rebSunLightAmount;
uniform vec2 rebSunLightdirection[1];

vec3 rebSunLightLightFunction(vec3 pos, vec3 normal, vec3 diffuseColor, int lightIndex){
	vec3 lightPos = rebSunLightPos[lightIndex];
	vec3 lightColor = rebSunLightColor[lightIndex] / 255.0;
	float lightIntensity = rebSunLightIntensity[lightIndex];
	vec2 direction = rebSunLightdirection[lightIndex];
	return mainLighting(diffuseColor, lightColor, lightIntensity, vec3(direction, lightPos.z - pos.z), normal, 1.0);
}
uniform vec3 rebPointLightPos[18];
uniform vec3 rebPointLightColor[18];
uniform float rebPointLightIntensity[18];
uniform int rebPointLightAmount;
uniform float rebPointLightradius[18];

vec3 rebPointLightLightFunction(vec3 pos, vec3 normal, vec3 diffuseColor, int lightIndex){
	vec3 lightPos = rebPointLightPos[lightIndex];
	vec3 lightColor = rebPointLightColor[lightIndex] / 255.0;
	float lightIntensity = rebPointLightIntensity[lightIndex];
	float radius = rebPointLightradius[lightIndex];
	float distance = length(lightPos - pos);
	vec3 direction = lightPos - pos;
	direction = normalize(direction);
	float att = pow(clamp(1.0 - pow(distance, 2.0) / pow(radius, 2.0), 0.0, 1.0), 2.0);
	return mainLighting(diffuseColor, lightColor, lightIntensity * att, direction, normal, att);
}
void main(){
	vec4 diffuseColor = texture2D(spriteMap, gl_TexCoord[0].xy);
	if (diffuseColor.a == 0){discard;}
	
	vec4 infoColor = texture2D(infoMap, gl_TexCoord[0].xy);
	cells = int(floor(infoColor.a * 255.0));
	if(cells == 0){ gl_FragColor = diffuseColor; return; }
	
	alpha = int(infoColor.r * 255.0);
	flipX = (infoColor.g > 0.45 && infoColor.g < 0.55) || infoColor.g > 0.9;
	flipY = (infoColor.g > 0.7 && infoColor.g < 0.8) || infoColor.g > 0.9;
	useSpecular = alpha > 0;
	normalRotation = infoColor.b * 2 * PI;
	
	vec3 resultColor = vec3(pow(ambientIntensity, 2) * ambientColor * diffuseColor.rgb);
	
	vec2 screenPos = vec2(gl_FragCoord.xy / resolution.xy);
	screenPos = vec2(screenPos.x, 1.0 - screenPos.y);
	screenPos -= viewport.xy;
	screenPos /= viewport.ba;
	
	vec4 normalColor = texture2D(normalMap, gl_TexCoord[0].xy);
	z = ((normalColor.a * 2.0) - 1.0) * 255.0;
	vec3 worldPos = vec3(viewBox.xy + viewBox.ba * screenPos, z);
	vec3 normal = (normalColor.rgb - 0.5) * 2.0;
	normal = normalize(normal);
	normal.y *= -1.0;
	normal = vec3(getWorldNormal(normal.xy), normal.z);
	if(rebSunLightAmount > 0){
	for(int k = 0; k < rebSunLightAmount; k++){
			resultColor += rebSunLightLightFunction(worldPos, normal, diffuseColor.rgb, k);
		}
	}
	if(rebPointLightAmount > 0){
	for(int k = 0; k < rebPointLightAmount; k++){
			resultColor += rebPointLightLightFunction(worldPos, normal, diffuseColor.rgb, k);
		}
	}
	resultColor *= cells; resultColor = floor(resultColor); resultColor /= cells - 0.5;
	gl_FragColor = vec4(resultColor * gl_Color.rgb, diffuseColor.a * gl_Color.a);
	//gl_FragColor = diffuseColor;
	//gl_FragColor = normalColor;
	//gl_FragColor = vec4(screenPos, 0.0, 1.0);
	//gl_FragColor = vec4(worldPos.x / 480.0, worldPos.y / 270.0, 0.0, 1.0);
	//gl_FragColor = vec4(normalColor.rgb, 1.0);
}
