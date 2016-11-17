/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   VolumeExtractAttribute.hpp
 * Author: Dominique DOLEDEC
 *
 * Created on May 27, 2016, 12:49 PM
 */

#ifndef VOLUMEEXTRACTATTRIBUTE_HPP
#define VOLUMEEXTRACTATTRIBUTE_HPP


//class base::Volume;
//class base::Horizon;
//class base::HorizonGroup;

namespace algorithm {
    
    
    /**
     *  Extract/Compute attribute along horizon (Seismic->Extract Attribute).
     * 
     * @image html VolumeExtractAttribute.png
     * 
     * \code{.py}
     *        
     * # Load Seismic
     * seismic_path="test.fdm"
     * data=Volume()
     * data.load(seismic_path)
     * # Load Horizon
     * horgroup_path="test.hrz"
     * hg=HorizonGroup()
     * hg.load(horgroup_path)
     * hrz=hg.getHorizonByName("hrzName")
     * # extract RMS
     * #  PEAK, TROUGH, PEAK_LENGTH, PEAK_AREA, INST_FREQ, 
     * #  INST_PHASE, REFL_STRENGTH, RESPONSE_ENERGY, RESPONSE_FREQ, 
     * #  RESPONSE_PHASE, RESPONSE_LENGTH, RESPONSE_DEPTH,  DIP_SEISMIC_EVENT,
     * #  AZIMUTH_SEISMIC_EVENT, RMS
     * extractAttr = VolumeExtractAttribute()
     * extractAttr.windowSizeAbove=5 # number of smaples
     * extractAttr.windowSizeBelow=5 # number of smaples
     * extractAttr.mode=VolumeExtractAttribute.RMS
     * extractAttr.run(vol, hg, hrz,"RMS_ATTR")
     * # Save
     * hg.save()
     * 
     * \endcode
     * 
     * @brief Seismic attribute along horizon. 
     */
class VolumeExtractAttribute {
    public:
        VolumeExtractAttribute();

        enum VolumeExtractAttribute {
            /// PEAK
            PEAK,
            /// TROUGH,
            TROUGH,
            /// PEAK_LENGTH,
            PEAK_LENGTH,
            /// PEAK_AREA,
            PEAK_AREA,
            /// INST_FREQ,
            INST_FREQ,
            /// INST_PHASE,
            INST_PHASE,

            /// REFL_STRENGTH,
            REFL_STRENGTH,
            /// RESPONSE_ENERGY,
            RESPONSE_ENERGY,
            /// RESPONSE_FREQ,
            RESPONSE_FREQ,
            /// RESPONSE_PHASE,
            RESPONSE_PHASE,

            /// RESPONSE_LENGTH,
            RESPONSE_LENGTH,
            /// RESPONSE_DEPTH,
            RESPONSE_DEPTH,
            /// DIP_SEISMIC_EVENT,
            DIP_SEISMIC_EVENT,
            /// AZIMUTH_SEISMIC_EVENT,
            AZIMUTH_SEISMIC_EVENT,
            /// RMS
            RMS
        };
        

        /** 
         * Extract attribute along horizon to seismic (Horizon Picking Window: Edit->Snap + Options->Snap Control).
         * @brief Extract attribute along horizon 
         * 
         * @param vol Volume: used for input
         * @param hg Horizon group
         * @param hrz Horizon object
         * @param attrName output attribute name
         * 
         */
        bool run(Volume vol, HorizonGroup hg, Horizon hrz,string attrName);
        
        /** 
         * 
         * @param vol Volume: used for input
         * @param hg Horizon group
         * @param attrName output attribute name 
         * 
         * @brief Extract attribute for all the horizons into the horizon group 
         */
        bool run( Volume vol, HorizonGroup hg, string attrName);

        /// number of samples above
        int windowSizeAbove;
        
        /// number of samples below
        int windowSizeBelow; 
        /// default RMS    
        VolumeExtractAttribute mode;   
    };


    
    
}

#endif /* VOLUMEEXTRACTATTRIBUTE_HPP */

