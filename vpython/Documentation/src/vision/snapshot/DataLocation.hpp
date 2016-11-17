



namespace vision {
    /** 
     * @ingroup snapshot
     */

    /**
     * Generic definition for location (where the data need to be in the scene)
     *     You cannot use this class directly, but rather use VolumeLocation, WellLocation ...
     * i.e: Well list, volume list, bookmarks ...
     */
    class DataLocation {
    public:

        /** @brief Inline value
         * @param  idx if several positions are set provide the index */
        int getIL(int idx = 0);
        /** @brief  Crossline value
         *  @param  idx if several positions are set provide the index */
        int getXL(int idx = 0);
    };

    /**
     *  Set the location from a well or a list of well
     * Example: 
     *      - in the main view it will move the inline/crossline slices to the position of the well
     *      - in the gather window it will select the centered gather according to the nearest well position
     * 
     * \code{.py}
     * from vision import *
     * #######  Create the parameters
     * param = CaptureParameters()

     * # Location from Well
     * # !!! This is the well IDs and not the well names
     * wellList=['w1','GB_015_29_01_2_1_2_1','GB_015_29_01_2_1']
     * 
     * wellLocation = WellLocation()
     * wellLocation.addWell(wellList)
     * param.setLocations(wellLocation)
     * 
     * \endcode
     * 
     */
    class WellLocation : public DataLocation {
    public:

        WellLocation();

        /** Set the list of well (ID)
         * @param lst  This is the well IDs and not the well names
         */
        void addWell(list<string> lst);

        /** number of positions */
        int size();
    };

    /**
     *  Set the location in the window from a list of Inline and/or Crossline and/or depth 
     * 
     * example: 
     * - in the main view it will move the inline/crossline slices to the position of the well
     * - in the gather window it will select the centered gather according to the nearest well position
     * 
     * \code{.py}
     * 
     * from vision import * 
     *       
     * #######  Create the parameters
     * param = CaptureParameters()   
     *     
     * # Location from IL,XL
     * volLocation=VolumeLocation()
     * volLocation.setIL(20200)
     * volLocation.setXL(41284, 41684, 64)       
     * param.setLocations(volLocation)
     * 
     * \endcode
     * 
     */
    class VolumeLocation : public DataLocation {
    public:

        enum Direction {
            InlineDirection, CrosslineDirection, TimeDirection
        };

        /**
         * Define inline as a single position or within a range
         * @brief Set the Inline position
         * @param min minimum range OR value if max=-1 and step=-1
         * @param max maximum range
         * @param step step range
         */
        void setIL(int min, int max = -1, int step = -1);

        /**
         * Define crossline as a single position or within a range
         * @brief Set the crossline position
         * @param min minimum range OR value if max=-1 and step=-1
         * @param max maximum range
         * @param step step range
         */
        void setXL(int min, int max = -1, int step = -1);

        /**
         * Define depth as a single position or within a range
         * @brief Set the depth position
         * @param min minimum range OR value if max=-1 and step=-1
         * @param max maximum range
         * @param step step range
         */
        void setZ(int min, int max = -1, int step = -1);


        /** 
         * Define il,xl position, from a list of couple (xl,il) 
         * 
         * \code{.py}
         * volLocation.setPositions( [(1603,4275),(1603,5000)] )
         * \endcode
         */
        void setPositions(std::vector< std::vector< int > >& posList);
    };

    /**
     * Location extracted from a user Note
     */
    class UserNoteLocation : public DataLocation {
    public:
        UserNoteLocation();

        virtual int size() const;

        /** Recenter view when moving to next item */
        bool isRecenterView() const;

        /** Set the user note to use
         *  @param idx of the current Map in vmUserNoteMapBModel
         */
        void setCurrentUserNote(int idx);


        /** load a note file, if not already loaded */
        bool load(string path, bool loadIfExist = false);
    };

}



#ifdef TO_CONFIRM
/**
 * Locations  returned by the bookmarks
 */
//    class BookmarkLocation : public DataLocation {
//    public:
//        BookmarkLocation();
//
//        BookmarkLocation& operator=(BookmarkLocation&);
//
//        int size();
//
//        int getIL(int idx = 0);
//        int getXL(int idx = 0);
//        double getX(int idx = 0);
//        double getY(int idx = 0);
//        double getZ(int idx = 0);
//    };
#endif


