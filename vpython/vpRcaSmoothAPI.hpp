/* 
 * File:   vpRcaSmoothAPI.hpp
 * Author: yanhliu
 *
 * Created on February 3, 2015, 1:12 PM
 */

#ifndef VPRCASMOOTHAPI_HPP
#define	VPRCASMOOTHAPI_HPP

#include <boost/python.hpp> 
#include "vmRcaStructSmoother.hpp"
#include "vlVolume.hpp"


enum RcaUnits { radian = 1, degree = 2, meter = 3 };

/*
 * python wrapper for RCA Struct smoothing wrapper
 *   
 */
class vpRcaSmoothAPI {
public:
    vpRcaSmoothAPI();
    void setup(Volume* input, Volume* theta, Volume* phi,
                boost::python::list params,bool isDip=false,RcaUnits ftype=radian);
    Volume* smooth();
    virtual ~vpRcaSmoothAPI();
    Volume *myInput;
    static const char* getRcaSmoothHelp();

private:
    void clear();
    Volume *myTheta;
    Volume *myPhi;
    std::vector<Vect4d> myParam;
};

#endif	/* VPRCASMOOTHAPI_HPP */

