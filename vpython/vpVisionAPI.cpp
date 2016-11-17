/* 
 * File:   vpVisionAPI.cpp
 * Author: yanhliu
 * 
 * Created on February 2, 2015, 3:21 PM
 */
#include <boost/python.hpp> 

#include "vpVisionAPI.hpp"

#include "vlColormap.hpp"
#include "vlHorizonGroup.hpp"
#include "vlLasWellLog.hpp"
#include "vlVolume.hpp"

#include "vmModel.hpp"
#include "vmView3D.hpp"
#include "vmApplication.hpp"


vpVisionAPI* vpVisionAPI::myInstance=NULL;


vpVisionAPI::vpVisionAPI() {

}

vpVisionAPI::~vpVisionAPI() {
}

vpVisionAPI& vpVisionAPI::getInstance()
{
    if (myInstance==NULL){
        myInstance= new vpVisionAPI;
    }
    return *myInstance;
}

void vpVisionAPI::setCursorPos(const Vect4d& survey, const Vect2i& line){
    myCursorSuveyPos = survey;
    myCursorLinePos = line;
}
Vect4d vpVisionAPI::getCursorLog(){
    return myCursorSuveyPos ;
}
Vect2i vpVisionAPI::getCursorLine(){
    return myCursorLinePos;
}

void vpVisionAPI::setUserArg(const Vect3d& args){
    myUserArg = args;
}

Vect3d vpVisionAPI::getUserArg(){
    return myUserArg;
}
/**
 * We ask python to manager memory created in python
 * on tornado side, we will do copy from python, in this case
 * , we can avoid python reference count mechanism crashing tornado 
 */
void vpVisionAPI::addAttribute(Volume* vol, std::string name)
{
    vmModel* model;
    model=&g_app.getModel(); 
    vmView3D& view3D = g_app.getView3D();
    
    if (vol==NULL){
        std::cerr<<"Empty volume is added, will not take effect\n";
        return;
    }
   
    // do copy
    Cube* input_cube=dynamic_cast<FdmCube*>(vol->getCube());
    if (input_cube==NULL){
        std::cerr<<"We currently only support FdmCube\n";
        return;
    }
    
    //create new volume and cube
    Volume* new_vol= new Volume;
    //(*new_vol)=(*vol);
        
    Cube* new_cube= new FdmCube(*input_cube); // limited support for fdm format
    new_vol->setCube(new_cube);

    vString aname=vol->getName();
    if (name.size()>0) {
        aname = name.c_str();
        new_vol->setPath(name.c_str());
    } else {
        new_vol->setPath(vol->getName());              
    }
    
    // ask tornado to manage this memory for us
    model->addAttribute(*new_vol, aname, model->getDomain());
    Send(*model,model->VolumeChanged, new_vol);
    view3D.requestRender();
}

void vpVisionAPI::addHorizonGroup(vgHorizonGroup* hg )
{
    vmModel* myModel;
    myModel=&g_app.getModel(); 
    vmView3D& view3D = g_app.getView3D();
    
    if (hg==NULL){
        std::cerr<<"Empty Horizon group is added, will not take effect\n";
        return;
    }
   
    vgHorizonGroup *tornadoHG = new vgHorizonGroup();
    (*tornadoHG) = (*hg); // hope it works
    
    // ask tornado to manage this memory for us
    myModel->addHorizonGroup( *tornadoHG );

    view3D.requestRender();
}


Volume* vpVisionAPI::getCurrentAttribute()
{
    vmModel* myModel;
    myModel=&g_app.getModel();
    return myModel->getCurAttribute();
}

Volume* vpVisionAPI::getCurrentSeismic(){
    vmModel* myModel;
    myModel=&g_app.getModel();
    return myModel->getCurSeismic();
}

vgHorizonGroup* vpVisionAPI::getCurrentHorizonGroup(){
    vmModel* myModel;
    myModel=&g_app.getModel();
    return myModel->getCurHorizonGroup();
}

