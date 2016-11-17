


#ifndef VOLUMEDIPCUBE_HPP
#define VOLUMEDIPCUBE_HPP

namespace algorithm {

    /**
     * @brief Compute Dips
     * 
     * @image html VolumeDipCube.png
     * 
     * \code{.py}
     * 
     * from algorithm import VolumeDipCreator
     * 
     * dipCreator = VolumeDipCreator()
     * 
     * # The path to save the outputs (dipx, dipy, aziumth, tetha)
     * # You can set a path and a prefix
     * #dipCreator.setOutputPath('/scratch/DATA/SEISMIC/','iter1')
     * dipCreator.setOutputPath('/scratch/DATA/SEISMIC/')
     * 
     * #Change Dipx/Dipy default
     * # default is DEGREES and the file name are 
     * # <path>+<prefix>+'dipx' and 'dipy'
     * # dipCreator.setDipOutput(VolumeDipCreator.RADIANS,'DipX','DipY')
     * 
     * # If you want to output also Theta & Phi uncomment this line
     * # default is DEGREES and the file name are 
     * # <path>+<prefix>+'az' and 'dipy'
     * # dipCreator.setAzThetaOutput(VolumeDipCreator.DEGREES,'Azimuth','Theta')
     * 
     * # Parameters
     * # subsamling (nb line,xl, samples)
     * dipCreator.setSubsampling(7,3,13)
     * # Filter: nb of (il,xl,samples)
     * dipCreator.setFilter(1,1,1)
     * 
     * #Run
     * dipCreator.run('/s0/scr/DATA/SEISMIC/bs_fnmig125_g2.vol')
     * 
     * \endcode
     * 
     * 
     * 
     */
    class VolumeDipCreator {
    public:

        /** Interpolation to use if there is no data information */
        enum FillGapMode {
            /** None*/ None,
            /** Linear*/ Linear,
            /** MinCurvature*/ MinCurvature
        };

        /** */
        enum VolumeDipUNIT {
            /** RADIANS*/ RADIANS,
            /** DEGREES*/ DEGREES,
            /** METERS*/ METERS
        };

        vpVolumeDipCreator();


        /** Compute the dips from the seismic/attribute */
        bool run( string volName);

        /**
         * Define the smoothing window size in number of il, xl, z.
         * This this number of traces left+right+center (must be a odd number) 
         * Like in the tornado window.
         * Default: no smoothing (1,1,1)
         */
        void setFilter(int xl, int il, int z);

        /**
         * Introducing a sub-sampling will decrease the time to compute the dips.
         * The Computation is done with this step and re-interpolation 
         * on the fly will be done in Tornado for the display.
         * Default: (2,2,2)
         */
        void setSubsampling(int xl, int il, int z);


        /**
         * Ouptut generated:
         *      <prefix>_dipx and <prefix>_dipy
         * 
         * @brief Dipx and Dipy output.
         * @param name  path and file prefix for output. <prefix>dipx and <prefix>dipy
         */
        void setOutputPath( string path, string prefix = "");


        /**
         * Ouptut generated: <prefix>_dipx and <prefix>_dipy
         * 
         * @brief Dipx and Dipy output.
         * @param name  path and file prefix for output. <prefix>-dipx and <prefix>-dipy
         */
        void setDipOutput(VolumeDipUNIT unit, string dipxName = "", string dipyName = "");

        /**
         * Ouptut generated: <prefix>_az and <prefix>_theta
         * @brief Azimuth & Theta output
         * @param name  path and file prefix for output
         */
        void setAzThetaOutput(VolumeDipUNIT unit, string azName = "", string tetaName = "");


        /** @warning Not available in batch mode */
        void fillGap(FillGapMode);
    };
}


#endif /* VOLUMEDIPCUBE_HPP */

