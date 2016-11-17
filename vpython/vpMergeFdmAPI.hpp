/* 
 * File:   vpMergeFdmAPI.hpp
 * Author: yanhliu
 *
 * Created on April 9, 2015, 10:28 AM
 */

#ifndef VPMERGEFDMAPI_HPP
#define	VPMERGEFDMAPI_HPP
#include <boost/python.hpp> 
#include "vlVolume.hpp"
#include "FdmMerge.hpp"
#include "gmiAppModel.hpp"
#include "vmBoolMask.hpp"
#include "vlSurveyRegion.hpp"
#include "vlHorizon.hpp"
class vpMergeFdmAPI {
public:
    enum TracePositioning{
        ByCoordinate,
        ByLine
    };
    vpMergeFdmAPI();
    /**
     * set the value at the empty area
     * @param val
     */
    void setNullValue(float val){this->myNullVal=val;};
    /**
     * set combine by line/coordinate
     * @param tp
     */
    void setCombineOpt(TracePositioning tp){myCombineOpt=static_cast<int>(tp);};
    /**
     * setup the volumes into merger
     */
    void setInMemList(boost::python::list lst);
    /**
     * setup file paths into merger
     */
    void setOnDiskList(boost::python::list lst);
    virtual ~vpMergeFdmAPI();
    /**
     * new geometry
     */
    void setGeometry(Vect4i Min, Vect4i Max, Vect4i Inc);
    /**
     * set the output name
     */
    void setTargetName(string name){myTargetName=name;};
    /*
     * set the flag with true/false to use smoothing after merge
     */
    void setToSmoothBoundary(bool flg);
    /**
     * set the smooth radius, will have no effect if smooth boundary flag is off
     */
    void setSmoothBoundaryRadius(int radius);
    /**
     * will save to memory and user can access the volume from getMergeVolume
     */
    void setSaveToMemory(bool flag){myInMemory = flag;};
    /**
     * set to save to disk, once merge is done, automatically save to given taget name
     */
    void setSaveToDisk(bool flag){myOnDisk=flag;};
    // perform merge
    bool doMerge();
    /**
     * After doMerge is called, if setSaveToMemory is called, user will have access to the new volume
     */
    Volume* getMergeVolume();
    /**
     * set region/horizon and use this to build a mask with the second volume in memory 
     */
    void setRegion(SurveyRegion* rgn);
    void setHorizon(vgHorizon* hrz);
protected:
    bool myPreCheck();
    void init();
    bool setupBoolMask();
private:
    float myNullVal;
    int myCombineOpt;
    vector<Volume*> myInMemList;
    vector<char*> myOnDiskList;
    Vect4i myMin;
    Vect4i myMax;
    Vect4i myInc;
    string myTargetName;
    bool isBoundarySmooth;
    int mySmoothRadius;
    GammaAppModel myModel; 
    bool myInMemory;
    bool myOnDisk;
    vmBoolMask *myMask;
    vector<char*> myAttachList;
    SurveyRegion *myRegion;
    vgHorizon* myHorizon;
};

#endif	/* VPMERGEFDMAPI_HPP */

