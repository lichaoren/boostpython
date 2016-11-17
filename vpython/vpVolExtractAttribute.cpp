/* 
 * File:   vpVolExtractAttribute.cpp
 * Author: ddoledec
 * 
 * Created on April 15, 2015, 3:00 PM
 */
#include <boost/python.hpp> 
#include <string> 

#include "vpVolExtractAttribute.hpp"
#include "vlHorizon.hpp"
#include "vlHorizonGroup.hpp"
#include "vlVolume.hpp"
#include "vlHorizon.hpp"

#include "vlV2HSampler.hpp"


    
//bool (vpVolExtractAttribute::*runHorizon)(const Volume& vol, vgHorizonGroup &hg, vgHorizon& hrz, const std::string& attrName, bool wholeGroup) const = &vpVolExtractAttribute::extractHorizon;
//bool (vpVolExtractAttribute::*runHorizonGroup)(const Volume& vol, vgHorizonGroup &hg, const std::string& attrName) const = &vpVolExtractAttribute::extractHorizonGroup;
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(ExtractAttr_run_overloads, vpVolExtractAttribute::extractHorizon, 4, 5)


void vpVolExtractAttribute::exposePython()
{
    namespace bp = boost::python;
    bp::scope vol_extract_scope = bp::class_<vpVolExtractAttribute>("VolumeExtractAttribute")
                .def("run", &vpVolExtractAttribute::extractHorizon, ExtractAttr_run_overloads())
                .def("run", &vpVolExtractAttribute::extractHorizonGroup)
                .def_readwrite("windowSizeAbove", &vpVolExtractAttribute::windowSizeAbove)
                .def_readwrite("windowSizeBelow", &vpVolExtractAttribute::windowSizeBelow)
                .def_readwrite("mode", &vpVolExtractAttribute::mode);
        
    bp::enum_<V2HSampler::ExtractedInformation>("ExtractAttributeMode")
            .value("PEAK", V2HSampler::PEAK)
            .value("TROUGH", V2HSampler::TROUGH)
            .value("PEAK_LENGTH", V2HSampler::PEAK_LENGTH)
            .value("PEAK_AREA", V2HSampler::PEAK_AREA)
            .value("INST_FREQ", V2HSampler::INST_FREQ)
            .value("INST_PHASE", V2HSampler::INST_PHASE)
            .value("REFL_STRENGTH", V2HSampler::REFL_STRENGTH)
            .value("RESPONSE_ENERGY", V2HSampler::RESPONSE_ENERGY)
            .value("RESPONSE_FREQ", V2HSampler::RESPONSE_FREQ)
            .value("RESPONSE_PHASE", V2HSampler::RESPONSE_PHASE)
            .value("RESPONSE_LENGTH", V2HSampler::RESPONSE_LENGTH)
            .value("RESPONSE_DEPTH", V2HSampler::RESPONSE_DEPTH)
            .value("DIP_SEISMIC_EVENT", V2HSampler::DIP_SEISMIC_EVENT)
            .value("AZIMUTH_SEISMIC_EVENT", V2HSampler::AZIMUTH_SEISMIC_EVENT)
            .value("RMS", V2HSampler::RMS);
 
    // Reference Enum to the class rather than the module
    vol_extract_scope.attr("PEAK") = V2HSampler::PEAK; 
    vol_extract_scope.attr("TROUGH") = V2HSampler::TROUGH; 
    vol_extract_scope.attr("PEAK_LENGTH") = V2HSampler::PEAK_LENGTH; 
    vol_extract_scope.attr("PEAK_AREA") = V2HSampler::PEAK_AREA; 
    vol_extract_scope.attr("INST_FREQ") = V2HSampler::INST_FREQ; 
    vol_extract_scope.attr("INST_PHASE") = V2HSampler::INST_PHASE; 
    vol_extract_scope.attr("REFL_STRENGTH") = V2HSampler::REFL_STRENGTH; 
    vol_extract_scope.attr("RESPONSE_ENERGY") = V2HSampler::RESPONSE_ENERGY; 
    vol_extract_scope.attr("RESPONSE_FREQ") = V2HSampler::RESPONSE_FREQ; 
    vol_extract_scope.attr("RESPONSE_PHASE") = V2HSampler::RESPONSE_PHASE; 
    vol_extract_scope.attr("RESPONSE_PHASE") = V2HSampler::RESPONSE_PHASE; 
    vol_extract_scope.attr("RESPONSE_LENGTH") = V2HSampler::RESPONSE_LENGTH; 
    vol_extract_scope.attr("RESPONSE_DEPTH") = V2HSampler::RESPONSE_DEPTH; 
    vol_extract_scope.attr("DIP_SEISMIC_EVENT") = V2HSampler::DIP_SEISMIC_EVENT; 
    vol_extract_scope.attr("AZIMUTH_SEISMIC_EVENT") = V2HSampler::AZIMUTH_SEISMIC_EVENT; 
    vol_extract_scope.attr("RMS") = V2HSampler::RMS; 
 
}

