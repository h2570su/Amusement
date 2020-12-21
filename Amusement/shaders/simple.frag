#version 430 core
out vec4 f_color;

in vec3 o_position;
in vec3 o_normal;
in vec2 o_texture_coordinate;
in vec3 o_color;

layout(std140, binding = 2) uniform environment
{
	vec3 u_viewer_pos;
	float u_time;
};

struct DirLight {
    vec4 direction;
  
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};  
struct PointLight {    
    vec4 position;   
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

	float constant;
    float linear;
    float quadratic;  
};  
struct SpotLight {    
    vec4 position;
        
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

	vec4 direction;

	float constant;
	float linear;
    float quadratic;  
	float cutoff;
	float outer_cutoff;
};    
#define NR_DIRECTIONAL_LIGHTS 4  
#define NR_POINT_LIGHTS 4  
#define NR_SPOT_LIGHTS 4
layout (std140, binding = 1) uniform lights
{
	DirLight dirLights[NR_DIRECTIONAL_LIGHTS];
	PointLight pointLights[NR_POINT_LIGHTS];
	SpotLight spotLights[NR_SPOT_LIGHTS];
};

uniform bool u_useTexture;
uniform sampler2D u_texture;



vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 materialColor)

{
    vec3 lightDir = normalize(-light.direction.xyz);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir  = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);

    // combine results
    vec3 ambient = light.ambient.rgb * materialColor;
    vec3 diffuse = light.diffuse.rgb * diff * materialColor;
    vec3 specular = light.specular.rgb * spec * materialColor;
    return (ambient + diffuse + specular);
}  

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir  = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);

    // attenuation
    float distance    = length(light.position.xyz - fragPos);
	light.constant=(light.constant==0.0f)?0.0000001f:light.constant;
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient.rgb  * materialColor;
    vec3 diffuse  = light.diffuse.rgb  * diff * materialColor;
    vec3 specular = light.specular.rgb * spec * materialColor;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 materialColor)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir  = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);

    // attenuation
	light.constant=(light.constant==0.0f)?0.0000001f:light.constant;
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction.xyz)); 
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient.rgb * materialColor;
    vec3 diffuse = light.diffuse.rgb * diff * materialColor;
    vec3 specular = light.specular.rgb * spec * materialColor;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{   
	vec3 sourceColor;
	
	if(u_useTexture)
	{    
		 sourceColor = vec3(texture(u_texture, o_texture_coordinate));		
	}
	else
	{
		sourceColor = o_color;		
	}
	

	vec3 result = vec3(0,0,0);
	vec3 _normal = normalize( o_normal);
	vec3 viewDir = normalize(u_viewer_pos - o_position);
	for(int i=0;i<NR_DIRECTIONAL_LIGHTS;i++)
	{
		result += CalcDirLight(dirLights[i], _normal, viewDir, sourceColor);
	}
	for(int i=0;i<NR_POINT_LIGHTS;i++)
	{
		result += CalcPointLight(pointLights[i], _normal, o_position, viewDir, sourceColor);
	}
	for(int i=0;i<NR_SPOT_LIGHTS;i++)
	{
		result += CalcSpotLight(spotLights[i], _normal, o_position, viewDir, sourceColor);
	}
	f_color = vec4(result, 1);

}