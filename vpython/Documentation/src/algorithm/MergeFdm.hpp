
#ifndef MERGEFDM_HPP
#define MERGEFDM_HPP


namespace algorithm {


    /**
     * 
     * @brief Attribute horizontal merge 
     * @warning his feature does not work on G2 torpy job, can be used on 'tornadoi -script xx.py -nodisplay'
     * 
     * 
     * In Tornado (Attribute->Update->Horizontal Merge ).
     * @image html MergeFdm.png
     * 
     * Below is an example job to do the MergeFdm with full volumes and with region
     * \code{.py}
     from base import *
       from algorithm import MergeFdm
       from algorithm import MergeCombineOption as MP
       
       path="/data2/devtest/tornado/yanhliu/Script_test/"
       data1, data2= Volume(), Volume()
       data1.load(path+'Tomerge1.fdm')
       data2.load(path+'toMerge2.fdm')
       
       vol_lst=[data1,data2]
       file_lst=[path+'toMerge3.fdm']
       
       merger=MergeFdm()
       #do the setups
       merger.setInMemData(vol_lst)
       merger.setOnDiskData(file_lst)
       merger.setNullValue(0)
       merger.setCombineOption(MP.ByLine)
       merger.setGeom([3693,8003,0,0],[4573,8883,1000,0],[4,4,1,0])
       merger.setTargetName('/data2/devtest/tornado/yanhliu/Script_test/test2.fdm')
       merger.setSaveToDisk(True)
       merger.doMerge()
       
       
       #now test with region method
       merger=MergeFdm()
       
       aRegion=MultiPolygon2D()
       aRegion.load("/data2/devtest/tornado/yanhliu/Script_test/mergeRgn.rgn")
       aSurveyRegion = SurveyRegion()
       aSurveyRegion.setPolygonCoordinates(aRegion.polygon(0))
       
       #do the setups
       lst2=vol_lst[:2] # the first 2
       merger.setInMemData(lst2)
       merger.setNullValue(0)
       merger.setToSmoothBoundary(True)
       merger.setSmoothBoundaryRadius(6)
       merger.setCombineOption(MP.ByLine)
       merger.setGeom([3709,8195,0,0],[4121,8631,1000,0],[4,4,1,0])
       merger.setTargetName('/data2/devtest/tornado/yanhliu/Script_test/test3.fdm')
       merger.setSaveToDisk(True)
       merger.setRegion(aSurveyRegion)
       merger.doMerge()
     * \endcode
     * 
     */

    class MergeFdm2 {
    public:

        enum MergeCombineOption {
            /// Merge according to cdp line
            ByCoordinate,
            /// Merge according to xy coordinate
            ByLine
        };
        
        
        /**
         * Set all the Volumes in volList as input for later merging.
         * All the loaded volumes will be ahead of volumes on disk which is set by @see setOnDiskData
         * 
         * @brief input list from your loaded volumes
         * @param lst the volumes stored in a list
         */
        void setInMemData(list<Volume> lst);

        /**
         * Set all the Volume paths from strList as input for later merging. 
         * All the volumes on disk will be behind loaded volumes  
         * which is set by  @see setInMemData
         * 
         * @brief input list from path
         * @param lst he volumes paths stored in a python list
         */
        void setOnDiskData(list< string > lst);

        /**
         * @brief set the value at the empty area
         */
        void setNullValue(float val);

        /**
         * @brief Specify whether to combine by line or by coordinate
         */
        void setCombineOpt(MergeCombineOption tp);

        /**
         * Set the min, max and inc of cdp lines
         * 
         * Example
         * \code{.py}
         * 
         * merger.setGeom([3709,8195,0,0],[4121,8631,1000,0],[4,4,1,0])
         * 
         * \endcode
         * 
         * @param min minimum of cdp lines
         * @param max maximum of cdp lines
         * @param inc increment of cdp lines
         */
        void setGeom(int min[4], int max[4], int inc[4]);

        /** 
         * This needs to be a full path, if save to disk is selected
         * @brief name of the merged FDM
         */
        void setTargetName(string name);
        
        /*
         * set the flag to use smoothing after merge
         */
        void setToSmoothBoundary(bool flg);

        /**
         * set the smooth radius, will have no effect if smooth boundary flag is off
         */
        void setSmoothBoundaryRadius(int radius);

        /**
         * @brief  will save to memory and user can access the volume from @ref getMergeVolume
         */
        void setSaveToMemory(bool flag);

        /**
         * Set whether to save the merged FDM to disk, if you set it to be True,
         * you will need to provide a full path to the output volume with @see setTargetName 
         * 
         * @brief Save to disk, once merge is done
         */
        void setSaveToDisk(bool flag);
        
        /**
         * @brief Perform the merge based on the setup
         */
        bool doMerge();
        
        /**
         * After doMerge is called, if setSaveToMemory is called, user will have access to 
         * the new volume
         */
        Volume* getMergedVolume();
        

        /**
         * Set a region as a mask to be applied the second FDM on the final list. Only one type 
         * of mask will work. If you use @ref setHorizon later, only horizon will take effect
         * @param rgn region defined by user or loaded from file
         */
        void setRegion(SurveyRegion rgn);
        
        /**
         * Set a horizon as a mask to be applied the second FDM on the final list. Only one type
         *  of mask will work. If you use @ref setRegion later, only region will take effect
         */
        void setHorizon( Horizon hrz);

    };
}

#endif /* MERGEFDM_HPP */

