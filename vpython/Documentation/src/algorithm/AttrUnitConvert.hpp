
#ifndef ATTRUNITCONVERT_HPP
#define ATTRUNITCONVERT_HPP


namespace algorithm {

    enum LengthUnit {
        METERS,
        FEET,
        /// international feet
        INTL_FEET
    };
    
    
    /**
     * Returns a constant multiplier value based on input unit and output unit. 
     * This value then can be used to convert all data from *input_unit* to *output_unit* 
     * by simple multiplication.
     * 
     * This function is used to perform unit conversion by working together with other APIs.
     * 
     * Example: convert the z axis from meter to feet 
     * \code{.py}
       from base import *
       from algorithm import convertUnit, LengthUnit
       #load fdm
       path1="/data2/devtest/tornado/yanhliu/rca/"
       fdm=Volume()
       fdm.load(path1+'titusrg_scn10_hgd_dipxsm.fdm')

       origin=fdm.getOrigin()
       step=fdm.getStep()
       print "Origin:",fdm.getOrigin()
       print "Step:",fdm.getStep()

       m2f=convertUnit(LengthUnit.METER, LengthUnit.FEET)
       print "Meter to feet constant:", m2f

       #only convert the z component
       origin[2]=origin[2]*m2f
       step[2]=step[2]*m2f
       print "Converting the z component of geometry:"


       print "Converting the data"
       fdm=fdm*m2f
       fdm.setOrigin(origin)
       fdm.setStep(step)
       print "New Origin:",fdm.getOrigin()
       print "New Step:", fdm.getStep()
       path2="/data2/devtest/tornado/yanhliu/Script_test/"
       fdm.saveAs(path2+'attrUnitConvert.fdm')
     * \endcode
     * 
     * 
     * @param input_unit
     * @param output_unit
     * @return 
     */
    float convertUnit(LengthUnit input_unit, LengthUnit output_unit);
    
    /**
     * @brief Attribute unit conversions between meter, feet and international feet
     * @note it supports both the interactive/g2 job with different API.
     * @warning do not use compress FDM (fdm.gz) as input for moduleConvert
     * @see convertUnit
     * 
     * In Tornado (Main Window: Attribute -> Attribute Create > Attribute Unit Conversion )
     * @image html AttrUnitConvert.png
     * 
     * \code{.py}
       from base import *
       from algorithm import LengthUnit
       from algorithm import AttrUnitConvert as AC
       
       #load data
       path='REMOTEFILE!houdat002:/data2/devtest/tornado/test_scripting/'
       
       input_data='xp_M_a2_regrid.fdm'
       output_data='xp_M_a2_regrid_inch.fdm'
       #setup converter
       converter=AC()
       converter.setup(LengthUnit.METER, LengthUnit.FEET, 'zv')
       converter.moduleConvert(path+input_data, path+output_data)
     * \endcode
     * 
     */
    class AttrUnitConvert {
    public:
        
        /**
         * @brief Set converting condtion
         * @param input    your input fdm unit
         * @param output   your output fdm unit
         * @param toConvert  setup xyzv for option, xyz is the axis, v is the value, you can setup any combinations
         */
        void setup(LengthUnit input, LengthUnit output, string toConvert);
        

        /**
         * @brief g2 version convert for running on Cluster torpy job
         * @param input  the path to fdm you want to convert 
         * @param output the path to fdm you want to save as
         * @note For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning the input data must not be zipped
         */
        void moduleConvert(string input, string output);
        
        /**
         * @brief convert the volume
         */
         void convert(Volume vol);
   };

}

#endif /* ATTRUNITCONVERT_HPP */

