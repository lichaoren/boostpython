

namespace base {

    /**
     * Well Log data model.
     * 
     * We encourage to manipulate the well logs as Numpy Arrays with getNPyData() and setNPyData() (more efficient & 
     * more algorithms provided). Therefore another way is to use the python list
     * available from Python core package.
     * 
     * Example to access data
     * \code{.py}
     * from vision import *
     * from base import Well
     * from base import WellLog
     * import numpy as np
     * 
     * well = vision.getWell('TEST_YZ')
     * wellLogDepth = well.getLog('DEPTH')
     * wellLogCali = well.getLog('CALI')
     * 
     * \endcode
     * 
     * 
     * Example to change log values (remove caliper values <0)
     * \code{.py}
     * 
     * caliper = wellLogCali.getData()
     * for i in range(len(caliper)) :
     * if ( caliper[i] < 0 ) :
     *   caliper[i]=0
     * wellLogCali.caliper.setData(caliper)
     * \endcode
     * 
     * 
     * Numpy formula
     * \code{.py}
     * 
     * wellLogRho = well.getNpyLog('RHOB').getNpyData()
     * wellLogSonic = well.getNpyLog('SONIC').getNpyData()
     * wellLogImp = well.addLog('Impedance')
     * 
     * # You can now use complex formula ...
     * wellLogImp.setNpyData( wellLogSonic*wellLogRho )
     * 
     * \endcode
     * 
     * 
     */
    class WellLog {
    public:
        WellLog();

        /** Standard python list access */
        list<float> getData();
         
        /** Standard python list access */
       void setData(list<float> data);

        /** Numpy array access */
        numpy::array getNPyData();
        /** Numpy array access */
        void setNPyData(numpy::array data);

        /** @brief Log name*/
        string getName();
        
    };
} // namespace base

