#ifndef VPVOLREGRIDAPI_HPP
#define	VPVOLREGRIDAPI_HPP
#include "vmVolRegridModel.hpp"
#include "vlBaseGeometry.hpp"



/**
 * his class supports tornado volume regrid operation (Util->Volume Regrid).
 * The VolumeRegrid state requires a number of parameters to be set, similar to the Volume Regrid dialog in Tornado.
 * 
 * @warning: this feature only supports G2 job and only work with uncompressed FDM files
 * @note: some of functionality is disabled for the purpose of 
 *            Cluster job, eg: regrid only use 4 point linear, value type
 *            only supports value and slowness 
 * @image html VolumeReplace_fig1.png Matching Tornado Volume Regrid parameters and scripting API functions
 * 
 * \code
 *        
 * from base import *
 * from algorithm import RegridValueType as vt
 * from algorithm import RegridOutputType as ot
 * from algorithm import RegridResampleType as st
 * from algorithm import AngleType
 * from algorithm import VolumeRegrid
 * 
 * path='REMOTEFILE!houdat002:/data2/devtest/tornado/yanhliu/Script_test/'   
 * geom_path=path+'example2.geom' #your geometry file saved by tornado
 * input_vol=path+'example_model_0331.fdm' # your input fdm for regrid
 * output_vol=path+'module_regrid.fdm' #your volume to be saved 
 * 
 * #load geometry and print geometry info
 * geomLoaded=Geometry()
 * geomLoaded.load(geom_path)
 * print "Min Lines:", geomLoaded.getMin()
 * print "Max Lines:", geomLoaded.getMax()
 * print "Line Increment per Grid:", geomLoaded.getInc()
 * print "Logical Origin:" ,geomLoaded.getOrigin()
 * print "Logical increment per Grid:",geomLoaded.getStep()
 * print "Number of grid:",geomLoaded.getSize()
 *        
 * #do regrid
 * regrid=VolumeRegrid()
 * regrid.setGeometry(geomLoaded) #set geometry
 * regrid.setValueType(vt.SLOWNESS) #VALUE, SLOWNESS
 * regrid.setResampleType(st.LINEAR) #BLOCKY,LINEAR,LINEAR_TTS
 * regrid.useLine(True) # True is use line, False use xy coordinate
 * regrid.setInput(input_vol) #set input vol
 * regrid.setOutput(output_vol) #set output vol
 * regrid.regrid() #do regrid
       
 * 
 * \endcode
 * 
 * @brief Regrid a volume (slowness, angle...)
 */
class vpVolumeRegrid : public vmVolRegridModel {
public:

// TODO: no implemented!
    enum RInterpMode {
        RLINEAR = 0,
        RNATURAL_NEIGHBOR = 1,
        RMINIMUM_CURVATURE = 2,
#ifdef USE_ALSO_GPL_DELTA
        RLINEAR_DELTA = 3,
#endif
        RNEAREST_NEIGHBOR = 4,
        R_DEFINE=5
    };
    
    /** Expose Methods to Python */
    static void exposePython();
    
    vpVolumeRegrid();
    virtual ~vpVolumeRegrid();

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
     * @param angleType  (only available for angle)
     */
    void setValueType( ValType regridMode, 
            AngleConv::Type angleType=AngleConv::RadSigned);

    /**
     * @brief Perform the regrid operation
     */
    bool regrid();
    
    /**
     * Set the object geometry to be regrided. The geommetry need to be loaded 
     * from file or prepared by using APIs in base.Geometry
     */
    void setGeometry(const BaseGeometry& geom){
        objGeom=geom;
    };

private:
    bool myOnDiskRegrid(BaseGeometry& inputGeom);
    bool myOnDiskRegridHelper(const GeomData& gd, const GeomData& gd_old,
        const BaseGeometry& outputGeom);
    BaseGeometry objGeom;
    string myInputFile;
    string myOutputFile;
};

#endif	/* VPVOLREGRIDAPI_HPP */


