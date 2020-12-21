#version 450 core
precision highp float;
out vec4 f_color;

in vec3 f_in_position;
in vec3 f_in_normal;
in vec2 f_in_texture_coordinate;
in vec3 f_in_color;
in vec4 f_in_screenCoord;

layout(std140, binding = 2) uniform environment
{
	vec3 u_viewer_pos;
	float u_time;
};
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct PointLight {    
    vec3 position;   
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;  
};  
struct SpotLight {    
    vec3 position;
        
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	float constant;
    float linear;
    float quadratic;  

	 vec3 direction;
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
uniform sampler2D u_reflectTexture;
uniform sampler2D u_refractTexture;
uniform samplerCube u_skybox;
uniform bool u_realTimeRender;

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
		 sourceColor = vec3(texture(u_reflectTexture, f_in_texture_coordinate));
		
	}
	else
	{
		sourceColor = f_in_color;		
	}
	
	vec3 result = vec3(0,0,0);
	vec3 _normal = normalize(f_in_normal);
	
	vec3 viewDir = normalize(u_viewer_pos - f_in_position);
	for(int i=0;i<NR_DIRECTIONAL_LIGHTS;i++)
	{
		result += CalcDirLight(dirLights[i], _normal, viewDir, sourceColor);
	}
	for(int i=0;i<NR_POINT_LIGHTS;i++)
	{
		result += CalcPointLight(pointLights[i], _normal, f_in_position, viewDir, sourceColor);
	}
	for(int i=0;i<NR_SPOT_LIGHTS;i++)
	{
		result += CalcSpotLight(spotLights[i], _normal, f_in_position, viewDir, sourceColor);
	}
	vec4 baseColor = vec4(result, 1);

	if(u_realTimeRender)
	{
	
		float _FresnelBase = 0.0;
		float _FresnelScale = 10.0;
		float _FresnelPower = 6.0;
		vec4 reflectColor = texture(u_reflectTexture, f_in_screenCoord.xy/f_in_screenCoord.w+(f_in_position.y/20));
		vec4 refractColor = texture(u_refractTexture, f_in_screenCoord.xy/f_in_screenCoord.w+(f_in_position.y/20));
		float fresnel = 0.0;
		if((-viewDir).y<0)
		{				
			fresnel = clamp( _FresnelBase + _FresnelScale * pow(1 - dot(_normal, viewDir), _FresnelPower), 0.0, 1.0);
		}
		else
		{				
		    fresnel = clamp( _FresnelBase + _FresnelScale * pow(1 - dot(-_normal, viewDir), _FresnelPower), 0.0, 1.0);
		}
		//fresnel =0;
		f_color = refractColor*(1-fresnel)+reflectColor*fresnel;
		f_color = baseColor*0.3+f_color*0.7;
	}
	else
	{
		float _FresnelBase = 0.0;
		float _FresnelScale = 1.0;
		float _FresnelPower = 2.0;
		vec4 reflectColor = texture(u_skybox, reflect(-viewDir, _normal));
		vec4 refractColor ;
		float fresnel = 0.0;
		if((-viewDir).y<0)
		{
			refractColor = texture(u_skybox, refract(viewDir, _normal, 1.0/1.33));
			fresnel = clamp( _FresnelBase + _FresnelScale * pow(1 - dot(_normal, viewDir), _FresnelPower), 0.0, 1.0);
		}
		else
		{
			refractColor = texture(u_skybox, refract(viewDir, -_normal, 1.0/1.33));
		    fresnel = clamp( _FresnelBase + _FresnelScale * pow(1 - dot(-_normal, viewDir), _FresnelPower), 0.0, 1.0);
		}
			//fresnel =0;
		f_color = refractColor*(1-fresnel)+reflectColor*fresnel;
			
	}
	//f_color = vec4(_normal,1);
	//f_color = baseColor;
	//f_color = vec4(f_in_texture_coordinate,0,1.0);	
}