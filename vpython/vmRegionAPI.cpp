#include "vmRegionAPI.hpp"



Vect4i vmBoolMaskWrapper::getBoundingBox() const
{
    Vect2i orig(0,0);
    Vect2i size(0,0);
    vmBoolMask::getBoundingBox(&orig,&size);
    return Vect4i( orig[0] , orig[1], size[0], size[1]);
}

Vect2i vmBoolMaskWrapper::getDim()
{
    int w,h;
    vmBoolMask::getDim(w,h);
    return Vect2i(w,h);
}

void vmBoolMaskWrapper::setAll(bool flag)
{
    Vect2i dim = getDim();
    for(int i = 0; i < dim[0]; i++){
      for(int j = 0; j < dim[1]; j++){
        vmBoolMask::set( i, j, flag );
      }
    }
}


/****************************************************************************************/



void vmPoly2DWrapper::getPolyCoordinates(vector<Vect2d>& points) const
{
    points.clear();
    Vect2d p;
    vector< vmPoint2D* > pt_lst;
    vmPoly2D::getPointList(pt_lst);
    int num = pt_lst.size();

    for (int i = 0; i < num; ++ i) {
        p[0] = pt_lst[i]->x;
        p[1] = pt_lst[i]->y;

        points.push_back(p);
    }
}

void vmPoly2DWrapper::delPoint(vmPoint2D* p)
{
    vmPoly2D::delPoint(p);
}

void vmPoly2DWrapper::setGeometry(const BaseGeometry& g)
{
    vmPoly2D::setGeometry(g);
}

void vmPoly2DWrapper::setGeometryValues(int w, int h, double ox, double oy, double dx, double dy)
{
    vmPoly2D::setGeometry(w,h,ox,oy,dx,dy);
}

vmBoolMaskWrapper* vmPoly2DWrapper::makeRawMask(const BaseGeometry& g)
{
    vmBoolMaskWrapper* m = new vmBoolMaskWrapper();
    vmPoly2D::setGeometry(g);
    vmPoly2D::makeRawMask(*m);
    return m;
}

/****************************************************************************************/


void vmMultiPoly2DWrapper::setGeometry(const BaseGeometry& g)
{
    vmMultiPoly2D::setGeometry(g);
}

void vmMultiPoly2DWrapper::setGeometryValues(int w, int h, double ox, double oy, double dx, double dy)
{
    vmMultiPoly2D::setGeometry(w,h,ox,oy,dx,dy);
}

vmPoly2D* vmMultiPoly2DWrapper::getPolygon(int idx)
{
    vmPoly2D* poly = new vmPoly2D();
    if(idx >= 0 && idx < getNum()){
        *poly = *(polygon(idx));
    }
    return poly;
}

void vmMultiPoly2DWrapper::addPoint(double x, double y)
{
    if(getNum() <= 0){
        addPolygon();
    }
    int numpoly = getNum();
    int idx = hitEdge(x,y);
    if(idx >= 0 && idx < numpoly){
        polygon(idx)->addPoint(x,y);
    }
}

void vmMultiPoly2DWrapper::addPointToCurrent(double x, double y)
{
    if(getNum() <= 0){
        addPolygon();
    }
    int numpoly = getNum();
    int idx = curPolygonIdx();
    if(idx >= 0 && idx < numpoly){
        polygon(idx)->addPoint(x,y);
    }
}



/****************************************************************************************/

void vmSurveyRegionWrapper::setPolygonCoordinates(const vmPoly2D& poly)
{
    vector< Vect2d > points;
    vector< vmPoint2D* > plist;
    poly.getPointList(plist);
    int num = plist.size();
    for (int i = 0; i < num; ++ i) {
        Vect2d p = Vect2d(plist[i]->x,plist[i]->y);

        points.push_back(p);
    }
    SurveyRegion::setPolygonCoord(points);
}

vmSurveyRegionWrapper& vmSurveyRegionWrapper::operator=(const vmSurveyRegionWrapper& base)
{
    if (this == &base) return *this;
    
    SurveyRegion::operator=(base);
    
    return *this;
}

BaseGeometry* vmSurveyRegionWrapper::getGeometry()
{
    return new BaseGeometry(*(SurveyRegion::getGeometry()));
}


