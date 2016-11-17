#include "vpBaseModule.hpp"


#include <boost/python.hpp>
#include "boost/python/extract.hpp"
#include "boost/python/numeric.hpp"
#include <boost/python/stl_iterator.hpp>
#include <numpy/ndarrayobject.h>

#include "vString.hpp"

#include "vlDirectInterpreter.hpp"
#include "vlHorizonGroup.hpp"
#include "vlLasWellLog.hpp"
#include "vlPythonInterpreter.hpp"

#include "vmGTransform.hpp"
#include "vmHorizonGroupRotate.hpp"
#include "vmOneSurveyTransform.hpp"
#include "vmRegionAPI.hpp"

#include "vpLasWellLog.hpp"
#include "vpRegionModule.hpp"
#include "vpVolumeAPI.hpp"
#include "vpWarehouseModule.hpp"
#include "vpWellLogCurve.hpp"

static vgHorizon* createHorizon(vgHorizonGroup* hg, const char* name) {
    return hg->createHorizon(name);
}
static vgHorizon* cloneHrz(vgHorizon* hrz){
  vgHorizon* tmp= new vgHorizon(*hrz);
  return tmp;
}


static vgHorizonGroup * createHorizonGroup(const BaseGeometry & geom) {
    vgHorizonGroup* hrzGrp = new vgHorizonGroup(geom);
    if (!hrzGrp) 
        return 0;

    // Create default attributes and avoid to create them in python
    hrzGrp->setDomain( ATTR_NAME_DEPTH );  
    hrzGrp->addAttr(ATTR_NAME_INLINE);
    hrzGrp->addAttr(ATTR_NAME_XLINE);
    hrzGrp->addAttr(ATTR_NAME_XCOORD);
    hrzGrp->addAttr(ATTR_NAME_YCOORD);
    
    return hrzGrp;
}
static vgHorizonGroup* cloneHrzGrp(vgHorizonGroup* hrzGrp) {
    vgHorizonGroup * ret = new vgHorizonGroup(*hrzGrp);
    return ret;
}

static bool horizonGroupSave(vgHorizonGroup* hg , bool isBinary=true, int format=1);

BOOST_PYTHON_FUNCTION_OVERLOADS(horizonGroupSave_overloads,  horizonGroupSave, 1, 3)

bool horizonGroupSave(vgHorizonGroup* hg, bool isBinary, int format)
{
    if(!hg)
    {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Failed saving horizon group.");
        return false;
    }
    
    if (hg->save(isBinary, format)==false)
    {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Failed saving horizon group.");
        return false;
    }
    return true;
}

static BaseGeometry* hrzGrpGetGeomotry(vgHorizonGroup* hg) {
	BaseGeometry* geo = new BaseGeometry();
	(*geo) = (*hg);
	return geo;
}

// we will use tuple to have the chance to return multiple different object
// at a time

/**
 * we change the interface to make it pythonic 
 * @param hrz input horizon
 * @param x_log log x
 * @param y_log log y
 * @param attr_index attribute index
 * @return (true/false, val)
 */
static boost::python::tuple borehole(vgHorizon* hrz, const double x_log, const double y_log, int attr_index) {
    double val;
    bool res;
    res = hrz->borehole(x_log, y_log, attr_index, val);
    boost::python::tuple tp = boost::python::make_tuple(res, val);
    return tp;
}

static boost::python::tuple getAttrByGridAndIndex(vgHorizon* hrz,
        int i, int j, int attr_index) {
    double val;
    bool res;
    res = hrz->getAttr(i, j, val, attr_index);
    return boost::python::make_tuple(res, val);
}

static boost::python::tuple getAttrByGridAndName(vgHorizon* hrz,
        int i, int j, const vString& name) {
    double val;
    bool res;
    res = hrz->getAttr(i, j, val, name);
    return boost::python::make_tuple(res, val);
}

static boost::python::tuple getAttrByLogAndIndex(vgHorizon* hrz,
        double x, double y, int attr_index) {
    double val;
    bool res;
    res = hrz->getAttr(x, y, val, attr_index);
    return boost::python::make_tuple(res, val);
}

