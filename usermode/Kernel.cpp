#include "headers.h"
#include "Kernel.h"




	HANDLE Kernel::CreateHandle() {
		HANDLE hDriver;
		hDriver = CreateFileA("\\\\.\\AATest", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

		if (!hDriver || hDriver == INVALID_HANDLE_VALUE) {
			cout << "INVALID_HANDLE_VALUE" << endl;
			cout << hDriver << endl;
		}

		return hDriver;

	}

	template <typename type>
	type Kernel::ReadVirtualMemory(ULONG ProcessID, ULONG ReadAddress) {

		KERNEL_READ_REQUEST ReadRequest;

		type buffer;
		ReadRequest.ProcessID = ProcessID;
		ReadRequest.Address = ReadAddress;
		ReadRequest.Size = sizeof(type);
		ReadRequest.Response = (ULONG)&buffer;


		if (DeviceIoControl(CreateHandle(), AADRV_READ_REQUEST, &ReadRequest, sizeof(ReadRequest), &ReadRequest, sizeof(ReadRequest), 0, 0))
			return buffer;
		else
			return *(type*)false;

	}


	bool Kernel::WriteVirtualMemory(ULONG ProcessID, ULONG WriteAddress, ULONG WriteValue, SIZE_T WriteSize) {

		DWORD bytesIO;
		KERNEL_WRITE_REQUEST WriteRequest;

		WriteRequest.ProcessID = ProcessID;
		WriteRequest.Address = WriteAddress;
		WriteRequest.Value = WriteValue;
		WriteRequest.Size = WriteSize;

		if (DeviceIoControl(CreateHandle(), AADRV_WRITE_REQUEST, &WriteRequest, sizeof(WriteRequest), 0, 0, &bytesIO, NULL))
			return true;
		else
			return false;
	}


	ULONG Kernel::GetModule() {

		ULONG address;
		DWORD bytesIO;

		if (DeviceIoControl(CreateHandle(), AADRV_GETMODULE_REQUEST, &address, sizeof(address), &address, sizeof(address), &bytesIO, NULL))
			return address;
		else
			return false;

	}

	ULONG Kernel::GetEngine() {

		ULONG engine;
		DWORD bytesIO;

		if (DeviceIoControl(CreateHandle(), AADRV_GETENGINE_REQUEST, &engine, sizeof(engine), &engine, sizeof(engine), &bytesIO, NULL))
			return engine;
		else
			return false;

	}

	ULONG Kernel::GetProcessID() {

		ULONG id;
		DWORD bytesIO;

		if (DeviceIoControl(CreateHandle(), AADRV_GETID_REQUEST, &id, sizeof(id), &id, sizeof(id), &bytesIO, NULL))
			return id;
		else
			return false;

	}






