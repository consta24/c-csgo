#pragma once
pragma pack(push, 1)
struct driver_request
{
	uint64_t 	process_id;
	uint64_t 	base_address;
	uint64_t 	size;

	uint8_t 	buffer[1];
};
#pragma pack(pop)

//client part
template <typename T>
bool request_write(const uint64_t base_address, const T& data)
{
	static_assert(sizeof(T) > 0, "nothing to write");

	constexpr size_t allocation_size = sizeof(driver_request) + sizeof(T) - 1;

	std::array<uint8_t, allocation_size> request_buffer; //allocate on heap if you want
	const auto request = reinterpret_cast<driver_request*>(request_buffer.data());

	request->process_id = pid;
	request->base_address = base_address;
	request->size = sizeof(T);

	std::memcpy(request->buffer, &data, sizeof(T));

	//call DeviceIoControl, buffer size is allocation_size

	...
}

//driver part
extern "C" NTSTATUS write(const uint64_t base_address, const uint64_t size, const void* buffer)
{
	...
}

extern "C" NTSTATUS on_device_io_control(DEVICE_OBJECT * device_object, IRP * irp)
{
	const auto io_stack = IoGetCurrentIrpStackLocation(irp);
	const auto control_code = io_stack->Parameters.DeviceIoControl.IoControlCode;

	if (control_code == IO_WRITE_REQUEST)
	{
		const auto request = reinterpret_cast<driver_request*>(irp->AssociatedIrp.SystemBuffer);

		//get process, attach to it etc, etc

		const auto status = write(request->base_address, request->size, request->buffer);
	}

	...
}