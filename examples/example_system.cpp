#include "../src/compactcfg.h"
#include "../src/handlers/ccfg_vector.h"

CompactConfig ccfg = {};

class ConfigSystem // Singleton
{
public:
	ConfigSystem() {}
	~ConfigSystem()
	{
		// Clear ccfg data when config vars are deallocated
		// Be sure to only have 1 instance of the ConfigSystem class for each ccfg instance because this will clear ALL data
		ccfg.ClearAllData();
	}

private:
	// This function is used to allow for one line registration of config vars during declaration
	template<typename T>
	static T RegVar(T val, T* item, HashType_t name, CategoryType_t category = 0, bool use_custom_bytes_handlers = false)
	{
		ccfg.CreateVariable<T>(name, item, category, use_custom_bytes_handlers);
		return val;
	}

public:
	// Variables will be registered on creation of this class - All within the line they're declared and with no extra code
	#define REG_VAR(type, name, val) type name = RegVar<type>(val, &name, CCFG_HASH(#name))
	#define REG_VAR_CUSTOM(type, name, val) type name = RegVar<type>(val, &name, CCFG_HASH(#name), 0, true)

	REG_VAR(bool, good_name, false);
	REG_VAR(float, float_var, 123.f);
	REG_VAR(int, asd123qwe, 123456);

	REG_VAR_CUSTOM(VectorData<float>, vector_custom_var, {});
};

ConfigSystem cfgsys = {};
int main()
{
	// Now your normal c++ class members function as config entries, with no need to add more code for each variable type!
	// (Assuming it doesnt involve pointers, in which case you should make a custom handler - see the vector handler example)

	// Simple way to implement resetting to defaults
	ccfg.SaveToFile("defaults"); // Save before changes are made
	ccfg.LoadFromFile("defaults"); // Then just load defaults file to reset

	printf("%i", cfgsys.asd123qwe);
}