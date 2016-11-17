#ifndef _VMBASEGEOMETRY_INC_
#define _VMBASEGEOMETRY_INC_


namespace base {

        /**
         * This script show how to set a Geometry and load a Geometry from
         * a geom file how to get information.
         * 
         * @example testBaseGeometry.py
         * 
         * 
         * GeometryFile("testGeometry.geomx")
         * \code
         *  min: (3693,8003,0)
         *  max: (4577,8883,480)
         *  inc: (4,4,1)
         *  size: (222,221,481)
         *  step: (100.000000,100.000000,25.000000)
         *  orig: (92300.000000,200050.000000,0.000000)
         *  datum: (0.000000)
         *  domain: ()
         *  north: (0.000000)
         *  rotate: (-0.000000000)
         *  rwSurvey00: (0.000000,0.000000,0.000000)
         *  yFlip: (0)
         *  surveyStart: (1.000000,1.000000,0.000000)
         *  surveyOrigin: (0.000000,0.000000,0.000000)
         * \endcode
         *
         */


        /**
         * Defined in the survey coordinate space.
         * 
         * A corresponding GUI/script mapping is shown below:
         * \image html geometry_fig1.png
         * \image latex geometry_fig1.png
         * 
         * Geometry parameters mapped to Tornado Geometry dialog
         * 
         *
         * Example:
         * \code{.py}
         * from base import Geometry
         * a = Geometry()
         * a.load('path_to_data')
         * m=a.getMax()
         * print m
         * 
         * [100, 100, 10000, 1]
         * 
         * m[1]=3000
         * a.setMax(m)
         * print m
         * 
         * [100,3000, 10000, 1] 
         * 
         * \endcode
         * 
         * 
         * @warning 
         * After modifying maximum line coordinate, either minimum, 
         * size or increment **must also be updated** as otherwise the geometry
         *  will become inconsistent.
         * @note
         * After modifying minimum line coordinate(s), consider updating the 
         * geometry origin to set proper X/Y survey coordinates.
         * @note When you operate on geometry, especially using set functions 
         * such as setInc, you should be careful with the original one
         * @note Geometry can be obtained from data, using getGeometry() call
         * Examples:
         * \code{.py}
         * from base import *
         * 
         * #get geometry from a loaded horizon group
         * hg = HorizonGroup()
         * hg.load("path/to/horizon/group")
         * hgGeometry = hg.getGeometry()
         * 
         * #load volume header and create geometry
         * #the volume itself is not loaded
         * vol = Volume()
         * volGeometry = vol.getGeometry("path/to/volume")
         * \endcode
         *
         * More complex example:
         * @ref testBaseGeometry.py
         *
         */
        class Geometry  {
        public:

            /** @brief get the minimum of crossline, inline.
             * @return list with 4 integers [nx,ny,nz,1]
             */
            int[4] getMin(); // minimum line (grid 0)

            /** @brief get the maximum of crossline, inline. 
             * @return list with 4 integers [nx,ny,nz,1]
             */
            int[4] getMax(); // maximum line (grid size-1)

            /** @brief set the minimum of crossline, inline. */
            void setMin(int[4] min);

            /** @brief set the maxmum of crossline, inline. */
            void setMax(int[4] max);

            /** @brief get the line increment for each grid. 
             * @return list with 4 integers [nx,ny,nz,1]
             */
            int[4] getInc(); // line increment with grid step

            /** @brief set the line increment for each grid. */
            void setInc(int[4] inc);

            /** @brief get the grid size. 
             * @return list with 4 integers [nx,ny,nz,1]
             */
            int[4] getSize();

            /** @brief . set the grid size */
            void setSize(int[4] size);

            /** @brief get the origin of the survey geometry.
             * @return: list([x,y,z, dummy]), survey origin
             */
            double[4] getOrigin();

            /** @brief set the origin of the geometry.
             * @param origin list([x,y,z, dummy]), survey origin */
            void setOrigin(double[4] origin);

            /** @brief get the increment of logical distance for each grid direction. 
             * @return list([dx,dy,dz, dummy])
             */
            double[4] getStep();

            /** @brief set the increment of logical distance for each grid direction. */
            void setStep(double[4] step);

            /** 
             * @brief convert line coordinate to grid index location.
             * @param vect list with 4 elements ([xl,il,z,dummy])
             * @return Grid index location
             * Example:
             * \code{.py}
             * from base import Geometry
             * a = Geometry()
             * a.load('path_to_data')
             * print a.lin2grd([100,25,0,0])
             * 
             * [1, 1, 0, 0]
             * \endcode
             */
            double[4] lin2grd(double[4] vect);

            /**
             * @brief convert survey coordinate to grid index location. 
             * @param vect list with 4 elements ([x,y,z,dummy])
             * @return Grid index location
             * Example:
             * \code{.py}
             * from base import Geometry
             * a = Geometry()
             * a.load('path_to_data')
             * print a.log2grd([1250.25,13124.50,0,0])
             * 
             * [100, 25, 0, 0]
             * \endcode
             */
            double[4] log2grd(double[4] vect);

            /** @brief load from a text file or store.
             * Generic load function from either text or store
             *
             * Example:
             * \code{.py}
             * from base import Geometry
             * a = Geometry()
             * a.load('path_to_data')
             * \endcode
             * @warning For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
             * 
             */
            bool load(string path);
        };

} // namespace base

#endif /* _VMBASEGEOMETRY_INC_ */
