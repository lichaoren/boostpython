/* 
 * File:   vpCaptureImage.cpp
 * Author: ddoledec
 * 
 * Created on January 12, 2016, 2:52 PM
 */

#include "vpCaptureImage.hpp"

#include <boost/python.hpp> 

#include "vmReportBookmarkDisplay.hpp"
#include "vmBookmarkLocation.hpp"
#include "vmCaptureImageFactory.hpp"
#include "vmImageRecorder.hpp"
#include "vmReportParameters.hpp"
#include "vmReportDisplayParameters.hpp"
#include "vmUserNoteLocation.hpp"
#include "vmView3DRecorder.hpp"
#include "vmVolumeLocation.hpp"

#include "vpReportFileParameters.hpp"
#include "vpReportDataParameters.hpp"
#include "vpWellLocation.hpp"

vmImageRecorder* createCaptureImage(vmCaptureImageFactory::FactoryType type)
{
    return vmCaptureImageFactory::getInstance().create(type);
}

vpCaptureImage::vpCaptureImage()
{
}

vpCaptureImage::vpCaptureImage(const vpCaptureImage& orig)
{
}

vpCaptureImage::~vpCaptureImage()
{
}
/// Suppress overloading ambiguity
vmReportFileParameters& (vmReportParameters::*getNoConstFileParameters)() = &vmReportParameters::getFileParameters;
vmReportDisplayParameters& (vmReportParameters::*getNoConstDisplayParameters)() = &vmReportParameters::getDisplayParameters;
vmReportDataParameters& (vmReportParameters::*getNoConstDataParameters)() = &vmReportParameters::getDataSelection;
vmDataLocation& (vmReportParameters::*getNoConstLocations)() = &vmReportParameters::getLocations;

    

void vpBookmarkDisplaySelectList(vmReportBookmarkDisplay* bkmrk,boost::python::list& lst)
{
    Py_ssize_t length = boost::python::len(lst);

    std::vector<vString> bkmrkList;
    for (Py_ssize_t i = 0; i < length; i++)
    {
        std::string token = boost::python::extract<string>(lst[i]);
        bkmrkList.push_back(token.c_str());
    }
    
    bkmrk->selectBookmark(bkmrkList);
}

void (vmReportBookmarkDisplay::*vpBookmarkDisplaySelectString)(const vString& ) = &vmReportBookmarkDisplay::selectBookmark;
        


void vpBookmarkSelectList(vmBookmarkLocation* bkmrk,boost::python::list& lst)
{
    Py_ssize_t length = boost::python::len(lst);

    std::vector<vString> bkmrkList;
    for (Py_ssize_t i = 0; i < length; i++)
    {
        std::string token = boost::python::extract<string>(lst[i]);
        bkmrkList.push_back(token.c_str());
    }
    
    bkmrk->selectBookmark(bkmrkList);
}

void (vmBookmarkLocation::*vpBookmarkSelectString)(const vString& ) = &vmBookmarkLocation::selectBookmark;






// Overloaded parameters
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpVolumeLocation_setILoverloads, vmVolumeLocation::setIL, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpVolumeLocation_setXLoverloads, vmVolumeLocation::setXL, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpVolumeLocation_setZLoverloads, vmVolumeLocation::setZ, 1, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpReportDataParameters_addSeismic, vpReportDataParameters::addSeismic, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpReportDataParameters_removeSeismic, vpReportDataParameters::removeSeismic, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpReportDataParameters_addAttribute, vpReportDataParameters::addAttribute, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpReportDataParameters_removeAttribute, vpReportDataParameters::removeAttribute, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpReportDataParameters_addGather, vpReportDataParameters::addGather, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpReportDataParameters_removeGather, vpReportDataParameters::removeGather, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpUserNoteLocation_loadOverloads, vmUserNoteLocation::load, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpBkmrkLocation_loadOverloads, vmBookmarkLocation::load, 1, 1)


// Overloaded methods as functions
bool setDataLocationCDP(vmVolumeLocation* dataLocation, boost::python::list& cdps) {
    int size = boost::python::len(cdps);
    if ( size <= 0)
    {
        cerr << "No CDPs !!" << endl;
        return false;
    }
    
   // Extract first element of first tuple in the list.
    std::vector< std::vector< int > > posList;
    for (int i = 0; i < size; ++i) {
        boost::python::tuple tpl = boost::python::extract< boost::python::tuple>( cdps[i]);
        int il = boost::python::extract<int>(tpl[0]);
        int xl = boost::python::extract<int>(tpl[1]);
        std::vector<int> point;
        point.push_back(il);
        point.push_back(xl);
        posList.push_back(point);
    }
    
    dataLocation->setPositions(posList);

    return true;
}