static boost::python::tuple getAttrByLogAndName(vgHorizon* hrz,
        double x, double y, const vString& name) {
    double val;
    bool res;
    res = hrz->getAttr(x, y, val, name);
    return boost::python::make_tuple(res, val);
}

static int addAttr(vgHorizon* hrz, const vString& attr_name) {
    return hrz->addAttr(attr_name);
}


static boost::python::object hrz_getValue(vgHorizon* hrz, int i, int j, const char* attr){
    double val;
    bool res;
    res = hrz->getAttr(i, j, val, attr);
    if (res)
        return boost::python::object(val);
    else
        return boost::python::object();
}


bool vpHorizonCopy(vgHorizon* hrz, vgHorizon* source, vmMultiPoly2D* poly=0 );

BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizon_HorizonCopy_overloads,  vpHorizonCopy, 2, 3)

bool vpHorizonCopy(vgHorizon* hrz, vgHorizon* source, vmMultiPoly2D* poly ) {  
    
    if( source==0 )
        return false;
    if( poly==0 ) {
        vString hrz_name = hrz->getName();
        hrz->copy(source);
        hrz->setName(hrz_name);
        return true;
    }
        
    HorizonRegionBuffer regionBuffer;
    poly->setGeometry(*hrz);

    regionBuffer.copy(*source, *poly);
    regionBuffer.paste(hrz);
    
    return true;
}


// this is for the purpose to keep consistent with the result in hrzsaveformatWindow
static Vect4d hrz_grd2wld(vgHorizon* hrz, Vect4i grd){
    float ox=hrz->getOrigin()[0];
    float oy=hrz->getOrigin()[1];
    float dx=hrz->getStep()[0];
    float dy=hrz->getStep()[1];
    float x=ox+grd[0]*dx;
    float y=oy+grd[1]*dy;
    Vect4d rwpos = hrz->log2wld(Vect4d(x,y,0));
    rwpos[0]=float(rwpos[0]);
    rwpos[1]=float(rwpos[1]);
    return rwpos;
}

static void hrz_setValue(vgHorizon* hrz, int i, int j, const char* attr,boost::python::object v){
    boost::python::extract<double> value(v);
    static float non=hrz->getNoValue();
    if (!value.check()){
        hrz->setAttr(i,j, non, attr);
        return;
    }
    
    double vd= value;
    hrz->setAttr(i,j, vd, attr);
} 

static vgHorizonGroup * hrz_transform(vgHorizonGroup* hg,  vector<GTransform *> transList)
{
    Vect4i inc = hg->getInc();
    Vect4d step = hg->getStep();
    Vect2d sab (step[0]/(double)inc[1], step[0]/(double)inc[1]);
    
    
    Vect2d   zeros = Vect2d(0, 0);
    vmOneSurveyTransform *tr = new vmOneSurveyTransform (zeros);
    tr->setLTransform (transList);
    
    vmHorizonGroupRotate *gr = new vmHorizonGroupRotate(hg);
    gr->setMapping (*hg, tr);
    gr->setSRate (hg->getStep()[2]);
    gr->setInc (inc[0], inc[1]);
    
    gr->setupOutput (zeros, sab);
    gr->rotate (NULL);
    
    vgHorizonGroup * output = gr->getOutput();
    delete tr;
    delete gr;
    
    return output;
}

static vgHorizonGroup * hrz_shift(vgHorizonGroup* hg, double x, double y)
{
    if(!hg)
    {
        return 0;
    }
        
    vector<GTransform *> transList;
    transList.push_back (new GShift(x, y));
    
    return hrz_transform( hg,  transList);
}

static vgHorizonGroup * hrz_rotate(vgHorizonGroup* hg,  double angle  )
{
    if(!hg)
    {
        return 0;
    }
    vector<GTransform *> transList;
    transList.push_back (new GRotate(angle));

    return hrz_transform( hg,   transList);
} 


