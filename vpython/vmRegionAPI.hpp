/* 
 * File:   vmRegionAPI.hpp
 * Author: Yanina Zholudz
 *
 * Created March, 2015
 */

#ifndef VMREGIONAPI_HPP
#define	VMREGIONAPI_HPP
#include <boost/python.hpp> 
#include "vlBaseGeometry.hpp"
#include "vmPoly2D.hpp"
#include "vmMultiPoly2D.hpp"
#include "vmBoolMask.hpp"
#include "vlSurveyRegion.hpp"

/**
 * @ Region functionality wrapper
 * 
 */

class vmBoolMaskWrapper : public vmBoolMask, public boost::python::wrapper<vmBoolMask>{
public:
    //need to overwrite as the getBoundingBox needs pointers
    Vect4i getBoundingBox() const;
    void printAll()
        { vmBoolMask::print(); }
    Vect2i getDim();
    void setAll(bool flag);
};

class vmPoly2DWrapper : public vmPoly2D, public boost::python::wrapper<vmPoly2D> {
public:
    /** Get the coordinates of the polygon */
    void getPolyCoordinates(vector<Vect2d>& v) const;
    void delPoint( vmPoint2D* );
    void setGeometry(const BaseGeometry& g);
    void setGeometryValues(int,int,double,double,double,double);
    vmBoolMaskWrapper* makeRawMask(const BaseGeometry &g);
};

class vmMultiPoly2DWrapper : public vmMultiPoly2D, public boost::python::wrapper<vmMultiPoly2D> {
public:
    //Need to overwrite for readability instead of mixing two types of overload:
    // two load functions both with default parameters
    bool loadFromFile(const vString& fname, bool loadLine = false, bool loadMore = false)
    {
        FnOK ok;        // Should we do anything with the generated messages?
        vmMultiPoly2D::load(ok, fname,loadLine,loadMore);
        return ok;
    }

    bool saveToFile(const vString& fname, bool saveAsLine = false)
    {
        FnOK ok;        // Should we do anything with the generated messages?
        vmMultiPoly2D::save(ok, fname,saveAsLine);
        return ok;
    }
    void setGeometry(const BaseGeometry& g);
    void setGeometryValues(int,int,double,double,double,double);
    vmPoly2D* getPolygon(int idx);
    void addPoint(double x, double y);
    void addPointToCurrent(double x, double y);
};


class vmSurveyRegionWrapper : public SurveyRegion, public boost::python::wrapper<SurveyRegion>{
public:
    vmSurveyRegionWrapper& operator=( const vmSurveyRegionWrapper& sr );

    void setPolygonCoordinates(const vmPoly2D& poly);
    
    BaseGeometry* getGeometry();
};

#endif	/* VMREGIONAPI_HPP */

