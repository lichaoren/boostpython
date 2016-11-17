/* 
 * File:   vpRcaSmoothAPI.cpp
 * Author: yanhliu
 * 
 * Created on February 3, 2015, 1:12 PM
 */
#include "vpRcaSmoothAPI.hpp"
#include "Pythoncompatibility.hpp"

#include "vlFdmCube.hpp"
#include "utilNumerical.hpp"

/**
 * helper function to convert given dip cube to model accepted theta/phi
 * @param dipxC a dipx cube 
 * @param dipyC a dipy cube
 * @param thetaC an existed cube to contain converted theta
 * @param phiC  an existed cube to contain converted phi
 * @param type 1: dip in radian, 2:degrees, 3: m/m
 *
 */
static bool myConvertDip(FdmCube& dipxC, FdmCube& dipyC, FdmCube &thetaC, FdmCube &phiC, int type) {
    if (type<1 || type>3)
        return false;
    int unit_in = type;
    Vect4i size = dipxC.getSize();
    if (size != dipyC.getSize()) {
        std::cerr << "Different geometry of dipx and dipy" << std::endl;
        return false;
    }
    double smallval = 0.0001;
    int nx = size[0];
    int ny = size[1];
    int nz = size[2];
    size_t n_size = (size_t) nx * ny*nz;
    Vect2d range1 = dipxC.getRange();
    Vect2d range2 = dipyC.getRange();

    float *data_in1 = (float*) dipxC.getData();
    float *data_in2 = (float*) dipyC.getData();
    float* data_out1 = (float*) thetaC.getData();
    float* data_out2 = (float*) phiC.getData();
    if (!data_in1 || !data_in2 || !data_out1 || !data_out2)
        return false;
    // now loop it
    for (size_t i = 0; i < n_size; ++i) {

        double dipx = data_in1[i];
        double dipy = data_in2[i];

        double phi = 0.0, theta = 0.0;

        if (unit_in == 3) { //m/m, follow UK code
            if (fabs(dipx) > smallval || fabs(dipy) > smallval) {
                phi = atan2(dipy, dipx);
                theta = atan(cos(phi) * dipx + sin(phi) * dipy);

                phi += M_PI;
            }

        } else { //radian or degree, follow HPC code

            if (unit_in == 2) { //degree to radian
                dipx = Deg2Rad(dipx);
                dipy = Deg2Rad(dipy);
            }
            dipXY2ThetaPhi(dipx,dipy,theta,phi,false,false);
        }

        data_out1[i] = (float) theta;
        data_out2[i] = (float) phi;
    }
    return true;
}


void vpRcaSmoothAPI::clear(){
    myInput = NULL;
    myTheta = NULL;
    myPhi = NULL;
    myParam.clear();
}
vpRcaSmoothAPI::vpRcaSmoothAPI() {
    myInput = NULL;
    myTheta = NULL;
    myPhi = NULL;
}

vpRcaSmoothAPI::~vpRcaSmoothAPI() {
    clear();
}

/**
 * 
 * @param input volume 
 * @param theta volume
 * @param phi volume
 * @param params python list
 * @param dip whether to use dip field, default is false
 * @param type 1:radian, 2 : degrees ,3 m/m
 * Will accept an input volume to use theta,phi volume as a guide
 * the boost::python::list will be translated to vector<Vect4d>
 */
void vpRcaSmoothAPI::setup(Volume* input, Volume* theta, Volume* phi,
        boost::python::list params, bool isDip, RcaUnits ftype) {
    this->myInput = input;
    this->myTheta = theta;
    this->myPhi = phi;
    // the smooth parameter
    Py_ssize_t length = boost::python::len(params);
    this->myParam.clear();
    // loop for list
    for (Py_ssize_t i = 0; i < length; i++) {
        // loop for each list 
        Py_ssize_t len_each = boost::python::len(params[i]);
        // need to make sure the parameter list is right
        if (len_each != 3)
            continue;
        Vect4d val;
        for (Py_ssize_t j = 0; j < len_each; j++) {
            val[j] = boost::python::extract<double>(params[i][j]);
        }
        val[3] = 1.;
        myParam.push_back(val);
    }
    if (!isDip)
        return;
    FdmCube *thetaCube = myTheta ? (FdmCube*) myTheta->getCube() : 0;
    FdmCube *phiCube = myPhi ? (FdmCube*) myPhi->getCube() : 0;
    if (thetaCube==NULL || phiCube==NULL){
        clear();
        return;
    }
        
    FdmCube* new_theta=new FdmCube(*thetaCube);
    FdmCube* new_phi=new FdmCube(*phiCube);
    int type=static_cast<int>(ftype);
    if (!myConvertDip(*thetaCube,*phiCube,*new_theta,*new_phi, type)){
        delete new_theta;
        delete new_phi;
        clear();
        return;
    }
    myTheta=new Volume;
    myPhi=new Volume;
    myTheta->setCube(new_theta);
    myPhi->setCube(new_phi);
}

Volume* vpRcaSmoothAPI::smooth() {
    if (!myInput||!myTheta||!myPhi||myParam.size()==0)
        return NULL;
    FdmCube *thetaCube = myTheta ? (FdmCube*) myTheta->getCube() : 0;
    FdmCube *phiCube = myPhi ? (FdmCube*) myPhi->getCube() : 0;
    FdmCube* inputCube = myInput ? (FdmCube*) myInput->getCube() : 0;
    if (!thetaCube || !phiCube || !inputCube) {
        std::cerr << "Non Fdm format is detected\n";
        return NULL;
    }
    BaseGeometry geom = *inputCube;
    vmRcaStructSmoother smoother(*inputCube, *thetaCube, *phiCube, myParam);
    bool res = false;
    res = smoother.run();
    if (!res) {
        return NULL;
    }
    FdmCube* out_cube = smoother.getResult();
    Volume *newVol = new Volume;
    newVol->setCube(out_cube);
    return newVol;
}

const char* vpRcaSmoothAPI::getRcaSmoothHelp() {
    static const char* helpInfo =
            "\tRcaSmooth.setup:\n"
            "\t          ((Volume)input,\n"
            "\t          (Volume) theta_or_dipx,\n"
            "\t          (Volume)phi_or_dipy,\n"
            "\t          (optional bool)isToUseDip\n"
            "\t          (Unit) unit)\n"
            "\t\tcategory: setup method in algorithm.RcaSmooth\n"
            "\t\tcall method: rcasmoother.setup(...)\n"
            "\t\tusage: to setup data for RCA Smoothing algorithm\n"
            "\n\n"
            "\t input: the Volume data to be smoothed\n"
            "\t theta_or_dipx: if isToUseDip is set, input the the dipx, otherwise\n"
            "\t                need to be theta,the default is theta\n"
            "\t phi_or_dipy:   if isToUseDip is set, input the dipy, otherwise\n"
            "\t                need to be phi, the default is phi\n"
            "\t isToUseDip(optional): the default is false, unless explicitly set it\n"
            "\t unit(optional): the default is radian, but you can set the unit with\n"
            "\t                 unit.radian, unit.degree, unit.meter\n"
            "Example:\n"
            "from base import Volume\n"
            "from algorithm import RcaSmooth\n"
            "data, theta, phi= Volume(), Volume(), Volume()\n"
            "data.load(path1)\n"
            "theta.load(path2)\n"
            "phi.load(path3)\n"
            "param=[[0,100,100],[2000,500,1000],[5000,2000,2000]]\n"
            "smoother = RcaSmooth()\n"
            "smoother.setup(data,theta,phi,param)\n"
            "output=smoother.smooth()\n"
            "\n\n";
    return helpInfo;
}