vpVolExtractAttribute::vpVolExtractAttribute() :
windowSizeAbove(10),
windowSizeBelow(10),
mode(15) // default RMS
{
}

vpVolExtractAttribute::~vpVolExtractAttribute()
{
}


bool vpVolExtractAttribute::extractHorizon(const Volume& vol, vgHorizonGroup &hg, vgHorizon& hrz, const std::string& attrName, bool wholeGroup)
{
    char* domain = 0;
    if (hg.getAttrIndex(ATTR_NAME_TWT) >= 0)
        domain = (char*) ATTR_NAME_TWT;
    else if (hg.getAttrIndex(ATTR_NAME_DEPTH) >= 0)
        domain = (char*) ATTR_NAME_DEPTH;
    else
        return false;

    // can do better ...
    if (mode == -1)
    {
        return false;
    }

    if (windowSizeAbove < 0 || windowSizeBelow < 0)
    {
        return false;
    }
    
    // Not implemented... IMAO: dunno why it's related to extract attribute
    if (mode >= V2HSampler::VOL_SHIFT)
        return false;
    
    // Create Attribute if needed
    if (hg.getAttrIndex(attrName.c_str()) < 0)
        hg.addAttr(attrName.c_str());    

    Vect2i myWinSize;
    myWinSize[0] = windowSizeAbove;
    myWinSize[1] = windowSizeBelow;

    V2HSampler vs;
    vs.setOp(mode);
    vs.setWin(myWinSize[0], myWinSize[1]);

    vs.setUseSubsample(false);
    //        if (mode == V2HSampler::AVERAGE)
    //        {
    //            vs.setUseSubsample(CheckBoxSubsample->isChecked());
    //            if (vs.getUseSubsample() && LineEditSubsampleRate->text().isEmpty())
    //            {
    //                warning("Must define a valid subsample rate!");
    //                return;
    //            }
    //            vs.setSubsampleRate(LineEditSubsampleRate->text().toFloat());
    //        }

    if (wholeGroup) {
      int nhrz = hg.getNumHorizon();
      for (int i = 0; i < nhrz; ++ i) {
        vgHorizon* horizon = hg.getHorizonByIndex(i);
        vs.sample((Volume*) & vol, horizon, attrName.c_str(), domain, 0);
      }
    }
    else 
        vs.sample((Volume*) & vol, &hrz, attrName.c_str(), domain, 0);

    return true;
}

bool vpVolExtractAttribute::extractHorizonGroup(const Volume& vol, vgHorizonGroup &hg, const std::string& attrName) {
    bool status = true;
    int nhrz = hg.getNumHorizon();
    for (int i = 0; i < nhrz; ++i) {
        vgHorizon* horizon = hg.getHorizonByIndex(i);
        status = extractHorizon(vol, hg, *horizon, attrName);
        if (status == false) {
            return false;
        }
    }

    return status;
}