// here are some wrappers to avoid boost complaining about overloaded functions
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vgHorizonGroup_load_overloads, load, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vgHorizonGroup_save_overloads, save, 0, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vgHorizonGroup_setpath_overloads, setPath, 1, 2)
        
vgHorizon* (vgHorizonGroup::*getHrzByIndex)(int) = &vgHorizonGroup::getHorizonByIndex;
vgHorizon* (vgHorizonGroup::*getHrzByName)(const char* name) = &vgHorizonGroup::getHorizonByName;

void (vgHorizon::*setAttrByGridAndIndex)(int, int, double, int) = &vgHorizon::setAttr;
void (vgHorizon::*setAttrByGridAndName)(int, int, double, const vString&) = &vgHorizon::setAttr;
void (vgHorizon::*setAttrByLogAndIndex)(double, double, double, int) = &vgHorizon::setAttr;
void (vgHorizon::*setAttrByLogAndName)(double, double, double, const vString&) = &vgHorizon::setAttr;



void* arrayToData(boost::python::numeric::array arr){
  if(!PyArray_Check(arr.ptr())){
    PyErr_SetString(PyExc_ValueError, "expected a PyArrayObject");
    return 0;
  }
  return PyArray_DATA(arr.ptr());
}


boost::python::object wrapHorizonGetAttributeTime(vgHorizon* hrz,
        vmMultiPoly2D* poly=0);
boost::python::object wrapHorizonGetAttributeDepth(vgHorizon* hrz,
        vmMultiPoly2D* poly=0);
boost::python::object wrapHorizonGetAttribute(vgHorizon* hrz, const vString attr_name,
        vmMultiPoly2D* poly=0);

BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizon_GetAttributeTime_overloads, wrapHorizonGetAttributeTime, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizon_GetAttributeDepth_overloads,  wrapHorizonGetAttributeDepth, 1, 2)
BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizon_GetAttribute_overloads,  wrapHorizonGetAttribute, 2, 3)


boost::python::object wrapHorizonGetAttribute(vgHorizon* hrz, const vString attr_name,
        vmMultiPoly2D* poly )
{
    using namespace boost::python;
    Vect4i vsize = hrz->getSize();
    float *data = hrz->getAttrPtr(attr_name);
    npy_intp shape[] = {vsize[0], vsize[1]}; // array size

    
    if (poly) {
        BaseGeometry* geom = hrz;
        vgHorizon *dest = new vgHorizon(*geom);
        dest->addAttr(attr_name);

        if (vpHorizonCopy(dest, hrz, poly)) {

            float *data_dest = dest->getAttrPtr(attr_name);

            PyArrayObject* arr = (PyArrayObject*) PyArray_SimpleNewFromData(2,
                    shape,
                    NPY_FLOAT,
                    data_dest);
                      
            PyObject* arr_copy = PyArray_Copy(arr);
            handle<> array(arr_copy);
            Py_DECREF(arr);
            delete dest;            
            
            return object(array);
        } else {

            //?? PyObject* obj = PyArray_SimpleNew( 2, shape,  NPY_FLOAT); 
            delete dest;
        }
    }

    /**
     *  Create an array wrapper around data pointed to by the given pointer. The 
     * array flags will have a default that the data area is well-behaved and 
     * C-style contiguous. The shape of the array is given by the dims c-array 
     * of length nd. The data-type of the array is indicated by typenum.
     */
    PyObject* obj = PyArray_SimpleNewFromData( 2, 
            shape, 
            NPY_FLOAT, 
            data);    
    
    
    
    handle<> array(obj);

    return object(array);
}

boost::python::object wrapHorizonGetAttributeDepth(vgHorizon* hrz,
        vmMultiPoly2D* poly)
{
    return wrapHorizonGetAttribute(hrz, ATTR_NAME_DEPTH, poly);
}

boost::python::object wrapHorizonGetAttributeTime(vgHorizon* hrz,
        vmMultiPoly2D* poly)
{
    return wrapHorizonGetAttribute(hrz, ATTR_NAME_TWT, poly);
}


