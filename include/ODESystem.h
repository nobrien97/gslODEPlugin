#include <functional>
#include <memory>
#include <vector>
#include <iostream>
#include "SharedLibrary.h"
#include "ode_api.h"
#include "gsl_odeiv2.h"

#if _WIN32
typedef unsigned int uint;
#endif

/*
    ODESystem
    Purpose: defines the structure of an ODE System
*/
class ODESystem
{
    public:
        /// @brief The number of variables
        /// @return a uint number of variables
        inline uint n_vars() const { return _n_vars; }
        /// @brief The number of parameters
        /// @return a uint number of parameters
        inline uint n_pars() const { return _n_pars; }

        /// @brief The ODE Function
        ode_func_t func;
        /// @brief The ODE Jacobian
        ode_jac_t jac;

        /// @brief Pointer to the (first element of the) variable values
        inline double* var_data()
        {
            return _vars.data();
        }

        /// @brief Pointer to the (first element of the) parameter values
        inline double* par_data()
        {
            return _pars.data();
        }

        /// @brief The solution in csv format: time, followed by each variable value
        inline std::string solution()
        {
            return _solution;
        }

        /// @brief Whether a Jacobian has been supplied
        inline bool hasJacobian()
        {
            return jac;
        }


        /// @brief Sets the parameter values
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

        /// @brief Sets the initial variable values
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

        /// @brief Solves the system of equations
        int solve(gsl_odeiv2_driver* d, int max_time, int par_id, double measure_interval);

        ODESystem(ode_func_t fn, ode_jac_t jc, int n_vars, int n_pars, std::shared_ptr<SharedLibrary> lib) : 
            func(fn), jac(jc), _n_vars(n_vars), _n_pars(n_pars), _library(lib)
        {
            _pars.reserve(_n_pars);
            _vars.reserve(_n_vars);
        };

        ~ODESystem() = default;

    private:
        uint _n_vars;
        uint _n_pars;

        std::vector<double> _pars;
        std::vector<double> _vars;

        std::string _solution;

        std::shared_ptr<SharedLibrary> _library;
};
