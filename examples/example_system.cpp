#include "../src/compactcfg.h"
#include "../src/handlers/ccfg_vector.h"

CompactConfig ccfg = {};

class ConfigSystem
{
public:
	ConfigSystem() {}
	~ConfigSystem()
	{
		// Clear ccfg data when config vars are deallocated
		// Dont make multiple instances of this ConfigSystem class, otherwise when one calls its destructor it will clear the other's data from ccfg too
		ccfg.ClearAllData();
	}

private:
	// This function is used to allow for one line registration of config vars during declaration
	template<typename T>
	static T RegVar(T* item, HashType_t name, T val, CategoryType_t category = 0, bool use_custom_bytes_handlers = false)
	{
		ccfg.CreateVariable<T>(name, item, category, use_custom_bytes_handlers);
		return val;
	}

public:
	// Variables will be registered on creation of this class - All within the line they're declared and with no extra code
	#define REG_VAR(type, name, ...) type name = RegVar<type>(&name, CCFG_HASH(#name), __VA_ARGS__)

	REG_VAR(bool, good_name, false);
	REG_VAR(float, float_var, 123.f);
	REG_VAR(int, asd123qwe, 123456);

	REG_VAR(VectorData<float>, vector_custom_var, std::vector<float>{ 1.f, 2.f, 3.f }, 0, true);

	// Variables can also be within other classes/structs, which can help with organization
	struct
	{
		REG_VAR(bool, asdasd, true);
	} example_area;
};

ConfigSystem cfgsys = {};
int main()
{
	// Now your normal C++ class members function as config entries, with no need to add more code for each variable type!
	// (Assuming it doesnt involve pointers, in which case you should make a custom handler - see the vector handler example)

	// Simple way to implement resetting to defaults
	ccfg.SaveToFile("defaults"); // Save before changes are made
	ccfg.LoadFromFile("defaults"); // Then just load defaults file to reset

	printf("%d", cfgsys.asd123qwe);
	printf("%d", cfgsys.example_area.asdasd);

	for (auto& i : cfgsys.vector_custom_var.vec)
		printf("%.0f", i);
}