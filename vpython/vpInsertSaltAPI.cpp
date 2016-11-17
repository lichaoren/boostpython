#include "vpInsertSaltAPI.hpp"

#include <boost/python.hpp> 

#include "NoValue.hpp"

#include "vlHorizonGroup.hpp"
#include "vlPythonInterpreter.hpp"
#include "vlVolume.hpp"

#include "vpVolumeProxy.hpp"

class vmSaltDefinition {
public:
    vmSaltDefinition();
    ~vmSaltDefinition();

    bool isCstVelocity() const;

    vector<pair<int, int> > getTopBot() const;
    void setTopBot(vector<pair<int, int> > mySeg);

    vector<pair<float, float> > getShifts() const;
    void setShifts(vector<pair<float, float> > mySegShift);

    float getVelCst() const;
    void setVelCst(float velCst);

    vString getVelVolume() const;
    void setVelVolume(const vString& velVolume);
private:
    vString myVelVolume;
    float myVelCst;
    bool  myVelocityIsCst;
    vector<pair<int, int> > mySeg;
    vector<pair<float, float> > mySegShift; // Time shift per hrz  
};

vmSaltDefinition::vmSaltDefinition() :
     myVelCst(4500),
     myVelocityIsCst(true)
{
}

vmSaltDefinition::~vmSaltDefinition() {
}

bool vmSaltDefinition::isCstVelocity() const {
    return myVelocityIsCst;
}

vector<pair<int, int> > vmSaltDefinition::getTopBot() const {
    return mySeg;
}

void vmSaltDefinition::setTopBot(vector<pair<int, int> > mySeg) {
    this->mySeg = mySeg;
}

vector<pair<float, float> > vmSaltDefinition::getShifts() const {
    return mySegShift;
}

void vmSaltDefinition::setShifts(vector<pair<float, float> > mySegShift) {
    this->mySegShift = mySegShift;
}

float vmSaltDefinition::getVelCst() const {
    if(!myVelocityIsCst)
        return -1;
    return myVelCst;
}

void vmSaltDefinition::setVelCst(float velCst) {
    this->myVelCst = velCst;
    myVelocityIsCst=true;
}

vString vmSaltDefinition::getVelVolume() const {
    if(myVelocityIsCst)
        return "";
    return myVelVolume;
}

void vmSaltDefinition::setVelVolume(const vString& velVolume) {
    this->myVelVolume = velVolume;       
    myVelocityIsCst=false;
}


void vpInsertSaltAPI::exposePython() {
    using namespace boost::python;
    
    void (vpInsertSaltAPI::*addSaltCst)(boost::python::list& hList, float) = &vpInsertSaltAPI::addSalt;
    void (vpInsertSaltAPI::*addSaltCube)(boost::python::list& hList,const string&) = &vpInsertSaltAPI::addSalt;
    
    
    class_<vpInsertSaltAPI>("InsertSalt")
            .def("setHorizonGroup", &vpInsertSaltAPI::setHorizonGroup)
            .def("setInputVolume", &vpInsertSaltAPI::setInputVolume)
            .def("setTargetVolume", &vpInsertSaltAPI::setTargetVolume)
            .def("setOrigSed", &vpInsertSaltAPI::setOrigSed)
            .def("setSaltVelocity", &vpInsertSaltAPI::setSaltVelocity)
            .def("setCheckSaltBoundary", &vpInsertSaltAPI::setCheckSaltBoundary)
            .def("setForceLinear", &vpInsertSaltAPI::setForceLinear)
            .def("setFloodMax", &vpInsertSaltAPI::setFloodMax)
            .def("addSegs", &vpInsertSaltAPI::addSegs)
            .def("setTopHrzIdx", &vpInsertSaltAPI::setTopHrzIdx)
            .def("setBtmHrzIdx", &vpInsertSaltAPI::setBtmHrzIdx)
            .def("insertSalt", &vpInsertSaltAPI::insertSalt)
            .def("run", &vpInsertSaltAPI::insertSalt)
//            .def("addSalt", &vpInsertSaltAPI::addSalt)
            .def("addSalt", addSaltCst)
            .def("addSalt", addSaltCube)
            .def("setHorizonPairs", &vpInsertSaltAPI::addHorizonPairs);
}



