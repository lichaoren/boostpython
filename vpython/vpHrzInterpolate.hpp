/* 
 * File:   vpHrzFeature.hpp
 * Author: yanhliu
 * Created on February 27, 2015, 3:11 PM
 *
 * Renamed file to vpHrzInterpolate.hpp
 * on April 23, 2015
 *
 */

#ifndef VPHRZINTERPOLATE_HPP
#define	VPHRZINTERPOLATE_HPP
#include "GridInterp.hpp"
#include "vmBoolMask.hpp"
#include "vlSurveyRegion.hpp"

class vgHorizon;

/**
 * A simple wrapper of horizon interpolation to a state object
 * for python script usage
 */
class vpHrzInterpolate {
public:
    vpHrzInterpolate();
    virtual ~vpHrzInterpolate();
    bool interp(vgHorizon& hrz, int index);
    void setRegion(SurveyRegion* rg);
    void clearRegion();
    // open this to benifit the user side
    bool IsWallToWall;
    float Radius;
    vmBoolMask* Mask;
    SurveyRegion* Region;
    GridInterp::Mode Mode;
};

#endif	/* VPHRZINTERPOLATE_HPP */


