/* 
 * File:   vpAttrReplaceAPI.cpp
 * Author: yanhliu
 * 
 * Created on March 3, 2015, 11:26 AM
 */

#include "vpAttrReplaceAPI.hpp"
#include "vlVolume.hpp"
#include "vpVolumeProxy.hpp"
#include "vpVox2Fdm.hpp"
vpAttrReplaceAPI::vpAttrReplaceAPI()
: vmAttrReplaceNMT() {
    myInit();
}

void vpAttrReplaceAPI::myInit() {
    vmAttrReplaceNMT::myScriptMode = true;
    myisInf = false;
    myisNan = false;
    myRangeType = -1;//vmAttrReplaceNMT::DataSel::MinMax;
    myRangeVal1 = FLT_MAX;
    myRangeVal2 = FLT_MAX;
    myReplaceMode = -1;//vmAttrReplaceNMT::FIXED_VALUE;
    myReplaceVal = FLT_MAX;
    myIsAngle=false;
    this->myAngleType=AngleConv::RadSigned;
    this->_myRegion=0;
    this->_RegionType=vmAttrReplaceNMT::RegionSel::All;
}

vpAttrReplaceAPI::~vpAttrReplaceAPI() {
}

void vpAttrReplaceAPI::setRegion(SurveyRegion* rgn, vmAttrReplaceNMT::RegionSel::Type tp){
    this->_RegionType=tp;
    this->_myRegion=rgn;
}

void vpAttrReplaceAPI::clearRegion(){
    this->_RegionType=vmAttrReplaceNMT::RegionSel::All;
    this->_myRegion=NULL;
}

void vpAttrReplaceAPI::replaceInf(bool isInf) {
    this->myisInf = isInf;
}

void vpAttrReplaceAPI::replaceNAN(bool isNAN) {
    this->myisNan = isNAN;
}

void vpAttrReplaceAPI::setRange(vmAttrReplaceNMT::DataSel::RangeType type, float val1, float val2) {
    this->myRangeType = static_cast<int>(type);
    this->myRangeVal1 = val1;
    this->myRangeVal2 = val2;
}

void vpAttrReplaceAPI::setReplaceType(vmAttrReplaceNMT::Mode mode, float val) {
    this->myReplaceMode = static_cast<int>(mode);
    this->myReplaceVal = val;
}

void vpAttrReplaceAPI::setAngleType(AngleConv::Type tp)
{
    this->myAngleType=tp;
    this->myIsAngle=true;
}


bool vpAttrReplaceAPI::myPreCheck() {
    
    // set inf && nan
    if (!myisInf && !myisNan && myRangeType==-1){
        cerr<<"You have to use some condition for replace"
                <<"eg: Inf, Nan or use range"<<endl;
        return false;
    }
    
    vmAttrReplaceNMT::dataSel().myIsInf=myisInf;
    vmAttrReplaceNMT::dataSel().myIsNan=myisNan;
    
    // set use range
    if (myRangeType!=-1){
        vmAttrReplaceNMT::dataSel().myIsRange=true;
        vmAttrReplaceNMT::dataSel().myRangeType=static_cast<vmAttrReplaceNMT::DataSel::RangeType>(myRangeType);
    }
    
    if (this->myRangeType == static_cast<int>(vmAttrReplaceNMT::DataSel::MinMax)) {
        if (myRangeVal1 == FLT_MAX || myRangeVal2 == FLT_MAX ||
                myRangeVal1 > myRangeVal2) {        // DD: equality is allowed as it's lessOrEqual & greaterOrEqual
            cerr << "Range type is set MinMax mode, but Min/Max value is not properly provided"
                    << endl;
            return false;
        }
    }
   
    vmAttrReplaceNMT::dataSel().myMaxVal=myRangeVal2;
    vmAttrReplaceNMT::dataSel().myMinVal=myRangeVal1;


    // set replace mode
    if (this->myReplaceMode == (int) vmAttrReplaceNMT::FIXED_VALUE ||
            this->myReplaceVal == (int) vmAttrReplaceNMT::VERTICAL_GRADIENT ||
            this->myReplaceMode == (int) vmAttrReplaceNMT::SCALED_VALUE) {
        if (this->myReplaceVal == FLT_MAX) {
            cerr << "Mode is set to be FIXED_VALUE, VERTICAL_GRADIENT or SCALED_VALUE, but invalid"
                    << "value is provided\n";
            return false;
        }
    }
    vmAttrReplaceNMT::setMode(static_cast<vmAttrReplaceNMT::Mode>(myReplaceMode));
    vmAttrReplaceNMT::dataSel().myRepVal=myReplaceVal;
    vmAttrReplaceNMT::dataSel().myGradVal=myReplaceVal;
    // now it is angle
    vmAttrReplaceNMT::dataSel().myIsAngle=this->myIsAngle;
    if (myIsAngle){
        AngleConv conv(this->myAngleType);
        vmAttrReplaceNMT::dataSel().myAngConv=conv;
    }
    
 
    vmAttrReplaceNMT::RegionSel::Type tp = this->_RegionType;
    vmAttrReplaceNMT::regionSel().setType(tp);
    if (this->_myRegion!=0){
        if (myInVol)
          this->_myRegion->setGeometry(myInVol);
    }
    vmAttrReplaceNMT::regionSel().horSel().setHGP(NULL);
    vmAttrReplaceNMT::regionSel().setRegion(this->_myRegion);
    // no need to set but set for guard
    vmAttrReplaceNMT::setIsSaveOutput(false);
    return true;
}

bool vpAttrReplaceAPI::doReplace(){
    if (!myPreCheck()){
        cerr<<"Replace failed"<<endl;
        myInit();
        return false;
    }
    vmAttrReplaceNMT::myPreRun();
    vmAttrReplaceNMT::myRun();
    vmAttrReplaceNMT::myPostRun();
    return true;
}

void vpAttrReplaceAPI::doReplace_Module(vString originPath, vString savePath, int taper, int direction){
    // let's assume that the local machine memory is about 16GB and we may let
    // the attribute size smaller than 8 G
    
    // firstly, check if it is a voxel cube;
    vpVolumeProxy proxy;
    proxy.setSubVolSize(8); // 8 GB is the maximum, otherwise , truncate it

    proxy.setDirection(direction);
    
    // check if the size is ok
    if (!proxy.isToSplit(originPath.c_str())){
        // if size is fine, go with usual mode
        Volume *vol=new Volume;
        vol->load(originPath.c_str());
        setVol(vol);
        if (doReplace()){
            vol->saveAs(savePath.c_str());
        }
        delete vol;
        return;
    }
    // not ok
    vString temp_path=originPath;
    vpVox2Fdm converter;
    // check if it is a voxel type
    if (converter.isVoxel(originPath.c_str())){
        cerr<<"ERROR:The voxel geo format is not supported"<<endl;
        return;
        // if it is a voxelgeo type, convert to
        //converter.convert2Fdm(originPath.c_str());
        //temp_path=converter.getTempFile().c_str();
    }
    // the size is not ok, we will split it up
    
    proxy.setOriginPath(temp_path.c_str());
    proxy.setToSaveAs(savePath.c_str());
    proxy.setTaper(taper);
    proxy.split();
    int n=proxy.getNumVolume();
    for (int i=0; i<n; i++){
        Volume* vol=proxy.getVolume(i);
        setVol(vol);
        doReplace();
        vol->save();
        delete vol;
    }
    proxy.merge();
    proxy.cleanUp();
//    converter.cleanUp();
   
}

