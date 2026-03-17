#include <string>
#include <stdexcept>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class SharedLibrary
{
private:
    void* _handle;

    /// @brief Opens a handle to a library
    /// @returns a void* to the library
    inline void* OpenHandle(const std::string& path) 
    {
#if _WIN32
	void* library(LoadLibraryA(path.c_str())); // HMODULE/HINSTANCE are void*

	if (!library) 
	{
		return nullptr;
	}

	return library;
#else
    void* library = dlopen(path.c_str(), RTLD_NOW); // Both Linux and Mac use dlopen

	if (library == NULL)
	{
		return nullptr;
	}
    // std::cout << "Loaded library at " << path << std::endl;

	return library;
#endif
    }

    /// @brief Closes a handle to a SharedLibrary (called in destructor)
    inline void CloseHandle()
    {
#if _WIN32
    FreeLibrary((HMODULE)_handle); // TODO: cast to HMODULE might not be necessary

#else
    int closeError = dlclose(_handle);

    if (closeError)
    {
        throw std::runtime_error("Unable to unload library, error code " + std::to_string(closeError));
    }

    // std::cout << "Closed library at " << reinterpret_cast<size_t>(_handle) << std::endl;

    return; 
#endif
    }

public:

    /// @brief Loads a new SharedLibrary from a filepath
    /// @param path The filepath to the .dll (Windows) or .so (Linux/Mac) shared library
    explicit SharedLibrary(const std::string& path)
        : _handle(OpenHandle(path))
    {
        if (_handle == nullptr)
        {
            // Throw an error
            throw std::runtime_error("Unable to find library at path " + path);
        }
    };

    // Destructor automatically closes the handle 
    ~SharedLibrary() 
    {
        if (_handle)
        {
            CloseHandle();
        }
    };

    // Remove copy constructor and reassignment
    SharedLibrary(const SharedLibrary&) = delete;
    SharedLibrary& operator=(const SharedLibrary&) = delete;

    // Allow for moving only
    SharedLibrary(SharedLibrary&& other) noexcept
        : _handle(other._handle)
        {
            other._handle = nullptr;
        }

    template<typename F>
    inline F GetSymbol(const std::string& name) const 
    {
#if _WIN32
        auto symbol = GetProcAddress((HINSTANCE)_handle, name.c_str());
        if (!symbol)
        {
            throw std::runtime_error("Unable to find symbol in library");
        }
        return reinterpret_cast<F>(symbol);
#else
        auto symbol = dlsym(_handle, name.c_str());

        if (!symbol)
        {
            throw std::runtime_error("Unable to find symbol '" + name + "' in library");
        }
#endif
        return reinterpret_cast<F>(symbol);
    }

};