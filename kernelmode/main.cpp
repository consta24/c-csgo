#include "log.h"

extern void NTAPI server_thread(void*);

extern "C" NTSTATUS DriverEntry(
	PDRIVER_OBJECT  driver_object,
	PUNICODE_STRING registry_path
)
{
	KeEnterGuardedRegion();

	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	PWORK_QUEUE_ITEM WorkItem = (PWORK_QUEUE_ITEM)ExAllocatePool(NonPagedPool, sizeof(WORK_QUEUE_ITEM));

	ExInitializeWorkItem(WorkItem, server_thread, WorkItem);

	ExQueueWorkItem(WorkItem, DelayedWorkQueue);

	KeLeaveGuardedRegion();

	return STATUS_SUCCESS;
}