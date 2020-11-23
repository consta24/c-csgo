#include "server_shared.h"
#include "sockets.h"
#include "imports.h"

ULONG get_module_32(IN PEPROCESS pProcess, LPCWSTR ModuleName) {

	ASSERT(pProcess != NULL);
	if (pProcess == NULL)
		return NULL;


	__try {
		LARGE_INTEGER time = { 0 };
		time.QuadPart = -250ll * 10 * 1000;

		PPEB32 pPeb32 = (PPEB32)PsGetProcessWow64Process(pProcess);
		if (pPeb32 == NULL) {
			return NULL;

		}

		for (INT i = 0; !pPeb32->Ldr && i < 10; i++) {
			KeDelayExecutionThread(KernelMode, TRUE, &time);
		}


		if (!pPeb32->Ldr) {
			return NULL;
		}


		for (PLIST_ENTRY32 pListEntry = (PLIST_ENTRY32)((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList.Flink;
			pListEntry != &((PPEB_LDR_DATA32)pPeb32->Ldr)->InLoadOrderModuleList;
			pListEntry = (PLIST_ENTRY32)pListEntry->Flink) {
			PLDR_DATA_TABLE_ENTRY32 pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY32, InLoadOrderLinks);

			if (RtlCompareMemory((PVOID)pEntry->BaseDllName.Buffer, ModuleName, pEntry->BaseDllName.Length) == pEntry->BaseDllName.Length) {
				return pEntry->DllBase;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		return NULL;
	}

	return NULL;
}

static uint64_t handle_copy_memory(const PacketCopyMemory& packet)
{
	PEPROCESS dest_process = nullptr;
	PEPROCESS src_process = nullptr;

	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.dest_process_id), &dest_process)))
	{
		return uint64_t(STATUS_INVALID_CID);
	}

	if (!NT_SUCCESS(PsLookupProcessByProcessId(HANDLE(packet.src_process_id), &src_process)))
	{
		ObDereferenceObject(dest_process);
		return uint64_t(STATUS_INVALID_CID);
	}

	SIZE_T   return_size = 0;
	NTSTATUS status = MmCopyVirtualMemory(
		src_process,
		(void*)packet.src_address,
		dest_process,
		(void*)packet.dest_address,
		packet.size,
		UserMode,
		&return_size
	);

	ObDereferenceObject(dest_process);
	ObDereferenceObject(src_process);

	return uint64_t(status);
}

static ULONG handle_get_client_address(const PacketGetClientAddress& packet)
{
	PEPROCESS process = nullptr;
	NTSTATUS  status = PsLookupProcessByProcessId(HANDLE(packet.process_id), &process);
	log("get_client_address started!");
	if (!NT_SUCCESS(status))
		return 0;
	KeAttachProcess((PRKPROCESS)process);
	const ULONG base_address = get_module_32(process, L"client_panorama.dll");
	KeDetachProcess();
	log("client_panorama.dll: %d\n", base_address);
	ObDereferenceObject(process);

	return base_address;
}

static ULONG handle_get_engine_address(const PacketGetEngineAddress& packet)
{
	PEPROCESS process = nullptr;
	NTSTATUS  status = PsLookupProcessByProcessId(HANDLE(packet.process_id), &process);
	log("get_engine_address started!");
	if (!NT_SUCCESS(status))
		return 0;
	KeAttachProcess((PRKPROCESS)process);
	const ULONG base_address = get_module_32(process, L"engine.dll");
	KeDetachProcess();
	log("engine.dll: %d\n", base_address);
	ObDereferenceObject(process);

	return base_address;
}

static uint64_t handle_clean_piddb_cache() {
	log("clean_piddb_cache started!");

	size_t size;
	uintptr_t ntoskrnlBase = get_kernel_address("ntoskrnl.exe", size);

	log("ntoskrnl.exe: %d\n", ntoskrnlBase);
	log("ntoskrnl.exe size: %d\n", size);

	// 1809: \x48\x8D\x0D\x00\x00\x00\x00\x4C\x89\x35\x00\x00\x00\x00\x49 xxx????xxx????x
	// 1903: \x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x3d\x00\x00\x00\x00\x0f\x83 xxx????x????x????xx

	RTL_OSVERSIONINFOW osVersion = { 0 };
	osVersion.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	RtlGetVersion(&osVersion);

	log("Windows Build Number: %d\n", osVersion.dwBuildNumber);
	log("Windows Major Version: %d\n", osVersion.dwMajorVersion);
	log("Windows Minor Version: %d\n", osVersion.dwMinorVersion);

	PRTL_AVL_TABLE PiDDBCacheTable = nullptr;

	if (osVersion.dwBuildNumber >= 18362)
		PiDDBCacheTable = (PRTL_AVL_TABLE)dereference(find_pattern<uintptr_t>((void*)ntoskrnlBase, size, "\x48\x8d\x0d\x00\x00\x00\x00\xe8\x00\x00\x00\x00\x3d\x00\x00\x00\x00\x0f\x83", "xxx????x????x????xx"), 3);
	else if (osVersion.dwBuildNumber >= 17763)
		PiDDBCacheTable = (PRTL_AVL_TABLE)dereference(find_pattern<uintptr_t>((void*)ntoskrnlBase, size, "\x48\x8D\x0D\x00\x00\x00\x00\x4C\x89\x35\x00\x00\x00\x00\x49", "xxx????xxx????x"), 3);
	else if (osVersion.dwBuildNumber >= 17134)
		PiDDBCacheTable = (PRTL_AVL_TABLE)dereference(find_pattern<uintptr_t>((void*)ntoskrnlBase, size, "\x48\x8D\x0D\x00\x00\x00\x00\x4C\x89\x35\x00\x00\x00\x00\x49", "xxx????xxx????x"), 3);

	log("PiDDBCacheTable: %d\n", PiDDBCacheTable);

	if (!PiDDBCacheTable) {
		log("PiDDBCacheTable equals 0\n");
		return 0;
	}

	uintptr_t entry_address = uintptr_t(PiDDBCacheTable->BalancedRoot.RightChild) + sizeof(RTL_BALANCED_LINKS);
	log("entry_address: %d\n", entry_address);

	piddbcache* entry = (piddbcache*)(entry_address);

	/*capcom.sys(drvmap) : 0x57CD1415 iqvw64e.sys(kdmapper) : 0x5284EAC3, also cpuz driver*/
	if (entry->TimeDateStamp == 0x57CD1415 || entry->TimeDateStamp == 0x5284EAC3) {
		entry->TimeDateStamp = 0x54EAC3;
		entry->DriverName = RTL_CONSTANT_STRING(L"monitor.sys");
	}

	ULONG count = 0;
	for (auto link = entry->List.Flink; link != entry->List.Blink; link = link->Flink, count++)
	{
		piddbcache* cache_entry = (piddbcache*)(link);

		if (cache_entry->TimeDateStamp == 0x57CD1415 || cache_entry->TimeDateStamp == 0x5284EAC3) {
			cache_entry->TimeDateStamp = 0x54EAC4 + count;
			cache_entry->DriverName = RTL_CONSTANT_STRING(L"monitor.sys");
		}
	}

	log("clean_piddb_cache finished!");

	return 1;
}

