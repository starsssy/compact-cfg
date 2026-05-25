#include "../src/compactcfg.h"


// This is a very basic example showing how to create variables, and save/load.
// Manually adding each variable like this will be very annoying,
// so it is recommended that you create your own compile time helpers for creating variables.
//
// See the full implementation example for reference, or just take the example entirely

CompactConfig cfg = {};
int var = 0;
int var2 = 0;

int main()
{
	var = var2 = 0;

	// See full usage example for usage tips - using your own define to create variables will make creation much easier.
	cfg.CreateVariable(CCFG_HASH("var"), &var, 123, 0, false);
	cfg.CreateVariable(CCFG_HASH("just needs to be a unique variable name"), &var2, 0, false);

	// var is now 123 after being set to the default value specified
	// var2 is still 0

	// Change then save to file
	var = var2 = 111;
	cfg.SaveToFile("filename.fileextension");


	// Set to 0 then load old values we just saved
	var = var2 = 0;
	cfg.LoadFromFile("filename.fileextension");

	// Both vars are now back to 111 after loading the saved file

	return 0;
}