vpInsertSaltAPI::vpInsertSaltAPI() : 
     myHG(0),
     mySingleLayerDefinition(true)   
{
    myInserter.setEnableProgress(false);
    myInserter.setupCut(false, 0);
    myInserter.setCheckSaltBoundary(false);
    myInserter.setFloodMax(false);
//    mySediment = "";
//    velocity=-1;
    
    // Add a layer (single definition))
    vmSaltDefinition dummySalt;
    mySaltDefinition.push_back(dummySalt);
}

vpInsertSaltAPI::~vpInsertSaltAPI() {
    delete myHG;
}

void vpInsertSaltAPI::setHorizonGroup(string horizonName) {
    vgHorizonGroup *hg = new vgHorizonGroup;
    if (!hg->load(horizonName.c_str())){        
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                " Failed to load Horizon!" );
    }
    myHG=hg;
}
void vpInsertSaltAPI::setInputVolume(string inputVolume) {
    myInput = inputVolume;
}
void vpInsertSaltAPI::setTargetVolume(string targetName) {
    myTarget = targetName;
}
void vpInsertSaltAPI::setOrigSed(string sedName) {
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning, 
            "==== Deprecated === Rather use addSalt()");

    vmSaltDefinition& saltDef = getSingleLayerDef();
    saltDef.setVelVolume(sedName.c_str());
}
void vpInsertSaltAPI::setSaltVelocity(float vel) {
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning, 
            "==== Deprecated === Rather use addSalt()");   
        
    vmSaltDefinition& saltDef = getSingleLayerDef();
    saltDef.setVelCst(vel);

    // velocity=vel;
}
void vpInsertSaltAPI::setCheckSaltBoundary(bool check) {
    myInserter.setCheckSaltBoundary(check);
}
void vpInsertSaltAPI::setForceLinear(bool check) {
    myInserter.setLinear(check);
}
void vpInsertSaltAPI::setFloodMax(bool flood) {
    myInserter.setFloodMax(flood);
}

vector<int> vpInsertSaltAPI::list2Vector(boost::python::list& lst){
    vector<int> res;
    Py_ssize_t length = boost::python::len(lst);
    for (Py_ssize_t i = 0; i < length; i++) {
        int val = boost::python::extract<int>(lst[i]);
        res.push_back(val);
    }
    return res;
}

/* 
 * segs pair is horizon index-1
 * bottom is -1
 * 
 */
void vpInsertSaltAPI::addSegs(Vect2i tbpairs) {
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning, 
            "==== Deprecated === Rather use addSalt()");       

    vmSaltDefinition& saltDef = getSingleLayerDef();
    
    std::vector<std::pair<int, int> > tbList = saltDef.getTopBot();
    tbList.push_back(pair<int, int>(tbpairs[0], tbpairs[1]));
    saltDef.setTopBot(tbList);
}


void vpInsertSaltAPI::addSalt(boost::python::list& hList, float velocity) {
    vmSaltDefinition saltLayer;
    saltLayer.setVelCst(velocity);
    
    addHorizonPairsToDefinition(hList,saltLayer);
        
    getMultiLayerDef().push_back(saltLayer);
}

void vpInsertSaltAPI::addSalt(boost::python::list& hList, const string& velocity) {    
    vmSaltDefinition saltLayer;
    saltLayer.setVelVolume(velocity.c_str());
    
    addHorizonPairsToDefinition(hList,saltLayer);
    
    getMultiLayerDef().push_back(saltLayer);
}


void vpInsertSaltAPI::addHorizonPairs(boost::python::list& lst) {
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning,
            "==== Deprecated === Rather use addSalt()");

    vmSaltDefinition& saltDef = getSingleLayerDef();
    addHorizonPairsToDefinition(lst, saltDef);
}

