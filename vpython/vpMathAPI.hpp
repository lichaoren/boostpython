/* 
 * File:   vpMathAPI.hpp
 * Author: yanhliu
 *
 * Created on March 11, 2015, 3:17 PM
 */

#ifndef VPMATHAPI_HPP
#define	VPMATHAPI_HPP
#include <boost/python.hpp> 
#include "vmVolMathModel.hpp"
#include "vlBaseGeometry.hpp"
#include "vlVolume.hpp"
using namespace std;

class vpMathAPI {
public:
    // internal structure for math

    struct mathStruct {

        mathStruct() {
            va = 0;
            vb = 0;
            constant1 = 0.;
            constant2 = 0.;
            mask = 0;
            flag = false;
        }

        ~mathStruct() {
            if (mask) {
                delete mask;
            }
            mask = 0;
        }
        BaseGeometry geom; // object geometry
        int iatt1; // if use volume -1: no
        int iatt2;
        int iop; // operator number
        Volume* va; // volume a
        Volume* vb;
        float constant1; // constant a
        float constant2;
        int domain; //domain type
        bool* mask; // mask
        vector<Volume*> volumes; // dummy
        bool flag;
    };
    vpMathAPI();
    vpMathAPI(float val);
    vpMathAPI(vgHorizonGroup* hg, boost::python::list hrzs, bool fillup, float nullVal = 0.);
    virtual ~vpMathAPI();
    static void setConstraint(vgHorizonGroup* hg, boost::python::list hrzs, bool fillUp);
    template<typename T1, typename T2>
    Volume* plus(T1 avol, T2 bvol);
    template<typename T1, typename T2>
    Volume* minus(T1 avol, T2 bvol);
    template<typename T1, typename T2>
    Volume* multiply(T1 avol, T2 bvol);
    template<typename T1, typename T2>
    Volume* divide(T1 avol, T2 bvol);
    template<typename T1, typename T2>
    Volume* time_sin(T1 avol, T2 bvol);
    Volume* sin(Volume* vol);
    template<typename T1, typename T2>
    Volume* time_arcsin(T1 avol, T2 bvol);
    Volume* arcsin(Volume* vol);
    template<typename T1, typename T2>
    Volume* time_cos(T1 avol, T2 bvol);
    Volume* cos(Volume* vol);
    template<typename T1, typename T2>
    Volume* time_arccos(T1 avol, T2 bvol);
    Volume* arccos(Volume* vol);
    template<typename T1, typename T2>
    Volume* time_tan(T1 avol, T2 bvol);
    Volume* tan(Volume *vol);
    template<typename T1, typename T2>
    Volume* time_arctan(T1 avol, T2 bvol);
    Volume* arctan(Volume* vol);
    Volume* sqrt(Volume* vol);
    template<typename T1, typename T2>
    Volume* time_log10(T1 vola, T2 volb);
    Volume* log10(Volume* vol);
    template<typename T1, typename T2>
    Volume* min(T1 avol, T2 bvol);
    template<typename T1, typename T2>
    Volume* max(T1 avol, T2 bvol);
    Volume* abs(Volume* avol);
    template<typename T1, typename T2>
    Volume* mod(T1 avol, T2 bvol);
    Volume *d_dx(Volume* avol);
    Volume *d_dy(Volume* bvol);
    Volume *d_dz(Volume* vol);
    Volume *sum_top_down(Volume* vol);
    Volume *sum_btm_up(Volume *vol);
    template<typename T1, typename T2>
    Volume* pow(T1 avol, T2 bvol);
    void setInput(boost::python::list lst);
    void setOutput(string output);
    void setFormula(string formula);
    void doMath();
    double valSin(double val){
        return std::sin(val);
    };
    double valArcsin(double val){
        return std::asin(val);
    }; 
    double valCos(double val){
        return std::cos(val);
    };
    double valArccos(double val){
        return std::acos(val);
    };
    double valTan(double val){
        return std::tan(val);
    };
    double valArctan(double val){
        return std::atan(val);
    };
    double valLog10(double val){
        return std::log10(val);
    };
    double valAbs(double val){
        return std::fabs(val);
    };
    double valPower(double a, double b){
        return std::pow(a,b);
    };
    double valSqrt(double val){
        return std::sqrt(val);
    };
private:
    template<typename T1, typename T2>
    mathStruct myPreCheckOperand(T1 arg1, T2 arg2, int op);
    template<typename T1, typename T2>
    Volume* mathHelper(T1 arg1, T2 arg2, int op);
    void bigVolumeMath(vector<string>& vars, string formula, string output);
    float null_value;
    vgHorizonGroup* myHG;
    bool useConstraint;
    vector<vgHorizon*> myHrzs;
    bool fillup;
    vmVolMathModel model;
    vector<string> myVolumeNames;
    string myFormulaString;
    string myOutputName;
};

/**
 * Use a special template to dump parameters, this is only a checking purpose
 * of T1 and T2 for function with two typename
 * @param arg
 * @param st
 * @param seq
 */
template<typename T>
void dump_vpMathStruct(T arg, vpMathAPI::mathStruct& st, int seq) {
    cerr << "should not be called" << endl;
    return;
}

template<>
void dump_vpMathStruct(float arg, vpMathAPI::mathStruct& st, int seq);
template<>
void dump_vpMathStruct(Volume* arg, vpMathAPI::mathStruct& st, int seq);

/**
 * Basic operator + - * /
 * @param arg1
 * @param arg2
 * @return 
 */
