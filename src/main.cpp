#include "ode_api.h"
#include "ODESystemLoader.h"
#include "getopt.h"
#include "version.h"
#include "rapidcsv.h"
#include "gsl/gsl_odeiv2.h"
#include <iostream>



#define no_arg 0
#define arg_required 1
#define arg_optional 2

    const struct option longopts[] =
    {
        { "input",    required_argument,    0,  'i' },
        { "output",   required_argument,    0,  'o' },
        { "system",   required_argument,    0,  's' },
        { "help",           no_argument,    0,  'h' },
        { "threads",  required_argument,    0,  't' }, 
        { "optimum",  required_argument,    0,  'p' },
        { "width",    required_argument,    0,  'w' },
        { "opt_file", required_argument,    0,  'O' },
        { "useGSL",         no_argument,    0,  'G' },
        {0,0,0,0}
    };


void doHelp(char* appname) {
    std::fprintf(stdout,
    "ODE Plugin test (V%i.%i.%i): A program for testing loading dynamic libraries for GSL ODE solving.\n"
    "\n"
    "Usage: %s [OPTION]...\n"
    "Example: %s -h\n"
    "\n"
    "-h             Print this help manual.\n"
    "\n"
    "-i             Specify input plugin path.\n"
    "               Example: -i /path/to/file.so\n"
    "               Input library should consist of a single class implementing all fields in 'ode_api.h'\n"
    "-p             Specify input parameter path.\n"
    "               Example: -p /path/to/file.csv\n"
    "               Input .csv should have a column per parameter, each row being a parameter combination.\n"
    "               Number of columns must match the number of parameters defined in the plugin .so\n"
    "\n"
    "\n",
    ODEPluginTest_VERSION_MAJOR,
    ODEPluginTest_VERSION_MINOR,
    ODEPluginTest_VERSION_PATCH,
    appname,
    appname
    );
}


int main(int argc, char *argv[])
{
    const struct option voptions[] = 
    {
        { "help",       no_argument,        0,  'h' },
        { "input",      required_argument,  0,  'i' },
        { "parameters", required_argument,  0,  'p' },
        {0,0,0,0}
    };

    int opt_idx = 0;
    int options;

    std::string plugin_path;

    rapidcsv::Document doc;

    const gsl_odeiv2_step_type* stepper = gsl_odeiv2_step_msbdf;
    double measure_interval = 0.1;
    double time = 10.0;
    int benchmark = 0;
    double a_err = 1e-10;
    double r_err = 1e-6;




    while (options != -1)
    {
        options = getopt_long(argc, argv, "hi:", voptions, &opt_idx);
    
        switch (options)
        {
        case 'h':
            doHelp(argv[0]);
            return 0;

        case 'i':
            // Read library
            plugin_path = (std::string)optarg;
            continue;
        case 'p':
            // Read parameter csv
            doc.Load(((std::string)optarg), rapidcsv::LabelParams(-1, -1));
            continue;
        case -1:
            break;
        }
    
    }

    // Check the path is valid
    if (plugin_path.empty()) {
        std::cout << "Plugin path is empty" << std::endl;
        return 1;
    }


    // Attempt to load the plugin
    ODESystem system = ODESystemLoader::load(plugin_path);

    gsl_odeiv2_system gslSys = {system.function, system.jacobian, system.n_vars(), system.par_data()};

    gsl_odeiv2_driver* d = gsl_odeiv2_driver_alloc_y_new(&gslSys, stepper,
                            1e-10, a_err, r_err);

    
    // Setup GSL to solve the system
    for (int i = 0; i < doc.GetRowCount(); ++i)
    {
        const std::vector<double> parameters = doc.GetRow<double>(i);


    }


    return 0;
}
