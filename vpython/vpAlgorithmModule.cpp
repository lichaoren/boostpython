#include "vpAlgorithmModule.hpp"
#include <boost/python.hpp>

#include "GridInterp.hpp"

// Volume

#include "vpRcaSmoothAPI.hpp"
#include "vpAttrReplaceAPI.hpp"
#include "vpInsertSaltAPI.hpp"
#include "vpVolExtractAttribute.hpp"
#include "vpVolRegridAPI.hpp"
#include "vpVolSmoothAttribute.hpp"
#include "vpVolumeDipConverter.hpp"
#include "vpVolumeDipCreator.hpp"
#include "vpMathAPI.hpp"
#include "vpMergeFdmAPI.hpp"
#include "vpAttrUnitConvert.hpp"
#include "vmUnitConst.hpp"

// Horizon
#include "vlHorizonResample.hpp"

#include "vpHrzSnap.hpp"
#include "vpHrzSmooth.hpp"
#include "vpHrzInterpolate.hpp"
#include "vpVolumeProxy.hpp"
#include "vpHorizonRegrid.hpp"
#include "vpHorizonTrimmer.hpp"
#include "vpConvertHrzDomain.hpp"
#include "vpRayTracing.hpp"
#include "vpWellTieMap.hpp"



static double convertUnit(vmUnitConst::unit input, vmUnitConst::unit output) {
    static vmUnitConst conv;
    return conv.getConvertConst(input, output);
}


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpRcaSmoothAPI_overloads, setup, 4, 6)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpAttrReplaceAPI_Rangeoverloads, vpAttrReplaceAPI::setRange, 2, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpAttrReplaceAPI_Typeoverloads, vpAttrReplaceAPI::setReplaceType, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpAttrReplaceAPI_Moduleoverloads, vpAttrReplaceAPI::doReplace_Module, 2, 4)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vlHorizonResample_overloads, vlHorizonResample::run, 1, 2)

