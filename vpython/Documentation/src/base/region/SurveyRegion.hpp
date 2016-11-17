#ifndef _SURVEYREGION_INC_
#define _SURVEYREGION_INC_

#include <PAIR.hpp>
#include <vlBase.hpp>
#include <VECTOR.hpp>

class Geometry;
class vmPoly2D;
class vmBoolMask;
class SurveyRegionFactory;

/*
 * \code{.py}
 * \endcode  
 */

namespace base {
    namespace region {

        /**
         * This class can can be used for different functionality with Volumes,
         * Horizon Groups, etc. It is usually created from :class:`Polygon2D`, or :class:`MultiPolygon2D`.
         */
        class SurveyRegion {
        public:
            SurveyRegion();

            /**  @brief Check if grid (i_idx/j_idx) location is inside the region 
             * @param pos  grid location [x_index, y_index]
             * @return true if inside 
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * mp.load('path_to_XY_region_file')
             * sr = SurveyRegion()
             * sr.setPolygonCoordinates(mp.polygon(0))
             * print sr.grdInside([23,58])
             * \endcode  
             */
            bool grdInside(int[2] pos);

            /**  @brief Check if linear coordinate (XL/IL) location is inside the region.
             * @param pos  [xl, il] pair 
             */
            bool linInside(int[2] pos);

            /**  @brief Check if logical coordinate (survey X/Y) location is inside the region.
             * @param pos  [x, y] pair 
             */
            bool logInside(double[2] pos);

            /**  @brief heck if world coordinate (world X/Y) location is inside the region.
             * @param pos  [x, y] pair 
             */
            bool wldInside(double[2] pos);

            Geometry getGeometry();

            void setGeometry(Geometry geom);

            /**  @brief Check if region is a rectangle */
            bool isRectDefined();
            /**  @brief Clear rectangle coordinates */
            void clearRectangle();

            /**  @brief Set new polygon coordinates (will remove previous points)
             * \code{.py}
             * from base.region import *
             * mp = MultiPolygon2D()
             * mp.load('path_to_XY_region_file')
             * sr = SurveyRegion()
             * sr.setPolygonCoordinates(mp.polygon(0))
             * \endcode  
             */
            void setPolygonCoordinates(const vmPoly2D& poly);

            //return polygon for both rectangular and polygonal region
            /**  @brief Get a list of polygon points*/
            vector<Vect2d> regionPolygon();

            /**  @brief Return region type (RECT , POLYGON) */
            int getType();

            /**  @brief Set region type (RECT , POLYGON)*/
            void setType(int type);

            /**  @brief Generate binary mask based on reference geometry */
            void makeMask(BoolMask, Geometry);

            /**  @brief Clear all data*/
            void clear();


        }; // class SurveyRegion
    } // namespace region
} // namespace base


#endif /* _SURVEYREGION_INC_ */
