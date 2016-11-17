/* 
 * File:   vpWellTieMap.cpp
 * Author: ddoledec
 * 
 * Created on April 21, 2016, 5:17 PM
 */
#include <boost/python.hpp>

#include "vpWellTieMap.hpp"

// vlVisLib
#include "vlFdmCube.hpp"
#include "vlHorizonGroup.hpp"
#include "vlLasWellLog.hpp"
#include "vlVolume.hpp"

// Python
#include "vlPythonInterpreter.hpp"


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpWellTieMap_overloads_interpmode, setInterpolationMode, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpWellTieMap_overloads_missingTops, interpolateMissingTops, 1, 2)

/** resolve overloaded method ambiguity */
bool (vpWellTieMap::*runCheckShot) (vgHorizonGroup* hg,
        boost::python::list& horizonList,
        Volume* vol) = &vpWellTieMap::run;
bool (vpWellTieMap::*runTopWellPairs) (vgHorizonGroup* hg,
        boost::python::list& horizonWellPair) = &vpWellTieMap::run;


vpWellTieMap::vpWellTieMap() {
}

vpWellTieMap::~vpWellTieMap() {
}

/**
 * Horizontal interpolation between misties
 * @param method  interpolation method
 * @param tension MinCurvature tension
 * @param level   MinCurvature level
 */
void vpWellTieMap::setInterpolationMode(GridInterp::Mode method, float tension, int level) {

    setInterpMode(method);
    if (method == GridInterp::MINIMUM_CURVATURE) {
        getMinCurvParams().myTension = tension;
        getMinCurvParams().myMaxMultiGridLevels = level;
    }
}

/**
 * 
 * @param verticalInterpolation
 * @param outputInterpolatedMisite
 */
void vpWellTieMap::interpolateMissingTops(bool verticalInterpolation,
        bool outputInterpolatedMisite) {

    setVerticalInterp(verticalInterpolation);
    setOutputInterp(outputInterpolatedMisite);
}


void vpWellTieMap::exposePython() {
    using namespace boost::python;

    class_<vpWellTieMap>("HorizonMisite")
            .def("setAnnotationFile", &vpWellTieMap::setInfoOutput)

            .def("run", runTopWellPairs)
            .def("run", runCheckShot)
    
            .def("setOutputAttrName", &vpWellTieMap::setOutAttr)
            .def("setDomain", &vpWellTieMap::setDomainAttr)

            .def("structureBased", &vpWellTieMap::setStructureBased)
            .def("interpolateMissingTops", &vpWellTieMap::interpolateMissingTops, vpWellTieMap_overloads_missingTops())
            .def("setInterpolationMode", &vpWellTieMap::setInterpolationMode, vpWellTieMap_overloads_interpmode())
            .def("setWellList", &vpWellTieMap::setWellList)
            ;
}

/** 
 * couple of horizon, well marker  
 * horizon_well_pair=[['topH1','TopWell1'],[''topH2','TopWell2']]
 * ### later ????
 * You can have a shift value 
 * horizon_well_pair=[[('topH1',-100),'TopWell1'],[''topH2',('TopWell2',100)]] 
 */
bool vpWellTieMap::setHorizonWellPairs(const vgHorizonGroup& myHG, boost::python::list& lst) {
    Py_ssize_t length = boost::python::len(lst);

    // loop for list
    vString message;


    vector<vString> topNames;
    vector<vgHorizon *> hlist;

    for (Py_ssize_t i = 0; i < length; i++) {
        Py_ssize_t len_each = boost::python::len(lst[i]);
        // need to make sure the parameter list is right
        if (len_each != 2)
            continue;

        string top;
        float shiftTop = 0;
        boost::python::extract<boost::python::tuple> tpl1(lst[i][0]);
        if (tpl1.check()) {
            boost::python::tuple tpl = tpl1();
            top = boost::python::extract<string>(tpl[0]);
            shiftTop = boost::python::extract<int>(tpl[1]);

            message << "Horizon: " << top.c_str() << "  Shift: " << shiftTop;
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message,
                    message);
        } else {
            top = boost::python::extract<string>(lst[i][0]);
        }

        float shiftBot = 0;
        string btm;
        boost::python::extract<boost::python::tuple> tpl2(lst[i][1]);
        if (tpl2.check()) {
            boost::python::tuple tpl = tpl2();
            btm = boost::python::extract<string>(tpl[0]);
            shiftBot = boost::python::extract<int>(tpl[1]);
            cerr << "Horizon: " << btm << "  Shift: " << shiftBot << endl;
        } else {
            btm = boost::python::extract<string>(lst[i][1]);
        }


        vgHorizon* horizon = top.empty() ? 0 : myHG.getHorizonByName(top.c_str());

        if (horizon == 0) {
            std::ostringstream message;
            message << "Your input horizon " << top.c_str() << " is a wrong name";
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                    message.str().c_str());

        }

        if (btm.empty()) {
            message << "No well top associated to horizon " << top.c_str();
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                    message);
        }

        message = vString("The pair [") << top.c_str() << "," << btm.c_str() << "] has been added";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);

        topNames.push_back(btm.c_str());
        hlist.push_back(horizon);
    }
    setTopList(topNames);
    setHrzList(hlist);

    return true;
}

