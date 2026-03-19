#include "ODESystem.h"
#include "ode_api.h"
#include <type_traits>
#include <stdexcept>
#include <memory>
#include <iostream>
#include "version.h"


/*
    ODESystemLoader
    Purpose: provides a static method to load an ODESystem from a filepath (.dll/.so)
*/
class ODESystemLoader
{
    public:
    static std::unique_ptr<ODESystem> load(const std::string& path)
    {
        using GetODEInfo = const struct ODEInfo* (*)(); 

#ifdef VERBOSE
            std::cout << "Loading ODESystem from path " << path << std::endl;
#endif
        
        std::shared_ptr<SharedLibrary> lib = std::make_shared<SharedLibrary>(path);

        if (lib == nullptr)
        {
            throw std::runtime_error("Unable to load library");
        }

#ifdef VERBOSE
            std::cout << "Getting ODE from library" << path << std::endl;
#endif
        
        GetODEInfo get_fn = lib->GetSymbol<GetODEInfo>("get_ode_system");

        const ODEInfo* info = get_fn();

        // Check if the API Version of the library matches the version used by the solver
        if (info->api_version != ODE_API_VERSION)
        {
            std::cout << "Warning: ODE was built with API version " << 
                info->api_version << " but solver is version " << ODE_API_VERSION << std::endl;
        }
        
        std::unique_ptr<ODESystem> sys(new ODESystem(info->ode_func, info->ode_jac, info->n_vars, info->n_pars, lib));

        return sys;
    }
};
