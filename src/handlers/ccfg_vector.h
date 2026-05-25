#include "../compactcfg.h"

// Example vector handler for CompactConfig

#include <vector>

template <typename T, unsigned short class_ver = 0>
class VectorData : BaseCustomBytes
{
public:
	// Used for in-code access, then saved to and from bytes with our functions
	std::vector<T> vec = {};

	VectorData() {}
	VectorData(const std::vector<T>& _vec) : vec{_vec} {}

	// Helpers to make this class act like an std::vector
	operator std::vector<T>& () const
	{
		return vec;
	}

	VectorData<T>& operator=(const std::vector<T>& other)
	{
		vec = other;
		return *this;
	}

	VectorData<T>& operator=(const VectorData<T>& other)
	{
		vec = other.vec;
		return *this;
	}

	T& operator[](const size_t& index) const
	{
		return vec[index];
	}

	T& at(const size_t& index) const
	{
		return vec.at(index);
	}

	// Size types should be SizeType_t for consistency

	/* BYTES FORMAT:
	* unsigned short class_ver; <-- (Size: 2) Specified version of the class
	* ^ Should be changed if the contents of the T type are changed to avoid incorrect copying
	* ^ Only necessary if the type is changed, but the variable name is not
	*
	* T vec[?]; <-- (Size: ? * sizeof(T)) Bytes of however many members are stored
	*/

private: // Private here but not in the base class. Still accessible through the base class
	virtual Bytes ToBytes() override
	{
		SizeType_t data_sz = (vec.size() * sizeof(T)) + sizeof(class_ver);

		char* bytes = (char*)malloc(data_sz);
		if (!bytes)
			return { nullptr, 0 };

		*(unsigned short*)(bytes) = class_ver;

		// Save vector data to memory, accounting for offset from class version
		if (!vec.empty())
			memcpy(bytes + sizeof(class_ver), vec.data(), vec.size() * sizeof(T));

		return { bytes, data_sz }; // Caller takes ownership of bytes memory
	}

	virtual void FromBytes(char* data, SizeType_t size) override
	{
		vec.clear();

		unsigned short data_ver = *(unsigned short*)data;
		if (class_ver != data_ver)
			return;

		size_t data_sz = size - sizeof(class_ver);
		size_t entry_count = data_sz / sizeof(T);

		char* vec_data = data + sizeof(class_ver);

		for (int i = 0; i < entry_count; i++)
		{
			vec.push_back(*(T*)(vec_data + (sizeof(T) * i)));
		}
	}
};
