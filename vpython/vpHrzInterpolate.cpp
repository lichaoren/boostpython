/* 
 * File:   vpHrzFeature.cpp
 * Author: yanhliu
 * Created on February 27, 2015, 3:11 PM
 *
 * Renamed file to vpHrzInterpolate.cpp
 * on April 23, 2015
 */

#include "vpHrzInterpolate.hpp"
#include "vlHorizon.hpp"
#include "vlHorizonUtil.hpp"

vpHrzInterpolate::vpHrzInterpolate(){
    this->Mask=NULL;
    this->Mode=GridInterp::LINEAR;
    this->Radius=100;
    this->IsWallToWall=false;
    this->Region=0;
}
vpHrzInterpolate::~vpHrzInterpolate(){
    if (this->Mask)
        delete Mask;
    Mask=0;
}

bool vpHrzInterpolate::interp(vgHorizon& hrz, int index)
{
    // only expose region to user, it is easier to use, to let user know the 
    // boolmask, it is too deep. Also, the interpolator is used for different type of
    // horizon group
    if(this->Region){
        if (this->Mask){
            delete Mask;
        }
        this->Mask=new vmBoolMask;
        this->Region->makeMask(*(this->Mask), hrz);
    }
    return HorizonUtil::gridInterp(hrz, index,this->IsWallToWall, Radius,
    			this->Mask, static_cast<int>(this->Mode),
			NULL, true);
}

void vpHrzInterpolate::setRegion(SurveyRegion* rg){
    this->Region=rg;
}

void vpHrzInterpolate::clearRegion(){
    this->Region=NULL;
}

