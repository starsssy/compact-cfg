#pragma once

#ifndef CCFG_HEADER
#define CCFG_HEADER

#include <cstring>
#include <fstream>
#include <unordered_map>
#include <functional>

// Not used directly by compactcfg, but included because usage depends on having a hash function
#include "./hash/fnv1a32.h" // https://github.com/irfantopal/fnv1a

// Compile time hash
#define CCFG_HASH FNV1A32

// Hash, size, and category type can be changed to fit your needs, although defaults should be sufficient
using HashType_t = uint32_t;
using SizeType_t = unsigned short;
using CategoryType_t = unsigned char; // Type for user categories (Not saved to file, set on var creation)

// User function to determine which categories should be loaded
using ShouldLoadCategory_t = std::function<bool(CategoryType_t)>;

enum ConfigResult_e
{
	CCFG_SUCCESS = 0,
	CCFG_FAILED_TO_OPEN,
	CCFG_FILE_EMPTY,
	CCFG_VERSION_MISMATCH,
};

// Size of bytes stored before each variable's data in file format
#define CCFG_SAVED_DATA_SIZE sizeof(HashType_t) + sizeof(SizeType_t)

// Abstract class for custom bytes handlers
// handlers/ccfg_vector.h <- EXAMPLE CUSTOM VECTOR HANDLER
class BaseCustomBytes // class YourClass : public BaseCustomBytes // Override the bytes functions
{
public:
	BaseCustomBytes() {};
	~BaseCustomBytes() {};

	struct Bytes
	{
		char* data = nullptr;
		SizeType_t size = 0;
	};

	// OVERRIDE THESE VIRTUAL FUNCTIONS IN YOUR CLASS!!!

	// Should return a Bytes class with .data being a malloc()'d block of memory containing
	// what you want it to be saved as
	virtual Bytes ToBytes() = 0;

	// No return, should use the data and data size to set variables in the class accordingly
	virtual void FromBytes(char* data, SizeType_t size) = 0;
};

// Data for a single config variable created with CreateVariable
class ConfigEntry
{
public:
	ConfigEntry() : category{ 0 }, size{ 0 }, use_custom_bytes_handlers{ false }, data{ nullptr } {}
	ConfigEntry(SizeType_t _size, void* _data, CategoryType_t _category, bool _use_custom_bytes_handlers) :
		category{ _category },
		size{ _size },
		use_custom_bytes_handlers{ _use_custom_bytes_handlers },
		data{ _data }
	{}

	~ConfigEntry() {}

	CategoryType_t category = 0;
	SizeType_t size = 0;
	bool use_custom_bytes_handlers = false;
	void* data = nullptr; // User specified and user owned data ptr
};

class CompactConfig
{
public:
	CompactConfig(unsigned short _config_version = 0) : config_version{ _config_version } {}
	~CompactConfig() {}

	ConfigResult_e SaveToFile(const char* filepath);
	ConfigResult_e LoadFromFile(const char* filepath, ShouldLoadCategory_t filter_fn = nullptr);

	void CreateVariable(HashType_t key_hash,
		void* data,
		SizeType_t sz,
		CategoryType_t category,
		bool use_custom_bytes_handlers)
	{
		config_data[key_hash] = ConfigEntry(sz, data, category, use_custom_bytes_handlers);
	}

	template<typename T>
	void CreateVariable(HashType_t key_hash,
		T* data,
		CategoryType_t category,
		bool use_custom_bytes_handlers)
	{
		config_data[key_hash] = ConfigEntry(sizeof(T), (void*)data, category, use_custom_bytes_handlers);
	}

	template<typename T>
	void CreateVariable(HashType_t key_hash,
		T* data,
		const T& default_val,
		CategoryType_t category,
		bool use_custom_bytes_handlers)
	{
		*data = default_val;
		CreateVariable<T>(key_hash, data, category, use_custom_bytes_handlers);
	}

	// Clears variable data, does not clear memory of entries.
	void ClearAllData()
	{
		config_data.clear();
	}

private:
	// File construction/access functions
	void ConfigWrite(std::ofstream& stream, uint32_t key_hash, const ConfigEntry& entry);
	size_t ConfigRead(void* entry_data, ShouldLoadCategory_t filter_fn = nullptr);

	// Saved at the start of the config file to check for mismatches.
	//
	// Should be changed only when previous versions are INCOMPATIBLE with current.
	// Incompatible can mean a variable's type has changed but name hasn't, hash or size types were changed, etc.
	//
	// If the incompatibility is in a variable using a custom handler,
	// it is best practice to change the version of the handler rather than the whole config - See example vector handler
	unsigned short config_version = 0;

	std::unordered_map<HashType_t, ConfigEntry> config_data{};
};

#endif
