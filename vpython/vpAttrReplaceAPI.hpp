/* 
 * File:   vpAttrReplaceAPI.hpp
 * Author: yanhliu
 *
 * Created on March 3, 2015, 11:26 AM
 */

#ifndef VPATTRREPLACEAPI_HPP
#define	VPATTRREPLACEAPI_HPP

#include "vmAttrReplaceNMT.hpp"
#include "AngleConverter.hpp"
#include "vlSurveyRegion.hpp"

class vpAttrReplaceAPI : public vmAttrReplaceNMT {
public:
    vpAttrReplaceAPI();
    virtual ~vpAttrReplaceAPI();
    void replaceInf(bool isInf);
    void replaceNAN(bool isNAN);
    void setRange(vmAttrReplaceNMT::DataSel::RangeType type, float val1, float val2 = FLT_MAX);
    void setReplaceType(vmAttrReplaceNMT::Mode mode, float val = FLT_MAX);
    void setRegion(SurveyRegion* rgn, vmAttrReplaceNMT::RegionSel::Type tp);
    void clearRegion();
    bool doReplace();
    void setAngleType(AngleConv::Type tp);
    void unSetAngleType(){myIsAngle=false;};
    void doReplace_Module(vString originPath, vString savePath, int taper=0, int direction=1);
private:
    bool myisInf;
    bool myisNan;
    int myRangeType;
    float myRangeVal1, myRangeVal2;
    int myReplaceMode;
    float myReplaceVal;
    vmAttrReplaceNMT::RegionSel::Type _RegionType;
    SurveyRegion* _myRegion;
    AngleConv::Type myAngleType;
    bool myIsAngle;
    bool myPreCheck();
    void myInit();
};

#endif	/* VPATTRREPLACEAPI_HPP */

