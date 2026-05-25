#include "compactcfg.h"

ConfigResult_e CompactConfig::SaveToFile(const char* filepath)
{
	std::ofstream stream(filepath, std::ios::binary | std::ios::ate);
	if (!stream.is_open() || stream.fail())
	{
		stream.close();
		return CCFG_FAILED_TO_OPEN;
	}

	// Save config_version to start of file - see notes above variable for info
	stream.write((const char*)&config_version, sizeof(config_version));

	for (auto& entry : config_data)
		ConfigWrite(stream, entry.first, entry.second);

	stream.close();

	return CCFG_SUCCESS;
}

ConfigResult_e CompactConfig::LoadFromFile(const char* filepath, ShouldLoadCategory_t filter_fn /*= nullptr*/)
{
	std::ifstream stream(filepath, std::ios::binary | std::ios::ate); // std::ios::ate - open and seek the end of the file
	if (!stream.is_open() || stream.fail())
	{
		stream.close();
		return CCFG_FAILED_TO_OPEN;
	}

	size_t file_size = stream.tellg();
	if (file_size == 0)
		return CCFG_FILE_EMPTY;

	char bytes[file_size];
	size_t bytes_read = 0;

	stream.seekg(0, std::ios::beg);
	stream.read(bytes, file_size);

	// Check for config_version mismatch - see notes above variable for info
	unsigned short file_version = *(unsigned short*)bytes;
	bytes_read += sizeof(file_version);
	if (config_version != file_version)
		return CCFG_VERSION_MISMATCH;

	while (bytes_read < file_size)
		bytes_read += ConfigRead(bytes + bytes_read, filter_fn);

	return CCFG_SUCCESS;
}

void CompactConfig::ConfigWrite(std::ofstream& stream, HashType_t key, const ConfigEntry& entry)
{
	if (entry.use_custom_bytes_handlers)
	{
		BaseCustomBytes::Bytes custom_data = ((BaseCustomBytes*)entry.data)->ToBytes();
		if (custom_data.data)
		{
			stream.write((const char*)&key, sizeof(HashType_t));
			stream.write((const char*)&custom_data.size, sizeof(SizeType_t));
			stream.write((const char*)custom_data.data, custom_data.size);

			free(custom_data.data);
		}
	}
	else
	{
		stream.write((const char*)&key, sizeof(HashType_t));
		stream.write((const char*)&entry.size, sizeof(SizeType_t));
		stream.write((const char*)entry.data, entry.size);
	}
}


size_t CompactConfig::ConfigRead(void* entry_data, ShouldLoadCategory_t filter_fn/* = nullptr*/)
{
	HashType_t key = *(HashType_t*)entry_data;
	SizeType_t data_sz = *(SizeType_t*)((uintptr_t)entry_data + sizeof(HashType_t));

	if (config_data.contains(key))
	{
		auto& entry = config_data[key];

		// If sizes mismatch, skip reading this and move onto next
		if (entry.size != data_sz)
			return CCFG_SAVED_DATA_SIZE + data_sz;

		if (!filter_fn || filter_fn(entry.category))
		{
			if (entry.use_custom_bytes_handlers)
				// Call the user's custom handler virtual func
				((BaseCustomBytes*)entry.data)->FromBytes((char*)((uintptr_t)entry_data + CCFG_SAVED_DATA_SIZE), data_sz);

			else
				memcpy(entry.data, (void*)((uintptr_t)entry_data + CCFG_SAVED_DATA_SIZE), data_sz);
		}
	}

	return CCFG_SAVED_DATA_SIZE + data_sz; // Return how much we just read
}
