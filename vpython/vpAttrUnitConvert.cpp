/* 
 * File:   vpAttrUnitConvert.cpp
 * Author: yanhliu
 * 
 * Created on May 5, 2015, 2:44 PM
 */

#include "vpAttrUnitConvert.hpp"
#include "vpVolumeProxy.hpp"
#include "vlPythonInterpreter.hpp"

vpAttrUnitConvert::vpAttrUnitConvert() {
    myConvertVal=1.;
    for (int i=0; i<5; i++){
        myAxis[i]=false;
    }
}

vpAttrUnitConvert::~vpAttrUnitConvert() {
}

void vpAttrUnitConvert::setConvert(vmUnitConst::unit input, vmUnitConst::unit output, string toConvert){
    
    if (input == output) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "ERROR: you input the same unit");
    }
    static vmUnitConst conv;
    myConvertVal = conv.getConvertConst(input, output);
    size_t found;
    found = toConvert.find("x");
    if (found != std::string::npos) {
        myAxis[0] = true;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Axis X will be converted");
    }
    found = toConvert.find("y");
    if (found != std::string::npos) {
        myAxis[1] = true;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Axis Y will be converted");
    }
    found = toConvert.find("z");
    if (found != std::string::npos) {
        myAxis[2] = true;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Axis Z will be converted");
    }
    found = toConvert.find("v");
    if (found != std::string::npos) {
        myAxis[3] = true;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Axis V will be converted");
    }
    int count = 0;
    for (int i=0; i<4; i++){
        if (myAxis[i])
            count++;
    }
    found=toConvert.find("o");
    if (found!=std::string::npos){
        myAxis[4]=true;          
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "The origin will be converted");
    }
    if (count==0){
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Please specify your converting options");
    }
}

void vpAttrUnitConvert::calcHeader(Volume* vol){
    this->myGeom=*vol;
    Vect4d step=vol->getStep();
    Vect4d orig= vol->getOrigin();
    for (int i=0; i<3; i++){
        if (this->myAxis[i]){
            step[i]*=this->myConvertVal;
            if (myAxis[4]){
                orig[i]*=this->myConvertVal;
            }
        }
     }
    myGeom.setStep(step);
    myGeom.setOrigin(orig);
}

void vpAttrUnitConvert::calcData(Volume* vol){
    if(!myAxis[3])
        return;
    float* data=(float*)vol->getData();
    Vect4i size=vol->getSize();
    size_t len=(size_t)size[0]*(size_t)size[1]*(size_t)size[2];
    for (size_t i=0; i<len; i++){
        data[i]*=this->myConvertVal;
    }
}


void vpAttrUnitConvert::convert(Volume* vol){
    if (!vol)
        return;
    calcHeader(vol);
    vol->setStep(this->myGeom.getStep());
    calcData(vol);
}


void vpAttrUnitConvert::convert_module(string input, string output){
    
    vpVolumeProxy proxySource;
    proxySource.setSubVolSize(3); 
    if (!proxySource.isToSplit(input.c_str())){
        Volume *vol= new Volume;
        vol->load(input.c_str());
        if (!vol){
            vString message("ERROR: cannot load data: ");
            message+=input.c_str();
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message);
        }
        calcHeader(vol);
        vol->setStep(this->myGeom.getStep());
        calcData(vol);
        vol->saveAs(output.c_str());
        delete vol;
        return;
    }
    
    Volume* dummy=new Volume;
    dummy->setCreateHint(Volume::eHintLSUB);
    dummy->loadHeader(input.c_str());
    calcHeader(dummy);
    delete dummy;
    
    proxySource.setOriginPath(input.c_str());
    proxySource.split();
    proxySource.setToSaveAs(output.c_str());
    
    int n=proxySource.getNumVolume();
    for (int i=0; i<n; i++){
        Volume* volTmp= proxySource.getVolume(i);
        calcData(volTmp);
        volTmp->save();
        delete volTmp;
    }
    proxySource.setPredictedGeometry(this->myGeom);
    proxySource.merge();
    proxySource.cleanUp();
}

