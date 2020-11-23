#pragma once

#include "headers.h"
#include "Entity.h"
#include "LocalPlayer.h"

using namespace hazedumper::netvars;
using namespace hazedumper::signatures;

void Radar(SOCKET connection, DWORD procID, DWORD moduleBase) {

	Entity entity;
	LocalPlayer player;
	

	for (short i = 0; i < 64; i++) {
		uintptr_t base = entity.GetEntityAddress(connection, procID, i, moduleBase);
		if (base == NULL)
			continue;
		driver::write(connection, procID, base + m_bSpotted, 1);
	}
}