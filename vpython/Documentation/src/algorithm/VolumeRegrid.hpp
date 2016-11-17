


class base::Geometry;


namespace algorithm {

    enum AngleType;

    
    /**
     * This class supports tornado volume regrid operation (Util->Volume Regrid).
     * The VolumeRegrid state requires a number of parameters to be set, similar to the Volume Regrid dialog in Tornado.
     * 
     * @warning: this feature only supports G2 job and only work with uncompressed FDM files
     * @note: some of functionality is disabled for the purpose of 
     *            Cluster job, eg: regrid only use 4 point linear, value type
     *            only supports value and slowness 
     * @image html VolumeRegrid.png Matching Tornado Volume Regrid parameters and scripting API functions
     * 
     * \code{.py}
     *        
     * from base import *
     * from algorithm import RegridValueType as vt
     * from algorithm import RegridResampleType as st
     * from algorithm import AngleType
     * from algorithm import VolumeRegrid
     * 
     * path='REMOTEFILE!houdat002:/data2/devtest/tornado/yanhliu/Script_test/'
     * 
     * ######## EXAMPLE when loading a geometry from file    
     * ### geom_path=path+'example2.geom' #your geometry file saved by tornado
     * input_vol=path+'example_model_0331.fdm' # your input fdm for regrid
     * output_vol=path+'module_regrid.fdm' #your volume to be saved 
     * 
     * # In this example we get the geometry from an existing attribute 
     * # but just load the geometry (not the data)
     * geomLoaded = Volume().getGeometry(input_vol)
     * 
     * # Print info
     * print "Min Lines:", geomLoaded.getMin()
     * print "Max Lines:", geomLoaded.getMax()
     * print "Line Increment per Grid:", geomLoaded.getInc()
     * print "Logical Origin:" ,geomLoaded.getOrigin()
     * print "Logical increment per Grid:",geomLoaded.getStep()
     * print "Number of grid:",geomLoaded.getSize()

     * # Let's create an attribute with a bigger definition (x 2)
     * seismicSteps = geomLoaded.getInc()
     * seismicSteps[0]=seismicSteps[0]/2
     * seismicSteps[1]=seismicSteps[1]/2
     * # set the new output geometry
     * geomLoaded.setInc(seismicSteps)
     *        
     * #do regrid
     * regrid=VolumeRegrid()
     * regrid.setGeometry(geomLoaded) #set geometry
     * regrid.setInput(input_vol) #set input vol
     * regrid.setOutput(output_vol) #set output vol
     *  
     * # type: VALUE, SLOWNESS, ANGLE
     * # angle type: DegreeSigned, DegreeUnsigned, RadianSigned, RadianUnsigned
     * regrid.setValueType(vt.ANGLE, AngleType.DegreeUnsigned)
     * regrid.setResampleType(st.LINEAR) #BLOCKY,LINEAR,LINEAR_TTS
     * 
     * regrid.regrid() #do regrid
     * 
     * \endcode
     * 
     * @brief Regrid a volume (slowness, angle...)
     */
    class VolumeRegrid {
    public:

        /** YOUR_SPECIFIED_TYPE, otherwise, the script will not run */
        enum RegridValueType {
            VALUE,
            SLOWNESS,
            ANGLE
        };

        /** YOUR_SPECIFIED_TYPE, otherwise, the script will not run */
        enum RegridResampleType {
            BLOCKY,
            LINEAR,
            LINEAR_TTS
        };


        /** @brief path of input volume
         * @note  For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning:: only supports FDM (uncompressed) for this function and only support fixed value replace
         */
        void setInput(string dataPath);

        /** @brief path of output volume
         * @note  For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning:: only supports FDM (uncompressed) for this function and only support fixed value replace
         */
        void setOutput(string dataPath);

        /**
         * Define the input data type to have the better regridding
         * @param regridMode unitless, slowness, angle ...
         * @param angleType DEGREES or RADIANS and range of [-PI,PI] or [0,PI](only available for angle)
         */
        void setValueType(RegridValueType regridMode,
                AngleType angleType=AngleType::RadSigned);
        
        /**
         * Vertical resampling
         */
         void setResampleType(RegridResampleType md);
        
        /**
         * @brief Perform the regrid operation
         */
        bool regrid();

        /**
         * Set the object geometry to be regrided. The geommetry need to be loaded 
         * from file or prepared by using APIs in base.Geometry
         */
        void setGeometry( base::Geometry geom);

    };
}




