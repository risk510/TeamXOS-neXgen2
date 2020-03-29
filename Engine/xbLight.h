#pragma once

#include <xtl.h>

class CXBLight
{
private:
	float position_x;
	float position_y;
	float position_z;

	float ambient_r;
	float ambient_g;
	float ambient_b;

	float diffuse_r;
	float diffuse_g;
	float diffuse_b;

	float specular_r;
	float specular_g;
	float specular_b;

	float attenuation_0;
	float attenuation_1;
	float attenuation_2;

	float range;

	int scene_ambient_r;
	int scene_ambient_g;
	int scene_ambient_b;

	D3DLIGHT8 d3dLight;

public:

	CXBLight::CXBLight();
	CXBLight::~CXBLight()
	{
	}

	void CXBLight::Do_Light();

	void CXBLight::SetPosition(float x, float y, float z);
	void CXBLight::SetAmbient(float r, float g, float b);
	void CXBLight::SetDiffuse(float r, float g, float b);
	void CXBLight::SetSpecular(float r, float g, float b);
	void CXBLight::SetAttenuation(float level1, float level2, float level3);
	void CXBLight::Setrange(float newrange);
	void CXBLight::SetSceneAmbient(int r, int g, int b);
};