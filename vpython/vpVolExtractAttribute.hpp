/* 
 * File:   vpVolExtractAttribute.hpp
 * Author: ddoledec
 *
 * Created on April 15, 2015, 3:00 PM
 */

#ifndef VPVOLEXTRACTATTRIBUTE_HPP
#define	VPVOLEXTRACTATTRIBUTE_HPP

#include <string> 

class Volume;
class SurveyRegion;
class vgHorizon;
class vgHorizonGroup;


/**
 * Extract information form the seismic according to a horizon (RMS...)
 */
class vpVolExtractAttribute {
public:

    vpVolExtractAttribute();
    virtual ~vpVolExtractAttribute();
    static void exposePython();

    /** Extract attribute along on horizon */
    bool extractHorizon( const Volume& vol, vgHorizonGroup &hg, vgHorizon& hrz, const std::string& attrName, bool wholeGroup = false);
    /** Extract attribute for all the horizons into the horizon group */
    bool extractHorizonGroup( const Volume& vol, vgHorizonGroup &hg, const std::string& attrName);

    int   windowSizeAbove;
    int   windowSizeBelow;
    int   mode; // default RMS=15
    
protected:
    
private:

};

#endif	/* VPVOLEXTRACTATTRIBUTE_HPP */

