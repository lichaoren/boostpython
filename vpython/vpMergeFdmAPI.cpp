/* 
 * File:   vpMergeFdmAPI.cpp
 * Author: yanhliu
 * 
 * Created on April 9, 2015, 10:28 AM
 */

#include "vpMergeFdmAPI.hpp"

vpMergeFdmAPI::vpMergeFdmAPI() {
    init();
}

void vpMergeFdmAPI::init() {
    myNullVal = 0.;
    myCombineOpt = 0;
    ;
    Vect4i tmp = Vect4i(-1, -1, -1);
    myMin = tmp;
    myMax = tmp;
    myInc = tmp;
    myTargetName = "";
    isBoundarySmooth = false;
    mySmoothRadius = 100;
    myInMemory = false;
    myOnDisk = false;
    myMask = 0;
    this->myAttachList.clear();
    this->myInMemList.clear();
    this->myOnDiskList.clear();
    this->myHorizon = 0;
    this->myRegion = 0;
}

vpMergeFdmAPI::~vpMergeFdmAPI() {
}

void vpMergeFdmAPI::setInMemList(boost::python::list lst) {
    this->myInMemList.clear();
    Py_ssize_t length = boost::python::len(lst);
    // loop for list
    for (Py_ssize_t i = 0; i < length; i++) {
        //extract a volume object
        try {
            Volume* vol = boost::python::extract<Volume*>(lst[i]);
            this->myInMemList.push_back(vol);
        } catch (...) {
            cerr << "You must have put some non-volume object into this list,"
                    "please check!" << endl;
        }
    }
    for (int j = 0; j < myInMemList.size(); j++) {
        cout << myInMemList[j]->getName() << " is added to merge list" << endl;
    }
}

void vpMergeFdmAPI::setOnDiskList(boost::python::list lst) {
    this->myOnDiskList.clear();
    Py_ssize_t length = boost::python::len(lst);
    // loop for list
    for (Py_ssize_t i = 0; i < length; i++) {
        //extract a c string object
        try {
            char const* vol = boost::python::extract<char const*>(lst[i]);
            this->myOnDiskList.push_back(strdup((const char *) vol));
        } catch (...) {
            cerr << "You must have put some non-string object into this list,"
                    "please check!" << endl;
        }
    }
    for (int j = 0; j < myOnDiskList.size(); j++) {
        cout << myOnDiskList[j] << " is added to merge list" << endl;
    }
}

void vpMergeFdmAPI::setGeometry(Vect4i Min, Vect4i Max, Vect4i Inc) {
    this->myMin = Min;
    this->myMax = Max;
    this->myInc = Inc;
}

void vpMergeFdmAPI::setToSmoothBoundary(bool flg) {
    this->isBoundarySmooth = flg;
}

void vpMergeFdmAPI::setSmoothBoundaryRadius(int radius) {
    this->mySmoothRadius = radius;
}

bool vpMergeFdmAPI::myPreCheck() {
    if ((!this->myOnDisk)&&(!this->myInMemory)) {
        cerr << "You need to setup at least one way to save, memory/disk" << endl;
        return false;
    }
    if ((this->myInMemList.size() + this->myOnDiskList.size()) < 2) {
        cerr << "At least 2 Fdms are needed " << endl;
        return false;
    }
    Vect4i tmp = Vect4i(-1, -1, -1);
    ;
    if (myMin == tmp || myMax == tmp || myInc == tmp) {
        cerr << "Please setup the start/end/increment" << endl;
        return false;
    }
    FdmMerge::ToSmoothBoundary = isBoundarySmooth;
    FdmMerge::BoundarySmoothRadius = mySmoothRadius;
    this->myAttachList.clear();

    string inMemNamePrefix = "InMemory:";
    for (int i = 0; i<this->myInMemList.size(); i++) {
        string name = inMemNamePrefix + myInMemList[i]->getName();
        this->myAttachList.push_back(strdup(name.c_str()));
    }
    for (int j = 0; j<this->myOnDiskList.size(); j++) {
        this->myAttachList.push_back(myOnDiskList[j]);
    }

    myModel.clearInMemoryFdms();

    for (int k = 0; k < myInMemList.size(); k++) {
        myModel.addInMemoryFdmCube(myInMemList[k]->getCube());
    }
    
    if (this->myRegion || this->myHorizon){
        bool res=setupBoolMask();
        if (!res){
            return false;
        }
    }
    return true;
}

bool vpMergeFdmAPI::doMerge() {
    if (!myPreCheck())
        return false;
    bool rc;
    rc = myModel.mergeFdm(this->myAttachList,
            (this->myOnDisk ? this->myTargetName.c_str() : ""),
            this->myNullVal, this->myCombineOpt,
            this->myMask, &myMin, &myMax, &myInc);
    if (!rc) {
        cerr << "Merge failed" << endl;
    }
    init();
    return rc;
}

Volume* vpMergeFdmAPI::getMergeVolume() {
    if (!this->myInMemory)
        return 0;
    FdmCube* myMergedCube = myModel.getMergedFdmCube();
    if (myMergedCube == 0)
        return 0;
    myMergedCube->setPath(this->myTargetName.c_str());
    Volume* myMergedVol = new Volume();
    myMergedVol->setCube(myMergedCube);
    return myMergedVol;
}

void vpMergeFdmAPI::setRegion(SurveyRegion* rgn) {
    if (this->myMask) {
        delete myMask;
        myMask = 0;
    }
    myRegion = rgn;
    myHorizon = 0;
}

void vpMergeFdmAPI::setHorizon(vgHorizon* hrz) {
    if (myMask) {
        delete myMask;
        myMask = 0;
    }
    myHorizon = hrz;
    myRegion = 0;
}

bool vpMergeFdmAPI::setupBoolMask() {
    if (this->myAttachList.size() != 2) {
        cerr << "Apply region requires exactly 2 fdms" << endl;
        return false;
    }
    FdmCube second_fdm;
    FdmCube* p_second_fdm = NULL;

    if (this->myInMemList.size() <= 1) {
        int ind = 0;
        if (this->myInMemList.size() == 0)
            ind = 1;
        second_fdm.setPath(this->myOnDiskList[ind]);

        if (!second_fdm.loadHeader()) {
            cerr << "Failed to get second FDM geometry!" << endl;
            return false;
        }
    } else {
        Volume* vol = this->myInMemList[1];
        p_second_fdm = new FdmCube(*(vol->getCube()));
        if (!p_second_fdm) {
            cerr << "Failed to get second FDM geometry! Could be memory issue." << endl;
            return false;
        }
    }
    if (this->myRegion){
        if (myRegion->regionPolygon().empty()){
            cerr<<"Empty region"<<endl;
            delete p_second_fdm;
            return false;
        }
        this->myMask=new vmBoolMask;
        myRegion->makeMask(*myMask,( p_second_fdm ? (* p_second_fdm) : second_fdm ));
    } else if (this->myHorizon){
        // horizon
        GenerateMask( ( p_second_fdm ? (* p_second_fdm) : second_fdm ), *myHorizon,myMask) ;
    } else{
        this->myMask=0;
        return false;
    }
    
    delete p_second_fdm;
    return true;
}

