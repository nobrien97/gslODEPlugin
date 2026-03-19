#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _WIN32
    #define ODE_EXPORT extern "C" __declspec( dllexport )
    #define ODE_CALL __cdecl
#else
    #define ODE_EXPORT extern "C"
    #define ODE_CALL
 #endif


#define ODE_API_VERSION 1

typedef int (ODE_CALL *ode_func_t)(
    double t,
    const double* y, 
    double* f, 
    void* params
);

typedef int (ODE_CALL *ode_jac_t)(
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

typedef const ODEInfo* (ODE_CALL *get_ode_system_fn)(void);

#ifdef __cplusplus
}
#endif