template<typename T1, typename T2>
Volume* vpMathAPI::plus(T1 arg1, T2 arg2) {

    return mathHelper<T1, T2>(arg1, arg2, 0);
}

template<typename T1, typename T2>
Volume* vpMathAPI::minus(T1 arg1, T2 arg2) {

    return mathHelper<T1, T2>(arg1, arg2, 1);
}

template<typename T1, typename T2>
Volume* vpMathAPI::multiply(T1 arg1, T2 arg2) {

    return mathHelper<T1, T2>(arg1, arg2, 2);
}

template<typename T1, typename T2>
Volume* vpMathAPI::divide(T1 arg1, T2 arg2) {

    return mathHelper<T1, T2>(arg1, arg2, 3);
}

/**
 * angle operators, sin, cos, tan, arcsin, arctan, arccos
 * some operators are extended
 * @param avol
 * @param bvol
 * @return 
 */
template<typename T1, typename T2>
Volume* vpMathAPI::time_sin(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 4);
}

template<typename T1, typename T2>
Volume* vpMathAPI::time_cos(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 5);
}

template<typename T1, typename T2>
Volume* vpMathAPI::time_tan(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 6);
}

template<typename T1, typename T2>
Volume* vpMathAPI::time_arcsin(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 7);
}

template<typename T1, typename T2>
Volume* vpMathAPI::time_arccos(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 8);
}

template<typename T1, typename T2>
Volume* vpMathAPI::time_arctan(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 9);
}

/**
 * log10
 * @param avol
 * @param bvol
 * @return 
 */
template<typename T1, typename T2>
Volume* vpMathAPI::time_log10(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 11);
}

/**
 * min max mod pow
 * @param avol
 * @param bvol
 * @return 
 */
template<typename T1, typename T2>
Volume* vpMathAPI::min(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 12);
}

template<typename T1, typename T2>
Volume* vpMathAPI::max(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 13);
}

template<typename T1, typename T2>
Volume* vpMathAPI::mod(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 19);
}

template<typename T1, typename T2>
Volume* vpMathAPI::pow(T1 avol, T2 bvol) {
    return mathHelper<T1, T2>(avol, bvol, 21);
}

template<typename T1, typename T2>
vpMathAPI::mathStruct vpMathAPI::myPreCheckOperand(T1 arg1, T2 arg2, int op) {

    mathStruct param;
    param.flag = false;
    dump_vpMathStruct(arg1, param, 1);
    dump_vpMathStruct(arg2, param, 2);
    param.iop = op;
    if (param.iatt1 == -1 && param.iatt2 == -1) {
        cerr << "fatal: type not accepted" << endl;
        return param;
    }
    int iattr1 = param.iatt1;
    int iattr2 = param.iatt2;
    Volume* vol_a = param.va;
    Volume* vol_b = param.vb;
    // do some pre-check and preparing
    if(iattr1 >= 0 && !vol_a)
    {
        cerr << "Left hand side of operand is not valid!" << endl;
        return param;
    }
    if(iattr2 >= 0 && !vol_b)
    {
        cerr << "Right hand side of operand is not valid!" << endl;
        return param;
    }

    /* Check geometry is consistent */
    if (iattr1 >= 0 && iattr2 >= 0) {
        if (vol_a->getInc()[0] != vol_b->getInc()[0] ||
                vol_a->getInc()[1] != vol_b->getInc()[1] ||
                vol_a->getStep()[2] != vol_b->getStep()[2] ||
                (vol_a->getMin()[0] - vol_b->getMin()[0]) %
                vol_a->getInc()[0] != 0 ||
                (vol_a->getMin()[1] - vol_b->getMin()[1]) %
                vol_a->getInc()[1] != 0 ||
                int(vol_a->getOrigin()[2] - vol_b->getOrigin()[2]) % int(vol_a->getStep()[2]) != 0) {
            cerr << "Inconsistent volume geometry" << endl;
            return param;
        }
    }
    /* Create new attribute */
    BaseGeometry geom;
    int domain;
    if (iattr1 >= 0 && iattr2 >= 0) {
        geom = model.calcMaxGeom(*vol_a, *vol_b);
        domain = vol_a->getDomain();
    } else if (iattr1 >= 0) {
        geom = *vol_a;
        domain = vol_a->getDomain();
    } else {
        geom = *vol_b;
        domain = vol_b->getDomain();
    }

    param.geom = geom;
    param.domain = domain;
    param.mask = 0; // temporarily not support constraint
    Volume* new_vol = new Volume;
    if (!new_vol || !new_vol->createFdm("unammed", geom)) {
        cerr << "Failed to create a new volume" << endl;
        return param;
    }
    param.volumes.resize(1);
    param.volumes[0] = new_vol;

    param.flag = true;
    return param;
}

template<typename T1, typename T2>
Volume* vpMathAPI::mathHelper(T1 arg1, T2 arg2, int op) {
    mathStruct param = this->myPreCheckOperand<T1, T2>(arg1, arg2, op);
    if (!param.flag)
        return 0;
    // we will build the mask here
    param.mask = 0;
    if (this->useConstraint) {
        model.createMask(param.geom, this->myHG, this->myHrzs, this->fillup, &(param.mask), param.domain);
    }
    return model.applyToFdmVol(param.geom, param.iatt1, param.iatt2,
            param.iop, param.va, param.vb,
            param.constant1, param.constant2,
            param.domain, param.mask, param.volumes, null_value, 0);

}

#endif	/* VPMATHAPI_HPP */

