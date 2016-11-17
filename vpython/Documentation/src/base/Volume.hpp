#ifndef _VOLUME_HPP_
#define _VOLUME_HPP_



namespace base {

    /**
     * An Example extracting the value from a FDM along an horizon and adding the result in a new attribute 'FDM'
     * @example extractAttributeAlongHorizon.py
     * 
     */
    
    
    
        
    /** 
     * Split a volume into sub-volumes (FDM). The size is set by the number of crosslines.
     * 
     * 
     * @note This function is available on cluster (minimize the memory). 
     * Allowing for example to split 1 To in 10 chunks of 100Go.
     * 
     * @param inputName   input name 
     * @param outputName  name without the extension. The results will be concatenated -\<number\>.fdm
     * @param nbSublines     number of sublines for each chunk
     * 
     * 
     * \code
     * from base import Volume
     * input_path ='TANGO!uds:/flat/d337/example.fdm'
     * output_path = 'REMOTEFILE!uds:/flat/d337/dom'
     * 
     * files = chunkVolume(input_path,output_path,20)
     * for filename in files:
     *     print(filename)
     * 
     * \endcode
     * 
     * > result
     * \code
     *      REMOTEFILE!uds:/flat/d337/dom-1.fdm
     *      REMOTEFILE!uds:/flat/d337/dom-2.fdm
     *      REMOTEFILE!uds:/flat/d337/dom-3.fdm
     *      REMOTEFILE!uds:/flat/d337/dom-4.fdm
     * \endcode
     * 
     * 
     */
    list<string> chunkVolume(string inputName, string outputName, int nbSublines);


    /**
     * Split the full volume according to an area of interest (region).
     * 
     * @warning This is only working for a VoxelGeo data and the output can only be a VeoxeGeo cube
     * 
     * @param inputName  origin
     * @param outputName file name with extension ".vol" or ".vol.gz"
     * @param region     area to extract
     * 
     * \code
     * from base import *
     * 
     * region_path =  '/s0/scr/regionEssai.rgn'
     * inp_vol_path = '/s0/scr/bs_fnmig125_g2.vol.gz'
     * out_vol_name = '/s0/scr/subVol.vol.gz'
     * 
     * chunkGeoVoxel(  inp_vol_path, 
     *                 out_vol_path, 
     *                 survey_region )
     * 
     * \endcode
     * 
     */
    bool chunkGeoVoxel(string inputName, string outputName, SurveyRegion region);


    
   /**
    * Create a Volume with given geometry.
    * 
    * @note The newly created volume will save data as FDM
    * 
    * \code{.py}
    * 
    * geomLoaded=Geometry()
    * path="/data2/devtest/tornado/yanhliu/rca/testGeometry.geom"
    * geomLoaded.load(path)
    * 
    * vol=createVolume(geomLoaded)
    * size=vol.getSize()
    * for i in xrange(size[0]):
    *     for j in xrange(size[1]):
    *         for k in xrange(size[2]):
    *           vol.setValue(i,j,k,3.14159)
    *           vol.saveAs("/data2/devtest/tornado/yanhliu/rca/testCreate.fdm")
    * 
    * \endcode
    * 
    * 
    */
   Volume createVolume( Geometry geom);


