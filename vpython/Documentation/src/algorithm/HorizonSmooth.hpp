
#ifndef HORIZONSMOOTH_HPP
#define HORIZONSMOOTH_HPP


namespace algorithm {


    /**
     * @brief Smooth horizon 
     * 
     * In Tornado (Horizon Picking Window: Edit->Smoothing ).
     * @image html HorizonSmooth.png
     * 
     * \code{.py}
     * from algorithm import HorizonSmooth
     * 
     * # Load Horizon
     * horgroup_path="test.hrz"
     * hg=HorizonGroup()
     * hg.load(horgroup_path)
     * hrz=hg.getHorizonByName("hrzName")
     * 
     * # Z attribute 
     * m=originhrz.getAttrIndex('DEPTH')
     * smoother=HorizonSmooth()
     * smoother.setRadiusX(2000) # in meter
     * smoother.setRadiusY(2000) # in meter
     * smoother.setMode(0)
     * smoother.run(hrz,idxRms)
       
     * # Save
     * hg.save()
     * 
     * \endcode
     * 
     */
    class HorizonSmooth {
    public:
    
    /**
     * @brief run the process
     * 
     * @param hrz Horizon to smooth
     * @param idx horizon attribute index for z (i.e:DEPTH/TWT)
     * @return True if success or False if fail
     */
    bool run( Horizon hrz, int idx);

    /**
     * @brief Smooth XL radius in meter
     */
    void setRadiusX(double rx);
    
    /**
     * @brief Smooth IL radius in meter
     */
    void setRadiusY(double ry);
    
    /**
     * @brief Type of filter
     * FILTER_MEAN, 
     * FILTER_MEAN_TRIANGLE, 
     * FILTER_MEDIAN, 
     * FILTER_ALPHATRIM, 
     * FILTER_EDGE_PRESERVE
     */
    void setMode(int type);

    };

}

#endif /* HORIZONSMOOTH_HPP */

