/*
 * Classification: Tornado
 *
 * Created on January 29, 2015, 2:38 PM
 * Author: ddoledec
 *
 * Copyright Notice:
 *     Copyright (c) CGG Interactive application
 *     Unpublished Work. All rights reserved.
 */

#include <Python.h>
#include <boost/python.hpp>

#include "vlDirectInterpreter.hpp"

#include "vlPythonInterpreter.hpp"
#include "vpAlgorithmModule.hpp"
#include "vpBaseModule.hpp"
#include "vpVisionModule.hpp"


void exec(char *commandes) {
    PyRun_SimpleStringFlags(reinterpret_cast<const char*> (commandes), NULL);
}

void testPython() {
    vlPythonInterpreter &interpreter = vlPythonInterpreter::getInstance();
    const char* path = "/auto/neo/home1/yanhliu/test.py";
    interpreter.setFile(const_cast<char*> (path));
    interpreter.run();
    interpreter.reset();
}


/*
 * Boost.python wrapper
 * base Module for Python Script:
 * includes data and region classes, data conversions (C++ to/from Python)
 *
 */

BOOST_PYTHON_MODULE(base) {
    using namespace boost::python;
    docstring_options local_docstring_options(true, false, false);

    //treat this module as a package
    scope().attr("__path__") = "base";

    vpBaseModule();
}


/**
 * Python vision package
 * this package can only be used in interactive mode
 */

BOOST_PYTHON_MODULE(vision) {
    using namespace boost::python;

    //treat this module as a package
    scope().attr("__path__") = "vision";

    vpVisionModule();
}


/**
 * python algorithm package
 * contains: rca_smoothing
 *            horizon interpolation
 */
BOOST_PYTHON_MODULE(algorithm) {
    using namespace boost::python;
    docstring_options local_docstring_options(true, false, false);

    //treat this module as a package
    scope().attr("__path__") = "algorithm";

    vpAlgorithmModule();
}

#include "MiscUtl.hpp"
#include "FileUtl.hpp"


vString getBestPythonPath();

/**
 * initialize the module table
 */
vlDirectInterpreter::vlDirectInterpreter() {
    namespace bp = boost::python;
    const char* module = "tornado";
    Py_SetProgramName(const_cast<char*> (module));

    // try to rely on a "cgg" distribution
    vString cggPythonPath=getBestPythonPath();
    if( !cggPythonPath.isEmpty() ) {
        cout << "Python path: " <<  (char*)cggPythonPath.c_str() << endl;
        Py_SetPythonHome( (char*)cggPythonPath.c_str());
    } else {
        cerr << "No python path is defined!" << endl;
        Py_NoSiteFlag = 1;
    }
  
    
    //only top level modules (packages) need to be initialized here
    //the sub-packages should be handled in the corresponding module constructors
    PyImport_AppendInittab((char *)"base", &initbase);
    PyImport_AppendInittab((char *)"vision", &initvision);
    PyImport_AppendInittab((char *)"algorithm", &initalgorithm);

    Py_Initialize();

    // DD keep it for later use
    // The aim is to add others path for the modules (numpy, scipy....)
//    PyObject* sysPath = PySys_GetObject("path");
//    PyList_Insert(sysPath, 0, PyString_FromString(cggPythonPath.c_str()));
    
}

/** Extract 3 digits from version sctring */
bool getReleaseFromString(const vString& vendorRelease, int &maj, int &medium, int &minor)
{
    maj=medium=minor=-1;
    size_t majorReleaseIdx = vendorRelease.find_first_of(".");
    if (majorReleaseIdx > 0)
    {
        size_t mediumReleaseIdx = vendorRelease.find_first_of(".", majorReleaseIdx + 1);
        if (mediumReleaseIdx > 0)
        {
            size_t minorReleaseIdx = vendorRelease.find_first_of(".", mediumReleaseIdx + 1);

            if (minorReleaseIdx > 0)
            {
                vString majorRelease = vendorRelease.get(0, majorReleaseIdx - 1);
                vString mediumRelease = vendorRelease.get(majorReleaseIdx + 1, mediumReleaseIdx - 1);
                vString minorRelease = vendorRelease.get(mediumReleaseIdx + 1, vendorRelease.length());
                if (majorRelease.isInteger() &&
                        mediumRelease.isInteger() &&
                        minorRelease.isInteger())
                {
                    maj=majorRelease.readi();
                    medium=mediumRelease.readi();
                    minor=minorRelease.readi();
                    return true;
                }
            }
        }
    }
    return false;
}
/**
 *  Local install if match requirements (build number) or Geovation path
 * @return empty string if may rely on installation or python home from geovation 
 * distribution
 */
vString getBestPythonPath()
{
    
    /** Path is defined by user*/
    vString pythonHome(getenv("PYTHONHOME"));
    if (pythonHome.isEmpty()==false)
    {
       return pythonHome;
    }

    vString beforeToken, afterToken;
    vString pythonBuildVersion(Py_GetVersion());
    int majorBuildRelease = -1, mediumBuildRelease = -1, minorBuildRelease = -1;
    if (pythonBuildVersion.splitAtToken(beforeToken, afterToken, ' ') > 0)
    {
        getReleaseFromString(beforeToken, majorBuildRelease, mediumBuildRelease, minorBuildRelease);
    }

    /** Detect default installation 
     *  The default is pointing to /vega/1/rel/Python<major>.<minor>
     * But, no standard (yet) between tango, geovation2 ... so detect again.
     */
    vString linkPythonPath="/vega/1/rel/Python"; // Path used to build Python
    vString VREL(getenv("VREL"));
    vString cggPythonPath( VREL + "/Python"); 
    cggPythonPath += vString::number(majorBuildRelease)+ "." + vString::number(mediumBuildRelease);
    linkPythonPath += vString::number(majorBuildRelease) + "." + vString::number(mediumBuildRelease);

    if (vUtl::directoryExists(linkPythonPath))
    {
        return linkPythonPath;
    } else if (vUtl::directoryExists(cggPythonPath)) {
        return cggPythonPath;
    }

    return Py_GetPythonHome();
}




vlDirectInterpreter::~vlDirectInterpreter() {
    Py_Finalize();
}


/**
 * run from file
 * @param fileName
 * @return
 */
bool vlDirectInterpreter::run(const char* fileName) {
    if (fileName == 0) {
        return false;
    }
    char aa[100] = "r";
    PyObject *pyFile = PyFile_FromString((char*) fileName, aa);
    if (pyFile == NULL)
        return false;

    int status = PyRun_SimpleFile(PyFile_AsFile(pyFile), fileName);

#if (PY_MAJOR_VERSION>2) || (PY_MINOR_VERSION>=5)
    Py_DECREF(fileName);
#else
#warning Change Py_DECREF
#endif
    fprintf(stderr,"INFO: finished python script\n");
    return (status == 0);
}

/**
 * run from command
 * @param cmd
 * @return
 */
bool vlDirectInterpreter::runCMD(const char* cmd) {
    using namespace boost::python;
    try {
        static object main_module((
                handle<>(borrowed(PyImport_AddModule("__main__")))));

        static object main_namespace = main_module.attr("__dict__");
        handle<> ignored((PyRun_String(
                cmd,
                Py_file_input,
                main_namespace.ptr(),
                main_namespace.ptr())));
    } catch (error_already_set) {
        PyErr_Print();
    }
    fprintf(stderr,"INFO: finished python script\n");
    return true;
}

