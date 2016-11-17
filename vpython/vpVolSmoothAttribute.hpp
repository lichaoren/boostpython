/* 
 * File:   vpVolSmoothAttribute.hpp
 * Author: ddoledec
 *
 * Created on April 28, 2015, 2:53 PM
 */

#ifndef VPVOLSMOOTHATTRIBUTE_HPP
#define	VPVOLSMOOTHATTRIBUTE_HPP

#include "vlVolSmooth.hpp"

// conversion a python list of 3-element list to to a std::vector of Vect3
void pylistToVec3d(boost::python::list&, vector<Vect3d>&);

/**
 * Expose Volume smooth to Python
 */
class vpVolSmoothAttribute : public vlVolSmooth {
public:
    
    vpVolSmoothAttribute();
    virtual ~vpVolSmoothAttribute();
// Need to write vlVolSmooth copy ... 
//    vpVolSmoothAttribute(const vpVolSmoothAttribute& orig);

//    interface expose
    static void exposePython();
    
    /** Run smooth */
    bool run( Volume& vol);
    
    /** Run smooth and chunk (avoid to load full cube in memory */
    bool runChunk(const std::string& inputAttrName, const std::string& outputAttrName);

    //    append a list of windows to the queue for run() or runChunk()
    void setSmoothWindows(boost::python::list& lst);
    //    deprecated: clear the windows queue and add a window of (rx, ry, rz)
    void setSmoothWindow( double rx, double ry, double rz );

    /*  Smooth with diff radius(x,y,z) function of depth.
     *  REMARK: The radius unit is meters,feet..
     */
    void setDepthVarying(boost::python::list params, vgHorizon* hrz=0);
   
private:
    vector<Vect3d> mySmoothWindows;
};

#endif	/* VPVOLSMOOTHATTRIBUTE_HPP */

