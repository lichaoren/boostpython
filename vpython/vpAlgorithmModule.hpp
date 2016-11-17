
/**
 * Created on April 23, 2015
 * Author: Yanina Zholudz
 * Extracted from vlDirectInterpreter
 * 
 * 
 * This is an algorithm module definition class for python tornado API.
 * 
 * Currently algorithm is supposed to be a top level package,
 * so it should be in vlDirectInterpreter:
 *
 * vlDirectInterpreter::vlDirectInterpreter() {
 *   namespace bp = boost::python;
 *   const char* module = "tornado";
 *   ...
 *   PyImport_AppendInittab((char *)"algorithm", &initalgorithm);
 *   ...
 *   Py_Initialize();
 * }
 * 
 * and the macro definition: 
 * 
 * BOOST_PYTHON_MODULE(algorithm) {
 *   using namespace boost::python;
 *   docstring_options local_docstring_options(true, false, false);
 *   scope().attr("__path__") = "algorithm";
 *   vpAlgorithmModule();
 *  }
 * 
 * 
 * 
 * To add/move to another package (as sub-module) simply add/move
 * the following two lines to the destination package definition:
 * 
 * std::string modulename(extract<const char*>(scope().attr("__name__")));
 * vpAlgorithmModule(modulename.c_str());
 * 
 */
#ifndef VPALGORITHMMODULE_HPP
#define VPALGORITHMMODULE_HPP


class vpAlgorithmModule {
public:

    /* Define algorithm module */
    vpAlgorithmModule(const char* package = 0);

};

#endif /* VPALGORITHMMODULE_HPP */

