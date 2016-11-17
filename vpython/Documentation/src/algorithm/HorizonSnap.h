

class base::Volume;
class base::Horizon;
class base::region::SurveyRegion;

namespace algorithm {

    using namespace base;
    using namespace base::region;
   /**
     * @example advancedHorizonSnap.py
     */

    /**
     * Snap horizon to seismic (Horizon Picking Window: Edit->Snap + Options->Snap Control).
     * 
     * @image html HorizonSnap.png Dialog from Tornado 
     * 
     * 
     * # Code snippet
     * 
     * \code
       from algorithm import HorizonSnap

       # Load Seismic
       seismic_path="test.fdm"
       data=Volume()
       data.load(seismic_path)

       # Load Horizon
       horgroup_path="test.hrz"
       hg=HorizonGroup()
       hg.load(horgroup_path)
       hrz=hg.getHorizonByName("hrzName")
       
       # Z attribute 
       m=originhrz.getAttrIndex('DEPTH')
       
       snap= HorizonSnap()
       snap.setMode(HorizonSnap.POSITIVE_PEAK)
       snap.setWindowSize(100)
       snap.run(vol,hrz,m)
       
       # Save
       hg.save(True)  
     * \endcode
     * 
     * 
     * # Example
     * @ref advancedHorizonSnap.py
     * 
     * 
     * @brief Snap horizon to seismic
     * 
     */
    class HorizonSnap {
    public:

        /** Adjust to the signal */
        enum SnapMode {
            POSITIVE_PEAK, NEGATIVE_PEAK,
            POSITIVE_ZEROCROSS, NEGATIVE_ZEROCROSS,
            POSITIVE_MAXGRADIENT, NEGATIVE_MAXGRADIENT
        };

        HorizonSnap();

        /** 
         * @param vol input seismic
         * @param hrz Horizon group 
         * @param idx  horizon attribute index for z (i.e:DEPTH/TWT). Use  z_index = hrz.getAttrIndex('DEPTH')

         * @brief run the process to extract parameter */
        bool run( Volume vol, Horizon hrz, int idx);

        /** 
         * @param region defined by user or loaded from file
         * @brief limit computation to a region*/
        void setRegion( SurveyRegion rg);
        
        /** @brief clear applied region*/
        void clearRegion();

        /** 
         * Number of samples bellow (and above) horizon
         * @brief vertical window to snap*/
        void setWindowSize(int size);

        /** @brief adjust mode to the signal*/
        void setMode(SnapMode mode);
    };

}
