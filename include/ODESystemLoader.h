#include "ODESystem.h"
#include "ode_api.h"
#include <type_traits>

class ODESystemLoader
{
    
    public:
    static ODESystem load(const std::string& path)
    {

        auto lib = std::make_shared<SharedLibrary>(path);
        using GetODEInfo = const struct ODEInfo* (*)(); 
        auto get_fn = lib->GetSymbol<GetODEInfo>("get_ode_system");

        const ODEInfo* info = get_fn();
        
        ODESystem sys(info->ode_func, info->ode_jac, info->n_vars, info->n_pars);
        return sys;
    }
};
