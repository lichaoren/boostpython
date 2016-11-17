/**
 * Created on April 23, 2015
 * Author: Yanina Zholudz
 * Extracted from vlDirectInterpreter
 * 
 * 
 * This is a base module definition class for python tornado API.
 * 
 * Currently base is supposed to be a top level package,
 * so it should be in vlDirectInterpreter:
 *
 * vlDirectInterpreter::vlDirectInterpreter() {
 *   namespace bp = boost::python;
 *   const char* module = "tornado";
 *   ...
 *   PyImport_AppendInittab((char *)"base", &initbase);
 *   ...
 *   Py_Initialize();
 * }
 * 
 * and the macro definition: 
 * 
 * BOOST_PYTHON_MODULE(base) {
 *   using namespace boost::python;
 *   docstring_options local_docstring_options(true, false, false);
 *   scope().attr("__path__") = "base";
 *   vpBaseModule();
 *  }
 * 
 * 
 * 
 * To add/move to another package (as sub-module) simply add/move
 * the following two lines to the destination package definition:
 * 
 * std::string modulename(extract<const char*>(scope().attr("__name__")));
 * vpBaseModule(modulename.c_str());
 * 
 */
#ifndef VPBASEMODULE_HPP
#define VPBASEMODULE_HPP


class vpBaseModule {
public:

    /* Define base module */
    vpBaseModule(const char* package = 0);

};

#endif /* VPBASEMODULE_HPP */

