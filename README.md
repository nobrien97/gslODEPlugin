# GSL ODE Plugin test

Test program for ODE definitions via dynamic library linking.
Sets up an interface and simple API for custom ODE functions
to be determined in a separate library and loaded at runtime.

## Manual
ODE Plugin test (V0.0.1): A program for testing loading dynamic libraries for GSL ODE solving.

Usage: ./ODEPluginTest [OPTION]...
Example: ./ODEPluginTest -h

-h             Print this help manual.

-i             Specify input plugin path.
               Example: -i /path/to/file.so OR -i /path/to/file.csv
               If a .csv is mentioned, each row should be a path to a different library.
               The number of rows must match the number of rows in the parameter/input files.
               Input library should consist of a single class implementing all fields in 'ode_api.h'
-o             Specify output path.
               Example -o /path/to/output.csv
-p             Specify input parameter path.
               Example: -p /path/to/file.csv
               Input .csv should have a column per parameter, each row being a parameter combination.
               Number of columns must match the number of parameters defined in the plugin .so
-n             Specify initial values file path.
               Example: -n /path/to/file.csv
               Input .csv should have a column per variable
               Number of rows must match the number of parameter combinations.
               Number of columns must match the number of variables for the system.

## TODO

- Double check unloading behaviour is working