vmSaltDefinition& vpInsertSaltAPI::getSingleLayerDef() {
    if (!mySingleLayerDefinition) {
        mySaltDefinition.clear();
        mySingleLayerDefinition = true;

        // Add a layer (single definition))
        vmSaltDefinition dummySalt;
        mySaltDefinition.push_back(dummySalt);
    }

    vmSaltDefinition& saltDef = mySaltDefinition.front();
    return saltDef;
}

list<vmSaltDefinition>& vpInsertSaltAPI::getMultiLayerDef() {
    if (mySingleLayerDefinition) {
        mySaltDefinition.clear();
        mySingleLayerDefinition = false;
    }

    return mySaltDefinition;
}

/** 
 * couple of top, bottom 
 * horizon_salt=[['SED_PATCH_T1','SED_PATCH_B1'],['OH_SED_T1','OH_SED_B1']]
 * Can have a shift value for each horizon 
 * horizon_salt2=[[('SED_PATCH_T1',-100),'SED_PATCH_B1'],['OH_SED_T1','OH_SED_B1']]
 */
void vpInsertSaltAPI::addHorizonPairsToDefinition(boost::python::list& lst, vmSaltDefinition& saltDef){
    Py_ssize_t length = boost::python::len(lst);
    // loop for list
    vString message;

    if (myHG == 0) {
        message = "Need to specify the Horizon Group Before defining the top/bottom";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                message);
        return;
    }
    
    
    vector<pair<int,int> > topBotList;
    vector<pair<float,float> > horizonShifts;
    for (Py_ssize_t i = 0; i < length; i++) {
        Py_ssize_t len_each = boost::python::len(lst[i]);
        // need to make sure the parameter list is right
        if (len_each != 2)
            continue;

        boost::python::list lstOfTopBot = boost::python::extract<boost::python::list>(lst[i]);
        
        string top;
        float shiftTop=0;
        boost::python::extract<boost::python::tuple> tpl1(lstOfTopBot[0]);
        if (tpl1.check())
        {
            boost::python::tuple tpl = tpl1();
            top = boost::python::extract<string>(tpl[0]);
            shiftTop = boost::python::extract<int>(tpl[1]);

            message << "Horizon: " << top.c_str() << "  Shift: " << shiftTop;
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, 
                    message);            
        } else
        {
            top = boost::python::extract<string>(lstOfTopBot[0]);
        }

        float shiftBot=0;
        string btm;
        boost::python::extract<boost::python::tuple> tpl2(lstOfTopBot[1]);
        if (tpl2.check())
        {
            boost::python::tuple tpl = tpl2();
            btm = boost::python::extract<string>(tpl[0]);
            shiftBot = boost::python::extract<int>(tpl[1]);
            
            std::ostringstream mess;
            mess << "Horizon: " << btm << "  Shift: " << shiftBot << endl;
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message,
                mess.str().c_str() );
        } else
        {
            btm = boost::python::extract<string>(lstOfTopBot[1]);
        }
        
        int index_top= top.empty()? -1: myHG->getHorizonIndexByName(top.c_str());
        int index_btm= btm.empty()? -1: myHG->getHorizonIndexByName(btm.c_str());
        if (!top.empty() && index_top<0){
            std::ostringstream message;
            message << "Your input horizon "  << top.c_str() << " is a wrong name";
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                    message.str().c_str());

        }
        if (!btm.empty() && index_btm<0){
            message <<"Your input horizon " << btm.c_str() <<" is a wrong name";
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                    message);
        }
        
        message = vString("The pair [")<< top.c_str() <<","<< btm.c_str() <<"] corresponding to [" << index_top << ","
             << index_btm << "] has been added to insert salt horizons";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);
        
        
        topBotList.push_back(pair<int,int>(index_top, index_btm));
        horizonShifts.push_back(pair<float,float>(shiftTop, shiftBot));
    
    }
    
    saltDef.setShifts(horizonShifts);
    saltDef.setTopBot(topBotList);    
}

void vpInsertSaltAPI::setTopHrzIdx(boost::python::list& lst) {
    vector<int> res=list2Vector(lst);
    myInserter.setTopHrzIdx(res);
}
void vpInsertSaltAPI::setBtmHrzIdx(boost::python::list& lst) {
    vector<int> res=list2Vector(lst);
    myInserter.setBtmHrzIdx(res);
}