bool wrapHorizonSetAttributeTime(vgHorizon* hrz,
        boost::python::numeric::array npdata,
        vmMultiPoly2D* poly=0);
bool wrapHorizonSetAttributeDepth(vgHorizon* hrz,
        boost::python::numeric::array npdata,
        vmMultiPoly2D* poly=0);
bool wrapHorizonSetAttribute(vgHorizon* hrz, 
        const vString attr_name,
        boost::python::numeric::array npdata,
        vmMultiPoly2D* poly=0);

BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizon_SetAttributeTime_overloads, wrapHorizonSetAttributeTime, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizon_SetAttributeDepth_overloads,  wrapHorizonSetAttributeDepth, 2, 3)
BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizon_SetAttribute_overloads,  wrapHorizonSetAttribute, 3, 4)

bool wrapHorizonSetAttribute(vgHorizon* hrz,
        const vString attr_name,
        boost::python::numeric::array npdata,
        vmMultiPoly2D* poly) {
    bool status = true;
    using namespace boost::python;
    Vect4i vsize = hrz->getSize();
    int size = vsize[0] * vsize[1];
    float *data = hrz->getAttrPtr(attr_name);
    float* arr = (float*) arrayToData(npdata);

    if (arr == 0)
        return false;

    if (poly) {
        BaseGeometry* geom = hrz;
        vgHorizon *dest = new vgHorizon(*geom);
        dest->addAttr(ATTR_NAME_INLINE );
        dest->addAttr(ATTR_NAME_XLINE);
        dest->addAttr(ATTR_NAME_XCOORD);
        dest->addAttr(ATTR_NAME_YCOORD);
        dest->addAttr(attr_name);
        float *data_dest = dest->getAttrPtr(attr_name);
        memcpy(data_dest, arr, sizeof (float)*size);

        status = vpHorizonCopy(hrz, dest, poly);
        delete dest;
    } else {
        memcpy(data, arr, sizeof (float)*size);
    }


    return status;
}

bool wrapHorizonSetAttributeDepth(vgHorizon* hrz,
        boost::python::numeric::array npdata,
        vmMultiPoly2D* poly)
{
    return wrapHorizonSetAttribute(hrz, ATTR_NAME_DEPTH, npdata, poly);
}

bool wrapHorizonSetAttributeTime(vgHorizon* hrz,
        boost::python::numeric::array npdata,
        vmMultiPoly2D* poly)
{
    return wrapHorizonSetAttribute(hrz, ATTR_NAME_TWT, npdata, poly);
}

/** Convenient to have a script compatible for interactive and batch */
bool vpIsTornadoInteractive() {
    vlPythonInterpreter& interpreter = vlPythonInterpreter::getInstance();
  
    return interpreter.getContext().isInteractiveMode();
}

/** Get info  (empty as it's jut use a static method .. for now)*/
class vpContext {
// @see vpIsTornadoInteractive
};


/** Horizon Group default constructor */
static boost::shared_ptr<vgHorizonGroup> makeHorizonGroup( BaseGeometry* geom=0);
BOOST_PYTHON_FUNCTION_OVERLOADS(vgHorizonGroup_init_overloads,  makeHorizonGroup, 0, 1)

static boost::shared_ptr<vgHorizonGroup> makeHorizonGroup( BaseGeometry* geom) {
    vgHorizonGroup *hrzGrp=0;
    
    if(geom)
        hrzGrp = new vgHorizonGroup(*geom);
    else
        hrzGrp = new vgHorizonGroup();
    
    // Create default attributes and avoid to create them in python
    hrzGrp->setDomain( ATTR_NAME_DEPTH );  
    hrzGrp->addAttr(ATTR_NAME_INLINE);
    hrzGrp->addAttr(ATTR_NAME_XLINE);
    hrzGrp->addAttr(ATTR_NAME_XCOORD);
    hrzGrp->addAttr(ATTR_NAME_YCOORD);

    return boost::shared_ptr<vgHorizonGroup>(hrzGrp);
}

