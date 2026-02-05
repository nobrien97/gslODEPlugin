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
        
        ODESystem sys;
        sys._n_pars = info->n_pars;
        sys._n_vars = info->n_vars;
        sys._func = std::function<std::remove_pointer<func>::type>(info->ode_func);
        sys._jac = std::function<std::remove_pointer<jac>::type>(info->ode_jac);
        return sys;
    }
};
