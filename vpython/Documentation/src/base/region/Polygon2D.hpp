#ifndef VMREGIONAPI_HPP
#define VMREGIONAPI_HPP



namespace base {
    namespace region {

        /**
         * Simple 2D Polygon
         */
        class Polygon2D {
        public:
            /** 
             * Adds a new point to the polygon, if it is located close 
             * to already defined points. Returns the new point.
             * @brief Add a new point at location x,y.
             * \code{.py}
             * from base.region import *
             * p = Polygon2D()
             * p.addPoint(1.1,2.2)
             * \endcode 
             */
            Point2D addPoint(double x, double y);

            /** 
             * The new point will be placed at the beginning of the 
             * previously defined points. Returns the new point.
             * @brief Add a new point at location x,y at the beginning
             * 
             * @return new point.
             */
            Point2D addPointSequence(double x, double y);
            
            /** 
             * @brief Add a new point at location x,y at specified index
             * Adds a new point to the polygon. The new point will be added at 
             * specified index. If index is invalid, the new point will be placed
             *  at the beginning of the previously defined points.
             * @param index index specifying location of a new point
             * @return new point.
             * 
             * \code{.py}
             * from base.region import *
             *  = Polygon2D()
             * p.addPointSequence(1.1,2.2)
             * p.addPointSequence(5.5,6.6)
             * p.addPointSequenceAt(1,3.3,4.4)
             * \endcode
             * 
             */
            Point2D addPointSequenceAt(int idx, double x, double y);
            
            /** Delete point pointed by index */
            void delPoint(Point2D);
            /** Get number of points */
            int getNum();

            /** 
             * Simplify the polygon shape removing points that are closer than 
             * provided threshold. Returns false, if could not simplify (e.g. region
             * is too small). 
             * @brief  Simplify the polygonal shape 
             * @param threshold maximum distance between points.
             * \code{.py}
             * from base.region import *
             * p = Polygon2D()
             * p.addPointSequence(1.1,2.2)
             * p.addPointSequence(1.1,2.3)
             * p.addPointSequence(1.1,2.4)
             * p.addPointSequence(1.1,2.5)
             * p.addPointSequence(5.5,6.6)
             * p.addPointSequence(2.2,3.3)
             * p.simplify(0.2)
             * 
             * \endcode
             */
            bool simplify(float thresh);

            /**  @brief  Set reference geometry used for mask calculation
             * Example
             * \code{.py}
             * from base.region import *
             * import Geometry
             * geo = Geometry()
             * geo.load('path_to_geometry_file')
             * mp = MultiPolygon2D()
             * mp.setGeometry(geo)
             * \endcode 
             */
            void setGeometry(Geometry g);
            
            /**  @brief Set reference geometry used for mask calculation
             *
             * \code{.py}
             * from base.region import *
             * w = 100
             * h = 100
             * origX = 0
             * origY = 0
             * dx = 10
             * dy = 10
             * mp = MultiPolygon2D()
             * mp.setGeometry(w,h,origX,origY,dx,dy)
             * \endcode                 */
            void setGeometry(int, int, double, double, double, double);
            

            Geometry getGeometry();

            /** Generate a binary mask from geometry's horizontal grid.
             *  Grid points inside polygon has their flag turned on if not reversed.
             * \code{.py}
             * from base.region import *
             * import Geometry
             * geo = Geometry.Geometry()
             * geo.load('path_to_geometry_file')
             * p = Polygon2D()
             * p.addPointSequence(1,1)
             * p.addPointSequence(10,1)
             * p.addPointSequence(10,10)
             * p.addPointSequence(1,10)
             * m = p.makeRawMask(geo)
             * 
             * \endcode
             */
            BoolMask makeRawMask(Geometry g);
            

            /**  @brief Taper length */
            double getTaperWidth();
            
            /**  
             * Set the width of taper zone for the weights grid in all defined 
             * polygons. The grid points within polygon have weights set to 1. 
             * The points outside polygon have weights 0. The taper zone can be
             * made smoother or sharper using this function.
             * @brief Change taper length.
             */
            void setTaperWidth(double length);
            
            /**  @brief Indicate whether the region should be reversed (applied to all polygons). */
            void setReverse(bool flag);
            /**  @brief Check if the region is reverse */
            bool isReverse();
            
        }; // class Polygon2D

        
        /**
         * Manages a collection of polygons
         */
        class MultiPolygon2D {
        public:

            /**  @brief  Adds a new empty polygon.
             * Adds a new empty polygon to the list and returns a pointer to it. 
             * \code{.py}
             * 
             * from base.region import *
             * mp = MultiPolygon2D()
             * polygon0 = mp.addPolygon()
             * \endcode               
             */
            Polygon2D addPolygon(); 
            
            /**  @brief  Removes a polygon
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * polygon0 = mp.addPolygon()
             * polygon1 = mp.addPolygon()
             * polygon2 = mp.addPolygon()
             * mp.removePolygon(0)
             * \endcode               
             * @param idx index of polygon to remove from 0 to number of polygons - 1
             */
            bool removePolygon(int idx);
            
            
            /**  @brief Get a polygon specified by `idx`
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * polygon0 = mp.addPolygon()
             * polygon1 = mp.addPolygon()
             * polygon2 = mp.addPolygon()
             * poly1 = mp.polygon(1)
             * \endcode               
             *  
             * @param idx index of polygon to remove from 0 to number of polygons - 1
             */
           Polygon2D polygon(int idx);
           
