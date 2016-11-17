
#ifndef HORIZONREGRID_HPP
#define HORIZONREGRID_HPP



namespace algorithm {


    /**
     * @brief Regrid horizon
     * 
     * In Tornado (Main Window: Horizon->Structural Changes->Regrid)
     * 
     * @image html HorizonRegrid.png
     * 
     * \code{.py}
     * from base import HorizonGroup
     * from algorithm import ExtendDir as ed
     * from algorithm import HorizonRegrid
     * 
     * #load the horizon group
     * hg= HorizonGroup()
     * hg.load('/data4/dtrain01/Tornado_p2/hrz/T2_tos1_wip_surf.hrz') 
     *        
     * #setup
     * hrzRegrid = HorizonRegrid()
     * hrzRegrid.setHorizonGroup(hg)
     * hrzRegrid.setXLRange(8600, 8800)
     * hrzRegrid.setILRange(1700, 2000)
     * hrzRegrid.setInc(2,2)
     * hrzRegrid.setExtend(ed.NO_EXTEND) 
     * hrzRegrid.setOutputPath('/data2/devtest/tornado/ruwu/horizon_cut/regrid.hrz')
     *        
     * #exe
     * output = hrzRegrid.doRegrid()
     *        
     * #save       
     * output.save()
     * 
     * \endcode
     * 
     */

    //        enum_<vpHorizonRegrid::ExtendDir>("ExtendDir")
    //    .value("EXTEND_ALONG_XL", vpHorizonRegrid::EXTEND_ALONG_XL)
    //    .value("EXTEND_ALONG_IL", vpHorizonRegrid::EXTEND_ALONG_IL)
    //    .value("NO_EXTEND", vpHorizonRegrid::NO_EXTEND);
    //    
    //    class_<vpHorizonRegrid>("HorizonRegrid")
    //    .def("doRegrid", &vpHorizonRegrid::doRegrid, return_value_policy<manage_new_object>())
    //    .def("setXLRange", &vlHorizonRegridProcess::setXLRange)
    //    .def("setILRange", &vlHorizonRegridProcess::setILRange)
    //    .def("setInc", &vlHorizonRegridProcess::setInc)
    //    .def("setOutputPath", &vlHorizonRegridProcess::setOutputPath)
    //    .def("setHorizonGroup", &vlHorizonRegridProcess::setHorizonGroup)
    //    .def("setExtend", &vpHorizonRegrid::setExtend);

    class HorizonRegrid {
    public:

        enum ExtendDir {
            EXTEND_ALONG_XL,
            EXTEND_ALONG_IL,
            NO_EXTEND
        };

        /**
         * @brief Start the regrid
         * @return regridded HorizonGroup
         */
        HorizonGroup doRegrid();

        void setXLRange(int minLine, int maxLine);

        void setILRange(int minLine, int maxLine);
        
        /**
         * @brief Increment for XL and IL
         */
        void setInc(int xl, int il);

        /**
         * @brief Set the path of output horizon
         */
        void setOutputPath(string path);

        /**
         * @brief Set the input horizon group
         */
        void setHorizonGroup(HorizonGroup inputHorizon);
        
        /**
         * @brief 2D Line extention
         * 
         * @param direction NO_EXTEND, EXTEND_ALONG_XL, or EXTEND_ALONG_IL
         */
        void setExtend(ExtendDir direction);

    };

}
    
#endif /* HORIZONREGRID_HPP */

