
#ifndef ATTRREPLACE_HPP
#define ATTRREPLACE_HPP


namespace algorithm {


    /**
     * @brief Replace value(s) inside a Volume
     * @note doReplaceFromModule should be used for g2 module script 
     * 
     * In Tornado (Attribute->Update->Replace)
     * @image html AttributeReplace.png
     * 
     * 
     * Below is an example job to do attribute replace with normal value and angle value:
     * \code{.py}
       from base import *
       from algorithm import ReplaceMode as mode
       from algorithm import ReplaceRangeType as tp
       from algorithm import AttrReplace
       from algorithm import AngleType
       
       path="/data2/devtest/tornado/rca/"
       data=Volume()
       data.load(path+'titusrg_test_start_0922.fdm_p')
       
       replacer=AttrReplace()
       replacer.replaceInf(True)
       replacer.replaceNAN(True)
       replacer.setReplaceType(mode.NEAREST_NEIGHBOR)
       replacer.setRange(tp.MinMax, 1000.,3000.)
       replacer.setAttribute(data)
       replacer.doReplace()
       data.saveAs(path+'testReplace.fdm')
       
       
       ######################################
       #now test the angle conversion
       
       data2_path=path+'titusrg_scn10_hgd_dipxsm.fdm'
       data2=Volume()
       data2.load(data2_path)
       replacer.setAngleType(AngleType.DegreeSigned)
       replacer.setRange(tp.MinMax,-10,10.)
       replacer.setAttribute(data2)
       replacer.setReplaceType(mode.LINEAR)
       replacer.doReplace()
       data2.saveAs("/data2/devtest/tornado/Script_test/testAngleReplace.fdm")

     * \endcode
     * 
     * 
     * 
     * Below is an example job to do attribute replace with region:
     * \code{.py}
     * 
       from base import *
       from base.region import *
       from algorithm import ReplaceMode as mode
       from algorithm import ReplaceRangeType as tp
       from algorithm import ReplaceRegionType as rt
       from algorithm import AttrReplace
       
       
       #load multiple regions at one run
       aRegion=MultiPolygon2D()
       aRegion.load("/data2/devtest/tornado/Script_test/testRgn2.rgn")
       npoly=aRegion.numPolygons()
       print "number of regions: ",npoly
              
       #load the data
       data=Volume()
       data.load('/data2/devtest/tornado/test_fdm.fdm')
       
       # get the replacer
       replacer=AttrReplace()
       replacer.replaceInf(True)
       replacer.replaceNAN(True)
       replacer.setReplaceType(mode.FIXED_VALUE, 0.)
       replacer.setRange(tp.MinMax, 0.,5000.)
       replacer.setAttribute(data)
       
       for poly in range(npoly):
            aSurveyRegion = SurveyRegion()
            aSurveyRegion.setPolygonCoordinates(aRegion.polygon(poly))
            replacer.setRegion(aSurveyRegion, rt.Current)
            replacer.doReplace()

       data.saveAs('/data2/devtest/tornado/test_fdm2.fdm')
     * \endcode
     * 
     * 
     * 
     * Below is an example job to setup the script for torpy to run:
     * \code{.py} 
       from base import Volume
       from algorithm import ReplaceMode as mode
       from algorithm import ReplaceRangeType as tp
       from algorithm import AttrReplace

       #test fdm
       input_path="REMOTEFILE!houdat002:/data2/devtest/tornado/yanhliu/voo/example_model_0331_regrid.fdm"
       output_path="REMOTEFILE!houdat002:/data2/devtest/tornado/yanhliu/voo/YLINE_example_model_0331.fdm"
       replacer=AttrReplace()
       replacer.replaceInf(True)
       replacer.replaceNAN(True)
       replacer.setReplaceType(mode.FIXED_VALUE, 0.)
       replacer.setRange(tp.MinMax, 1000.,3000.)
       replacer.doReplaceFromModule(input_path,output_path,10)
     * \endcode
     * 
     * 
     */


    //            class_<vpAttrReplaceAPI>("AttrReplace")
    //            .def("replaceInf", &vpAttrReplaceAPI::replaceInf)
    //            .def("replaceNAN", &vpAttrReplaceAPI::replaceNAN)
    //            .def("setRange", &vpAttrReplaceAPI::setRange, vpAttrReplaceAPI_Rangeoverloads())
    //            .def("setReplaceType", &vpAttrReplaceAPI::setReplaceType, vpAttrReplaceAPI_Typeoverloads())
    //            .def("doReplace", &vpAttrReplaceAPI::doReplace)
    //            .def("setAttribute", &vpAttrReplaceAPI::setVol)
    //            .def("setAngleType", &vpAttrReplaceAPI::setAngleType)
    //            .def("setRegion", &vpAttrReplaceAPI::setRegion)
    //            .def("clearRegion", &vpAttrReplaceAPI::clearRegion)
    //            .def("unsetAngleType", &vpAttrReplaceAPI::unSetAngleType)
    //            .def("doReplaceFromModule", &vpAttrReplaceAPI::doReplace_Module, vpAttrReplaceAPI_Moduleoverloads());

    class AttrReplace {
    public:

        enum ReplaceRangeType {
            MinMax,
            Greater,
            GreaterOrEqual,
            Less,
            LessOrEqual
        };

        enum ReplaceMode {
            LINEAR,
            NATURAL_NEIGHBOR,
            MINIMUM_CURVATURE,
            NEAREST_NEIGHBOR,
            HRZ_NEAREST_NEIGHBOR,
            KRIGING,
            FIXED_VALUE,
            SCALED_VALUE,
            VERTICAL_GRADIENT,
            VERTICAL_LINEAR
        };

        enum ReplaceRegionType {
            All, CurrRegn, OutCurrRegn, HrznOnAll, HrznOnCurr, HrznOnOut
        };

        /** @brief Specify whether to replace infinity values. */
        void replaceInf(bool isInf);
        /** @brief  Specify whether to replace nan values*/
        void replaceNAN(bool isNAN);
        
        /**
         * @brief Set the range of values to replace (ReplaceRangeType.MinMax)
         * 
         * @param val1 Minimum value 
         * @param val2 Maximum value
         */
        void setRange(ReplaceRangeType type, float val1, float val2 = FLT_MAX);

        /**
         * @brief Set the replace mode. Provide replacement value, if necessary
         */
        void setReplaceType(ReplaceMode mode, float val = FLT_MAX);
        
        /** @brief  Set the Volume to do replace operation on
         *  @note After replace operation, the loaded volume is modified. Use `Volume.saveAs` to save it to a new location
         */
        void setAttribute(Volume inputAttribute);
        
        /** @brief  Set input angle type. If not set, input assumed to be not angle.*/
        void setAngleType(AngleConv::Type tp);
        
        /** @brief Set limiting condition with region, if not set the replace operation is applied to the whole volume */
        void setRegion(ReplaceRegionType tp);
        
        /** @brief Clear the region setting */
        void clearRegion();
        
        /** @brief Clear the angle setup */
        void unSetAngleType();
        
        
        /** @brief Run replace process (if data loaded in memory) */
        bool doReplace();

        /** 
         * @brief Run replace process (if data loaded in memory)for g2 job torpy with big data handle
         * @note For G2 job, add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @param originPath fdm path (non zipped file)
         * @param savePath   fdm path (non zipped file)
         * @param taper      if it is too large for input file, use some taper between each chunk, can set to be 0
         */
        void doReplaceFromModule(String originPath, String savePath, int taper = 0);
    };

}

#endif /* ATTRREPLACE_HPP */

