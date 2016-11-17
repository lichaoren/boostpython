#ifndef VMREGIONAPI_HPP
#define VMREGIONAPI_HPP


namespace base {
    namespace region {

        /**
         * This class defines a boolean mask used by a region. This is 
         * used, for example, to determine whether a point is within region
         *  or outside it.
         * 
         * Exemple
         * \code{.py}
         * from base.region import *
         * bm = BoolMask()
         * bm.setDim(10,10)
         * bm.set(1,1,True)
         * bm.set(9,9,True)
         * bm.printAll()
         * 
        0000000000
        0100000000
        0000000000
        0000000000
        0000000000
        0000000000
        0000000000
        0000000000
        0000000000
        0000000001
         * 
         * 
         * \endcode  
         * @note
        * BoolMask is usually not manipulated directly, but is used by region internally.
        */
        class BoolMask {
        public:
                        
            /** 
             * @brief Set width and height of the boolean mask.
             * @param w width of boolean mask
             * @param h height of boolean mask
             * 
             * @note using **setDim(0,0)** is equivalent to setting the BoolMask 
             * to uninitialized
             */
            void setDim(int w, int h);

            /** @brief Get width and height of the boolean mask.
             *
             * \code{.py}
             * from base.region import *
             * bm = BoolMask()
             * bm.setDim(50,50)
             * d = bm.getDim()
             * print d
             * 
             * [50,50]
             * \endcode  
             */
            int[2] getDim();
                        
            /** @brief Set a point true/false.
             * 
             * \code{.py}
             * from base.region import *
             * bm = BoolMask()
             * bm.setDim(50,50)
             * bm.set(23,44,True)
             * \endcode
             * 
             * @warning If x or y are outside the mask dimentions, nothing will be done.
             */
            void set(int x, int y, bool flag);
                        
            /** @brief Set all points true/false. */
            void setAll(bool flag);
                        
            /** @brief Check the value of a point (true/false). */
            bool isSet(int x, int y);
                        
            /** @brief Reverse all values of the boolean mask. */
            void reverse();
                        
            /** @brief Print the values of all points to the shell. */
            void printAll();
                        
            /** @brief Delete the boolean mask's internal structure, make it uninitialized.
             * Example:
             * \code{.py}
             * rom base.region import *
             * bm = BoolMask()
             * bm.setDim(150,150)
             * #do something
             * bm.destroy()
             * bm.setDim(50,50)
             * #do something else
             * \endcode  
             * 
             * 
             */
            void destroy();
                        
            /** @brief Check if boolean mask was not initialized. */
            bool isNull();
                        
            /** @brief Change width and height of the mask. */
            void resize(int w, int h);
                        
            /** @brief et the origin and size of the bounding box around all set flags.
             * @return origin and size of the bounding box 
             * Example:
             * \code{.py}
             * from base.region import *
             * bm.setDim(50,50)
             * bm.setAll(True)
             * bb = bm.getBoundingBox()
             * print bb
             * 
             * [0,0,50,50]
             * 
             * \endcode  
             * 
             */
            int[4] getBoundingBox();
                        
            /** @brief Returns the number of flags set to true.*/
            long getCount();
                        
            /** @brief . */
            void generateMask(Geometry geom, Horizon hrz, BoolMask pm);

        };

    } // namespace region
} // namespace base

#endif	/* VMREGIONAPI_HPP */


