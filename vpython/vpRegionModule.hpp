/**
 * Created on April 23, 2015
 * Author: Yanina Zholudz
 * Extracted from vlDirectInterpreter
 * 
 * 
 * This is a region module definition class for python tornado API.
 * 
 * Currently region is supposed to be sub-module of base package,
 * so the constructor should be in the definition of the base package.
 * 
 * It can be added/moved to another package by simply adding/moving
 * the following two lines to the destination package definition:
 * 
 * std::string modulename(extract<const char*>(scope().attr("__name__")));
 * vpRegionModule(modulename.c_str());
 * 
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
 *   PyImport_AppendInittab((char *)"region", &initregion);
 *   ...
 *   Py_Initialize();
 * } 
 * 
 * and the macro: 
 * 
 * BOOST_PYTHON_MODULE(region) {
 *   using namespace boost::python;
 *   docstring_options local_docstring_options(true, false, false);
 *   scope().attr("__path__") = "base";
 *   vpRegionModule();
 * }
 * 
 */
#ifndef VPREGIONMODULE_HPP
#define	VPREGIONMODULE_HPP


class vpRegionModule {
public:

    /* Define region module */
    vpRegionModule(const char* package = 0);

};

#endif /* VPREGIONMODULE_HPP */

