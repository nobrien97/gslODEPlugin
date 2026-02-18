#include <functional>
#include <memory>
#include <vector>
#include <iostream>
#include "SharedLibrary.h"
#include "ode_api.h"
#include "gsl_odeiv2.h"

class ODESystem
{
    public:
        // using ODESystemFunc = std::function<int(double, const double*, double*, void*)>;
        // using ODESystemJac = std::function<int(double, const double*, double*, double*, void*)>;

        inline uint n_vars() const { return _n_vars; }
        inline uint n_pars() const { return _n_pars; }
    
        ode_func_t func;
        ode_jac_t jac;

        inline double* par_data()
        {
            return _pars.data();
        }

        inline double* var_data()
        {
            return _vars.data();
        }

        inline std::string solution()
        {
            return _solution;
        }

        inline bool hasJacobian()
        {
            return jac;
        }

        inline void setParData(const std::vector<double>& newData)
        {
            if (newData.size() != n_pars())
            {
                std::cout << "ODESystem::setParData: Replacement data must have the same length as existing data" << std::endl;
                return;
            }

            for (int i = 0; i < newData.size(); ++i)
            {
                _pars[i] = newData[i];
            }

            return;
        }

        inline void setInitVarData(const std::vector<double>& newData)
        {
            if (newData.size() != n_vars())
            {
                std::cout << "ODESystem::setInitVarData: Replacement data must have the same length as existing data" << std::endl;
                return;
            }

            for (int i = 0; i < newData.size(); ++i)
            {
                _vars[i] = newData[i];
            }

            return;

        }

        int solve(gsl_odeiv2_driver* d, int max_time, int par_id, double measure_interval);

        ODESystem(ode_func_t fn, ode_jac_t jc, int n_vars, int n_pars, std::shared_ptr<SharedLibrary> lib) : 
            func(fn), jac(jc), _n_vars(n_vars), _n_pars(n_pars), _library(lib)
        {
            _pars.reserve(_n_pars);
            _vars.reserve(_n_vars);
        };

        ~ODESystem() = default;

    private:
        friend class ODESystemLoader;

        uint _n_vars;
        uint _n_pars;

        std::vector<double> _pars;
        std::vector<double> _vars;

        std::string _solution;

        std::shared_ptr<SharedLibrary> _library;
};