    /**
     * 
     * Model of a Volume used for Seismic and/or Attribute data.
     * 
     * 
     * # Creating a Volume object:
     * 
     * \code{.py}
     * from base import Volume
     * a = Volume()
     * \endcode
     * Another way to create a Volume is to use @ref createVolume()
     * 
     * 
     * # Some API usage:
     * The following example will scan the whole volume to print the crossline, inline, x, y 
     * (in survey coordinate) and value within range [58,59]
     * 
     * \code{.py}
         from base import Volume 
             
         save_path="/auto/neo/home1/yanhliu/test.txt"
         f=open(save_path,'w')
         path1="/data2/devtest/tornado/yanhliu/rca/"
         fdm=Volume()
         fdm.load(path1+'titusrg_scn10_hgd_dipxsm.fdm')
             
         size=fdm.getSize()
         min_val=58
         max_val=59
         print size
         val=0
         for i in xrange(size[0]):
             for j in xrange(size[1]):
                 for k in xrange(size[2]):
                   val=fdm.getValue(i,j,k)
                   if (val<min_val or val>max_val):
                       continue
                   line=fdm.grd2lin([i,j,k,0])
                   logic=fdm.grd2log([i,j,k,0])
                   f.write("%s %s %s %s %s\\n" %(line[0],line[1],logic[0],logic[1],val))

         f.close()
     * \endcode
     * 
     *          
     * # Built-in Volume math:
     * Support is provided for Volume math with operators: +, -, *, /, and %. 
     * These can be used with two Volumes or a Volume and a number
     * 
     * @warning if data size is larger than real memory size, the result is undefined
     * @note This class supports operations on the tornado fdm and GeoVoxel format data
     * @note There are different formats Tornado can operate on, but the current API only supports:
     * FDM and VoxelGeo format.
     * 
     * \code{.py}
     * from base import Volume
     * a,b = Volume(), Volume() 
     * a.load('a.fdm')
     * b.load('b.fdm')
     * c=(a+b)*(b+3)/b%2 # c is automatically a Volume 
     * c.saveAs("c.fd")    
     * \endcode
     * 
     * # Examples
     * @ref extractAttributeAlongHorizon.py Interesting to see how to use coordinates (grid, log)
     * 
     * 
     * 
     */
    class Volume {
    public:

        /** Create a copy (duplicate memory) of the current volume*/
        Volume clone();

        /** Load from a "store".
         * @param path can be a file (fdm, GeoVoxel, ...)
         * @note For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         *       Only uncompressed FDM is supported for G2 job.
         * 
         */
        int load(string path);

        /** 
         * @note overwrite the original input data, consider  use SaveAs()
         * @warning Only Fdm and GeoVoxel are supported.
         * @brief Save data to file.
         */
        int save();

        /** @brief Save the Volume to a specific file path.
         * \code{.py}
         * from base import Volume
         * a = Volume()
         * a.load('a.fdm')
         * c=(a*1.2) # c is automatically a Volume 
         * c.saveAs("c.fd")    
         * \endcode             
         */
        int saveAs(string path);


        /**
         * 
         * @see Geometry
         */
        base::Geometry getGeometry(string path);

        /** Load a subset of the volume from file specified by 
         * path, and within line limits provided by XLRange,ILRange..
         * 
         * @param path path to file
         * @param il range of IL [min,max]
         * @param xl range of XL [min,max]
         * @return true if success
         * 
         * Example
         * \code{.py}
         * from base import Volume
         * a = Volume()
         * xl = [11000,12000]
         * il = [4500, 5000]
         * a.loadSubset('path_to_volume',xl, il)
         * \endcode
         * 
         * @warning On cluster this function may failed if there is not enough memory
         * 
         * @note the loaded volume will have a modified geometry 
         *   according to provided minimum, increment, and size.
         * @note For G2 job, add 'REMOTEFILE!houdat0XX:' before your 
         *  path for Cluster accessing data island.
         */
        bool loadSubset(string path, int[2] xl, int[2] il);

        /** Load a subset of the volume from file specified by 
         * path, and within the region..
         * 
         * @param path path to file
         * @param region survey region to define subset to load
         * @return true if success
         * 
         * Example
         * \code{.py}
         * from base import Volume
         * region = MultiPolygon2D()
         * region.load('path_to_region')
         * surveyRegion = SurveyRegion()
         * surveyRegion.setPolygonCoordinates(region.polygon(0))
         * a = Volume()
         * a.loadSubsetRegion('path_to_volume',surveyRegion)
         * \endcode
         * 
         * 
         * @note the loaded volume will have a modified geometry 
         *   according to provided minimum, increment, and size.
         * @note For G2 job, add 'REMOTEFILE!houdat0XX:' before your 
         *  path for Cluster accessing data island.
         */
        bool loadSubsetRegion(string path, SurveyRegion region);

        /** @brief Get the step of the geometry for this Volume
         * @return [x,y,z,dummy]
         */
        doube[4] getStep();

        /** @brief Returns [nx,ny,nz,dummy].*/
        int[4] getSize();

        /** @brief Minimum xl,il,z */
        int[4] getMin();

        /** @brief Maximum xl,il,z */
        int[4] getMax();

        /** @brief  Increment xl,il,z*/
        int[4] getInc();

        /** @brief Set the step of the geometry for this Volume.
         * @see Geometry
         */
        void setStep(doube[4] step);

