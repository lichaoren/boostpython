/* 
 * File:   vpVolumeDipConverter.cpp
 * Author: Dominique DOLEDEC
 * 
 * Created on May 23, 2016, 4:17 PM
 */
#include <boost/python.hpp> 

#include "vlVolume.hpp"
#include "vpVolumeProxy.hpp"

#include "vlPythonInterpreter.hpp"
#include "vpVolumeDipConverter.hpp"

#define MEMORY_PER_VOLUME 2  // 8 GB for 4 volumes


vpVolumeDipConverter::vpVolumeDipConverter() {
}

vpVolumeDipConverter::~vpVolumeDipConverter() {
}

Volume* getResultA(vpVolumeDipConverter* converter) {
    return converter->getOutputVol(0);
}

Volume* getResultB(vpVolumeDipConverter* converter) {
    return converter->getOutputVol(1);
}

/** just to return a boolean and not exposing fnok */
bool dipConvert(vpVolumeDipConverter *converter,
        vlVolumeDipConverter::VolumeDipType inputType, vlVolumeDipConverter::VolumeDipUNIT inputUnit,
        vlVolumeDipConverter::VolumeDipType outputType, vlVolumeDipConverter::VolumeDipUNIT outputUnit) {
    FnOK status = converter->convert(inputType, inputUnit, outputType, outputUnit);
    return status.isOK();
}


void vpVolumeDipConverter::exposePython() {
    namespace bp = boost::python;

    // Remove ambiguity
    void (vlVolumeDipConverter::*setInputString)(const vString& pathA, const vString & pathB) = &vlVolumeDipConverter::setInput;
    void (vlVolumeDipConverter::*setInputVol)(Volume* volA, Volume * volB) = &vlVolumeDipConverter::setInput;


    bp::enum_<vlVolumeDipConverter::VolumeDipType>("DipType")
            .value("THETA_PHI", vlVolumeDipConverter::THETA_PHI)
            .value("DIPX_DIPY", vlVolumeDipConverter::DIPX_DIPY)
            .value("NORMALIZED_UTM", vlVolumeDipConverter::NORMALIZED_UTM);


    bp::enum_<vlVolumeDipConverter::VolumeDipUNIT>("DipUnit")
            .value("RADIANS", vlVolumeDipConverter::RADIANS)
            .value("DEGREES", vlVolumeDipConverter::DEGREES)
            .value("UNITLESS", vlVolumeDipConverter::UNITLESS)
            .value("METERS", vlVolumeDipConverter::METERS);


    bp::scope local_scope = bp::class_<vpVolumeDipConverter>("VolumeDipConverter")
            .def("run", dipConvert)
            .def("setInput", setInputString)
            .def("setInput", setInputVol)
            .def("setOutput", &vlVolumeDipConverter::setOutput)
            .def("setExternalGeometry", &vlVolumeDipConverter::setExternalGeometry)
            .def("getOutputA", getResultA, bp::return_value_policy<bp::manage_new_object>())
            .def("getOutputB", getResultB, bp::return_value_policy<bp::manage_new_object>())
            ;

    
    // Reference Enum to the class rather than the module
    local_scope.attr("THETA_PHI") = vlVolumeDipConverter::THETA_PHI;
    local_scope.attr("DIPX_DIPY") = vlVolumeDipConverter::DIPX_DIPY;
    local_scope.attr("NORMALIZED_UTM") = vlVolumeDipConverter::NORMALIZED_UTM;
    local_scope.attr("RADIANS") = vlVolumeDipConverter::RADIANS;
    local_scope.attr("DEGREES") = vlVolumeDipConverter::DEGREES;
    local_scope.attr("METERS") = vlVolumeDipConverter::METERS;
    local_scope.attr("UNITLESS") = vlVolumeDipConverter::UNITLESS;    
}


FnOK vpVolumeDipConverter::convert(vlVolumeDipConverter::VolumeDipType inputType, vlVolumeDipConverter::VolumeDipUNIT inputUnit,
        vlVolumeDipConverter::VolumeDipType outputType, vlVolumeDipConverter::VolumeDipUNIT outputUnit) {


    vpVolumeProxy proxy;
    proxy.setSubVolSize(MEMORY_PER_VOLUME);
    
    FnOK messHandler;

    // Enough memory --> No chunk
    if (getInputVol(0) != 0 && getInputVol(1) != 0) {
        messHandler = vlVolumeDipConverter::convert(inputType, inputUnit,
                outputType, outputUnit);

    } else if (proxy.isToSplit(getInputPath(0).c_str())) {
        messHandler = convertWithChunk(inputType, inputUnit,
                outputType, outputUnit);
    } else {
        messHandler = vlVolumeDipConverter::convert(inputType, inputUnit,
            outputType, outputUnit); 
    }
    
    if (messHandler.isOK() == false) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, messHandler.getMsg());
    } else {
        // Save volume from script
        if (getOutputVol(0))
            getOutputVol(0)->save();
        if (getOutputVol(1))
            getOutputVol(1)->save();            
    }
    
    return messHandler;
}


FnOK vpVolumeDipConverter::convertWithChunk(vlVolumeDipConverter::VolumeDipType inputType, vlVolumeDipConverter::VolumeDipUNIT inputUnit,
        vlVolumeDipConverter::VolumeDipType outputType, vlVolumeDipConverter::VolumeDipUNIT outputUnit) {

    vpVolumeProxy proxySourceA;
    proxySourceA.setSubVolSize(MEMORY_PER_VOLUME);
    proxySourceA.setOriginPath(getInputPath(0).c_str());
    proxySourceA.split();

    vpVolumeProxy proxySourceB;
    proxySourceB.setSubVolSize(MEMORY_PER_VOLUME);
    proxySourceB.setOriginPath(getInputPath(1).c_str());
    proxySourceB.split();

    vpVolumeProxy proxyDestA;
    proxyDestA.setSubVolSize(MEMORY_PER_VOLUME);
    proxyDestA.setOriginPath(getInputPath(0).c_str());
    proxyDestA.split();
    proxyDestA.setToSaveAs(getOutputPath(0).c_str());

    vpVolumeProxy proxyDestB;
    proxyDestB.setSubVolSize(MEMORY_PER_VOLUME);
    proxyDestB.setOriginPath(getInputPath(0).c_str());
    proxyDestB.split();
    proxyDestB.setToSaveAs(getOutputPath(1).c_str());

    double rotAngle = 0;
    bool yflipped = false;
    getRotAngleAndYflipped(rotAngle, yflipped);

    // Assuming all the cubes have the same size
    int n = proxySourceA.getNumVolume();
    FnOK messHandler;
    vString message;
    for (int i = 0; i < n; i++) {
        Volume* input_vol1 = proxySourceA.getVolume(i);
        Volume* input_vol2 = proxySourceB.getVolume(i);
        Volume* output_vol1 = proxyDestA.getVolume(i);
        Volume* output_vol2 = proxyDestB.getVolume(i);
        message = vString("Processing Chunk ")+ i;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);

        messHandler = convertDipAngles(
                input_vol1, inputType, inputUnit,
                input_vol2, outputType, outputUnit,
                output_vol1, output_vol2,
                rotAngle, yflipped);
        if (messHandler.isOK() == false)
            return messHandler;
                
        delete input_vol1;
        delete input_vol2;
        output_vol1->save(); delete output_vol1;
        output_vol2->save(); delete output_vol2;
             
        message = vString("Done ")+ message;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);
    }
    proxyDestA.merge(); proxyDestA.cleanUp();
    proxyDestB.merge(); proxyDestB.cleanUp();
    
    return messHandler;
}

