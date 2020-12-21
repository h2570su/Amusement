#include "LightManager.h"
std::vector<LightManager::DirectionalLight> LightManager::dirs = std::vector<LightManager::DirectionalLight>();
std::vector<LightManager::PointLight> LightManager::points = std::vector<LightManager::PointLight>();
std::vector<LightManager::SpotLight> LightManager::spots = std::vector<LightManager::SpotLight>();
UBO LightManager::ubo = UBO();
int LightManager::addDirLight(const DirectionalLight & light)
{
	if (LightManager::dirs.size() < LightManager::NR_DIRECTIONAL_LIGHTS)
	{
		LightManager::dirs.push_back(light);
		return LightManager::dirs.size() - 1;
	}
	else
	{
		int idx = 0;
		while (LightManager::dirs[idx].ambient == glm::vec4(0) && LightManager::dirs[idx].diffuse == glm::vec4(0) && LightManager::dirs[idx].specular == glm::vec4(0))
		{
			idx++;
			if (idx >= LightManager::dirs.size())
			{
				break;
			}
		}
		if (idx >= LightManager::dirs.size())
		{
			return -1;
		}
		else
		{
			return idx;
		}
	}
}

int LightManager::addPointLight(const PointLight & light)
{
	if (LightManager::points.size() < LightManager::NR_POINT_LIGHTS)
	{
		LightManager::points.push_back(light);
		return LightManager::points.size() - 1;
	}
	else
	{
		int idx = 0;
		while (LightManager::points[idx].ambient == glm::vec4(0) && LightManager::points[idx].diffuse == glm::vec4(0) && LightManager::points[idx].specular == glm::vec4(0))
		{
			idx++;
			if (idx >= LightManager::points.size())
			{
				break;
			}
		}
		if (idx >= LightManager::points.size())
		{
			return -1;
		}
		else
		{
			return idx;
		}
	}
}

int LightManager::addSpotLight(const SpotLight & light)
{
	if (LightManager::spots.size() < LightManager::NR_POINT_LIGHTS)
	{
		LightManager::spots.push_back(light);
		return LightManager::spots.size() - 1;
	}
	else
	{
		int idx = 0;
		while (LightManager::spots[idx].ambient == glm::vec4(0) && LightManager::spots[idx].diffuse == glm::vec4(0) && LightManager::spots[idx].specular == glm::vec4(0))
		{
			idx++;
			if (idx >= LightManager::spots.size())
			{
				break;
			}
		}
		if (idx >= LightManager::spots.size())
		{
			return -1;
		}
		else
		{
			return idx;
		}
	}
}

void LightManager::delDirLight(const int idx)
{
	if (idx >= 0 && idx < LightManager::dirs.size())
	{
		LightManager::dirs[idx].ambient = glm::vec4(0);
		LightManager::dirs[idx].diffuse = glm::vec4(0);
		LightManager::dirs[idx].specular = glm::vec4(0);
	}
}

void LightManager::delPointLight(const int idx)
{
	if (idx >= 0 && idx < LightManager::points.size())
	{
		LightManager::points[idx].ambient = glm::vec4(0);
		LightManager::points[idx].diffuse = glm::vec4(0);
		LightManager::points[idx].specular = glm::vec4(0);
	}
}

void LightManager::delSpotLight(const int idx)
{
	if (idx >= 0 && idx < LightManager::spots.size())
	{
		LightManager::spots[idx].ambient = glm::vec4(0);
		LightManager::spots[idx].diffuse = glm::vec4(0);
		LightManager::spots[idx].specular = glm::vec4(0);
	}
}

void LightManager::modDirLight(const int idx, const DirectionalLight & light)
{
	if (idx >= 0 && idx < LightManager::dirs.size())
	{
		LightManager::dirs[idx] = light;
	}
}

void LightManager::modPointLight(const int idx, const PointLight & light)
{
	if (idx >= 0 && idx < LightManager::points.size())
	{
		LightManager::points[idx] = light;
	}
}

void LightManager::modSpotLight(const int idx, const SpotLight & light)
{
	if (idx >= 0 && idx < LightManager::spots.size())
	{
		LightManager::spots[idx] = light;
	}
}

void LightManager::initUBO()
{
	LightManager::ubo = UBO();
	ubo.size = sizeof(DirectionalLight)*LightManager::NR_DIRECTIONAL_LIGHTS +
		(sizeof(PointLight)+4)*LightManager::NR_POINT_LIGHTS +
		(sizeof(SpotLight)+12)*LightManager::NR_SPOT_LIGHTS;
	glGenBuffers(1, &LightManager::ubo.ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, LightManager::ubo.ubo);
	glBufferData(GL_UNIFORM_BUFFER, LightManager::ubo.size, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

}

void LightManager::updateUBO(int binding)
{
	glBindBuffer(GL_UNIFORM_BUFFER, LightManager::ubo.ubo);

	int ptr = 0;
	for (int i = 0; i < LightManager::NR_DIRECTIONAL_LIGHTS; i++)
	{
		if (i < LightManager::dirs.size())
		{
			glBufferSubData(GL_UNIFORM_BUFFER, ptr, sizeof(DirectionalLight), &LightManager::dirs[i]);
		}
		ptr += sizeof(DirectionalLight);
	}
	for (int i = 0; i < LightManager::NR_POINT_LIGHTS; i++)
	{
		if (i < LightManager::points.size())
		{
			glBufferSubData(GL_UNIFORM_BUFFER, ptr, sizeof(PointLight), &LightManager::points[i]);
		}
		ptr += sizeof(PointLight)+4;
	}
	for (int i = 0; i < LightManager::NR_SPOT_LIGHTS; i++)
	{
		if (i < LightManager::spots.size())
		{
			glBufferSubData(GL_UNIFORM_BUFFER, ptr, sizeof(SpotLight), &LightManager::spots[i]);
		}
		ptr += sizeof(SpotLight)+12;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, binding, LightManager::ubo.ubo, 0, LightManager::ubo.size);
}