        /** @brief Set the size of this Volume's geometry.*/
        void setSize(int[4] size);

        void setMin(int[4] min);

        void setMax(int[4] max);

        void setInc(int[4] inc);

        /** @brief Transform grid to X/Y coordinate.
         * @param vect grid location [xlIdx,ilIdx,depthIdx,0]
         * @return survey X/Y coordinates at given grid position
         * @see Geometry
         */
        doube[4] grd2log(doube[4] vect);

        /** @brief Transform grid to inline/crossline coordinate.
         * @param vect grid location [xlIdx,ilIdx,depthIdx,0]
         * @return survey il,xl location at given grid position
         * @see Geometry
         * 
         */
        doube[4] grd2lin(doube[4] vect);

        /** @brief Transform inline/crossline to grid coordinate.*/
        doube[4] lin2grd(doube[4] vect);
        /** @brief Transform crossline/inline to X/Y survey coordinate.*/
        doube[4] lin2log(doube[4] vect);

        /** @brief Transform X/Y survey to grid coordinate.*/
        doube[4] log2grd(doube[4] vect);
        /** @brief Transform X/Y survey coordinates to crossline/inline coordinate.*/
        doube[4] log2lin(doube[4] vect);

        /** @brief Transform X/Y survey to World wide X/Y.*/
        doube[4] log2wld(doube[4] vect);

        /** @brief Transform X/Y world to X/Y survey coordinate.*/
        doube[4] wld2log(doube[4] vect);

        /** @brief Origin of the geometry for this Volume.
         * @return returns [x,y,z,dummy]
         */
        doube[4] getOrigin();

        /** @brief Set the origin of the geometry for this Volume.
         * @param origin [x,y,z,dummy]
         */
        void setOrigin(doube[4] origin);


        /** @brief Value for X/Y survey coordinate.*/
        double getValue(doube[4] pos_log);
        /** @brief Value for grid position.*/
        double getValue(int[4] pos_vol);
        /** @brief  Value for grid position.*/
        double getValue(int vx, int vy, int vz);

        /** @brief Set value for X/Y survey coordinate.*/
        void setValue(doube[4] pos_log, double value);
        /** @brief Set value for grid position.*/
        void setValue(int[4] pos_vol, double value);
        /** @brief Set value for grid position.*/
        void setValue(int vx, int vy,
                int vz, double val);

        /**
         * Get trace data array at given grid  position
         * First trace is (0,0)
         * @param xlIdx crossline grid index 
         * @param ilIdx inline grid index
         * @note This method is much more faster rather than reading the full
         *  trace with getValue(i,j,k)
         */
        list<float> getTrace(int xlIdx, int ilIdx);

        /**
         * Set trace data array at given grid  position
         * First trace is (0,0)
         * @param xlIdx crossline grid index 
         * @param ilIdx inline grid index
         * @param data array (same size as the full trace length)
         * @note This method is much more faster rather than reading the full
         *  trace with getValue(i,j,k)
         */
        bool setTrace(int il, int xl, list<float> data);


        /** Get type of the attribute (SEMBLANCE, VINT...) defined in the storage.*/
        string getAttributeType();

        /** 
         * Set type of the attribute (SEMBLANCE, VINT...).
         * Useful to load an attribute with a defined colormap in Tornado OR for batch modules using this parameter.
         */
        void setAttributeType(string attr);

        /** @brief Add two volumes.*/
        Volume operator=(Volume);
        /** @brief Add volume and a constant.*/
        Volume operator=(float);
        /** @brief subtract two volumes.*/
        Volume operator-(Volume);
        /** @brief subtract volume and a constant.*/
        Volume operator-(float);
        /** @brief divide two volumes.*/
        Volume operator/(Volume);
        /** @brief divide volume and a constant.*/
        Volume operator/(float);
        /** @brief Multiply two volumes.*/
        Volume operator*(Volume);
        /** @brief Multiply volume and a constant.*/
        Volume operator*(float);

        // function computes the floating-point remainder of dividing x by y.  The return value is x - n * y, where n is the quotient of x / y, rounded toward zero to an integer
        /** @brief Modulo of two volumes.*/
        Volume operator%(Volume);
        /** @brief Modulo between a volume and a constant.*/
        Volume operator%(float);

    };

} // namespace base


#endif  // _VOLUME_HPP_
