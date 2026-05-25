# Compact Config
Compact Config is a cross platform C++ config system focused on small file sizes and speed.

Each config entry is stored in file as a name hash (4 bytes), a size (2 bytes), and the bytes of the entry itself. This keeps file sizes extremely small compared to using plaintext options like JSON or INI, while still retaining full functionality and control.

I originally made this a few years ago for use in another project, but only recently cleaned it up and separated it from the other project.

## Usage
Include both ```compactcfg.h``` and ```compactcfg.cpp``` in your project, then include ```compactcfg.h``` wherever you intend to use it.

See examples [basic_usage.cpp](https://github.com/starsssy/compact-cfg/blob/master/examples/basic_usage.cpp) for basic usage, and [example_system.cpp](https://github.com/starsssy/compact-cfg/blob/master/examples/example_system.cpp) for a full implementation (recommended).

## Custom Handlers
For config variables that store data in pointers rather than within the memory of the base class (For example a vector), compact-cfg uses custom handlers.

A custom handler is a class that is derived from the ```BaseCustomBytes``` class and overrides two virtual functions, one for writing to bytes, and one for reading from bytes.

USE [ccfg_vector.h](https://github.com/starsssy/compact-cfg/blob/master/src/handlers/ccfg_vector.h) AS A REFERENCE!!! (and probably also take it and use it)

```cpp
class ExampleHandler : BaseCustomBytes
{
public:
	...
private:
	virtual Bytes ToBytes() override { ... }
	virtual void FromBytes(char* data, SizeType_t size) override { ... }
}
```

The ToBytes function needs to return a BaseCustomBytes::Bytes struct containing a malloc()'d block of memory with the desired data inside, along with the size of the data.

The FromBytes function takes in a block of memory and its size, and needs to interpret that memory and set appropriate variables with it.

### Example:
In a custom vector handler, the ToBytes function would return a Bytes struct that looks like this: 

```{ (Ptr to malloc()'d memory with vector data inside), (Size in bytes of vector data) }```

And the FromBytes function would take in the block of memory and its size, then use it to set the vector data:

```cpp
int count = size / sizeof(T);
for (int i = 0; i < count; ++i) { set vector data, etc. }
```

