/* 
 * File:   vpMathAPI.cpp
 * Author: yanhliu
 * 
 * Created on March 11, 2015, 3:17 PM
 */

#include "vpMathAPI.hpp"
#include "vlFormulaEval.hpp"
#include "vpVolumeProxy.hpp"
#include "vlPythonInterpreter.hpp"

vpMathAPI::vpMathAPI() {
    model.withGui = false;
    this->null_value = 0.;
    this->myHG = 0;
    this->useConstraint = false;
}

vpMathAPI::vpMathAPI(float val) {
    model.withGui = false;
    this->null_value = val;
    this->myHG = 0;
    this->useConstraint = false;
}

vpMathAPI::vpMathAPI(vgHorizonGroup* hg, boost::python::list hrzs, bool fillup, float nullVal) {
    model.withGui = false;
    this->null_value = nullVal;
    this->useConstraint = false;
    if (hg == 0 || boost::python::len(hrzs) == 0)
        return;
    this->myHG = hg;
    this->myHrzs.clear();
    for (int i = 0; i<int(boost::python::len(hrzs)); i++) {
        myHrzs.push_back(boost::python::extract<vgHorizon*>(hrzs[i]));
    }
    this->fillup = fillup;
    this->useConstraint = true;
}

vpMathAPI::~vpMathAPI() {
}

template<>
void dump_vpMathStruct(float arg, vpMathAPI::mathStruct& st, int seq) {
    if (seq == 1) {
        st.constant1 = arg;
        st.iatt1 = -1;
        st.va = 0;
    } else {
        st.constant2 = arg;
        st.iatt2 = -1;
        st.vb = 0;
    }
    return;
}

template<>
void dump_vpMathStruct(Volume* arg, vpMathAPI::mathStruct& st, int seq) {
    if (seq == 1) {
        st.constant1 = 0;
        st.iatt1 = 1;
        st.va = arg;
    } else {
        st.constant2 = 0;
        st.iatt2 = 1;
        st.vb = arg;
    }
    return;
}

Volume* vpMathAPI::sin(Volume* vol) {
    return this->time_sin<float, Volume*>(1., vol);
}

Volume* vpMathAPI::cos(Volume* vol) {
    return this->time_cos<float, Volume*>(1., vol);
}

Volume* vpMathAPI::tan(Volume* vol) {
    return this->time_tan<float, Volume*>(1., vol);
}

Volume* vpMathAPI::arcsin(Volume* vol) {
    return this->time_arcsin<float, Volume*>(1., vol);
}

Volume* vpMathAPI::arccos(Volume* vol) {
    return this->time_arccos<float, Volume*>(1., vol);
}

Volume* vpMathAPI::arctan(Volume* vol) {
    return this->time_arctan<float, Volume*>(1., vol);
}

Volume* vpMathAPI::sqrt(Volume* vol) {
    return mathHelper<Volume*, float>(vol, 0., 10);
}

Volume* vpMathAPI::log10(Volume* vol) {
    return this->time_log10<float, Volume*>(1., vol);
}

Volume* vpMathAPI::abs(Volume* avol) {
    return mathHelper<Volume*, float>(avol, 0., 14);
}

Volume* vpMathAPI::sum_top_down(Volume* vol) {
    return mathHelper<Volume*, float>(vol, 1., 15);
}

Volume* vpMathAPI::d_dx(Volume* avol) {
    return mathHelper<Volume*, float>(avol, 0., 16);
}

Volume* vpMathAPI::d_dy(Volume* bvol) {
    return mathHelper<Volume*, float>(bvol, 1., 17);
}

Volume* vpMathAPI::d_dz(Volume* vol) {
    return mathHelper<Volume*, float>(vol, 0., 18);
}

Volume* vpMathAPI::sum_btm_up(Volume* vol) {
    return mathHelper<Volume*, float>(vol, 0., 20);
}

static Volume* evalVolumes(FormulaEval& eval, const vector<Volume*> &vols,
        const vector<int>& indices) {

#if 0
    for (int t = 0; t < vols.size(); t++) {
        cerr << vols[t]->getPath() << " : " << indices[t] << endl;
    }
#endif    
    Volume* vol = new Volume;
    vol->createFdm("", *vols[0]);
    float* objData = (float*) vol->getData();

    Vect4i size = vols[0]->getSize();
    vector<float*> datas;
    for (int eachVol = 0; eachVol < vols.size(); eachVol++) {
        float* data = (float*) vols[eachVol]->getData();
        datas.push_back(data);
    }
    size_t total = (size_t) size[0]*(size_t) size[1]*(size_t) size[2];
    vector<double> prepare(indices.size());
    for (size_t i = 0; i < total; i++) {

        for (int j = 0; j < indices.size(); j++) {
            prepare[j]=datas[indices[j]][i];
        }
        float value = (float) eval.eval(prepare);
        objData[i] = value;
    }

    return vol;
}

void vpMathAPI::setInput(boost::python::list lst) {
    Py_ssize_t length = boost::python::len(lst);
    for (Py_ssize_t i = 0; i < length; i++) {
        string path = boost::python::extract<string>(lst[i]);
        myVolumeNames.push_back(path);
    }
}

void vpMathAPI::setOutput(string output) {
    myOutputName = output;
}

void vpMathAPI::setFormula(string formula) {
    myFormulaString = formula;
}

