
#ifndef GRIDINTERP_HPP
#define GRIDINTERP_HPP


namespace algorithm {

/**
     * @brief Horizon interpolation
     * 
     * 
     * In Tornado (Horizon->Horizon Picking... > Edit->Interpolate Horizon->Grid Interpolate Horizon).
     * @image html gridInterp.png
     * 
     * \code{.py}
       from algorithm import GridInterp as gp
       from algorithm import InterpMode as mode
       from base import *
       
       f="/data2/devtest/tornado/yanhliu/rca/titus_test.hrz"
       #test loading
       hg=HorizonGroup()
       hg.load(f)
       #test basic interface for horizon
       n=hg.getNumHorizon()
       for i in range(n):
       hrz=hg.getHorizonByIndex(i)
       print hrz.getName()
       m=hrz.getNumAttr()
       for j in range(m):
           print "-"*5, hrz.getAttrName(j)
           print "*"*5, hrz.getAttrIndex("DEPTH")
       
       #test interpolation
       interpolator= gp()
       interpolator.IsWallToWall=True
       interpolator.Mode=mode.LINEAR # MINIMUM_CURVATURE #LINEAR # NEAREST_NEIGHBOR # NATRUAL_NEIGHBOR
       interpolator.Radius=100
       
       #loop the horizon group to interpolate the DEPTH attribute
       for i in range(n):
           hrz=hg.getHorizonByIndex(i)
           print "get the horizon:",hrz.getName()
           m=hrz.getAttrIndex("DEPTH")
           if (m<0):
              continue
           interpolator.interp(hrz,m)
       #test horizon save
       hg.setPath(f.split('.')[0]+'_py.hrz')
       hg.save(True)


       aRegion=MultiPolygon2D()
       aRegion.load("/data2/devtest/tornado/yanhliu/Script_test/testRgn.rgn")
       print "aPoly num points before: ",aRegion.numPolygons()
       aSurveyRegion = SurveyRegion()
       aSurveyRegion.setPolygonCoordinates(aRegion.polygon(0))
       interpolator.setRegion(aSurveyRegion)
       hg2=HorizonGroup()
       hg2.load('/data2/devtest/tornado/yanhliu/train_data/test_hg.hrz')
       hrz=hg2.getHorizonByIndex(hg2.getHorizonByName("BOS2"))
       hrz2=hg2.getHorizonByIndex(hg2.getHorizonByName("TOS2"))
       m=hrz.getAttrIndex("DEPTH")
       interpolator.interp(hrz,m)
       interpolator.interp(hrz2,m)
       hg2.setPath("/data2/devtest/tornado/yanhliu/Script_test/rgnInterpHrz.hrz")
       hg2.save()
     * \endcode
     * 
     */
    class GridInterp {
    public:

        /**
         * There are currently four interpolation algorithms: LINEAR, 
         * NATURAL_NEIGHBOR, LINEAR_DELTA and NEAREST_NEIGHBOUR.
         *
         * The LINEAR algorithm performs a regular Delaunay triangulation and a
         * simple linear interpolation between the nodes. This method is fast.
         *
         * Natural neighbor interpolation goes further, using Voronoi cells to
         * identify natural neighbors, constructing a new Voronoi cell for the
         * point to be interpolated, and then weighting the interpolation
         * according to the cell's overlap with those of the natural neighbors.
         * This more complicated scheme does a much better job of extrapolating
         * into large empty regions, although it is considerably more expensive
         * in CPU time.
         */
        enum InterpMode {
            LINEAR,
            NATURAL_NEIGHBOR,
            MINIMUM_CURVATURE,
            NEAREST_NEIGHBOR
        };

        /**
         * Set the horizon to be interpolated
         * @param hrz   Input horizon 
         * @param index Horizon attribute index
         */
        bool interp(Horizon hrz, int index);

        /**
         * Set the region to limit interpolation
         */
        void setRegion(SurveyRegion rg);

        /**
         * Clear the region limitation
         */
        void clearRegion();

        /// True if use "Wall To Wall", default is "Preserve Boundary"
        bool IsWallToWall = false;
        /// Interpolation type to be used
        GridInterp::InterpMode Mode = InterpMode.LINEAR;
        /// interpolation radius
        float Radius = 100;
    };

}



#endif /* GRIDINTERP_HPP */

