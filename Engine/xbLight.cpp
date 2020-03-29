#include "xblight.h"

CXBLight::CXBLight()
{

	position_x = 0.0f;
	position_y = 0.0f;
	position_z = -45.0f;

	ambient_r=0.5f;
	ambient_g=0.5f;
	ambient_b=0.5f;

	diffuse_r =1.0f;
	diffuse_g =1.0f;
	diffuse_b =1.0f;

	specular_r = 0.5f;
	specular_g = 0.5f;
	specular_b = 0.5f;

	attenuation_0 = 1.0f;
	attenuation_1 = 0.0f;
	attenuation_2 = 0.0f;

	range =1000.0f;

	scene_ambient_r=32;
	scene_ambient_g=32;
	scene_ambient_b=32;
}


void CXBLight::Do_Light()
{
	ZeroMemory(&d3dLight, sizeof(D3DLIGHT8));
	d3dLight.Type = D3DLIGHT_POINT;
	d3dLight.Diffuse.r = diffuse_r;
	d3dLight.Diffuse.g = diffuse_g;
	d3dLight.Diffuse.b = diffuse_b;
	d3dLight.Ambient.r = ambient_r;
	d3dLight.Ambient.g = ambient_r;
	d3dLight.Ambient.b = ambient_r;
	d3dLight.Specular.r = specular_r;
	d3dLight.Specular.g = specular_g;
	d3dLight.Specular.b = specular_b;
	d3dLight.Position.x = position_x;
	d3dLight.Position.y =position_y;
	d3dLight.Position.z = position_z;
	d3dLight.Attenuation0 = attenuation_0;
	d3dLight.Attenuation1 = attenuation_1;
	d3dLight.Attenuation2 = attenuation_2;
	d3dLight.Range = range;
	D3DDevice::SetLight(0, &d3dLight);
	D3DDevice::LightEnable(0, TRUE);
	D3DDevice::SetRenderState(D3DRS_LIGHTING, TRUE);
	D3DDevice::SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(scene_ambient_r,scene_ambient_g, scene_ambient_b));
	D3DDevice::SetRenderState( D3DRS_ZENABLE,          TRUE );
	D3DDevice::SetRenderState( D3DRS_DITHERENABLE,     TRUE );
	D3DDevice::SetRenderState( D3DRS_SPECULARENABLE,   TRUE );
	D3DDevice::SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	D3DDevice::SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	D3DDevice::SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	D3DDevice::SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	D3DDevice::SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT);
	D3DDevice::SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	D3DDevice::SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);

}


void CXBLight::SetPosition(float x, float y, float z)
{
	position_x = x;
	position_y = y;
	position_z = z;
}

void CXBLight::SetAmbient(float r, float g, float b)
{
	ambient_r=r;
	ambient_g=g;
	ambient_b=b;
}

void CXBLight::SetDiffuse(float r, float g, float b)
{
	diffuse_r =r;
	diffuse_g =g;
	diffuse_b =b;
}

void CXBLight::SetSpecular(float r, float g, float b)
{
	specular_r = r;
	specular_g = g;
	specular_b = b;
}

void CXBLight::SetAttenuation(float level1, float level2, float level3)
{
	attenuation_0 = level1;
	attenuation_1 = level2;
	attenuation_2 = level3;
}

void CXBLight::Setrange(float newrange)
{
	range=newrange;
}

void CXBLight::SetSceneAmbient(int r, int g, int b)
{
	scene_ambient_r=r;
	scene_ambient_g=g;
	scene_ambient_b=b;
}

