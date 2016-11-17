
namespace algorithm {




    /**
     * Dips can be computed from different types (theta,phi),(dipx,dipy) or (Cx,Cy)
     * and with different units (radians, degrees, meters). The aim of this converter
     * is to do the conversion from/to this different types.
     *  @brief Convert dips units (Attribute)
     * 
     * @image html VolumeDipConverter.png Tornado dialog
     * 
     * ## Example with torpy
     * 
     * \code
     * from algorithm import VolumeDipConverter
     * 
     * #input
     * dipx_file='REMOTEFILE!hou202ws:/data/dipx.fdm'
     * dipy_file='REMOTEFILE!hou202ws:/data/dipy.fdm'
     * 
     * #output
     * theta_file='REMOTEFILE!hou202ws:/data/theta.fdm'
     * phi_file='REMOTEFILE!hou202ws:/data/phi.fdm'
     * 
     * converter = VolumeDipConverter()
     * converter.setInput( dipx_file, dipy_file)
     * converter.setOutput(theta_file, phi_file)
     * 
     * # Type: THETA_PHI, DIPX_DIPY, NORMALIZED_UTM
     * # Unit: RADIANS, DEGREES, METERS, UNITLESS
     * converter.run(VolumeDipConverter.DIPX_DIPY,  VolumeDipConverter.DEGREES, 
     *                   VolumeDipConverter.THETA_PHI , VolumeDipConverter.DEGREES)
     * 
     * \endcode
     * 
     * ## Example in interactive
     * 
     * \code
     * 
     * 
     * #input
     * inputA = '/s0/scr/DATA/legacy_dipx.fdm'
     * inputB = '/s0/scr/DATA/legacy_dipy.fdm'
     * 
     * #output
     * outputA  ='/s0/scr/DATA/dipConvertA1.fdm_p'
     * outputB  ='/s0/scr/DATA/dipConvertB1.fdm_p'
     * 
     * converter.run(VolumeDipConverter.DIPX_DIPY,  VolumeDipConverter.DEGREES, 
     *                   VolumeDipConverter.THETA_PHI , VolumeDipConverter.DEGREES)
     * 
     * # Make it visible in tornado
     * vision.addAttribute( converter.getResultA() )
     * vision.addAttribute( converter.getResultB() )
     * 
     * \endcode
     * 
     * 
     * 
     */
    class VolumeDipConverter {
    public:

        /** Type of attribute (in/out)*/
        enum VolumeDipType {
            THETA_PHI,
            DIPX_DIPY,
            NORMALIZED_UTM
        };

        /** Unit of the attribute (in/out)*/
        enum VolumeDipUNIT {
            RADIANS,
            DEGREES,
            METERS,
            UNITLESS
        };



        VolumeDipConverter();

        /** Convert From input */
        bool run(VolumeDipType inputType, VolumeDipUNIT inputUnit,
                VolumeDipType outputType, VolumeDipUNIT outputUnit);


        /** If attributes are not loaded (batch mode) */
        void setInput(const vString& pathA, const vString& pathB);

        /** If volume already loaded (avoid in batch mode)*/
        void setInput(Volume* volA, Volume* volB);


        /** Path of the output volumes */
        void setOutput(string pathA, string pathB);

        /** Geometry from file or remote storages when transforming from/to Cx,Cy */
        void setExternalGeometry(string path);


        /** After conversion get the converted attribute (interactive mode only). 
         * @warning this is only available in interactive mode and no chunk is needed */
        Volume getOutputA();

        /** After conversion get the converted attribute (interactive mode only). 
         * @warning this is only available in interactive mode and no chunk is needed */
        Volume getOutputB();
    };




}