void vpCaptureImage::exposePython()
{
    using namespace boost::python;
    namespace bp = boost::python;

    // Generic Parameters
    {
        bp::scope param_scope = bp::class_<vmReportParameters>("CaptureParameters")
                .def("getFileParameters", getNoConstFileParameters, return_internal_reference<>())
                .def("setFileParameters", &vmReportParameters::setFileParameters)

                .def("getDisplayParameters", getNoConstDisplayParameters, return_internal_reference<>())
                .def("setDisplayParameters", &vmReportParameters::setDisplayParameters)

                .def("getDataParameters", getNoConstDataParameters, return_internal_reference<>())
                .def("setDataParameters", &vmReportParameters::setDataSelection)

                .def("getLocations", getNoConstLocations, return_internal_reference<>())
                .def("setLocations", &vmReportParameters::setLocations)
                ;
    }

    // File Parameters
    {
        bp::scope file_param_scope = bp::class_<vpReportFileParameters, boost::noncopyable>("CaptureFileParameters")
                .def("setFormat", &vpReportFileParameters::setImageFormat)
                .def("setPath", &vpReportFileParameters::setImagePath)
                .def("setNaming", &vpReportFileParameters::setNaming)
                .def("setPrefix", &vpReportFileParameters::setImagePrefix)
                ;
    }

    // Display Parameters
    {
        bp::scope display_param_scope = bp::class_<vmReportDisplayParameters>("CaptureDisplayParameters")
                .def("getPath", &vmReportDisplayParameters::getPanelSettingPath)
                .def("setPath", &vmReportDisplayParameters::setPanelSettingPath)
                ;
    }
    
    // Bookmark Display
    {
        bp::enum_<vmReportBookmarkDisplay::ColormapType>("ColorMapMode")
            .value("BOOKMARK_COLORS", vmReportBookmarkDisplay::BOOKMARK_COLORS)
            .value("CURRENT_COLORS", vmReportBookmarkDisplay::CURRENT_COLORS);

        bp::enum_<vmView3DRecorder::DataType>("DataType")
            .value("HORIZON", vmView3DRecorder::APPLY_DATA_HORIZON)
            .value("WELL", vmView3DRecorder::APPLY_DATA_WELL)
            .value("ATTRIBUTE", vmView3DRecorder::APPLY_DATA_ATTRIBUTE)
            .value("SEISMIC", vmView3DRecorder::APPLY_DATA_SEISMIC)
            .value("ALL", vmView3DRecorder::APPLY_DATA_ALL);
        
        
        bp::scope vol_location_param_scope = bp::class_<vmReportBookmarkDisplay, bases < vmReportDisplayParameters > > ("BookmarkDisplay")
                .def("selectBookmark", vpBookmarkDisplaySelectString )
                .def("selectBookmark", vpBookmarkDisplaySelectList )
                .def("setColormap", &vmReportBookmarkDisplay::setColormap )
                .def("setXML", &vmReportBookmarkDisplay::setXML )
                .def("load", &vmReportBookmarkDisplay::setPanelSettingPath);
        
        vol_location_param_scope.attr("BOOKMARK_COLORS") = vmReportBookmarkDisplay::BOOKMARK_COLORS; 
        vol_location_param_scope.attr("CURRENT_COLORS") = vmReportBookmarkDisplay::CURRENT_COLORS; 
        
           
        vol_location_param_scope.attr("HORIZON") = vmView3DRecorder::APPLY_DATA_HORIZON; 
        vol_location_param_scope.attr("WELL") = vmView3DRecorder::APPLY_DATA_WELL; 
        vol_location_param_scope.attr("ATTRIBUTE") = vmView3DRecorder::APPLY_DATA_ATTRIBUTE; 
        vol_location_param_scope.attr("SEISMIC") = vmView3DRecorder::APPLY_DATA_SEISMIC; 
        vol_location_param_scope.attr("ALL") = vmView3DRecorder::APPLY_DATA_ALL; 
    }    
    
    

    // Data Parameters
    {
        bp::scope data_param_scope = bp::class_<vpReportDataParameters, boost::noncopyable>("CaptureDataParameters")
                .def("addSeismic", &vpReportDataParameters::addSeismic, vpReportDataParameters_addSeismic())
                .def("removeSeismic", &vpReportDataParameters::removeSeismic, vpReportDataParameters_removeSeismic())
                .def("addAttribute", &vpReportDataParameters::addAttribute, vpReportDataParameters_addAttribute())
                .def("removeAttribute", &vpReportDataParameters::removeAttribute, vpReportDataParameters_removeAttribute())
                .def("addGather", &vpReportDataParameters::addGather, vpReportDataParameters_addGather())
                .def("removeGather", &vpReportDataParameters::removeGather, vpReportDataParameters_removeGather())
                .def("clear", &vpReportDataParameters::clear)
                .def("size", &vpReportDataParameters::clear)
                ;
    }

    // Location Parameters
    {
        bp::scope location_param_scope = bp::class_<vmDataLocation, boost::noncopyable>("DataLocation", no_init)
                .def("getIL", &vmDataLocation::getIL)
                .def("getXL", &vmDataLocation::getXL)
                ;
    }

    // Volume  Location
    {
        bp::enum_<vmVolumeLocation::Direction>("DataDirection")
                .value("IL", vmVolumeLocation::InlineDirection)
                .value("XL", vmVolumeLocation::CrosslineDirection)
                .value("TIME", vmVolumeLocation::TimeDirection)
                ;
        bp::scope vol_location_param_scope = bp::class_<vmVolumeLocation, bases < vmDataLocation > > ("VolumeLocation")
                .def("setIL", &vmVolumeLocation::setIL, vpVolumeLocation_setILoverloads())
                .def("setXL", &vmVolumeLocation::setXL, vpVolumeLocation_setXLoverloads())
                .def("setZ", &vmVolumeLocation::setZ, vpVolumeLocation_setZLoverloads())
                .def("setPositions", setDataLocationCDP )
                ;

        vol_location_param_scope.attr("IL") = vmVolumeLocation::InlineDirection;
        vol_location_param_scope.attr("XL") = vmVolumeLocation::CrosslineDirection;
        vol_location_param_scope.attr("TIME") = vmVolumeLocation::TimeDirection;
    }

    // Well location
    {
        bp::scope well_location_param_scope = bp::class_<vpWellLocation, bases < vmDataLocation > > ("WellLocation")
                .def("size", &vpWellLocation::size)
                .def("addWell", &vpWellLocation::addWell)
                ;

    }
    
    // User Notes Location
    {
        bp::scope well_location_param_scope = bp::class_<vmUserNoteLocation, bases < vmDataLocation > > ("UserNoteLocation")
                .def("size", &vmUserNoteLocation::size)
                .def("recenterView", &vmUserNoteLocation::setRecenterView)
                .def("setUserNoteIndex", &vmUserNoteLocation::setCurrentUserNote)
                .def("load", &vmUserNoteLocation::load, vpUserNoteLocation_loadOverloads())
                ;        
        
    }

    // Bookmark Location
    {
        bp::scope bkmrk_location_param_scope = bp::class_<vmBookmarkLocation, bases < vmDataLocation > > ("BookmarkLocation")
                .def("size", &vmBookmarkLocation::size)
                .def("selectBookmark", vpBookmarkSelectString )
                .def("selectBookmark", vpBookmarkSelectList )
                .def("updateDisplay",  &vmBookmarkLocation::updateDisplay)
                .def("load", &vmBookmarkLocation::load, vpBkmrkLocation_loadOverloads())
                ;        
        
    }    
    
    

    // Image Recorder
    {
        bp::enum_<vmCaptureImageFactory::FactoryType>("Window")
                .value("MAIN_VIEW", vmCaptureImageFactory::MAIN_VIEW)
                .value("GATHER", vmCaptureImageFactory::GATHER)
                .value("ATTR_PROFILE", vmCaptureImageFactory::ATTR_PROFILE);


        bp::scope recorder_param_scope = bp::class_<vmImageRecorder>("ImageRecorder")
                .def("capture", &vmImageRecorder::capture)
                ;

        recorder_param_scope.attr("MAIN_VIEW") = vmCaptureImageFactory::MAIN_VIEW;
        recorder_param_scope.attr("GATHER") = vmCaptureImageFactory::GATHER;
        recorder_param_scope.attr("ATTR_PROFILE") = vmCaptureImageFactory::ATTR_PROFILE;
    }


    // Factory 
    {
        def("captureImage", createCaptureImage, return_value_policy<manage_new_object>());
    }

}