Volume* vpVisionAPI::getAttribute(const char* attr){
    vmModel* myModel;
    myModel=&g_app.getModel();
    vector<Volume*> lst;
    int domain=myModel->getDomain();
    myModel->getAttributeList(lst,domain);
    Volume* res=0;
    for (int i=0; i<lst.size(); i++){
        if (strcmp(attr, lst[i]->getName())==0)
            res=lst[i];
    }
    return res;
}

Volume* vpVisionAPI::getSeismic(const char* seis){

    vmModel* myModel;
    myModel=&g_app.getModel();
    vector<Volume*> lst;
    int domain=myModel->getDomain();
    myModel->getSeismicList(lst,domain);
    Volume* res=0;
    for (int i=0; i<lst.size(); i++){
        if (strcmp(seis, lst[i]->getName())==0)
            res=lst[i];
    }
    return res;
}
void vpVisionAPI::setCurrentSpectrum(const char* dataType, float min, float max, char* palette) {

    vmModel* model;
    model = &g_app.getModel();
    if(!model)
        return;
    vmView3D& view3D = g_app.getView3D();
    Vect2d range(min, max);

    Volume* vol = 0;
    if (strcmp(dataType, "HORIZON") == 0) {
        Spectrum spectrum;
        view3D.getHrzSpect("DEPTH", spectrum);
        spectrum.setRange(range);

        if (palette) {
            int cmpId = Spectrum::getColormapInv().getColormapIndex(palette);
            if (cmpId > 0 && cmpId != INTERIM_CMAP)
                spectrum.setSpectId(cmpId);
        }
        view3D.setHrzSpect("DEPTH", spectrum);

    } else if (strcmp(dataType, "SEISMIC") == 0) {

        vol = model->getCurSeismic();

    } else if (strcmp(dataType, "ATTRIBUTE") == 0) {

        vol = model->getCurAttribute();

    } else if (strcmp(dataType, "GATHER") == 0) {
        vol = model->getCurGatherVol();
    }


    if (vol) {
        Spectrum spectrum;
        view3D.getVolSpect(*vol, spectrum);

        spectrum.setRange(range);
        if (palette) {
            int cmpId = Spectrum::getColormapInv().getColormapIndex(palette);
            if (cmpId > 0 && cmpId != INTERIM_CMAP)
                spectrum.setSpectId(cmpId);
        }
        view3D.setVolSpect(*vol, spectrum);
    }
}



/**
 * @param name    data name in Tornado session
 */
Volume* vpVisionAPI::getGather(const char* name) const {
    vmModel* myModel;
    myModel=&g_app.getModel();
       
    vString vpath( name );
    vector<Volume*> volumeList;
    myModel->getGatherList(volumeList, myModel->getDomain());
    vector<Volume*>::const_iterator iter = volumeList.begin();
    for( ; iter!=volumeList.end(); iter++ ) {
        Volume*gather = (*iter);
        vString vol_path = gather->getPath();
        vString vol_name = gather->getName();
        if( vol_path == vpath || 
            IsSameFile(vol_path, vpath)||
            vol_name== name )
            return gather;  
    }
    return 0;
}



vgHorizonGroup* vpVisionAPI::getHorizonGroup(const char* hg){
    
    vmModel* myModel;
    myModel=&g_app.getModel();
    vector<vgHorizonGroup*> lst;
    myModel->getHorizonGroupList(lst);
    vgHorizonGroup* res=0;
    for (int i=0; i<lst.size(); i++){
        if (strcmp(hg, lst[i]->getName())==0)
            res=lst[i];
    }
    return res;
}
    
LasWellLog* vpVisionAPI::getWell(const char* well_name) {
    vector<LasWellLog*> wls;
    g_app.getModel().getWellLogList(wls);
    vString wellName(well_name);
    
    int i;
    for(i = 0; i < wls.size(); ++i ) {
        if ( wls[i]->getWellName()==wellName) 
            return wls[i];
    }  
    return 0;
}


