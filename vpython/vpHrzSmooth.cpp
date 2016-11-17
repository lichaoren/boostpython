/* 
 * File:   vpHrzSmooth.cpp
 * Author: ddoledec
 * 
 * Created on April 16, 2015, 11:28 AM
 */
#include "vpHrzSmooth.hpp"

#include <boost/python.hpp> 

#include "vlHorizon.hpp"
#include "vlHorizonUtil.hpp"
#include "vlSurveyRegion.hpp"
#include "vmBoolMask.hpp"

#include "vlHrzSmoother.hpp"

void vpHrzSmooth::exposePython()
{
    using namespace boost::python;

    class_<vpHrzSmooth>("HorizonSmooth")
    .def("run", &vpHrzSmooth::run)
    .def("setRadiusX", &vpHrzSmooth::setRadiusx)
    .def("setRadiusY", &vpHrzSmooth::setRadiusy)
    .def("setMode", &vpHrzSmooth::setMode);
}


vpHrzSmooth::vpHrzSmooth() :
myRegion(0)
{
}

vpHrzSmooth::~vpHrzSmooth()
{
}

bool vpHrzSmooth::run(vgHorizon& hrz, int idx)
{   
    setHorizon( &hrz );    
        
    if (this->myRegion)
    {
        vmBoolMask mask;
        this->myRegion->makeMask(mask, hrz);
        setMask( &mask );
            
        return HrzSmoother::smooth(idx);
    }

    return HrzSmoother::smooth(idx);
}



void vpHrzSmooth::setRegion(SurveyRegion* rg)
{
    this->myRegion=rg;
}

void vpHrzSmooth::clearRegion()
{
    this->myRegion=NULL;
}
