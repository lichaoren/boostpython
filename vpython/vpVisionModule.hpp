
/**
 * Created on April 24, 2015
 * Author: Yanina Zholudz
 * Extracted from vlDirectInterpreter
 * 
 * 
 * This is a vision module definition class for python tornado API.
 * 
 * Currently vision is supposed to be a top level package,
 * so it should be in vlDirectInterpreter:
 *
 * vlDirectInterpreter::vlDirectInterpreter() {
 *   namespace bp = boost::python;
 *   const char* module = "tornado";
 *   ...
 *   PyImport_AppendInittab((char *)"vision", &initvision);
 *   ...
 *   Py_Initialize();
 * }
 * 
 * and the macro definition: 
 * 
 * BOOST_PYTHON_MODULE(vision) {
 *   using namespace boost::python;
 *   docstring_options local_docstring_options(true, false, false);
 *   scope().attr("__path__") = "vision";
 *   vpVisionModule();
 *  }
 * 
 * 
 * 
 * To add/move to another package (as sub-module) simply add/move
 * the following two lines to the destination package definition:
 * 
 * std::string modulename(extract<const char*>(scope().attr("__name__")));
 * vpVisionModule(modulename.c_str());
 * 
 */
#ifndef VPVISIONMODULE_HPP
#define VPVISIONMODULE_HPP


class vpVisionModule {
public:

    /* Define vision module */
    vpVisionModule(const char* package = 0);

};

#endif /* VPVISIONMODULE_HPP */

