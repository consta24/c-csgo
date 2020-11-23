#pragma once
#include "headers.h"
#include "PatternScan.h"

DWORD PatternScan::FindPattern(DWORD offsetBase, Offset off) {

	if (!offsetBase)
		return NULL;
	vector<DWORD> pattern = off.pattern;
	for (int i = 0; i < pattern.size() - sizeof(offsetBase); i++) {
		bool success = true;
		for (DWORD j = 0; j < pattern.size(); j++) {
			if (pattern[j] == '?') {
				continue;
			}
			DWORD buffer = driver::read<DWORD>(offsetBase + i + j);
			if (!buffer)
				return NULL;
			cout << "compare: " << pattern[j] << "====" << buffer << endl;
			if (pattern[j] != buffer)
				success = false;
			if (success == false)
				break;
		}
		if (success) {
			DWORD offset = i + off.offset;
			DWORD ptr = driver::read<DWORD>(offsetBase + offset);
			DWORD result = driver::read<DWORD>(ptr + off.ptrOffset);
			return result;
		}
	}
	return NULL;
}