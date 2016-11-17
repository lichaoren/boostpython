/* 
 * File:   vpWellLogCurve.cpp
 * Author: Dominique DOLEDEC
 * 
 * Created on May 6, 2016, 4:25 PM
 */
#include <boost/python.hpp>
#include "boost/python/numeric.hpp"
#include <boost/python/extract.hpp>
#include <boost/python/handle.hpp>
#include <numpy/ndarrayobject.h>

#include "vpWellLogCurve.hpp"

#include "vlPythonInterpreter.hpp"

vpWellLogCurve::vpWellLogCurve() :
myLogCurve(0) {
}

vpWellLogCurve::vpWellLogCurve(WellLogCurve *logCurve) :
myLogCurve(logCurve) {
}

vpWellLogCurve::vpWellLogCurve(const vpWellLogCurve& orig) {
    if (this == &orig)
        return;
    this->myLogCurve = orig.myLogCurve;
}

vpWellLogCurve::~vpWellLogCurve() {
}

void vpWellLogCurve::exposePython() {
     using namespace boost::python;
   
    // PyArray_API scope is only to the file !
    int isNumpyArrayInitialized = _import_array();    

    
    class_<vpWellLogCurve, boost::noncopyable> wellLogClass("WellLog");
    wellLogClass.def(init<WellLogCurve*>())
            .def("getName", &vpWellLogCurve::getName)
            .def("getData", &vpWellLogCurve::getData)
            .def("setData", &vpWellLogCurve::setData)
            ;
    /** Only available with Numpy */
    if (isNumpyArrayInitialized != -1) {
        wellLogClass
                .def("getNpyData", &vpWellLogCurve::getNPyData)
                .def("setNpyData", &vpWellLogCurve::setNPyData)
                ;
    }
}


boost::python::list vpWellLogCurve::getData() const {
    boost::python::list pyArray;
    WellLogCurve* wellLog = myLogCurve;
    if (wellLog) {
        for (int i = 0; i < wellLog->size; ++i) {
            pyArray.append(wellLog->data[i]);
        }
    }
    return pyArray;

}

void vpWellLogCurve::setData(const boost::python::list& tpl) {
    WellLogCurve* wellLog = myLogCurve;

    if (wellLog) {
        if (wellLog->size != boost::python::len(tpl)) {
            vString mess("Unable to set values to the log number of samples does NOT match");
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, mess);
        }

        for (int i = 0; i < wellLog->size; ++i) {
            wellLog->data[i] = boost::python::extract<float>(tpl[i]);
        }
    }
}

boost::python::object vpWellLogCurve::getNPyData() const {
    using namespace boost::python;

    WellLogCurve* wellLog = myLogCurve;

    npy_intp shape[] = {wellLog->size}; // array size
    /**
     *  Create an array wrapper around data pointed to by the given pointer. The 
     * array flags will have a default that the data area is well-behaved and 
     * C-style contiguous. The shape of the array is given by the dims c-array 
     * of length nd. The data-type of the array is indicated by typenum.
     */
    float* data = new float[wellLog->size * 2];
    memcpy(data, wellLog->data, sizeof (float)*wellLog->size);

    PyObject* obj = PyArray_SimpleNewFromData(1,
            shape,
            NPY_FLOAT,
            data);

    handle<> array(obj);

    return object(array);
}


void vpWellLogCurve::setNPyData(const boost::python::object& npdata) {
    using namespace boost::python;

    WellLogCurve* wellLog = myLogCurve;

    if (wellLog) {
        if (!PyArray_Check( npdata.ptr())) {
            const boost::python::list& datalist = extract<const boost::python::list&>(npdata);
            
            setData(datalist);
            return;
        }
        PyArrayObject *py_array = (PyArrayObject *)npdata.ptr();
        int nbValues = PyArray_DIM(py_array,0);
        if (wellLog->size!=nbValues) {
            PyErr_SetString(PyExc_ValueError, "expected size of array do no match!!");
            return;
        }
        
        int type = PyArray_TYPE(py_array);
        if (type==NPY_FLOAT ) {
            float* arr = (float*) PyArray_DATA(py_array);         
          if (arr == 0)
            return;
                    
            memcpy(wellLog->data, arr, sizeof (float)*wellLog->size);

        } else {
            PyObject* tmp = PyArray_Cast(py_array, PyArray_FLOAT);
            if (tmp == 0)
                return;

            float* arr = (float*) PyArray_DATA(tmp);
                   
            int debu = PyArray_DIM(tmp,0);

            if (arr == 0)
                return;
            
            memcpy(wellLog->data, arr, sizeof (float)*wellLog->size);
        }

    }
} 


vString vpWellLogCurve::getName() const {
    return this->myLogCurve->name;
}

int vpWellLogCurve::getType() const {
    return this->myLogCurve->type;
}

void vpWellLogCurve::setType(int type) {
    this->myLogCurve->type = type;
}