static uint64_t handle_clean_unloaded_drivers() {
	log("clean_uloaded_drivers started!\n");
	ULONG bytes = 0;
	auto status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);

	if (!bytes)
		return 0;

	PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePool(NonPagedPool, bytes);

	status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

	if (!NT_SUCCESS(status)) {
		log("ZwQuerySystemInformation failed(unloaded drivers)\n");
		ExFreePool(modules);
		return 0;
	}

	PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
	uintptr_t ntoskrnlBase = 0;
	size_t ntoskrnlSize = 0;

	ntoskrnlBase = get_kernel_address("ntoskrnl.exe", ntoskrnlSize);

	ExFreePool(modules);

	if (ntoskrnlBase <= 0) {
		log("get_kernel_address failed(unloaded drivers)\n");
		return 0;
	}

	// 1809: \x48\x8B\x05\x00\x00\x00\x00\x48\xF7\xD8\x1B\xC9\x81 xxx????xxxxxx
	// 1903: \x4C\x8B\x00\x00\x00\x00\x00\x4C\x8B\xC9\x4D\x85\x00\x74 xx?????xxxxx?x + 3 + current signature address = MmUnloadedDrivers

	RTL_OSVERSIONINFOW osVersion = { 0 };
	osVersion.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	RtlGetVersion(&osVersion);

	log("Windows Build Number: %d\n", osVersion.dwBuildNumber);
	log("Windows Major Version: %d\n", osVersion.dwMajorVersion);
	log("Windows Minor Version: %d\n", osVersion.dwMinorVersion);

	uintptr_t mmUnloadedDriversPtr = 0;

	if (osVersion.dwBuildNumber >= 18362)
		mmUnloadedDriversPtr = find_pattern<uintptr_t>((void*)ntoskrnlBase, ntoskrnlSize, "\x4C\x8B\x00\x00\x00\x00\x00\x4C\x8B\xC9\x4D\x85\x00\x74", "xx?????xxxxx?x");
	else if (osVersion.dwBuildNumber >= 17763)
		mmUnloadedDriversPtr = find_pattern<uintptr_t>((void*)ntoskrnlBase, ntoskrnlSize, "\x48\x8B\x05\x00\x00\x00\x00\x48\xF7\xD8\x1B\xC9\x81", "xxx????xxxxxx");
	else if (osVersion.dwBuildNumber >= 17134)
		mmUnloadedDriversPtr = find_pattern<uintptr_t>((void*)ntoskrnlBase, ntoskrnlSize, "\x48\x8B\x05\x00\x00\x00\x00\x48\xF7\xD8\x1B\xC9\x81", "xxx????xxxxxx");

	log("mmUnloadedDriversPtr: %d\n", mmUnloadedDriversPtr);

	if (!mmUnloadedDriversPtr) {
		log("mmUnloadedDriversPtr equals 0(unloaded drivers)\n");
		return 0;
	}

	uintptr_t mmUnloadedDrivers = dereference(mmUnloadedDriversPtr, 3);

	memset(*(uintptr_t * *)mmUnloadedDrivers, 0, 0x7D0);

	log("clean_uloaded_drivers finished!\n");

	return 1;
}

uint64_t handle_incoming_packet(const Packet& packet)
{
	switch (packet.header.type)
	{
	case PacketType::packet_copy_memory:
		return handle_copy_memory(packet.data.copy_memory);

	case PacketType::packet_get_client_address:
		return handle_get_client_address(packet.data.get_client_address);

	case PacketType::packet_get_engine_address:
		return handle_get_engine_address(packet.data.get_engine_address);

	case PacketType::packet_clean_piddbcachetable:
		return handle_clean_piddb_cache();

	case PacketType::packet_clean_mmunloadeddrivers:
		return handle_clean_unloaded_drivers();

	default:
		break;
	}

	return uint64_t(STATUS_NOT_IMPLEMENTED);
}

// Send completion packet.
bool complete_request(const SOCKET client_connection, const uint64_t result)
{
	Packet packet{ };

	packet.header.magic = packet_magic;
	packet.header.type = PacketType::packet_completed;
	packet.data.completed.result = result;

	return send(client_connection, &packet, sizeof(packet), 0) != SOCKET_ERROR;
}
