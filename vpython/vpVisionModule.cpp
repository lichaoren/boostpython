#include "vpVisionModule.hpp"

#include <boost/python.hpp>

#include "vpCaptureImage.hpp"
#include "vpVisionAPI.hpp"
#include "vpPointAnnotation.hpp"

#include "vlLasWellLog.hpp"
#include "vlVolume.hpp"
#include "vlHorizonGroup.hpp"


static vpVisionAPI& vision = vpVisionAPI::getInstance();


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpVisionAPI_addAttrOverload, addAttribute, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( vision_getWellList_overloads, getWellList, 0, 1)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS( vision_spectrum_overloads, setCurrentSpectrum, 3, 4)


vpVisionModule::vpVisionModule(const char* package)
{
    using namespace boost::python;

    //map vision namespace to a (sub)module
    //make "from [package.]vision import *" work
    std::string modulename = (package) ? std::string(package)+".vision"
                                       : std::string("vision");
    object visionModule(handle<>(borrowed(PyImport_AddModule(modulename.c_str()))));
    
    //make "[from package ]import vision" work
    //as well as import [package.]vision
    scope().attr("vision") = visionModule;

    //need these parentheses for the scoping of the following classes
    //(so that they are properly "placed inside" vision module)
    {
        //change scope to vision to add classes within vision module
        scope vision_scope = visionModule;

        // Image capture
        vpCaptureImage::exposePython();
        vpPointAnnotation::exposePython();
        
                
        enum_<WellSelectionMode>("WellSelectionMode")
            .value("ALL", WELL_ALL)
            .value("WITH_SONIC", WELL_WITH_SONIC)
            .value("WITH_TOP", WELL_WITH_TOPS)
            .value("WITH_CHECKSHOT", WELL_WITH_CHECKSHOT)
            ;

        
        //add definitions in vision package
        class_<vpVisionAPI, boost::shared_ptr<vpVisionAPI>, boost::noncopyable>
            ("Vision", no_init)
            .def("addAttribute", &vpVisionAPI::addAttribute, vpVisionAPI_addAttrOverload())
            .def("addHorizonGroup", &vpVisionAPI::addHorizonGroup)
        
            .def("getCurrentAttribute", &vpVisionAPI::getCurrentAttribute,
            return_internal_reference<>())
            .def("getCurrentHorizon", &vpVisionAPI::getCurrentHorizon,
            return_internal_reference<>())
            .def("setCurrentHorizon", &vpVisionAPI::setCurrentHorizon)        
            .def("getCurrentHorizonGroup", &vpVisionAPI::getCurrentHorizonGroup,
            return_internal_reference<>())
             
            .def("setCurrentHorizonGroup", &vpVisionAPI::setCurrentHorizonGroup)  
            .def("removeHorizonGroup", &vpVisionAPI::removeHorizonGroup)        
        
            .def("getCurrentSeismic", &vpVisionAPI::getCurrentSeismic,
            return_internal_reference<>())
            .def("getAttribute", &vpVisionAPI::getAttribute,
            return_internal_reference<>())
            .def("getHorizonGroup", &vpVisionAPI::getHorizonGroup,
            return_internal_reference<>())
            .def("getSeismic", &vpVisionAPI::getSeismic,
            return_internal_reference<>())
            .def("getGather", &vpVisionAPI::getGather,
            return_internal_reference<>())
            .def("getWell", &vpVisionAPI::getWell, return_internal_reference<>())
            .def("getWellList",  &vpVisionAPI::getWellList, vision_getWellList_overloads())
        
            .def("showWell",  &vpVisionAPI::showWellList)
            .def("showWell",  &vpVisionAPI::showWell)

            .def("setCurrentSpectrum", &vpVisionAPI::setCurrentSpectrum, vision_spectrum_overloads())        
        
            .def("getCursorLog", &vpVisionAPI::getCursorLog)
            .def("getCursorLine", &vpVisionAPI::getCursorLine)
            .def("refresh", &vpVisionAPI::refresh)
            .def("getUserArg",&vpVisionAPI::getUserArg)
            ;
        
        scope().attr("vision") = object(ptr(&vision));
    }
}
