#include "ode_api.h"
#include "math.h"
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_errno.h>

static int nar_ode(double t, const double* y, double* dydt, void* params)
{
    // Params is length 7
    double* p = (double*)params;

    // Extract parameters for clarity
    double tstart = p[0];
    double tend = p[1];
    double b = p[2];

    // Get n to do the power calculations now
    double n = p[5];

    double KXZn = pow(p[3], n); // Raise to power of n
    double KZn = pow(p[4], n);
    double a = p[6];
    int X = t >= tstart && t < tend; 

    double Xn = pow(X, n);
    double Zn = pow(y[0], n);

    dydt[0] = b * (Xn / (KXZn + Xn)) * (KZn / (KZn + Zn)) - a * y[0];
    return GSL_SUCCESS;
}

static int nar_jac(double t, const double* y, double* dfdy, double* dfdt, void* params)
{
    gsl_matrix_view dfdy_mat = gsl_matrix_view_array (dfdy, 1, 1);
    gsl_matrix *m = &dfdy_mat.matrix;

    // Extract parameters for clarity
    double* p = (double*)params;
    double tstart = p[0];
    double tend = p[1];
    double b = p[2];

    // Get n to do the power calculations now
    double n = p[5];

    double KXZn = pow(p[3], n); // Raise to power of n
    double KZn = pow(p[4], n);
    double a = p[6];
    int X = t > tstart && t <= tend; 

    double Xn = pow(X, n);
    double Zn = pow(y[0], n);
       
    gsl_matrix_set (m, 0, 0, -b * (Xn / (KXZn + Xn)) * (KZn * n * pow(y[0], n-1)) / ((KZn + Zn) * (KZn + Zn)) - a);

    dfdt[0] = 0.0;

    return GSL_SUCCESS;

}


extern "C"
const ODEInfo* get_ode_system(void) 
{
    static ODEInfo odesys = {ODE_API_VERSION, 1, 7, nar_ode, nar_jac};
    return &odesys;
}