/** Create a volume with the same geometry */
static Volume* dupVolume(Volume* input, const vString& path){
    Volume* vol= new Volume;
    vol->createFdm(path, *input);
    return vol;
}

    /**
 * Shift/Unshift Horizons before insert salt
 * @param unapply true: apply, false:unapply
 * @return 
 */
bool vpInsertSaltAPI::shiftHorizons(vmSaltDefinition& saltDefinition, bool apply )
{
    vector<pair<float,float> > horizonShifts = saltDefinition.getShifts();
    vector<pair<int,int> > topBotList = saltDefinition.getTopBot();
    
    int nbHrzPairs = horizonShifts.size();
    vgHorizonGroup* hg = getHorizonGrp();
    if (hg == 0 || nbHrzPairs != topBotList.size())
        return false;
    Vect4i size_geom = hg->getSize();
    size_t nbValues = size_geom[0] * size_geom[1];
    float NoValue;
    vUtl::setNoValue(NoValue);

    for (int i = 0; i < nbHrzPairs; i++)
    {
        pair<int, int> hrzIdxPair = topBotList[i];
        pair<float, float> shiftPair = horizonShifts[i];
        for (int j = 0; j < 2; j++)
        {
            float shift = 0;
            int hrzIdx = 0;
            if (j == 0)
            {
                shift = shiftPair.first;
                hrzIdx = hrzIdxPair.first;
            } else
            {
                shift = shiftPair.second;
                hrzIdx = hrzIdxPair.second;
            }
            
            if (shift != 0)
            {
                vgHorizon* hrz = hg->getHorizonByIndex(hrzIdx);
                float* pt = hrz->getAttrPtr(ATTR_NAME_DEPTH);
                if (pt == 0)
                {
                    pt = hrz->getAttrPtr(ATTR_NAME_TWT);
                    if (pt == 0)
                        return false;
                }
                float* ptEnd = pt + nbValues;
                while (pt != ptEnd)
                {
                    if ((*pt) != NoValue)
                        if (apply)
                            (*pt) += shift;
                        else
                            (*pt) -= shift;
                    pt++;
                }
            }
        }
    }

    return true;
}

vgHorizonGroup* vpInsertSaltAPI::getHorizonGrp() const
{
    return myHG;
}

void vpInsertSaltAPI::insertSaltNoChunk() {
       
    if (mySaltDefinition.size() <= 0 || myHG == 0)
    {
        std::ostringstream message;
        message << "No Horizon !!!! ";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                message.str().c_str());
        return;
    }
        
    myInserter.setHorizons(myHG);

    list<vmSaltDefinition>::const_iterator iter = mySaltDefinition.begin();

    Volume *source_vol = new Volume;
    source_vol->load(myInput.c_str());
    Volume* target_vol = dupVolume(source_vol, myTarget.c_str());

    Volume* work_vol = source_vol;
    while (iter != mySaltDefinition.end()) {
        vmSaltDefinition saltDef = (*iter);

        if (saltDef.isCstVelocity()) {
            float velocity = saltDef.getVelCst();

            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message,
                    "Will insert a constant salt velocity ");

            myInserter.setSaltVel(velocity);
        }
        
        vector<pair<int,int> > mySeg = saltDef.getTopBot();
        myInserter.setSaltPairs(mySeg);

        shiftHorizons(saltDef, true);

        Volume* sed_vol = 0;
        if (saltDef.isCstVelocity()) {
            myInserter.insertSalt(work_vol, target_vol, sed_vol);
        } else {
            sed_vol = new Volume;
            vString sediment = saltDef.getVelVolume();
            sed_vol->load(sediment.c_str());
            myInserter.insertSediment(work_vol, target_vol, sed_vol);
        }

        if (sed_vol)
            delete sed_vol;

        shiftHorizons(saltDef, false);
        iter++;
        work_vol=target_vol;
    }

    target_vol->saveAs(myTarget.c_str());
    delete target_vol;

    delete source_vol;
}
#define MEMORY_PER_VOLUME 3  // 9 GB for 3 volumes
void vpInsertSaltAPI::insertSalt() {
    if (mySaltDefinition.size() <= 0 || myHG == 0) {
        std::ostringstream message;
        message << "No Horizon !!!! ";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                message.str().c_str());
        return;
    }

    vpVolumeProxy proxy;
    proxy.setSubVolSize(MEMORY_PER_VOLUME); 

    // Enough memory --> No chunk
    if (proxy.isToSplit(myInput.c_str())) {
        insertSaltChunk();
    } else {
        insertSaltNoChunk();
    }

}

