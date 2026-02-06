#include "ode_api.h"
#include "math.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_errno.h>

static int lor_ode(double t, const double* y, double* dydt, void* params)
{
    // Parameters
    (void)(t);
    double* p = (double*)params;
    double sigma = p[0];
    double rho = p[1];
    double beta = p[2];

    // Equations
    dydt[0] = sigma * (y[1] - y[0]);
    dydt[1] = y[0] * (rho - y[2]) - y[1];
    dydt[2] = y[0] * y[1] - beta * y[2];    
    return GSL_SUCCESS;
}

static int lor_jac(double t, const double* y, double* dfdy, double* dfdt, void* params)
{
    (void)(t); /* avoid unused parameter warning */
    double* p = (double*)params;
    double sigma = p[0];
    double rho = p[1];
    double beta = p[2];

    gsl_matrix_view dfdy_mat = gsl_matrix_view_array (dfdy, 3, 3);
    gsl_matrix * m = &dfdy_mat.matrix;
    gsl_matrix_set (m, 0, 0, -sigma);
    gsl_matrix_set (m, 0, 1, sigma);
    gsl_matrix_set (m, 0, 2, 0.0);
    
    gsl_matrix_set (m, 1, 1, rho - y[2]);
    gsl_matrix_set (m, 1, 1, -1.0);
    gsl_matrix_set (m, 1, 2, -y[0]);

    gsl_matrix_set (m, 2, 0, y[1]);
    gsl_matrix_set (m, 2, 1, y[0]);
    gsl_matrix_set (m, 2, 2, -beta);

    dfdt[0] = 0.0;
    dfdt[1] = 0.0;
    dfdt[2] = 0.0;
    return GSL_SUCCESS;
}


extern "C"
const ODEInfo* get_ode_system(void) 
{
    static ODEInfo odesys = {ODE_API_VERSION, 3, 3, lor_ode, lor_jac};
    return &odesys;
}