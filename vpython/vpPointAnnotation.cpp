/* 
 * File:   vpPointAnnotation.cpp
 * Author: ddoledec
 * 
 * Created on April 27, 2016, 10:50 AM
 */
#include <boost/python.hpp>

#include "vpPointAnnotation.hpp"

#include "vlPythonInterpreter.hpp"
#include "vlDevWellLog.hpp"
#include "vlLasWellLog.hpp"


#include "vmApplication.hpp"
#include "vmModel.hpp"
#include "vmPointAnnotator.hpp"

vpPointAnnotation::vpPointAnnotation() {
    // Default displayed fields
    mySelectedFields.push_back(5);  // depth
    mySelectedFields.push_back(6);  // mistie
    mySelectedFields.push_back(7);  // mistie %
}

vpPointAnnotation::vpPointAnnotation(const vpPointAnnotation& orig) {
}

vpPointAnnotation::~vpPointAnnotation() {
}
/** DisplaySettings saved in Tornado */
bool vpPointAnnotation::loadDisplaySettings(const std::string& path) {
    FnOK ok;

    ProjectSession session;
    if (!session.loadProject(path.c_str())) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                "Unable to load annotations preference file!");
        return false;        
    }
    ProjectSection* section = session.getRoot();
    getAnnotator()->loadDisplaySettings(ok, section);

    return ok.isOK();
}


void vpPointAnnotation::exposePython() {
    using namespace boost::python;
    namespace bp = boost::python;

    {
        bp::class_<vpPointAnnotation>("PointAnnotation")
                .def("load", &vpPointAnnotation::load)
                .def("loadDisplaySettings", &vpPointAnnotation::loadDisplaySettings)
                .def("unload", &vpPointAnnotation::unload)
                .def("spread", &vpPointAnnotation::spread)
                .def("fields", &vpPointAnnotation::setSelectedFieldNames)
                .def("setVisible", &vpPointAnnotation::setVisible);
    }
}


/** Load plot into the view3D*/
bool vpPointAnnotation::load(const std::string& filename, int genMode) {
    if (filename.empty()) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                "Unable to load PointAnnotation: No file name!");
        return false;
    }

    if (getAnnotator()==0) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                "Only interactive mode!");
        return false;
    }
    
    getAnnotator()->clear();
    if(genMode==0) { // Generate From Wells
        return loadFromWells(filename);
    } else if(genMode==1) { // Generate From Horizons
        return loadFromHrz(filename);
    }
     
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                "Unable to load PointAnnotation: bad mode!");
    return false;
}


vector<DevWellLog*> vpPointAnnotation::getSelectedWell() {
    vector<LasWellLog*> wells;
    g_app.getModel().getWellLogList(wells); 
    
    vector<DevWellLog*> selwells;
    for(int i = 0; i < wells.size(); ++ i) {
         LasWellLog* well = wells[i];   
        selwells.push_back(dynamic_cast<DevWellLog*>(well));
    }
    
    return selwells;
}

vector<int> vpPointAnnotation::getSelectedFieldNames() const {
    return mySelectedFields;  
}


bool vpPointAnnotation::loadFromWells(const std::string& filename)
{
    vString path = filename.c_str();

    vector<DevWellLog*> selwells = getSelectedWell();
    vector<int> field_names = getSelectedFieldNames();

    vmWellMistie mistie(field_names, selwells, path);

    getAnnotator()->getModel()->addWellMistie(mistie);
    g_app.getView3D().render();
    
    return true;
}

bool vpPointAnnotation::loadFromHrz(const std::string& filename) {

    vString path = filename.c_str();

    vgHorizonGroup* hg = g_app.getModel().getCurHorizonGroup();

    if (hg == 0) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                "Unable to load PointAnnotation: No Horizon Group!");
        return false;
    }
    vgHorizon* hrz = hg->getCurrentHorizon();
    if (hrz == 0) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                "Unable to load PointAnnotation: No horizon!");
        return false;
    }

     vector<int> field_names = getSelectedFieldNames();
    vmHrzMistie mistie(field_names, hrz, path);
    getAnnotator()->getModel()->addHrzMistie(mistie);
    
    g_app.getView3D().render();

    return true;
}

/** Remove plot from the view3D*/
bool vpPointAnnotation::unload() {
    getAnnotator()->clear();
    g_app.getView3D().render();

    return true;
}

/** Spread the labels to avoid any overlap */
void vpPointAnnotation::spread(bool flag) {
    getAnnotator()->setAutoSpread(flag);
    getAnnotator()->spreadLabels();
     
    g_app.getView3D().render();
}

/** Change visibility */
void vpPointAnnotation::setVisible(bool flag, const std::string wellname) {
    getAnnotator()->setLineDrawable(!flag);
    getAnnotator()->setLabelDrawable(!flag);
        
    g_app.getView3D().render();
}

vmPointAnnotator* vpPointAnnotation::getAnnotator()
{
    return g_app.getView3D().getPointAnnotator();
}
    
void vpPointAnnotation::setSelectedFieldNames(boost::python::list& lst) {
    Py_ssize_t length = boost::python::len(lst);
     
    mySelectedFields.clear();
    if (length<=0) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning,
                "No information displayed!");
        return ;
    }
        
    vector<vString> fields;
    getAnnotator()->getAllDisplayFields(fields );    
    for (Py_ssize_t i = 0; i < length; i++) {
        string name = boost::python::extract<string>(lst[i]);
        for (int j=0;j<fields.size();j++ ) {
            if (name.compare(fields[j].c_str())==0) {
              mySelectedFields.push_back(j);
              break;
            }
        }
    }

}