void vpMathAPI::doMath() {
    bigVolumeMath(myVolumeNames, myFormulaString, myOutputName);
}

void vpMathAPI::bigVolumeMath(vector<string>& vars, string formulaStr, string output) {
    vString message;
    FormulaEval formula;
    formula.setQuiet(false);
        
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Doing pre-check of the input formula");
    // set the formula for parsing
    if (!formula.setFormula(formulaStr.c_str())) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Wrong input formula");
    }

    // parse the formula to get the variable names
    vector<vString> variables = formula.getVariables();
    if (variables.empty() || vars.empty()) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Failed to scan the variables");
    }

    // make sure there are not too much data, we assume min memmory is 16 GB
    if (vars.size() >= 8) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Too much variables, the upper limit is 15");
    }
    int i;
    vector<int> indices;
    // parse each variables and store the indices into vector
    for (i = 0; i < variables.size(); i++) {
        int tmp;
        if (sscanf(variables[i].c_str(), "VOL%d", &tmp) < 1) {
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                    "The naming of your input must be VOL followed by a number");
        }
        indices.push_back(tmp);
    }

    // check the illegal indices
    for (i = 0; i < variables.size(); i++) {
        // the indices should be <=n-1
        if (indices[i] >= vars.size()) {
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                    "Check your index followed by VOL or check if you have set your variable to variable list");
        }
    }        
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "The inputs are legal");
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Doing geometry check, requiring all the input have the same geometry");    
    
    // check the geometry load the header
    vector<Volume*> vols;
    for (i = 0; i < vars.size(); i++) {
        Volume* dummy = new Volume;
        dummy->setCreateHint(Volume::eHintLSUB);
        if (!dummy->loadHeader(vars[i].c_str())) {
            message="Cannot open your provided volume: ";
            message += vars[i].c_str();
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message);
            delete dummy;
        }
        vols.push_back(dummy);
    }
    BaseGeometry firstGeom = *vols[0];
    Vect4i geomSize = firstGeom.getSize();
    size_t eachSize;
    eachSize = sizeof (float)*(size_t) geomSize[0]*(size_t) geomSize[1]*
                (size_t) geomSize[2];
    // make sure each data has the same geometry
   
    if (vols.size() > 1) {
        for (i = 1; i < vols.size(); i++) {
            BaseGeometry otherGeom = *vols[i];
            if (firstGeom.getMin() != otherGeom.getMin() ||
                    firstGeom.getMax() != otherGeom.getMax() ||
                    firstGeom.getInc() != otherGeom.getInc() ||
                    firstGeom.getSize() != otherGeom.getSize() ||
                    firstGeom.getOrigin() != otherGeom.getOrigin() ||
                    firstGeom.getStep() != otherGeom.getStep()
                    ) {
                vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                        "The input geometry need to be the same");
            }
        }
    }
    // clean up the volumes only with headers
    for (i = 0; i < vols.size(); i++) {
        delete vols[i];
    }
    vols.clear();     
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Geometry of the inputs are all good");

    // the file size of chunk, we assume 8G on cluster, but leave 1G for tornado
    // also, we need to produce an output volume out, (16-1)/(n+1)
    int fsize = 8 / (vars.size() + 1);
    
    bool isMemGood = eachSize < (size_t) fsize*GEGA;

    // if we do not need to split, directly run it
    if (isMemGood) {
        for (i = 0; i < vars.size(); i++) {
            Volume* dummy = new Volume;
            dummy->load(vars[i].c_str());
            vols.push_back(dummy);
        }
        Volume* res = evalVolumes(formula, vols,
                indices);
        res->saveAs(output.c_str());
        delete res;
        return;
    }

    // do trunk and do Volume math
    vector<vpVolumeProxy*> proxy(vars.size());
    for (i=0; i<proxy.size(); i++){
        proxy[i]=new vpVolumeProxy();
    }
    for (i = 0; i < proxy.size(); i++) {
        // install each volue to their proxy
        //proxy[i]->init();
        proxy[i]->setSubVolSize(fsize);
        proxy[i]->setOriginPath(vars[i].c_str());
        proxy[i]->split();
        proxy[i]->setToSaveAs(output.c_str());
    }
    int j;
    int n = proxy[0]->getNumVolume();
    
    message=vString("The job has been divided to ") + vString::number(n)+ vString(" pieces");
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);

    for (i = 0; i < n; i++) {
        // loop each chunk
        vector<Volume*> localVols;
        for (j = 0; j < proxy.size(); j++) {
            // loop all needed volume chunks
            Volume* tmpvol = proxy[j]->getVolume(i);
            localVols.push_back(tmpvol);
        }
        Volume* oneRes = evalVolumes(formula, localVols,
                indices);
        // save result to file system
        oneRes->saveAs(localVols[0]->getPath());
        
        // clean up memory !important
        delete oneRes;
        for (j = 0; j < proxy.size(); j++) {
            delete localVols[j];
        }
        localVols.clear();
        
        message="The job #";
        message += vString::number( i+1);
        message += " has finished";
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);
    }
    
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Merging all the results");

    // house keeping jobs
    proxy[0]->merge();
    proxy[0]->cleanUp();
    for (i=0; i<proxy.size(); i++){
        delete proxy[i];
    }
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, "Job done !");
}

