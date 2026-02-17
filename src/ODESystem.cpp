#include "ODESystem.h"
#include <sstream>
#include <iostream>
#include "gsl_errno.h"
#include "gsl_odeiv2.h"

int ODESystem::solve(gsl_odeiv2_driver* d, int max_time, int par_id, double measure_interval)
{
    double t = 0;
    int status = 0;
    double* y = var_data();

    std::stringstream ss;

    int max_iter = max_time / measure_interval;
    for (int i = 0; i <= max_iter; ++i)
    {
        double ti = i * measure_interval;
        status = gsl_odeiv2_driver_apply(d, &t, ti, y);
        if (status != GSL_SUCCESS)
        {
            std::cout << "Stepper function " << d->s->type->name << " failed on step " << i << " - error " << gsl_strerror(status) << "\n";
            break;
        }
        
        // write solution
        ss << par_id << "," << ti << ",";

        for (size_t j = 0; j < n_vars() - 1; ++j)
        {
            ss << y[j] << ",";
        }
        
        ss << y[n_vars() - 1] << "\n";

        _solution += ss.str();

        // Clear the stringstream for the next iteration
        ss.str(std::string());
    }

    return GSL_SUCCESS;
}