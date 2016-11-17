
#ifndef CONVERTHRZDOMAIN_HPP
#define CONVERTHRZDOMAIN_HPP

namespace algorithm {


    /**
     * @brief Horizon Domain Converting
     * 
     * 
     * In Tornado (Main Window: Horizon -> Update Horizon Attributes -> Horizon Domain Converting)
     * @image html ConvertHrzDomain.png
     * 
     * Below is an example python script:
     * \code{.py}
        from algorithm import ConvertType as ct
        from algorithm import VelocityOpt as vo
        from algorithm import ConvertHrzDomain

        hg = '/data4/dtrain01/Tornado_p2/hrz/T2_tos1_wip_surf.hrz'
        vel = '/data2/devtest/tornado/ruwu/tmp.fdm'
        outputPath = '/data2/devtest/tornado/ruwu/script/tmp.hrz'

        #setup
        convertHrzDomain = ConvertHrzDomain()
        convertHrzDomain.setInputHorizonPath(hg)
        convertHrzDomain.setVelocityModelPath(vel)
        convertHrzDomain.setConvertType(ct.DEPTH_TO_TIME) #or ct.TIME_TO_DEPTH, ct.DEPTH_TO_DEPTH, ct.TIME_SHIFT
        convertHrzDomain.setOutputPath(outputPath)
        convertHrzDomain.setVelocityOpt(vo.BLOCKY) # or vo.LINEAR

        # to specify the updated velocity model, use: 
        #update_vel = '/data2/devtest/tornado/ruwu/tmp2.fdm'
        #convertHrzDomain.setUpdatedVelocityModelPath(update_vel)

        # to set the shift volume, use:
        #convertHrzDomain.setShiftVolume(vel)

        #execute
        convertHrzDomain.exe()
     * \endcode
     * 
     */
    

    class ConvertHrzDomain {
    public:

        enum ConvertType {
            DEPTH_TO_TIME,
            TIME_TO_DEPTH,
            DEPTH_TO_DEPTH,
            TIME_SHIFT
        };

        enum VelocityOpt {
            BLOCKY,
            LINEAR
        };
                
        /** @brief set the convert option (default value: DEPTH_TO_TIME)*/
        void setConvertType( ConvertType type);
        /** @brief set the velocity option (default value: BLOCKY)*/
        void setVelocityOpt( VelocityOpt option );
        /** @brief set the path of input horizon group*/
        void setInputHorizonPath( string path);
        /** @brief set the path of shift volume*/
        void setShiftVolume( string myVelPath);
        /** @brief set the path of velocity model*/
        void setVelocityModelPath( string myVelPath);
        /** @brief set the path of updated velocity model*/
        void setUpdatedVelocityModelPath( string myUpdatedVelPath);

        /** @brief start the convert process*/
        void exe();
        /** @brief set the path of output horizon group*/
        void setOutputPath( string myOutputPath);

    };

}

#endif /* CONVERTHRZDOMAIN_HPP */