bool vpWellTieMap::setHorizons(const vgHorizonGroup& myHG, 
        boost::python::list& lst) {
    Py_ssize_t length = boost::python::len(lst);

    // loop for list
    vString message;

    vector<vgHorizon *> hlist;
    vector<vString> topNames;
    for (Py_ssize_t i = 0; i < length; i++) {
        string hrz_name = boost::python::extract<string>(lst[i]);

        vgHorizon* horizon = hrz_name.empty() ? 0 : myHG.getHorizonByName(hrz_name.c_str());
        if (horizon == 0) {
            std::ostringstream message;
            message << "Your input horizon " << hrz_name.c_str() << " is a wrong name";
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                    message.str().c_str());
            return false;
        }

        hlist.push_back(horizon);
        topNames.push_back("");
    }

   
    setTopList(topNames); // Unused, but mandatory
    setHrzList(hlist);

    return true;
}

bool vpWellTieMap::initHorizonGroup(vgHorizonGroup& hg) {
    // Check DOMAIN
    if (getDomainAttr().isEmpty()) {
        vString message, domainName;

        if (hg.getAttrIndex(ATTR_NAME_DEPTH) != 1)
            domainName = ATTR_NAME_DEPTH;
        else if (hg.getAttrIndex(ATTR_NAME_DEPTH) != 1)
            domainName = ATTR_NAME_TWT;
        else {
            message = "No Domain defined in the process vpWellTieMap!";
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message);
            return false;
        }

        message = vString("Domain ") + domainName + " is used";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning, message);
        setDomainAttr(domainName);
    }
    return true;
}



bool vpWellTieMap::run(vgHorizonGroup* hg,
        boost::python::list& horizonList,
        Volume* vol) {

    if (!vol) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "WellTieMap: Unable to load velocity model!");
        return false;
    }
      
    if (!hg) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "WellTieMap: Unable to load Horizon Group!");
        return false;
    }
    
    if (!initHorizonGroup(*hg))
        return false;

    /** set the list of pairs (hrz/well top) to the base class*/
    if (setHorizons(*hg, horizonList) == false)
        return false;

    /** Velocity model */
    FdmCube* fdm = vol?dynamic_cast<FdmCube*> (vol->getCube()):0;
    if (!fdm) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "WellTieMap: Unable to load velocity model!");
        return false;
    }
    
    setByCheckshots(true);
    setInput(vlWellTieMap::GENERAL);

    vector<LasWellLog*> wells = getSelectedWells();
    FnOK status = calcMistie(wells, hg, fdm);
    if (!status.isOK()) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                status.getMsg());
        return false;   
    }
    return status.isOK();
}

bool vpWellTieMap::run(vgHorizonGroup* hg, 
        boost::python::list& horizonWellPair) {
    
    if (!hg) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "WellTieMap: Unable to load Horizon Group!");
        return false;
    }
    
    if (!initHorizonGroup(*hg))
        return false;

    /** set the list of pairs (hrz/well top) to the base class*/
    if (setHorizonWellPairs(*hg, horizonWellPair) == false)
        return false;

    setInput(vlWellTieMap::GENERAL);
    setByCheckshots(false);
    
    vector<LasWellLog*> wells = getSelectedWells();
    FnOK status = calcMistie(wells, hg);

    return status.isOK();
}

/** 
 * @note if not defined the process will take all the wells loaded in tornado
 * @brief Define the list of well to use for the process.
 * @param wellList @see vision.getWellList()
 */
void vpWellTieMap::setWellList(boost::python::list wellList) {
    myWellList.clear();

    Py_ssize_t length = boost::python::len(wellList);
    for (Py_ssize_t i = 0; i < length; i++) {
        LasWellLog* wl = boost::python::extract<LasWellLog*>(wellList[i]);
        if (wl) {
            myWellList.push_back(wl);
        }
    }

}





/** This must be improved. We need to be able to load/get well from python
 * the access to the model must disappear from here, this is why I keep the headers here.
 */
#include "vmModel.hpp"
#include "vmApplication.hpp"

vector<LasWellLog*> vpWellTieMap::getSelectedWells() {
    if (myWellList.size()>0)
        return myWellList;
    
    vmModel& model = g_app.getModel();
    vector<LasWellLog*> wells;
    model.getWellLogList(wells); 
    
    return wells;
}





