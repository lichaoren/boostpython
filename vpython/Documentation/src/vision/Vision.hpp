#ifndef VPVISIONAPI_HPP
#define	VPVISIONAPI_HPP


class base::Horizon;
class base::HorizonGroup;
class base::Volume;
class base::Well;

namespace vision {

    using namespace base;

    /**
     * This script show how to create an image for each horizon found in a 
     * directory.
     *
     * @example imagesForAllHrzInADirectory.py
     *
     */

    
    
    
    /**
     * This class support communications between python and Tornado UI.
     * 
     * The example below shows how to use Vision module to get the current 
     * attribute, seismic and horizon to modify there value under Tornado GUI mode
     * \code{.py}
     * 
     * from base import *
     * from Vision import * # warning, do not run with -script, will crash
     * 
     * #use vision package to get the current working horizon
     * hrz=vision.getCurrentHorizon()
     * 
     * #use Horizon Interface to do update
     * size=hrz.getSize()
     * 
     * for i in range(size[0]):
     *     for j in range(size[1]):
     *         # return two, found is boolean, val is value for Depth
     *         # if found value, val will be updated
     *         val=hrz.getValue(i,j,'DEPTH')
     *         #only update those without values
     *         if ( val!=None):
     *             hrz.setValue(i,j,'DEPTH',200)
     * 
     * 
     * # now test the current attribute
     * attr=vision.getCurrentAttribute()
     * size=attr.getSize()
     * for i in range(size[0]/2,size[0]):
     *     for j in range(size[1]/2, size[1]):
     *         for k in range(size[2]/2,size[2]):
     *             val=attr.getValue(i,j,k)
     *             attr.setValue(i,j,k,val/2)

     * # now test the current seismic
     * seis=vision.getCurrentSeismic()
     * size=seis.getSize()
     * for i in range(size[0]/2,size[0]):
     *     for j in range(size[1]/2, size[1]):
     *         for k in range(size[2]/2,size[2]):
     *             val=seis.getValue(i,j,k)
     *             seis.setValue(i,j,k,val*2)
     * 
     * \endcode
     * 
     * This example to show how to use cursor input for your python script. It will replace a small cubic area with a specifi value. The 3D region is defined by cursor input.
     * - 1. Set Cursor XYZ for Script Arguments, not user input
     * - 2. mouse click
     * - 3. if you are happy with the positon, click Run button to run the script; otherwise back to step 2
     * this example 
     * \code{.py}
     * 
     * from base import *
     * from vision import *

     * vol = vision.getCurrentAttribute()
     * pos =vol.log2grd(vision.getCursorLog())
     * print pos
     * v = vol.getValue(int(pos[0]),int(pos[1]),int(pos[2]))
     * print v
     * vol.setValue(int(pos[0]), int(pos[1]), int(pos[2]), 50)
     * v = vol.getValue(int(pos[0]),int(pos[1]),int(pos[2]))
     * print v
     * 
     * x_len = 5
     * y_len = 10
     * z_len = 8
     * 
     * for x in range( x_len)
     *    for y in range(y_len)
     *       for z in range(z_len)
     *              vol.setValue(int(pos[0])+i, int(pos[1])+y, int(pos[2])+z, 50)
     * 
     * \endcode
     * 
     * 
     * @note 
     * A more practical way is to check if you have got a None object due to your typo
     * 
     */
    class Vision {
    public:

        /** @brief Add Attribute in Tornado (new data).
         @param name name of the new data in tornado */
        void addAttribute(Volume vol, std::string name = "");
        /** @brief Add Horizon Group in Tornado (new data)  
         * @param name name of the new data in tornado */
        void addHorizonGroup(HorizonGroup hg);

        /** @brief Current displayed attribute
         * @return if does not exist, returns None
         */
        Volume getCurrentAttribute();
        /** @brief Current displayed Seismic
         * @return if does not exist, returns None
         */
        Volume getCurrentSeismic();
        /** @brief Current displayed Horizon
         * @return if does not exist, returns None
         */
        vorizon getCurrentHorizon();
        /** @brief Current displayed HorizonGroup         
         * @return if does not exist, returns None
         */
        HorizonGroup getCurrentHorizonGroup();
        
        /** Change current displayed horizon.
         * 
         * @param name either the name (filename without extension) OR the full path
         * 
         * @ref imagesForAllHrzInADirectory.py
         * 
         */
        bool setCurrentHorizonGroup( string name);
   
        /** Remove the horizon group from tornado
         * @param name either the name (filename without extension) OR the full path
         */
        bool removeHorizonGroup( string nane);
        
        /** @brief Change current displayed horizon */
        bool setCurrentHorizon(string hName);

        /** @brief get the Attribute listed in data manager by name.
         * @return if does not exist, returns None
         */
        Volume getAttribute(string attr);
        /** @brief get the Seismic listed in data manager by name.
         * @return if does not exist, returns None
         */
        Volume getSeismic(string seis);
        /** @brief get the Gather listed in data manager by name.
         * @return if does not exist, returns None
         */
        Volume getGather(string name);
        /** @brief get the HorizonGroup listed in data manager by name.
         * @return if does not exist, returns None
         */
        HorizonGroup getHorizonGroup(string hg);
        /** @brief get the Well listed in data manager by name.
         * @return if does not exist, returns None
         */
        Well getWell(string hg);
        

        /** Change the limits and palette
         * @param dataType "HORIZON", "ATTIRBUTE", "SEISMIC","GATHER"
         * @param palette name as defined in tornado
         */
        void setCurrentSpectrum( string dataType, float min, float max, string palette = 0);        

        /** @brief Get X,Y,Z coordinate for UI*/
        double[4] getCursorLog();
        /** @brief Get Inline,Crossline, Depth coordinate for UI*/
        int[2] getCursorLine();

        /** @brief */
        double[3] getUserArg();
        
        /** @brief Refresh the display */
        void refresh();
    };
}

#endif /* VPVISIONAPI_HPP */

