/**
 * Created on April 23, 2015
 * Author: Yanina Zholudz
 * extracted from vlDirectInterpreter
 * 
 * 
 * 
 * This is a warehouse module definition class for python tornado API.
 * Its purpose is to provide automatic conversions between 
 * C++ and Python base types (strings, lists, tuples, etc.).
 * 
 * 
 * 
 * Currently warehouse is supposed to be a sub-package of base package,
 * 
 * It can be added/moved to another package by simply adding/moving
 * the following two lines to the destination package definition:
 * 
 * std::string modulename(extract<const char*>(scope().attr("__name__")));
 * vpWarehouseModule(modulename.c_str());
 * 
 * 
 * 
 * To add it as an independent module, register with vlDirectInterpreter.
 * Add to constructor:
 *
  * vlDirectInterpreter::vlDirectInterpreter() {
 *   namespace bp = boost::python;
 *   const char* module = "tornado";
 *   ...
 *   PyImport_AppendInittab((char *)"warehouse", &initwarehouse);
 *   ...
 *   Py_Initialize();
 * } 
 * 
 * and the macro definition: 
 * 
 * BOOST_PYTHON_MODULE(warehouse) {
 *   using namespace boost::python;
 *   docstring_options local_docstring_options(true, false, false);
 *   scope().attr("__path__") = "warehouse";
 *   vpWarehouseModule();
 * }
 */
#ifndef VPWAREHOUSEMODULE_HPP
#define VPWAREHOUSEMODULE_HPP

class vpWarehouseModule {
public:

    /* Define warehouse module */
    vpWarehouseModule(const char* package = 0);

};

#endif /* VPWAREHOUSEMODULE_HPP */

