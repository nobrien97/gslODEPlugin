#include <string>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

class SharedLibrary
{
private:
    void* _handle;
    inline void* OpenHandle(const std::string& path) 
    {
#if _WIN32
	lib_ptr<void> library(LoadLibraryA(path->c_str()));

	if (!library) 
	{
		return nullptr;
	}

	return library;
#endif
    void* library = dlopen(path.c_str(), RTLD_NOW);

	if (library == NULL)
	{
		return nullptr;
	}

	return library;
 
    }

    inline void CloseHandle()
    {
#if _WIN32
    FreeLibrary((HINSTANCE)_handle);

	return;
#endif
    dlclose(_handle);
    return; 
    }

public:
    explicit SharedLibrary(const std::string& path)
        : _handle(OpenHandle(path))
    {
        if (_handle == nullptr)
        {
            // Throw an error
            throw std::runtime_error("Unable to find library at path");
        }
    };

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
        auto symbol = GetProcAddress((HINSTANCE)handle_, name->c_str());
        if (!symbol)
        {
            throw std::runtime_error("Unable to find symbol in library");
        }
        return reinterpret_cast<F>(symbol);
#endif
        auto symbol = dlsym(handle_, name->c_str());

        if (!symbol)
        {
            throw std::runtime_error("Unable to find symbol in library");
        }

        return reinterpret_cast<F>(symbol);
    }

};