vpAlgorithmModule::vpAlgorithmModule(const char* package)
{
    using namespace boost::python;

    //map algorithm namespace to a (sub)module
    //make "from [package.]algorithm import *" work
    std::string modulename = (package) ? std::string(package)+".algorithm"
                                       : std::string("algorithm");
    object algorithmModule(handle<>(borrowed(PyImport_AddModule(modulename.c_str()))));
    
    //make "[from package ]import algorithm" work
    //as well as import package.algorithm
    scope().attr("algorithm") = algorithmModule;

    //need these parentheses for the scoping of the following classes
    //(so that they are properly "placed inside" algorithm module)
    {
        //change scope to algorithm to add classes within algorithm module
        scope algorithm_scope = algorithmModule;

        //add definitions for algorithm package
        vpHrzSnap::exposePython();
        vpHrzSmooth::exposePython();
        vpHorizonRegrid::exposePython();
        vpHorizonTrimmer::exposePython();
        vpConvertHrzDomain::exposePython();
        vpWellTieMap::exposePython();

        vpVolExtractAttribute::exposePython();
        vpVolSmoothAttribute::exposePython();
        vpInsertSaltAPI::exposePython();
        vpVolumeDipConverter::exposePython();  // convert dip units (angle, dipx...)
        vpVolumeRegrid::exposePython();
        vpVolumeDipCreator::exposePython();  // compute dipx/dipy

        class_<vlHorizonResample>("HorizonResample")
                .def("setHorizonGroup", &vlHorizonResample::setInputHorizon)
                .def("run", &vlHorizonResample::run, return_value_policy<manage_new_object>()) //, vlHorizonResample_overloads() )
                .def("setInterpolate", &vlHorizonResample::setInterpolate)
                .def("setGeom", &vlHorizonResample::setGeom);
        
        
        enum_<RcaUnits>("unit")
            .value("radian", radian)
            .value("degree", degree)
            .value("meter", meter)
            ;
        enum_<vpVolumeProxy::Direction>("Direction")
            .value("XLINE", vpVolumeProxy::XLINE)
            .value("ILINE", vpVolumeProxy::ILINE)
            .value("ZLINE", vpVolumeProxy::ZLINE)
            ;
        enum_<vmUnitConst::unit>("LengthUnit")
            .value("METER", vmUnitConst::METERS)
            .value("FEET", vmUnitConst::FEET)
            .value("INTL_FEET", vmUnitConst::INTL_FEET)
            ;
        def("convertUnit", &convertUnit);
        enum_<vmAttrReplaceNMT::Mode>("ReplaceMode")
            .value("LINEAR", vmAttrReplaceNMT::LINEAR)
            .value("NATURAL_NEIGHBOR", vmAttrReplaceNMT::NATURAL_NEIGHBOR)
            .value("MINIMUM_CURVATURE", vmAttrReplaceNMT::MINIMUM_CURVATURE)
            .value("NEAREST_NEIGHBOR", vmAttrReplaceNMT::NEAREST_NEIGHBOR)
            .value("HRZ_NEAREST_NEIGHBOR", vmAttrReplaceNMT::HRZ_NEAREST_NEIGHBOR)
            .value("KRIGING", vmAttrReplaceNMT::KRIGING)
            .value("FIXED_VALUE", vmAttrReplaceNMT::FIXED_VALUE)
            .value("SCALED_VALUE", vmAttrReplaceNMT::SCALED_VALUE)
            .value("VERTICAL_GRADIENT", vmAttrReplaceNMT::VERTICAL_GRADIENT)
            .value("VERTICAL_LINEAR", vmAttrReplaceNMT::VERTICAL_LINEAR);

        enum_<vmAttrReplaceNMT::DataSel::RangeType>("ReplaceRangeType")
            .value("MinMax", vmAttrReplaceNMT::DataSel::MinMax)
            .value("Greater", vmAttrReplaceNMT::DataSel::Greater)
            .value("GreaterOrEqual", vmAttrReplaceNMT::DataSel::GreaterOrEqual)
            .value("Less", vmAttrReplaceNMT::DataSel::Less)
            .value("LessOrEqual", vmAttrReplaceNMT::DataSel::LessOrEqual);
        enum_<vmAttrReplaceNMT::RegionSel::Type>("ReplaceRegionType")
            .value("All", vmAttrReplaceNMT::RegionSel::All)
            .value("Current", vmAttrReplaceNMT::RegionSel::CurrRegn)
            .value("Outside", vmAttrReplaceNMT::RegionSel::OutCurrRegn);
        enum_<GridInterp::Mode>("InterpMode")
            .value("LINEAR", GridInterp::LINEAR)
            .value("NATRUAL_NEIGHBOR", GridInterp::NATURAL_NEIGHBOR)
            .value("MINIMUM_CURVATURE", GridInterp::MINIMUM_CURVATURE)
            .value("NEAREST_NEIGHBOR", GridInterp::NEAREST_NEIGHBOR);

        enum_<AngleConv::Type>("AngleType")
            .value("DegreeSigned", AngleConv::DegSigned)
            .value("DegreeUnsigned", AngleConv::DegUnsigned)
            .value("RadianSigned", AngleConv::RadSigned)
            .value("RadianUnsigned", AngleConv::RadUnsigned);

        enum_<vpMergeFdmAPI::TracePositioning>("MergeCombineOption")
            .value("ByLine", vpMergeFdmAPI::ByLine)
            .value("ByCoordinate", vpMergeFdmAPI::ByCoordinate);
    
        class_<vpRcaSmoothAPI>("RcaSmooth", vpRcaSmoothAPI::getRcaSmoothHelp())
            .def("setup", &vpRcaSmoothAPI::setup, vpRcaSmoothAPI_overloads())
            .def("smooth", &vpRcaSmoothAPI::smooth, return_value_policy<manage_new_object>())
            .def_readwrite("objectVolume", &vpRcaSmoothAPI::myInput)
            ;
   
        class_<vpAttrUnitConvert>("AttrUnitConvert")
            .def("setup", &vpAttrUnitConvert::setConvert)
            .def("moduleConvert", &vpAttrUnitConvert::convert_module)
            .def("convert", &vpAttrUnitConvert::convert)
        ;
        class_<vpHrzInterpolate>("GridInterp")
            .def("interp", &vpHrzInterpolate::interp)
            .def("setRegion", &vpHrzInterpolate::setRegion)
            .def("clearRegion", &vpHrzInterpolate::clearRegion)
            .def_readwrite("IsWallToWall", &vpHrzInterpolate::IsWallToWall)
            .def_readwrite("Mode", &vpHrzInterpolate::Mode)
            .def_readwrite("Radius", &vpHrzInterpolate::Radius);
        class_<vpAttrReplaceAPI>("AttrReplace")
            .def("replaceInf", &vpAttrReplaceAPI::replaceInf)
            .def("replaceNAN", &vpAttrReplaceAPI::replaceNAN)
            .def("setRange", &vpAttrReplaceAPI::setRange, vpAttrReplaceAPI_Rangeoverloads())
            .def("setReplaceType", &vpAttrReplaceAPI::setReplaceType, vpAttrReplaceAPI_Typeoverloads())
            .def("doReplace", &vpAttrReplaceAPI::doReplace)
            .def("setAttribute", &vpAttrReplaceAPI::setVol)
            .def("setAngleType", &vpAttrReplaceAPI::setAngleType)
            .def("setRegion", &vpAttrReplaceAPI::setRegion)
            .def("clearRegion", &vpAttrReplaceAPI::clearRegion)
            .def("unsetAngleType", &vpAttrReplaceAPI::unSetAngleType)
            .def("doReplaceFromModule", &vpAttrReplaceAPI::doReplace_Module, vpAttrReplaceAPI_Moduleoverloads());
    
        class_<vpMergeFdmAPI>("MergeFdm")
            .def("setInMemData", &vpMergeFdmAPI::setInMemList)
            .def("setOnDiskData", &vpMergeFdmAPI::setOnDiskList)
            .def("setNullValue", &vpMergeFdmAPI::setNullValue)
            .def("setCombineOption", &vpMergeFdmAPI::setCombineOpt)
            .def("setGeom", &vpMergeFdmAPI::setGeometry)
            .def("setTargetName", &vpMergeFdmAPI::setTargetName)
            .def("setToSmoothBoundary", &vpMergeFdmAPI::setToSmoothBoundary)
            .def("setSmoothBoundaryRadius", &vpMergeFdmAPI::setSmoothBoundaryRadius)
            .def("setSaveToMemory", &vpMergeFdmAPI::setSaveToMemory)
            .def("setSaveToDisk", &vpMergeFdmAPI::setSaveToDisk)
            .def("doMerge", &vpMergeFdmAPI::doMerge)
            .def("getMergedVolume", &vpMergeFdmAPI::getMergeVolume,return_value_policy<manage_new_object>() )
            .def("setRegion", &vpMergeFdmAPI::setRegion)
            .def("setHorizon", &vpMergeFdmAPI::setHorizon)
            ;
        // We want to respect the original dialog, therefore, we still keep those time_xxx here
        // in order to support overloading, we implement each possibility
        class_<vpMathAPI>("VolumeMath")
            .def(init<>())
            .def(init<float>())
            .def(init < vgHorizonGroup*, boost::python::list, bool>())
            .def(init < vgHorizonGroup*, boost::python::list, bool, float>())
            .def("plus", &vpMathAPI::plus<float, Volume*>, return_value_policy<manage_new_object>())
            .def("plus", &vpMathAPI::plus<Volume*, float>, return_value_policy<manage_new_object>())
            .def("plus", &vpMathAPI::plus<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("minus", &vpMathAPI::minus<float, Volume*>, return_value_policy<manage_new_object>())
            .def("minus", &vpMathAPI::minus<Volume*, float>, return_value_policy<manage_new_object>())
            .def("minus", &vpMathAPI::minus<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("multiply", &vpMathAPI::multiply<float, Volume*>, return_value_policy<manage_new_object>())
            .def("multiply", &vpMathAPI::multiply<Volume*, float>, return_value_policy<manage_new_object>())
            .def("multiply", &vpMathAPI::multiply<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("divide", &vpMathAPI::divide<float, Volume*>, return_value_policy<manage_new_object>())
            .def("divide", &vpMathAPI::divide<Volume*, float>, return_value_policy<manage_new_object>())
            .def("divide", &vpMathAPI::divide<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("time_sin", &vpMathAPI::time_sin<float, Volume*>, return_value_policy<manage_new_object>())
            .def("time_sin", &vpMathAPI::time_sin<Volume*, float>, return_value_policy<manage_new_object>())
            .def("time_sin", &vpMathAPI::time_sin<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("sin", &vpMathAPI::sin, return_value_policy<manage_new_object>())
            .def("time_cos", &vpMathAPI::time_cos<float, Volume*>, return_value_policy<manage_new_object>())
            .def("time_cos", &vpMathAPI::time_cos<Volume*, float>, return_value_policy<manage_new_object>())
            .def("time_cos", &vpMathAPI::time_cos<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("cos", &vpMathAPI::cos, return_value_policy<manage_new_object>())
            .def("time_tan", &vpMathAPI::time_tan<float, Volume*>, return_value_policy<manage_new_object>())
            .def("time_tan", &vpMathAPI::time_tan<Volume*, float>, return_value_policy<manage_new_object>())
            .def("time_tan", &vpMathAPI::time_tan<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("tan", &vpMathAPI::tan, return_value_policy<manage_new_object>())
            .def("time_arcsin", &vpMathAPI::time_arcsin<float, Volume*>, return_value_policy<manage_new_object>())
            .def("time_arcsin", &vpMathAPI::time_arcsin<Volume*, float>, return_value_policy<manage_new_object>())
            .def("time_arcsin", &vpMathAPI::time_arcsin<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("arcsin", &vpMathAPI::arcsin, return_value_policy<manage_new_object>())
            .def("time_arccos", &vpMathAPI::time_arccos<float, Volume*>, return_value_policy<manage_new_object>())
            .def("time_arccos", &vpMathAPI::time_arccos<Volume*, float>, return_value_policy<manage_new_object>())
            .def("time_arccos", &vpMathAPI::time_arccos<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("arccos", &vpMathAPI::arccos, return_value_policy<manage_new_object>())
            .def("time_arctan", &vpMathAPI::time_arctan<float, Volume*>, return_value_policy<manage_new_object>())
            .def("time_arctan", &vpMathAPI::time_arctan<Volume*, float>, return_value_policy<manage_new_object>())
            .def("time_arctan", &vpMathAPI::time_arctan<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("arctan", &vpMathAPI::arctan, return_value_policy<manage_new_object>())
            .def("sqrt", &vpMathAPI::sqrt, return_value_policy<manage_new_object>())
            .def("time_log10", &vpMathAPI::time_log10<float, Volume*>, return_value_policy<manage_new_object>())
            .def("time_log10", &vpMathAPI::time_log10<Volume*, float>, return_value_policy<manage_new_object>())
            .def("time_log10", &vpMathAPI::time_log10<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("log10", &vpMathAPI::log10, return_value_policy<manage_new_object>())
            .def("min", &vpMathAPI::min<float, Volume*>, return_value_policy<manage_new_object>())
            .def("min", &vpMathAPI::min<Volume*, float>, return_value_policy<manage_new_object>())
            .def("min", &vpMathAPI::min<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("max", &vpMathAPI::max<float, Volume*>, return_value_policy<manage_new_object>())
            .def("max", &vpMathAPI::max<Volume*, float>, return_value_policy<manage_new_object>())
            .def("max", &vpMathAPI::max<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("mod", &vpMathAPI::mod<float, Volume*>, return_value_policy<manage_new_object>())
            .def("mod", &vpMathAPI::mod<Volume*, float>, return_value_policy<manage_new_object>())
            .def("mod", &vpMathAPI::mod<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("pow", &vpMathAPI::pow<float, Volume*>, return_value_policy<manage_new_object>())
            .def("pow", &vpMathAPI::pow<Volume*, float>, return_value_policy<manage_new_object>())
            .def("pow", &vpMathAPI::pow<Volume*, Volume*>, return_value_policy<manage_new_object>())
            .def("abs", &vpMathAPI::abs, return_value_policy<manage_new_object>())
            .def("d_dx", &vpMathAPI::d_dx, return_value_policy<manage_new_object>())
            .def("d_dy", &vpMathAPI::d_dy, return_value_policy<manage_new_object>())
            .def("d_dz", &vpMathAPI::d_dz, return_value_policy<manage_new_object>())
            .def("sum_top_down", &vpMathAPI::sum_top_down, return_value_policy<manage_new_object>())
            .def("sum_btm_up", &vpMathAPI::sum_btm_up, return_value_policy<manage_new_object>())
            .def("setInputVolumes", &vpMathAPI::setInput)
            .def("setOutputPath", &vpMathAPI::setOutput)
            .def("SetFormula", &vpMathAPI::setFormula)
            .def("calculate", &vpMathAPI::doMath)
            .def("sin", &vpMathAPI::valSin)
            .def("arcsin", &vpMathAPI::valArcsin)
            .def("cos", &vpMathAPI::valCos)
            .def("arccos", &vpMathAPI::valArccos)
            .def("tan", &vpMathAPI::valTan)
            .def("arctan", &vpMathAPI::valArctan)
            .def("log10", &vpMathAPI::valLog10)
            .def("abs", &vpMathAPI::valAbs)
            .def("pow", &vpMathAPI::valPower)
            .def("sqrt", &vpMathAPI::valSqrt)
            ;
        class_<vpRayTracing>("ShotSel")
            .def("start", &vpRayTracing::onShootRays)
            .def("save", &vpRayTracing::onSave)
            .def("parse", &vpRayTracing::onParse)  
            .def("setMethod", &vpRayTracing::setMethod)
            .def("setVelocityPath", &vpRayTracing::setVelocityPath) 
            .def("setRefHorizon", &vpRayTracing::setRefHorizon) 
            .def("setWaterBottom", &vpRayTracing::setWaterBottom) 
            .def("setRayAngle", &vpRayTracing::setRayAngle) 
            .def("setAcceptAllRays", &vpRayTracing::setAcceptAllRays) 
            .def("setAcceptCompleteRays", &vpRayTracing::setAcceptCompleteRays) 
            .def("setAcceptRaysOnSurfaceLine", &vpRayTracing::setAcceptRaysOnSurfaceLine) 
            .def("setShotUp", &vpRayTracing::setShotUp)  
            .def("setShotDown", &vpRayTracing::setShotDown) 
            .def("setDepthDipAsSupplied", &vpRayTracing::setDepthDipAsSupplied) 
            .def("setDepthDipAsHorizon", &vpRayTracing::setDepthDipAsHorizon)
            .def("setMultiSourceFile", &vpRayTracing::setMultiSourceFile)
            .def("setMultiSourceInLine", &vpRayTracing::setMultiSourceInLine)
            .def("setMultiSourceInDirLine", &vpRayTracing::setMultiSourceInDirLine)
            .def("setMultiSourceSeisFile", &vpRayTracing::setMultiSourceSeisFile)
            .def("setMultiSourceSrcRecFile", &vpRayTracing::setMultiSourceSrcRecFile)
            .def("setRoseDiagram", &vpRayTracing::setRoseDiagram)
            .def("setShotFold", &vpRayTracing::setShotFold)
            .def("setRayPath", &vpRayTracing::setRayPath)
            .def("setSummary", &vpRayTracing::setSummary)
            .def("setThreadNum", &vpRayTracing::setThreadNum)
            .def_readwrite("method", &vpRayTracing::myMethod)
            .def_readwrite("velocityPath", &vpRayTracing::myVelPath)
            .def_readwrite("deltaPath", &vpRayTracing::myDelPath)
            .def_readwrite("epilsonPath", &vpRayTracing::myEpiPath)
            .def_readwrite("thetaPath", &vpRayTracing::myThePath)
            .def_readwrite("phiPath", &vpRayTracing::myPhiPath)
            .def_readwrite("horizonPath", &vpRayTracing::myHrzPath)
            .def_readwrite("reflectionHrzName", &vpRayTracing::myRefHrz)
            .def_readwrite("wattomBottomPath", &vpRayTracing::myWBPath)
            .def_readwrite("wattomBottomHrzName", &vpRayTracing::myWBHrz)
            .def_readwrite("rayAngleMinX", &vpRayTracing::myMinAX)
            .def_readwrite("rayAngleMaxX", &vpRayTracing::myMaxAX)
            .def_readwrite("rayAngleNumX", &vpRayTracing::myNumAX)
            .def_readwrite("rayAngleNumY", &vpRayTracing::myNumAY)
            .def_readwrite("rayAngleNumAzimuth", &vpRayTracing::myNumAAz)
            .def_readwrite("rayAngleMinY", &vpRayTracing::myMinAY)
            .def_readwrite("rayAngleMaxY", &vpRayTracing::myMaxAY)
            .def_readwrite("rayAngleMinAzimuth", &vpRayTracing::myMinAAz)
            .def_readwrite("rayAngleMaxAzimuth", &vpRayTracing::myMaxAAz)
            .def_readwrite("rayAngleMax", &vpRayTracing::myMaxRayAngle)
            .def_readwrite("RayToAccept", &vpRayTracing::myAcceptRays)
            .def_readwrite("Receiver", &vpRayTracing::mySurface)
            .def_readwrite("FirstX", &vpRayTracing::myFirstx)
            .def_readwrite("LastX", &vpRayTracing::myLastx)
            .def_readwrite("FirstY", &vpRayTracing::myFirsty)
            .def_readwrite("LastY", &vpRayTracing::myLasty)
            .def_readwrite("Radius", &vpRayTracing::myRadius)
            .def_readwrite("IncX", &vpRayTracing::myIncx)
            .def_readwrite("IncY", &vpRayTracing::myIncy)
            .def_readwrite("ReceiverDepth", &vpRayTracing::myRecvDepth)
            .def_readwrite("ShotDirection", &vpRayTracing::myUp)
            .def_readwrite("DepthDipMode", &vpRayTracing::myDepthDipMode)
            .def_readwrite("DipX", &vpRayTracing::myDipInX)
            .def_readwrite("DipY", &vpRayTracing::myDipInY)
            .def_readwrite("DipHrz", &vpRayTracing::myDipHrz)
            .def_readwrite("SourceMode", &vpRayTracing::mySrcMode)
            .def_readwrite("SourceFile", &vpRayTracing::mySrcFile)
            .def_readwrite("SourcePosX", &vpRayTracing::myX)
            .def_readwrite("SourcePosY", &vpRayTracing::myY)
            .def_readwrite("SourcePosZ", &vpRayTracing::myZ)
            .def_readwrite("SourceIncX", &vpRayTracing::myShotXInc)
            .def_readwrite("SourceIncY", &vpRayTracing::myShotYInc)
            .def_readwrite("SourceNum", &vpRayTracing::myShotNum)
            .def_readwrite("SourceLineIncX", &vpRayTracing::myLineXInc)
            .def_readwrite("SourceLineIncY", &vpRayTracing::myLineYInc)
            .def_readwrite("SourceLineNum", &vpRayTracing::myLineNum)
            .def_readwrite("SourceDircLineSpace", &vpRayTracing::myLineSpacing)
            .def_readwrite("SourceDircShotSpace", &vpRayTracing::myShotSpacing)
            .def_readwrite("SourceDircLineAngle", &vpRayTracing::myLineAngle)
            .def_readwrite("SeismicGrpName", &vpRayTracing::mySeisGrp)
            .def_readwrite("SeismicShotXAttr", &vpRayTracing::mySeisSx)
            .def_readwrite("SeismicShotYAttr", &vpRayTracing::mySeisSy)
            .def_readwrite("SeismicShotZAttr", &vpRayTracing::mySeisSz)
            .def_readwrite("SeismicReceiverXAttr", &vpRayTracing::mySeisRx)
            .def_readwrite("SeismicReceiverYAttr", &vpRayTracing::mySeisRy)
            .def_readwrite("SeismicReceiverZAttr", &vpRayTracing::mySeisRz)
            .def_readwrite("RayFilePath", &vpRayTracing::myOutPath)
            .def_readwrite("ThreadNum", &vpRayTracing::myNumberOfThreads)
            .def_readwrite("SummaryFilePath", &vpRayTracing::mySummaryFile)
            .def_readwrite("OutputAngleInfo", &vpRayTracing::myAngleInfo)
            .def_readwrite("OutputSaltTime", &vpRayTracing::myExcludeSalt)
            .def_readwrite("OutputInitialAngle", &vpRayTracing::myInitialVector)
            .def_readwrite("RoseDiagramOutFile", &vpRayTracing::myRoseOutPath)
            .def_readwrite("ShotFoldOutFile", &vpRayTracing::myFoldOutPath)
            .def_readwrite("RoseGeom_xl", &vpRayTracing::myRoseGeom_xl)
            .def_readwrite("RoseGeom_sl", &vpRayTracing::myRoseGeom_sl)
            .def_readwrite("FoldGeom_xl", &vpRayTracing::myFoldGeom_xl)
            .def_readwrite("FoldGeom_sl", &vpRayTracing::myFoldGeom_sl)
            ;
    }
}

