#pragma once
#include "headers.h"

using namespace std;

struct Offset {
	vector<DWORD> pattern;
	int offset;
	int ptrOffset;
	Offset(vector<DWORD> p, int o, int pO) {
		pattern = p;
		offset = o;
		ptrOffset = pO;
	}
};

struct PatternScan {

	DWORD FindPattern(DWORD offsetBase, Offset off);

};
