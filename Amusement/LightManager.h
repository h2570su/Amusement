#pragma once
#include <vector>
#include <glm-0.9.8.5/glm/glm/glm.hpp>
#include "RenderUtilities/BufferObject.h"
class LightManager
{
public:
	struct DirectionalLight
	{
		glm::vec4 dir;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
	};

	struct PointLight
	{
		glm::vec4 pos;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;

		float attenuation_constant;
		float attenuation_linear;
		float attenuation_quadratic;
	};

	struct SpotLight
	{
		glm::vec4 dir;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;

		glm::vec4 direction;

		float attenuation_constant;
		float attenuation_linear;
		float attenuation_quadratic;
		
		//inner cutoff, in degrees
		float cutoff;
		//outer cutoff, in degrees
		float outer_cutoff;
	};
	static int addDirLight(const DirectionalLight& light);
	static int addPointLight(const PointLight& light);
	static int addSpotLight(const SpotLight& light);
	static void delDirLight(const int idx);
	static void delPointLight(const int idx);
	static void delSpotLight(const int idx);
	static void modDirLight(const int idx, const DirectionalLight& light);
	static void modPointLight(const int idx, const PointLight& light);
	static void modSpotLight(const int idx, const SpotLight& light);

	static void initUBO();
	static void updateUBO(int binding);
private:

	const static int NR_DIRECTIONAL_LIGHTS = 4;
	const static int NR_POINT_LIGHTS = 4;
	const static int NR_SPOT_LIGHTS = 4;

	static std::vector<DirectionalLight> dirs;
	static std::vector<PointLight> points;
	static std::vector<SpotLight> spots;

	static UBO ubo;
	
};

