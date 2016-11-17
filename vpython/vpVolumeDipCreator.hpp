/* 
 * File:   vpVolumeDipCube.hpp
 * Author: Dominique DOLEDEC
 *
 * Created on June 1, 2016, 12:42 PM
 */

#ifndef VPVOLUMEDIPCUBE_HPP
#define VPVOLUMEDIPCUBE_HPP
#include <string>

#include "vString.hpp"


class BaseGeometry;
class vpVolumeProxy;


class vpVolumeDipCreator {
public:
    
    /** Interpolation to use if there is no data information */
    enum FillGapMode {
        /** None*/ None,
        /** Linear*/ Linear,
        /** MinCurvature*/ MinCurvature
    };
    
    /** */
     enum VolumeDipUNIT {
         /** No otuput*/ NONE, 
         /** RADIANS*/ RADIANS, 
         /** DEGREES*/ DEGREES, 
         /** METERS*/  METERS
    };   
    
    
    
    vpVolumeDipCreator();
    virtual ~vpVolumeDipCreator();
    
      /** Expose Methods to Python */
    static void exposePython();  
    
    /** Compute the dips from the seismic/attribute */
    bool run(const std::string& volName );

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
    void setSubsamplingStep(int xl, int il, int z);

    void setNumVolSize(float num);

       
    /**
     * Ouptut generated:
     *      <prefix>_dipx and <prefix>_dipy
     * 
     * @brief Dipx and Dipy output.
     * @param name  path and file prefix for output. <prefix>-dipx and <prefix>-dipy
     */
    void setOutputPath(const std::string& path, std::string prefix = std::string());
    
    
    /**
     * Ouptut generated: <prefix>_dipx and <prefix>_dipy
     * 
     * @brief Dipx and Dipy output.
     * @param name  path and file prefix for output. <prefix>-dipx and <prefix>-dipy
     */
    void setDipOutput(VolumeDipUNIT unit, std::string dipxName = std::string(),
                      std::string dipyName = std::string());
    
    /**
     * Ouptut generated: <prefix>_az and <prefix>_theta
     * @brief Azimuth & Theta output
     * @param name  path and file prefix for output
     */
    void setAzThetaOutput(VolumeDipUNIT unit, std::string azName=std::string(),
                          std::string tetaName=std::string() );
    
    
    /** Not available in batch mode */
    void fillGap(FillGapMode);
    
protected:
    vString getDipXFileName() const;
    vString getDipYFileName() const;
    vString getAzFileName() const;
    vString getTetaFileName() const;
    
    /** Initialize the given input volume proxy using the current settings. */
    void myInitInputVolume(vpVolumeProxy& input, const std::string& volName) const;

    /** Initialize a geometry to match the input proxy after sub-sampling. */
    void myInitSubSampleGeometry(BaseGeometry& geom,
            const vpVolumeProxy& input) const;

    /** Initialize the given output volume proxy to match the input proxy. */
    void myInitOutputVolume(vpVolumeProxy& output,
            const vpVolumeProxy& input) const;

private:
    float myChunkSize; 

    // Process Parameters
    vString myOutputPath;    
    vString myOutputPrefix;    
    
    vString myOutputFileX;
    vString myOutputFileY;
 
    vString myOutputAzm;
    vString myOutputTheta;
    
    bool myHorFilter;

    VolumeDipUNIT myDipUnit;
    VolumeDipUNIT myAzThetaUnit;
    
    FillGapMode myFillGapMode;
              
    int mySmoothSize[3];
    int mySubsampling[3];        
};

#endif /* VPVOLUMEDIPCUBE_HPP */

