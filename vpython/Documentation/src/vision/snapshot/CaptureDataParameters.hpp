



namespace vision {

    /** 
     * @ingroup snapshot
     */

    
    /**
     * Data to use (as a toggle list)
     * 
     * When you want to capture images it can be interesting to define the data you 
     * are interested in (seismic, attribute, gather...)
     * The CaptureDataParameters looks like the toggle list that is used in Tornado, as 
     * you can define the data (seismic, attribute..) for each group index. 
     * 
     * Let's define 2 groups
     * \code{.py}
       from vision import *
       
       ### Set Toggle Group 0
       ### Add seismic
       seismic1=vision.getSeismic('xp_M_aa.fdm')
       dataParam.addSeismic(seismic1,0)
       ### Add Attribute
       attribute1=vision.getAttribute('xp_M_a3.fdm')
       dataParam.addAttribute(attribute1,0)
     * 
       ### Set Toggle Group 1
       ### Add seismic
       seismic2=vision.getSeismic('xp_M_a.fdm')
       dataParam.addSeismic(seismic2,1)
       ### Add Attribute
       attribute2=vision.getAttribute('xp_M_a2.fdm')
       dataParam.addAttribute(attribute2,1)
       
       param.setDataParameters(dataParam)
     * \endcode
     * 
     * @brief Data to capture
     */
    class CaptureDataParameters {
    public:

        /** @brief Add seismic to capture 
         * @param  name of the seismic as defined in tornado
         * @param idx index to use in the toggle list (start at 0)
         */
        void addSeismic(string name, int idx = 0);
        
        
        /** @brief Add Attribute to capture 
         * @param  name of the attribute as defined in tornado
         * @param idx index to use in the toggle list (start at 0)
         */
        void addAttribute(string name, int idx = 0);
        
        
        /** @brief Add Gather to capture 
         * @param  name of the gather as defined in tornado
         * @param idx index to use in the toggle list (start at 0)
         */ 
        void addGather(string name, int idx = 0);

        /** Remove all the items in the toggle list 
         */
        void clear();
        
        /** Number of data group */
        int size() const;

    };


}


