/* 
 * File:   vpHrzSmooth.hpp
 * Author: ddoledec
 *
 * Created on April 16, 2015, 11:28 AM
 */

#ifndef VPHRZSMOOTH_HPP
#define	VPHRZSMOOTH_HPP

#include "vlHrzSmoother.hpp"

class vgHorizon;
class SurveyRegion;

/** Horizon Smoother */
class vpHrzSmooth : public HrzSmoother {
public:
    vpHrzSmooth();
    virtual ~vpHrzSmooth();
    static void exposePython();

    void setRegion(SurveyRegion* rg);
    void clearRegion();
    
    bool run(vgHorizon& hrz, int idx);
    void setMode(int type) { setFilter( type ); }
    
protected:
    SurveyRegion* myRegion;
};

#endif	/* VPHRZSMOOTH_HPP */

