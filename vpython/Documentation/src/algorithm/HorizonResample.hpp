

#ifndef HORIZONRESAMPLE_HPP
#define HORIZONRESAMPLE_HPP


namespace algorithm {


    /**
     * @brief Resample the Horizon accroding to a different geometry 
     * 
     * In Tornado (Main Window: Horizon->Structural Changes->Resample...  ).
     * @image html HorizonResample.png
     * 
     * \code{.py}
     * from base import *
     * from algorithm import HorizonResample
     * 
     * #load the horizon group
     * hg= HorizonGroup()
     * hg.load('/data2/devtest/tornado/test_scripting/hrz_gb.hrz')
     * 
     * #Get geometry from a volume
     * data=Volume()
     * geom = data.getGeometry('/s0/scr/DATA/essaiCreateModel.fdm')
     * 
     * #Resample
     * hrzRegrid = HorizonResample()
     * hrzRegrid.setHorizonGroup(hg)
     * hrzRegrid.setGeom(geom)
     * output=hrzRegrid.run('/tmp/Resampled.hrz')
     * output.save()
     * 
     * \endcode
     * 
     */
     
    class HorizonResample {
    public:
        /**
         * @brief HorizonGroup to Resample
         */
        void setInputHorizon(HorizonGroup inputHorizon);

        /** 
         * @brief Run Resampling and return the new Horizon Group
         * @param fname file name associated with the horizonGroup (need to save Horizon after process)
         * @return 
         */
        HorizonGroup run( string fname);

        /** @brief If you want to interpolate the horizon after the resampling */
        void setInterpolate(bool interpolate);
        
        /** New geometry */
        void setGeom(Geometry* myGeom);
    };
       
}
    
    
#endif /* HORIZONRESAMPLE_HPP */

