


class base::Volume;
class base::Horizon;

namespace algorithm {

    using namespace base;

    /**
     * @example testVolumeSmooth.py
     */
    
    
    /**
     * This class supports tornado smoothing (Attribute->Update->Attribute Smooth...)
     * 
     * @note Use runChunk() on cluster machines or run when you are sure to have enough memory
     * 
     * # Examples
     * @ref testVolumeSmooth.py
     * 
     * 
     * ## Snippet: 
     * 
     * \code
     * from base import Volume
     * from algorithm import VolumeSmooth

      # PARAMETERS (requested by user) : input attribute
      projectPath=????? # i.e: "/data2/devtest/tornado/test_scripting/"
      fdm_name=????? # i.e: "titusrg_test_start_0922.fdm_p"

      # Initialize parameters for smoothing
      volSmoother = VolumeSmooth()

      # The Type of data is an important parameter
      # i.e: use VELOCITY to smooth in slowness
      # Choices: MISC, VELOCITY
      volSmoother.setDataType(VolumeSmooth.VELOCITY)

      # Set the interpolation method
      # Choices: MEAN,MEDIAN,ALPHA_TRIM,B_SPLINE,MEAN_EXCLUDE,COMPRESS,MEAN_DVK,
      #          MEAN_DIRECTION,MEAN_SINGLE_FAULT,MEAN_MULTI_FAULTS,APPROX_MEDIAN
      volSmoother.setMethod(VolumeSmooth.MEAN)

      # Window size in meter/depth unit
      volSmoother.setSmoothWindow(100,100,50)

      # Run the smooth and save it as FDM with "smooth_" prefix
      volSmoother.runChunk(projectPath+fdm_name,projectPath+'smooth_'+fdm_name)
     * 
     * \endcode
     * 
     * @brief Attribute Smoothing
     * 
     */
    class VolumeSmooth {
    public:

        /** A couple of methods are available for smoothing        
         * @see setMethod() 
         */
        enum SmoothMethod {
            MEAN,
            MEDIAN,
            ALPHA_TRIM,
            B_SPLINE,
            MEAN_EXCLUDE,
            COMPRESS,
            MEAN_DVK,
            MEAN_DIRECTION,
            MEAN_SINGLE_FAULT,
            MEAN_MULTI_FAULTS,
            APPROX_MEDIAN
        };

        /** Smooth the velocity in slowness
         * @see setDataType() 
         */
        enum SmoothDataType {
            MISC,
            VELOCITY
        };

        /** For isotropic or anisotropic smoothing  
         * @see setDataMedium() 
         */
        enum SmoothDataMedium {
            ISOTROPIC,
            VTI,
            TTI
        };


        VolumeSmooth();

        /** 
         * @brief Run smooth when volume can fit into memory
         */
        bool run(Volume vol);


        /** 
         * Smooth the Volume; but this method will not load the full cube in memory
         * (divide the volume into chunk). This solution need to be used when running
         *  the process on a cluster machine.
         * 
         * @note For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning only supports FDM (uncompressed) files
         * @brief Run smooth and chunk (avoid to load full cube in memory) 
         */
        bool runChunk(string inputAttrName, string outputAttrName);

        /** @brief Interpolation Method
         * @param method xxxx
         */
        void setMethod(SmoothMethod opt);

        /** @brief fast mean smoothing (method must be set to MEAN) */
        void setFastMethod(bool flag);

        /** @brief use double precision math for fast mean smoothing */
        void setDoublePrecision(bool flag);

        /** @brief set precision for Fast Median algorithm */
        void setMedianPrecision(float p);

        /** @brief Velocity to smooth in slowness*/
        void setDataType(SmoothDataType type);

        /** @brief For anisotropic modelISOTROPIC, VTI, TTI */
        void setDataMedium(SmoothDataMedium mode);

        //  ppend a list of windows to the queue for run() or runChunk()
        void setSmoothWindow(list lst);

        /** 
         * @brief Size of the smoothing window
         * @note clear the windows set by list setSmoothWindow(list)
         * @deprecated  
         */
        void setSmoothWindow(double rx, double ry, double rz);

        /*  
         * Smooth with diff radius(x,y,z) function of depth.
         *  REMARK: The radius unit is meters,feet..
         */
        /**
         * Define different smoothing parameters according to the depth
         * @param param list of [depth, radiusX, radiusY, radiusZ, interpolate(0=false,1=true)]
         * @param hrz form where you want to apply the smooth (if not provided origin z=0 is used)
         * 
         * \code
         * # order: depth, radiusX, radiusY, radiusZ, interpolate(0=false,1=true)
         * # units are related to the domain (meter,feet...)
         * depthVarying=[
         *      [0,    10, 10, 2, 1],
         *      [3000, 50, 50, 5, 1],
         *      [6000, 100, 100, 10, 1],
         *     [9000, 200, 200, 15, 1]
         *     ]
         * volSmoother.setDepthVarying(depthVarying)
         * 
         * # OR along a horizon
         * volSmoother.setDepthVarying(depthVarying, originhrz)
         * 
         * \endcode
         */
        void setDepthVarying(list<(depth, rx, ry, rz, interpolate) > params, Horizon hrz = 0);


        /**@brief value for alpha smooth    */
        void setAlpha(int a);

    };

}
