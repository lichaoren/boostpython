#include <boost/python.hpp>
#include "vpRegionModule.hpp"
//GenerateMask in vmBoolMask needs horizon
#include "vlHorizon.hpp"
#include "vmRegionAPI.hpp"


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vmMultiPoly2DWrapper_load_overloads, loadFromFile, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vmMultiPoly2DWrapper_save_overloads, saveToFile, 1, 2)
bool (vmMultiPoly2DWrapper::*loadMPolyFromFile)(const vString&, bool, bool) = &vmMultiPoly2DWrapper::loadFromFile;
bool (vmMultiPoly2DWrapper::*saveMPolyToFile)(const vString&, bool) = &vmMultiPoly2DWrapper::saveToFile;

static vmPoly2D* fncPolygon(vmMultiPoly2D*, int);

vpRegionModule::vpRegionModule(const char* package)
{
    using namespace boost::python;

    //map region namespace to a (sub)module
    //make "from package.region import *" work
    std::string modulename = (package) ? std::string(package)+".region"
                                       : std::string("region");
    object regionModule(handle<>(borrowed(PyImport_AddModule(modulename.c_str()))));
    
    //make "[from package ]import region" work
    //as well as import [package.]region
    scope().attr("region") = regionModule;

    //need these parentheses for the scoping of the following classes
    //(so that they are properly "placed inside" region module)
    {
        //change scope to region to add classes within region module
        scope region_scope = regionModule;

        //add region namespace classes
        class_<vmBoolMaskWrapper, boost::noncopyable>("BoolMask")
            .def("setDim", &vmBoolMask::setDim)
            .def("getDim", &vmBoolMaskWrapper::getDim)
            .def("set", &vmBoolMask::set)
            .def("setAll", &vmBoolMaskWrapper::setAll)
            .def("isSet", &vmBoolMask::isSet)
            .def("reverse", &vmBoolMask::reverse)
            .def("printAll", &vmBoolMaskWrapper::printAll)
            .def("destroy", &vmBoolMask::destroy)
            .def("isNull", &vmBoolMask::isNull)
            .def("resize", &vmBoolMask::resize)
            .def("getBoundingBox", &vmBoolMaskWrapper::getBoundingBox)
            .def("getCount", &vmBoolMask::getCount)
            .def("generateMask", GenerateMask)
            .def("GenerateMask", GenerateMask);

        class_<vmPoint2D, boost::noncopyable>("Point2D", init<double, double>());


        class_<vmPoly2DWrapper, boost::noncopyable>("Polygon2D")
            .def("addPoint", &vmPoly2D::addPoint, return_internal_reference<>())
            .def("addPointSequence", &vmPoly2D::addPointSequence, return_internal_reference<>())
            .def("addPointSequenceAt", &vmPoly2D::addPointSequenceAt, return_internal_reference<>())
            .def("delPoint", &vmPoly2DWrapper::delPoint)
            .def("getNum", &vmPoly2D::getNum)
            .def("simplify", &vmPoly2D::simplify)
            .def("setGeometry", &vmPoly2DWrapper::setGeometry)
            .def("setGeometry", &vmPoly2DWrapper::setGeometryValues)
            .def("getGeometry", &vmPoly2D::getGeometry, return_value_policy<return_by_value>())
            .def("makeRawMask", &vmPoly2DWrapper::makeRawMask, return_internal_reference<>())
            .def("setTaperWidth", &vmPoly2D::setTaperWidth)
            .def("getTaperWidth", &vmPoly2D::getTaperWidth)
            .def("setReverse", &vmPoly2D::setReverse)
            .def("isReverse", &vmPoly2D::isReverse);


        
        class_<vmMultiPoly2DWrapper, boost::noncopyable>("MultiPolygon2D")
            .def("addPolygon", &vmMultiPoly2D::addPolygon, return_internal_reference<>())
            .def("removePolygon", &vmMultiPoly2D::removePolygon)
            .def("polygon", fncPolygon, return_internal_reference<>())
            .def("getPolygon", &vmMultiPoly2DWrapper::getPolygon, return_value_policy<manage_new_object>())
            //may need this?       .def("getPolygon", &vmMultiPoly2D::getPolygon,return_value_policy<return_by_value>())
            .def("numPolygons", &vmMultiPoly2D::numPolygons)
            .def("getNum", &vmMultiPoly2D::getNum)
            .def("addPoint", &vmMultiPoly2DWrapper::addPoint)
            .def("addPointToCurrent", &vmMultiPoly2DWrapper::addPointToCurrent)
            .def("simplifyPolygon", &vmMultiPoly2D::simplifyPolygon)
            .def("curPolygonIdx", &vmMultiPoly2D::curPolygonIdx)
            .def("setCurPolygonIdx", &vmMultiPoly2D::setCurPolygonIdx)
            .def("delPoint", &vmMultiPoly2D::delPoint)
            .def("setPointErr", &vmMultiPoly2D::setPointErr)
            .def("setGeometry", &vmMultiPoly2DWrapper::setGeometry)
            .def("setGeometry", &vmMultiPoly2DWrapper::setGeometryValues)
            .def("getGeometry", &vmMultiPoly2D::getGeometry, return_internal_reference<>())
            //may need this?          .def("fetchGeometry", &vmMultiPoly2D::fetchGeometry, return_value_policy<return_by_value>())
            .def("setTaperWidth", &vmMultiPoly2D::setTaperWidth)
            .def("getTaperWidth", &vmMultiPoly2D::getTaperWidth)
            .def("setReverse", &vmMultiPoly2D::setReverse)
            .def("isReverse", &vmMultiPoly2D::isReverse)
            .def("load", loadMPolyFromFile, vmMultiPoly2DWrapper_load_overloads())
            .def("save", saveMPolyToFile, vmMultiPoly2DWrapper_save_overloads());
    //may need this?            .def("generateAllRegionsFromHorizon", &vmMultiPoly2D::generateAllRegionsFromHrz)
    //modify to not send signals       .def("generateCurrentRegionFromHorizon", &vmMultiPoly2D::generateCurrentRegionFromHrz);


        class_<vmSurveyRegionWrapper, boost::noncopyable>("SurveyRegion")
            .def("grdInside", &SurveyRegion::grdInside)
            .def("linInside", &SurveyRegion::linInside)
            .def("logInside", &SurveyRegion::logInside)
            .def("wldInside", &SurveyRegion::wldInside)
            .def("getGeometry", &vmSurveyRegionWrapper::getGeometry, return_value_policy<manage_new_object>())
            .def("setGeometry", &SurveyRegion::setGeometry)
            .def("isRectDefined", &SurveyRegion::isRectDefined)
            .def("clearRectangle", &SurveyRegion::clearRect)
            .def("setPolygonCoordinates", &vmSurveyRegionWrapper::setPolygonCoordinates)
            .def("regionPolygon", &SurveyRegion::regionPolygon, return_value_policy<return_by_value>())
            .def("getType", &SurveyRegion::getType)
            .def("setType", &SurveyRegion::setType)
            .def("makeMask", &SurveyRegion::makeMask)
            .def("clear", &SurveyRegion::clear);
    }
}

vmPoly2D* fncPolygon(vmMultiPoly2D* caller, int idx) {
    return caller->polygon(idx);
}