bool isNotSonix(LasWellLog* wl) {

    if(wl->getCurveIndex("DT") == 0 && wl->getCurveIndex("DTCO") == 0)
        return true;
    
    return false;
}

bool isNotTop(LasWellLog* wl) {
    const vector<TopInformation*>& tlst = wl->getTopInfo ();
    int ntop = tlst.size();
    return tlst.size()>0? false:true;
}

 bool isNotWithCheckshot(LasWellLog* wl) {
     return wl->getCheckshotData().empty()? true: false;
}


boost::python::list vpVisionAPI::getWellList(WellSelectionMode flag) {
    boost::python::list wls;
    vector<LasWellLog*> wellv;
    g_app.getModel().getWellLogList(wellv);

    
    if (flag == WELL_WITH_SONIC) {
        wellv.erase(std::remove_if( wellv.begin(), wellv.end(), isNotSonix), wellv.end());
    } else if (flag == WELL_WITH_TOPS) {
        wellv.erase(std::remove_if(wellv.begin(), wellv.end(), isNotTop), wellv.end());
    } else if (flag == WELL_WITH_CHECKSHOT) {
        wellv.erase(std::remove_if(wellv.begin(), wellv.end(), isNotWithCheckshot), wellv.end());
    }

    vector<LasWellLog*>::const_iterator iter = wellv.begin();
    for (; iter != wellv.end(); iter++) {
        LasWellLog* well = (*iter);
        wls.append(boost::ref(well)); // return the same instance (not a copy)
    }

    return wls;
}

bool vpVisionAPI::showWell(WellSelectionMode flag) {

    vector<LasWellLog*> wellv;
    g_app.getModel().getWellLogList(wellv);
    // hide all the wells
    vector<LasWellLog*>::const_iterator iter1 = wellv.begin();
    for (; iter1 != wellv.end(); iter1++) {
        LasWellLog* wl = (*iter1);
        if (wl) {
            WellDisplayOptions opts = wl->getDisplayOptions();
            opts.setVisible(false);
            wl->setDisplayOptions(opts);
        }
    }


    if (flag == WELL_WITH_SONIC) {
        wellv.erase(std::remove_if(wellv.begin(), wellv.end(), isNotSonix), wellv.end());
    } else if (flag == WELL_WITH_TOPS) {
        wellv.erase(std::remove_if(wellv.begin(), wellv.end(), isNotTop), wellv.end());
    } else if (flag == WELL_WITH_CHECKSHOT) {
        wellv.erase(std::remove_if(wellv.begin(), wellv.end(), isNotWithCheckshot), wellv.end());
    }
    // Show well according to the selection
    vector<LasWellLog*>::const_iterator iter2 = wellv.begin();
    for (; iter2 != wellv.end(); iter2++) {
        LasWellLog* wl = (*iter2);
        if (wl) {
            WellDisplayOptions opts = wl->getDisplayOptions();
            opts.setVisible(true);
            wl->setDisplayOptions(opts);
        }
    }
    return true;
}



bool vpVisionAPI::showWellList(boost::python::list wellList) {
    Py_ssize_t length = boost::python::len(wellList);
        
    vector<LasWellLog*> wellv;
    g_app.getModel().getWellLogList(wellv);
    // hide all the wells
    vector<LasWellLog*>::const_iterator iter = wellv.begin();
    for (; iter != wellv.end(); iter++) {
        LasWellLog* wl = (*iter);
         if (wl) {
            WellDisplayOptions opts = wl->getDisplayOptions();
            opts.setVisible(false);
            wl->setDisplayOptions(opts);
        }       
    }

    // Show selection only
    for (Py_ssize_t i = 0; i < length; i++) {
        LasWellLog* wl = boost::python::extract<LasWellLog*>(wellList[i]);
        if (wl) {
            WellDisplayOptions opts = wl->getDisplayOptions();
            opts.setVisible(true);
            wl->setDisplayOptions(opts);
        }
    }
    
    return true;
}
/** Change current displayed horizon.  */
bool vpVisionAPI::setCurrentHorizonGroup(const vString& ehName) {
    vmModel& model = g_app.getModel();
    
    // Just the name
    vgHorizonGroup* hg = model.getHorizonGroup(ehName);
    if (hg) {
        int hgIdx = model.getHorizonGroupIndex(hg);
        model.setCurHorizonGroup(hgIdx);
        return true;
    }
    
    // Or the full path
    int nbHg = model.getNumHorizonGroup();
    for (int i = 0; i < nbHg; i++) {
        hg = model.getHorizonGroup(i);

        if (hg && strcmp(hg->getPath(), ehName.c_str()) == 0) {
            int hgIdx = model.getHorizonGroupIndex(hg);
            model.setCurHorizonGroup(hgIdx);
            return true;
        }
    }

    return false;
}

