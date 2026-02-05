#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ODE_API_VERSION 1

typedef int (*func)(
    double t,
    const double* y, 
    double* f, 
    void* params
);

typedef int (*jac)(
    double t,
    const double* y, 
    double* dfdy,
    double* dfdt, 
    void* params
);

struct ODEInfo
{
    int api_version;
    int n_vars;
    int n_pars;
    func ode_func;
    jac ode_jac;
};

const struct ODEInfo* get_ode_system(void);

#ifdef __cplusplus
}
#endif

