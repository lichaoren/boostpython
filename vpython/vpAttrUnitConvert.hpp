/* 
 * File:   vpAttrUnitConvert.hpp
 * Author: yanhliu
 *
 * Created on May 5, 2015, 2:44 PM
 */

#ifndef VPATTRUNITCONVERT_HPP
#define	VPATTRUNITCONVERT_HPP
#include "vmUnitConst.hpp"
#include "vlVolume.hpp"

class vpAttrUnitConvert {
public:
    vpAttrUnitConvert();
    void setConvert(vmUnitConst::unit input, vmUnitConst::unit output, string toConvert);
    void convert_module(string input, string output);
    void convert(Volume* vol);
    virtual ~vpAttrUnitConvert();
private:
    void calcHeader(Volume* vol);
    void calcData(Volume* vol);
    BaseGeometry myGeom;
    float myConvertVal;
    bool myAxis[5];
};

#endif	/* VPATTRUNITCONVERT_HPP */

