#include <functional>
#include <memory>
#include <vector>
#include "SharedLibrary.h"
#include "ode_api.h"

class ODESystem
{
    public:
        // using ODESystemFunc = std::function<int(double, const double*, double*, void*)>;
        // using ODESystemJac = std::function<int(double, const double*, double*, double*, void*)>;

        inline int n_vars() const { return _n_vars; }
        inline int n_pars() const { return _n_pars; }
    
        func function;
        jac jacobian;
        // void func(double t, const double y[], double f[], void* params)
        // {
        //     _func(t, y, f, params);
        // }

        // void jac(double t, const double y[], double* dfdy, double dfdt[], void* params)
        // {
        //     _jac(t, y, dfdy, dfdt, params);
        // }

        void* par_data()
        {
            return static_cast<void*>(_pars.data());
        }

        ODESystem(func fn, jac jc, int n_vars, int n_pars) : 
            function(fn), jacobian(jc), _n_vars(n_vars), _n_pars(n_pars)
        {
            _pars.reserve(_n_pars);
        };

        ~ODESystem();

    private:
        friend class ODESystemLoader;

        int _n_vars;
        int _n_pars;

        std::vector<double> _pars;

        // ODESystemFunc _func;
        // ODESystemJac _jac;

        std::shared_ptr<SharedLibrary> _library;
};
