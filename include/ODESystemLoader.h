#include "ODESystem.h"
#include "ode_api.h"
#include <type_traits>
#include <memory>

class ODESystemLoader
{
    
    public:
    static std::unique_ptr<ODESystem> load(const std::string& path)
    {

        auto lib = std::make_shared<SharedLibrary>(path);
        using GetODEInfo = const struct ODEInfo* (*)(); 
        auto get_fn = lib->GetSymbol<GetODEInfo>("get_ode_system");

        const ODEInfo* info = get_fn();
        
        std::unique_ptr<ODESystem> sys(new ODESystem(info->ode_func, info->ode_jac, info->n_vars, info->n_pars, lib));
        return sys;
    }
};
