#pragma once
#include "headers.h"
#include "LocalPlayer.h"




static bool ScreenTransform(SOCKET connection, DWORD procID, const D3DXVECTOR3& point, D3DXVECTOR3& screen, DWORD moduleBase)
{
	LocalPlayer localPlayer;
	const D3DXMATRIX& w2sMatrix = localPlayer.GetViewMatrix(connection, procID, moduleBase);

	screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
	screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
	screen.z = 0.0f;

	float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

	if (w < 0.001f) {
		screen.x *= 100000;
		screen.y *= 100000;
		return true;
	}

	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;

	return false;
}

static bool WorldToScreen(SOCKET connection, DWORD procID, const D3DXVECTOR3& origin, D3DXVECTOR3& screen, int w, int h, DWORD moduleBase)
{	
	if (!ScreenTransform(connection, procID, origin, screen, moduleBase)) {
		screen.x = (w / 2.0f) + (screen.x * w) / 2;
		screen.y = (h / 2.0f) - (screen.y * h) / 2;

		return true;
	}
	return false;
}