void vpInsertSaltAPI::insertSaltChunk() {
    double source_rx = 0, source_ry = 0, source_rz = 0;
    if (vpVolumeProxy::getVolumeStep(myInput.c_str(), source_rx, source_ry, source_rz) == false) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                "Unable to load Salt volume !");
        return;
    }

    // 9 GB for 3 volumes
    vpVolumeProxy proxySource;
    proxySource.setSubVolSize(MEMORY_PER_VOLUME);
    proxySource.setOriginPath(myInput.c_str());
    proxySource.split();
    proxySource.setToSaveAs(myTarget.c_str());
    int n = proxySource.getNumVolume();

    myInserter.setHorizons(myHG);

    vString infoMessage;
    infoMessage << "Expected job number: " << n;
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message,
            infoMessage);

    for (int i = 0; i < n; i++) {
        Volume* vol_s = proxySource.getVolume(i);
        Volume* vol_t = dupVolume(vol_s, vol_s->getPath());
        Volume* work_vol = vol_s;

        list<vmSaltDefinition>::const_iterator iter = mySaltDefinition.begin();
        while (iter != mySaltDefinition.end()) {
            vmSaltDefinition saltDef = (*iter);

            if (saltDef.isCstVelocity()) {
                float velocity = saltDef.getVelCst();

                std::ostringstream message;
                message << "Will insert the salt velocity " << velocity;
                vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message,
                        message.str().c_str());
                
                myInserter.setSaltVel(velocity);
            }

            vector<pair<int, int> > mySeg = saltDef.getTopBot();
            myInserter.setSaltPairs(mySeg);

            shiftHorizons(saltDef, true);


            if (saltDef.isCstVelocity()) {
                myInserter.insertSalt(work_vol, vol_t, 0);
            } else {
                // Sediment Velocity
                vpVolumeProxy proxySed;
                proxySed.setSubVolSize(MEMORY_PER_VOLUME);

                vString sediment = saltDef.getVelVolume();

                double sed_rx = 0, sed_ry = 0, sed_rz = 0;
                if (vpVolumeProxy::getVolumeStep(sediment.c_str(), sed_rx, sed_ry, sed_rz) == false) {
                    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Unable to load Sediment volume");
                    return;
                }

                if (sed_rx != source_rx ||
                        sed_ry != source_ry ||
                        sed_rz != source_rz) {
                    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Inconsistent Sediment and Salt geometries");
                    return;
                }

                proxySed.setOriginPath(sediment.c_str());
                proxySed.split();

                Volume* vol_d = proxySed.getVolume(i);
                myInserter.insertSediment(work_vol, vol_t, vol_d);

                delete vol_d;
            }
            
            shiftHorizons(saltDef, false);            
            iter++;
            
            work_vol = vol_t;
        }

        vol_t->save();
        delete vol_s;
        delete vol_t;

        infoMessage = vString("==================== Job #") << (i + 1) << "/" << n << " is done";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message,
                infoMessage);

    }

    proxySource.merge();
    proxySource.cleanUp();
}

/* Benchmark
 * New
 * 426.279u 31.239s 14:26.39 52.8%	0+0k 56045912+16754928io 0pf+0w
 * 592.582u 61.155s 11:54.65 91.4%	0+0k 11263536+55790832io 0pf+0w
 * 584.414u 60.059s 15:28.61 69.4%	0+0k 55959600+55790808io 2pf+0w
 * 
 * Prev
 * 956.686u 156.093s 23:07.16 80.2%	0+0k 55959096+223163232io 0pf+0w
 */

