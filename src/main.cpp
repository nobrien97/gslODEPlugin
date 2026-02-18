#include "ode_api.h"
#include "ODESystemLoader.h"
#include "getopt.h"
#include "version.h"
#include "rapidcsv.h"
#include <iostream>
#include <memory>


#define no_arg 0
#define arg_required 1
#define arg_optional 2

const struct option longopts[] =
{
    { "input",          required_argument,    0,  'i' },
    { "output",         required_argument,    0,  'o' },
    { "parameters",     required_argument,    0,  'p' },
    { "help",           no_argument,          0,  'h' },
    { "initial",        required_argument,    0,  'n' },
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
    "               Example: -i /path/to/file.so OR -i /path/to/file.csv\n"
    "               If a .csv is mentioned, each row should be a path to a different library.\n"
    "               The number of rows must match the number of rows in the parameter/input files.\n"
    "               Input library should consist of a single class implementing all fields in 'ode_api.h'\n"
    "-o             Specify output path.\n"
    "               Example -o /path/to/output.csv\n"
    "-p             Specify input parameter path.\n"
    "               Example: -p /path/to/file.csv\n"
    "               Input .csv should have a column per parameter, each row being a parameter combination.\n"
    "               Number of columns must match the number of parameters defined in the plugin .so\n"
    "-n             Specify initial values file path.\n"
    "               Example: -n /path/to/file.csv\n"
    "               Input .csv should have a column per variable\n"
    "               Number of rows must match the number of parameter combinations.\n"
    "               Number of columns must match the number of variables for the system.\n"
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
        { "input",       required_argument,  0,  'i' },
        { "output",      required_argument,  0,  'o' },
        { "parameters",  required_argument,  0,  'p' },
        { "help",        no_argument,        0,  'h' },
        { "initial",     required_argument,  0,  'n' },
        {0,0,0,0}
    };

    int opt_idx = 0;
    int options;

    std::string plugin_path;
    std::string output_path = "./";

    rapidcsv::Document parameterDoc;
    rapidcsv::Document initValsDoc;
    rapidcsv::Document pluginDoc;

    const gsl_odeiv2_step_type* stepper = gsl_odeiv2_step_msbdf;
    double measure_interval = 0.1;
    double time = 10.0;
    double a_err = 1e-10;
    double r_err = 1e-6;
    double step_size = 0.01;
    


    while ((options = getopt_long(argc, argv, "hi:p:o:n:", voptions, &opt_idx)) != -1)
    {
        switch (options)
        {
        case 'h':
            doHelp(argv[0]);
            return 0;

        case 'i':
            // Read library
            plugin_path = (std::string)optarg;
            continue;
        case 'o':
            // Output path
            output_path = (std::string)optarg;
            continue;
        case 'p':
            // Read parameter csv
            parameterDoc.Load(((std::string)optarg), rapidcsv::LabelParams(-1, -1));
            continue;
        case 'n':
            // Read initial values
            initValsDoc.Load(((std::string)optarg), rapidcsv::LabelParams(-1, -1));
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

    bool pluginPathSupplied = false; 

    // Check if the path is to a document
    std::string::size_type idx = plugin_path.rfind(".csv");
    if (idx != std::string::npos)
    {
        pluginDoc.Load(plugin_path, rapidcsv::LabelParams(-1, -1));
        pluginPathSupplied = true;
    }

    // Check parameters exist
    if (!parameterDoc.GetRowCount())
    {
        std::cout << "Parameter data cannot be empty." << std::endl;
        return 1;
    }

    // Check initial values exist
    if (!initValsDoc.GetRowCount())
    {
        std::cout << "Initial values data cannot be empty." << std::endl;
        return 1;
    }

    // Check that pluginDoc is valid if supplied
    if (pluginPathSupplied && !pluginDoc.GetRowCount())
    {
        std::cout << "Plugin data invalid." << std::endl;
        return 1;
    }

    // Setup output
    std::vector<std::unique_ptr<std::string>> result(parameterDoc.GetRowCount());

    std::vector<std::unique_ptr<ODESystem>> systems;
    
    // Fill systems
    for (int i = 0; i < pluginDoc.GetRowCount(); ++i)
    {
        std::vector<std::string> systemPath = pluginDoc.GetRow<std::string>(i);
        // Attempt to load the plugin
        systems.emplace_back(ODESystemLoader::load(systemPath[0]));
    }

    // Handle single case
    if (!pluginPathSupplied)
    {
        systems.emplace_back(ODESystemLoader::load(plugin_path));
    }

    // Check systems vector is properly filled
    if (systems.size() != 1 && systems.size() != parameterDoc.GetRowCount())
    {
        std::cout << "Invalid number of systems for parameter inputs." << std::endl;
        return 1;
    }
    
    // Parameter id for the row
    uint par_id = 0;
    

    // Setup GSL to solve the system
    for (int i = 0; i < parameterDoc.GetRowCount(); ++i)
    {
        // Get parameters from this row
        const std::vector<double> parameters = parameterDoc.GetRow<double>(i);
        
        // Get initial values
        const std::vector<double> initVals = initValsDoc.GetRow<double>(i);
        
        // Setup the ODE system for this iteration
        ODESystem* system = systems[0].get();
        if (pluginPathSupplied)
        {
            system = systems[i].get();
        }

        system->setParData(parameters);
        system->setInitVarData(initVals);

        // GSL ODE system
        gsl_odeiv2_system gslSys;        
        
        gslSys.function = system->func;
        gslSys.jacobian = system->jac;
        gslSys.params = system->par_data();
        gslSys.dimension = system->n_vars();

        gsl_odeiv2_driver* d = gsl_odeiv2_driver_alloc_y_new(&gslSys, stepper, 
            1e-10, a_err, r_err);
        
        // Solve
        int solve_error = system->solve(d, time, par_id++, measure_interval);

        // Check for failure
        if (solve_error)
        {
            return 1;
        }

        // Reset the driver/state
        //gsl_odeiv2_driver_reset(d);
        gsl_odeiv2_driver_free(d);
        
        // Write result to output
        result[i] = std::unique_ptr<std::string>(new std::string(system->solution()));
    }

    // Write to file
    std::ofstream file;
    file.open(output_path);

    for (int i = 0; i < result.size(); ++i)
    {
        file << (*result[i]);
    }

    file.close();

    return 0;
}
