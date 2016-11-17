/* 
 * File:   vpHrzSnap.cpp
 * Author: ddoledec
 * 
 * Created on April 15, 2015, 9:42 AM
 */
#include <boost/python.hpp> 

#include "vpHrzSnap.hpp"

#include "vlVolume.hpp"
#include "vlHorizon.hpp"
#include "vlHorizonUtil.hpp"
#include "vlSurveyRegion.hpp"

#include "vmBoolMask.hpp"



void vpHrzSnap::exposePython()
{    
    namespace bp = boost::python;
    bp::enum_<vpHrzSnap::Mode>("SnapMode")
            .value("POSITIVE_PEAK", vpHrzSnap::POSITIVE_PEAK)
            .value("NEGATIVE_PEAK", vpHrzSnap::NEGATIVE_PEAK)
            .value("POSITIVE_ZEROCROSS", vpHrzSnap::POSITIVE_ZEROCROSS)
            .value("NEGATIVE_ZEROCROSS", vpHrzSnap::NEGATIVE_ZEROCROSS)
            .value("POSITIVE_MAXGRADIENT", vpHrzSnap::POSITIVE_MAXGRADIENT)
            .value("NEGATIVE_MAXGRADIENT", vpHrzSnap::NEGATIVE_MAXGRADIENT);
    
    bp::scope snap_scope = bp::class_<vpHrzSnap>("HorizonSnap")
            .def("run", &vpHrzSnap::snap)
            .def("setRegion", &vpHrzSnap::setRegion)
            .def("clearRegion", &vpHrzSnap::clearRegion)
            .def("setWindowSize", &vpHrzSnap::setWindowSize)
            .def("setMode", &vpHrzSnap::setMode);
    
    snap_scope.attr("POSITIVE_PEAK") = vpHrzSnap::POSITIVE_PEAK; 
    snap_scope.attr("NEGATIVE_PEAK") = vpHrzSnap::NEGATIVE_PEAK; 
    snap_scope.attr("POSITIVE_ZEROCROSS") = vpHrzSnap::POSITIVE_ZEROCROSS; 
    snap_scope.attr("NEGATIVE_ZEROCROSS") = vpHrzSnap::NEGATIVE_ZEROCROSS; 
    snap_scope.attr("POSITIVE_MAXGRADIENT") = vpHrzSnap::POSITIVE_MAXGRADIENT; 
    snap_scope.attr("NEGATIVE_MAXGRADIENT") = vpHrzSnap::NEGATIVE_MAXGRADIENT; 
}


vpHrzSnap::vpHrzSnap() :
windowSize(10),
snapMode(vpHrzSnap::POSITIVE_PEAK),
myRegion(0)
{
}

vpHrzSnap::~vpHrzSnap()
{
}

bool vpHrzSnap::snap(const Volume& vol, vgHorizon& hrz, int idx)
{
    float** hGrid = hrz.getAttrPtr2d(idx);
    if (hGrid == 0)
    {
        fprintf(stderr, "WARNING: %s %d Empty attribute, return\n", __FILE__, __LINE__);
        return false;
    }
    
    vmSnapper snapper;
    if (!snapper.init((Volume*) & vol, &hrz))
        return false;

    // Parameters
    snapper.setSnapWinSize(windowSize);
    snapper.setSnapMode(snapMode);

    if (this->myRegion)
    {
        vmBoolMask mask;
        this->myRegion->makeMask(mask, hrz);
        snapper.snapHorizon((Volume*) & vol, hGrid, &mask);
    } else
    {
        snapper.snapHorizon((Volume*) & vol, hGrid, 0);
    }

    return true;
}

void vpHrzSnap::setRegion(SurveyRegion* rg)
{
    this->myRegion=rg;
}

void vpHrzSnap::clearRegion()
{
    this->myRegion=NULL;
}
