#ifndef _HorizonGroup_HPP_
#define _HorizonGroup_HPP_


namespace base {

    /**
     * This class supports operations on the tornado horizon group data
     * 
     *  Support provided for both binary formats and ascii format as used by Tornado.
     * 
     * \code{.py}
     * from base import HorizonGroup
     * a = HorizonGroup()
     * a.load('path_to_data')
     * 
     * \endcode   
     *  
     */
    class HorizonGroup {
    public:

        HorizonGroup();

        /**
         * Load a group of horizons from a horizon file
         * 
         * @warning For G2 job
         * @warning           add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         */
        bool load(string path);

        /** 
         * Save a group of horizons from a horizon file. 
         * format is for binary version only
         * @param as_binary default is binary (more efficient)
         * @param version for binary version only; default is 2
         * @warning  -Will overwrite the inpuWt data 
         *           -Consider using :func:`setPath` before save
         */
        bool save(bool as_binary = false, int version = 2);

        /** Set the new path to horizon group, will be saved to the given path 
         * when @ref save() is called
         * @param  notify  when using in interactive mode notify tornado that you changed the horizon 
         */
        void setPath(string new_path, bool notify = true);

        /** Get the number of horizons in the horizon group */
        int getNumHorizon();

        /** Get the *name* horizon object in the horizon group
         *   it will return None if there are no horizons in the group
         *   using **hg.getHorizonByName('hrzname')** is equivalent 
         *      to **hg.getHorizonByIndex( hg.getHorizonIndexByName('hrzname') )
         * @see getHorizonIndexByName() getHorizonByIndex()
         */
        base::Horizon getHorizonByName(string name);
        /** @ref getHorizonByName() */
        base::Horizon getHorizon(string name);

        /** 
         * Get the index of horizon in the horizon group
         * @return  -1 if there is no *name* horizon in the group
         */
        int getHorizonIndexByName(string name);
        /** 
         * Get the horizon object stored at the specified index of horizon group 
         * @return  horizon object, or None if failed
         */
        int getHorizonIndex(base::Horizon* hrz);


        /**
         * Create an empty horizon with given name
         * @return  horizon object, or None if failed
         */
        base::Horizon createHorizon(string name);

        /** Remove horizon at index i */
        void removeHorizonByIndex(int idx);

        /** 
         * Add an attribute to the group. 
         * Data is initialized to 0 / novalue.
         * @return true if add succeeded or attribute already exists; false if add failed.
         * 
         * \code{.py}
         * from base import HorizonGroup
         * a = HorizonGroup()
         * a.load('path_to_data') 
         * a.addAttribute('test')
         * \endcode  
         *      
         */
        bool addAttr(string attr);
        /** @see addAtt() */
        bool addAttribute(string attr);

        /** 
         * Lateral shift the horizon by a 2D vector (x,y).
         * @param X and Y are the distance in meter or feet
         */
        HorizonGroup shift(double x, double y);
        
        /** 
         * @param angle in degree.
         */
        HorizonGroup rotate(double angle);


        /** Get display name of the horizon group (similar to base name, but excludes any tags) */
        string getName();

        /**
         * Get the size of grid in x, y, z dimension.
         * For the concept of geometry, check Tornado Geometry
         * @return list([nx,ny,nz, dummy])
         */
        list[4] getSize();

        /** Geometry of the Horizon Group (and all the horizons)*/
        base::Geometry getGeometry(HorizonGroup * hg);

        /** Create a copy of the Horizon Group (not visible from Tornado app) */
        HorizonGroup clone(HorizonGroup * hrzGrp);
    };
};

#endif /* _HorizonGroup_HPP_ */