vpBaseModule::vpBaseModule(const char* package)
{
    using namespace boost::python;
     
    // numpy requires this
    int isNumpyArrayInitialized = _import_array();
    
    if (isNumpyArrayInitialized != -1)
        boost::python::numeric::array::set_module_and_type("numpy", "ndarray");
    
   
    //map base namespace to a (sub)module
    //make "from [package.]base import *" work
    std::string modulename = (package) ? std::string(package)+".base"
                                       : std::string("base");
    object baseModule(handle<>(borrowed(PyImport_AddModule(modulename.c_str()))));
    
    //make "[from package ]import base" work
    //as well as import [package.]base
    scope().attr("base") = baseModule;

    //need these parentheses for the scoping of the following classes
    //(so that they are properly "placed inside" base module)
    {
        //change scope to base to add classes within base module
        scope base_scope = baseModule;

        //get base package full name
        std::string modulename(extract<const char*>(scope().attr("__name__")));
        //add warehouse sub-module
        vpWarehouseModule(modulename.c_str());
        //add region sub-module to base module
        vpRegionModule(modulename.c_str());
        
        // Test if GUI is available or not
        def("isTornadoInteractive", vpIsTornadoInteractive);

        class_<vpContext>("Context")
                /* other */
                .def("isInteractive", vpIsTornadoInteractive)
                .staticmethod("isInteractive")
                ;
        
        
        //add classes in base package
        class_<BaseGeometry, boost::noncopyable>("Geometry")
            .def("getMin", &BaseGeometry::getMin)
            .def("getMax", &BaseGeometry::getMax)
            .def("setMin", &BaseGeometry::setMin)
            .def("setMax", &BaseGeometry::setMax)
            .def("getInc", &BaseGeometry::getInc)
            .def("setInc", &BaseGeometry::setInc)
            .def("getSize", &BaseGeometry::getSize)
            .def("setSize", &BaseGeometry::setSize)
            .def("getOrigin", &BaseGeometry::getOrigin)
            .def("setOrigin", &BaseGeometry::setOrigin)
            .def("getStep", &BaseGeometry::getStep)
            .def("setStep", &BaseGeometry::setStep)
            .def("lin2grd", &BaseGeometry::lin2grd)
            .def("log2grd", &BaseGeometry::log2grd)
            .def("load", &BaseGeometry::load);

            
        /// Seismic & Attribute Implementation
        vpVolumeAPI::exposePython();


        
        def("createHorizonGroup", createHorizonGroup, return_value_policy<manage_new_object>());
        
        class_<vgHorizonGroup, boost::noncopyable>("HorizonGroup")
                .def("__init__", make_constructor(makeHorizonGroup))
                .def("load", &vgHorizonGroup::load, vgHorizonGroup_load_overloads())
                .def("setPath", &vgHorizonGroup::setPath, vgHorizonGroup_setpath_overloads())
                .def("save", horizonGroupSave, horizonGroupSave_overloads())
                .def("getNumHorizon", &vgHorizonGroup::getNumHorizon)
                .def("getHorizonByName", getHrzByName, return_internal_reference<>())
                .def("getHorizonIndexByName", &vgHorizonGroup::getHorizonIndexByName)
                .def("getHorizonByIndex", getHrzByIndex, return_internal_reference<>())
                .def("getHorizon", getHrzByName, return_internal_reference<>())
                .def("createHorizon", createHorizon, return_internal_reference<>())
                .def("removeHorizonByIndex", &vgHorizonGroup::removeHorizon)
                .def("addAttr", &vgHorizonGroup::addAttr)
                .def("addAttribute", &vgHorizonGroup::addAttr)
                .def("shift", hrz_shift, return_value_policy<manage_new_object>())
                .def("rotate", hrz_rotate, return_value_policy<manage_new_object>())
                .def("getName", &vgHorizonGroup::getName)
                .def("getSize", &vgHorizonGroup::getSize)
                .def("getGeometry", hrzGrpGetGeomotry, return_value_policy<manage_new_object>())
                .def("create", createHorizonGroup, return_value_policy<manage_new_object>())
                .def("clone", cloneHrzGrp, return_value_policy<manage_new_object>())
        .staticmethod("create");
        
 
        
        class_<vgHorizon, boost::noncopyable> hrzClass("Horizon");

        hrzClass.def("getName", &vgHorizon::getName, return_value_policy<return_by_value>())
                .def("getAttrIndex", &vgHorizon::getAttrIndex)
                .def("setName", &vgHorizon::setName)
                .def("getAttrName", &vgHorizon::getAttrName, return_value_policy<return_by_value>())
                .def("getNumAttr", &vgHorizon::getNumAttr)
                .def("borehole", borehole, return_value_policy<return_by_value>())
                .def("getAttrByGrid", getAttrByGridAndIndex, return_value_policy<return_by_value>())
                .def("getAttrByGrid", getAttrByGridAndName, return_value_policy<return_by_value>())
                .def("getAttrByLog", getAttrByLogAndIndex, return_value_policy<return_by_value>())
                .def("getAttrByLog", getAttrByLogAndName, return_value_policy<return_by_value>())
                .def("setAttrByGrid", setAttrByGridAndIndex)
                .def("setAttrByGrid", setAttrByGridAndName)
                .def("setAttrByLog", setAttrByLogAndIndex)
                .def("setAttrByLog", setAttrByLogAndName)
                .def("getOrigin", &vgHorizon::getOrigin, return_value_policy<return_by_value>())
                .def("getSize", &vgHorizon::getSize, return_value_policy<return_by_value>())
                .def("getStep", &vgHorizon::getStep, return_value_policy<return_by_value>())
                .def("getMin", &vgHorizon::getMin, return_value_policy<return_by_value>())
                .def("getMax", &vgHorizon::getMax, return_value_policy<return_by_value>())
                .def("getInc", &vgHorizon::getInc, return_value_policy<return_by_value>())
                .def("copy", vpHorizonCopy, vgHorizon_HorizonCopy_overloads())
                .def("grd2log", &vgHorizon::grd2log)
                .def("grd2lin", &vgHorizon::grd2lin)
                .def("grd2wld", hrz_grd2wld, return_value_policy<return_by_value>())
                .def("getValue", hrz_getValue, return_value_policy<return_by_value>())
                .def("setValue", hrz_setValue)
                .def("clone", cloneHrz, return_value_policy<manage_new_object>())
                ;

        /** Only available with Numpy */
        if (isNumpyArrayInitialized != -1)
        {
            hrzClass.def("getAttribute", wrapHorizonGetAttribute, vgHorizon_GetAttribute_overloads())
                    .def("setAttribute", wrapHorizonSetAttribute, vgHorizon_SetAttribute_overloads())
                    .def("getDepth", wrapHorizonGetAttributeDepth, vgHorizon_GetAttributeDepth_overloads())
                    .def("setDepth", wrapHorizonSetAttributeDepth, vgHorizon_SetAttributeDepth_overloads())
                    .def("getTime", wrapHorizonGetAttributeTime, vgHorizon_GetAttributeTime_overloads())
                    .def("setTime", wrapHorizonSetAttributeTime, vgHorizon_SetAttributeTime_overloads());
        }
        
        
        // Expose Well Curve as vpWellLogCurve (not as WellLogCurve)
        vpWellLogCurve::exposePython();

        class_<LasWellLog, boost::noncopyable> ("Well")
            .def("getName", &LasWellLog::getWellName)
            .def("getID", &LasWellLog::getWellID)
            .def("getVerticalShift", &LasWellLog::getVerticalShift )
            .def("getDepthType", &LasWellLog::getDepthType)
            .def("getLog", vpLasWellLog::getLog, return_value_policy<manage_new_object>())
            .def("addLog", vpLasWellLog::addLog, return_value_policy<manage_new_object>() )
        ;        
        // register_ptr_to_python< shared_ptr<LasWellLog> >();
        
    }
}