            /**  @brief  Copy of polygon specified by `idx`
             */
            Polygon2D getPolygon(int idx);
            
            
            /**  @brief  number of polygons*/
            int numPolygons();
            /**  @brief  number of polygons
             * @see numPolygons()
             */
            int getNum();
            
            /**  @brief Add point to the closest polygon 
             * 
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * polygon0 = mp.addPolygon()
             * polygon0.addPointSequence(1,1)
             * polygon0.addPointSequence(10,1)
             * polygon0.addPointSequence(10,10)
             * polygon0.addPointSequence(1,10)
             * polygon1 = mp.addPolygon()
             * polygon1.addPointSequence(21,21)
             * polygon1.addPointSequence(31,21)
             * polygon1.addPointSequence(31,31)
             * polygon1.addPointSequence(21,31)
             * mp.addPoint(5,6) # adds point to polygon0 
             * \endcode               
             * 
             */
            void addPoint(double x, double y);
            
            /** @brief  Add point to the current polygon
             */
            void addPointToCurrent(double x, double y);
            
            /**  @brief Simplify polygon
             * Simplify polygon specified by `idx` using `threshold`
             */
            bool simplifyPolygon(int idx, float thresh);

            /**  @brief  index of current polygon */
            int curPolygonIdx();

            /**  @brief Set index of current polygon
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * polygon0 = mp.addPolygon()
             * polygon1 = mp.addPolygon()
             * mp.setCurPolygonIdx(0)
             * \endcode               
             */
            void setCurPolygonIdx(int i);
            
            /**  
             * @brief Delete point
             * 
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * polygon0 = mp.addPolygon()
             * polygon0.addPointSequence(1,1)
             * point = polygon0.addPointSequence(10,1)
             * polygon0.addPointSequence(10,10)
             * polygon0.addPointSequence(1,10)
             * polygon1 = mp.addPolygon()
             * polygon1.addPointSequence(21,21)
             * polygon1.addPointSequence(31,21)
             * polygon1.addPointSequence(31,31)
             * polygon1.addPointSequence(21,31)
             * mp.delPoint(0,point)
             * \endcode
             *                
             *  * @param  idx index of the polygon to delete the point from
             * @param  point point to delete         * 
             */
            void delPoint(int idx, Point2D point);
            
            /**  @brief  Set error range for testing hit */
            void setPointErr(double err);

            /**  @brief  Set reference geometry used for mask calculation
             * Example
             * \code{.py}
             * from base.region import *
             * import Geometry
             * geo = Geometry()
             * geo.load('path_to_geometry_file')
             * mp = MultiPolygon2D()
             * mp.setGeometry(geo)
             * \endcode 
             */
            void setGeometry(Geometry g);
            /**  @brief Set reference geometry used for mask calculation
             *
             * \code{.py}
             * from base.region import *
             * w = 100
             * h = 100
             * origX = 0
             * origY = 0
             * dx = 10
             * dy = 10
             * mp = MultiPolygon2D()
             * mp.setGeometry(w,h,origX,origY,dx,dy)
             * \endcode                 */
            void setGeometry(int, int, double, double, double, double);
            
            /**  @brief  Get reference geometry used for mask calculation*/
            Geometry getGeometry();

            /**  @brief Taper length */
            double getTaperWidth();
            
            /**  
             * Set the width of taper zone for the weights grid in all defined 
             * polygons. The grid points within polygon have weights set to 1. 
             * The points outside polygon have weights 0. The taper zone can be
             * made smoother or sharper using this function.
             * @brief Change taper length.
             */
            void setTaperWidth(double length);
            
            /**  @brief Indicate whether the region should be reversed (applied to all polygons). */
            void setReverse(bool flag);
            /**  @brief Check if the region is reverse */
            bool isReverse();


            /**  @brief  Load region from file 
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * 
             * mp.load('path_to_XY_region_file')
             * mp.load('path_to_INL_CRL_region_file',True,True)
             * \endcode
             *
             *
             * @param fileName
             * @param asLine Interpret values in the file as X / Y coordinates or Inline / Crossline. The default is `asLine == false` and the values are assumed to be X / Y coordinates.
             * @param append Clear previously defined polygons, or to append the polygons from file. The default is `append == false` and previous polygons will be deleted.
             */
            bool load(string fileName, bool asLine = false, bool append = false);
            
            /**  @brief  Save region to file.
             * 
             * \code{.py}
             * from base.region import *
             * import Geometry
             * mp = MultiPolygon2D()
             * geo = Geometry()
             * geo.load('path_to_geometry')
             * mp.setGeometry(geo)
             * mp.load('path_to_XY_region_file')
             * mp.save('path_to_INL_CRL_region_file',True)
             * \endcode
             * 
             * @param asLine Interpret values in the file as X / Y coordinates or Inline / Crossline. The default is `asLine == false` and the values are assumed to be X / Y coordinates.
             */
            bool save(string fileName, bool saveAsLine = false);
        }; // class MultiPolygon2D

    } // namespace region
} // namespace base

#endif /* VMREGIONAPI_HPP */

