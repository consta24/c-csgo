#pragma once
#include "stdint.h"

constexpr auto packet_magic = 0x12345568;
constexpr auto server_ip = 0x7F000001; // 127.0.0.1
constexpr auto server_port = 28055;

enum class PacketType
{
	packet_copy_memory,
	packet_get_client_address,
	packet_get_engine_address,
	packet_clean_piddbcachetable,
	packet_clean_mmunloadeddrivers,
	packet_hwid_spoofing,
	packet_completed
};

struct PacketCopyMemory
{
	uint32_t dest_process_id;
	uint64_t dest_address;

	uint32_t src_process_id;
	uint64_t src_address;

	uint32_t size;
};

struct PacketGetClientAddress
{
	uint32_t process_id;
};

struct PacketGetEngineAddress
{
	uint32_t process_id;
};

struct PacketCleanPiDDBCacheTable {
};

struct PacketCleanMMUnloadedDrivers {
};

struct PackedCompleted
{
	uint64_t result;
};

struct PacketHeader
{
	uint32_t   magic;
	PacketType type;
};

struct Packet
{
	PacketHeader header;
	union
	{
		PacketCopyMemory	 copy_memory;
		PacketGetClientAddress get_client_address;
		PacketGetEngineAddress get_engine_address;
		PacketCleanPiDDBCacheTable clean_piddbcachetable;
		PacketCleanMMUnloadedDrivers clean_mmunloadeddrivers;
		PackedCompleted		 completed;
	} data;
};