/** Remove the horizon group from tornado */
bool vpVisionAPI::removeHorizonGroup(const vString& ehName) {
    vmModel& model = g_app.getModel();
    
    // Just the name
    vgHorizonGroup* hg = model.getHorizonGroup(ehName);
    if (hg) {
        int hgIdx = model.getHorizonGroupIndex(hg);
        model.unloadHorizonGroup(hgIdx);
        return true;
    }
    
    // Or the full path
    int nbHg = model.getNumHorizonGroup();
    for (int i = 0; i < nbHg; i++) {
        hg = model.getHorizonGroup(i);

        if (hg && strcmp(hg->getPath(), ehName.c_str()) == 0) {
            int hgIdx = model.getHorizonGroupIndex(hg);
            model.unloadHorizonGroup(hgIdx);
            return true;
        }
    }

    return false;
}


bool vpVisionAPI::setCurrentHorizon(const vString& ehName)
{
    vmModel* myModel;
    myModel=&g_app.getModel();
    vgHorizonGroup *eg = myModel->getCurHorizonGroup();
    
    if(!eg)
        return false;
    
    vString eName = eg->getName();
    

    vlAppModel& model = vlAppModel::getRef();
    int nhg = model.getNumHorizonGroup();
    for(int i = 0; i < nhg; ++ i)
    {
        vgHorizonGroup* hg = model.getHorizonGroup(i);
        if(!hg)
            continue;
        
        vString hName = hg->getName();
        if(eName != hName)
            continue;
        
        for(int j=0; j<(hg->getNumHorizon()); j++)
        {
            vgHorizon * hor = hg->getHorizonByIndex(j);
            if(!hor)
                continue;
            
            vString hhName = hor->getName();
            if(hhName == ehName)
            {
                model.setCurHorizon(j);
                return true;
            }
        }
        
    }
    
    return false;
}

vgHorizon* vpVisionAPI::getCurrentHorizon()
{
    vmModel* myModel;
    myModel=&g_app.getModel();
    vgHorizonGroup *eg = myModel->getCurHorizonGroup();
    
    if(!eg)
        return 0;
    
    vgHorizon * ehor =eg->getCurrentHorizon() ;
    
    if(!ehor)
        return 0;
    vString eName = eg->getName();
    vString ehName = ehor->getName();
    
    
    vlAppModel& model = vlAppModel::getRef();
    int nhg = model.getNumHorizonGroup();
    for(int i = 0; i < nhg; ++ i)
    {
        vgHorizonGroup* hg = model.getHorizonGroup(i);
        if(!hg)
            continue;
        
        
        vString hName = hg->getName();
        if(eName != hName)
            continue;
        for(int j=0; j<(hg->getNumHorizon()); j++)
        {
            vgHorizon * hor = hg->getHorizonByIndex(j);
            if(!hor)
                continue;
            
            vString hhName = hor->getName();
            if(hhName == ehName)
            {
                return hor;
            }
        }
        
    }
    
    return 0;
}

void vpVisionAPI::refresh()
{
    vmView3D* myV3D;
    myV3D=&g_app.getView3D();
    myV3D->refresh();
    myV3D->render();
}
