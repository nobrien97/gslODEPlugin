#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define ODE_API_VERSION 1

typedef int (*ode_func_t)(
    double t,
    const double* y, 
    double* f, 
    void* params
);

typedef int (*ode_jac_t)(
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
    ode_func_t ode_func;
    ode_jac_t ode_jac;
};

const struct ODEInfo* get_ode_system(void);

#ifdef __cplusplus
}
#endif

