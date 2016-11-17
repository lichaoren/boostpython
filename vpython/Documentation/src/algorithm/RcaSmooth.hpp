
#ifndef RCASMOOTH_HPP
#define RCASMOOTH_HPP


namespace algorithm {

    /**
     * This class supports tornado dip guided smoothing 
     * @note This class does not support processing big volumes in G2 torpy job. 
     * 
     * The RcaSmooth state has a number of parameters to be set, similar to 
     * Dip Guided Smooth dialog used in Tornado (Attribute->Update->Dip Guided Smooth...)
     * 
     * @image html RcaSmooth.png
     * 
     * @brief Dip guided attribute smooth 
     * 
     * \code{.py}
     *  from base import *
     *  from algorithm import RcaSmooth as sm
      
     *  path="/data2/devtest/tornado/yanhliu/rca/"
     *  data, theta, phi = Volume(), Volume(), Volume()
     *  data.load(path+'titusrg_test_start_0922.fdm_p')
     *  theta.load(path+'titusrg_scn10_hgd.fdm_theta')
     *  phi.load(path+'titusrg_scn10_hgd.fdm_phi')
     *  smoother=sm()
     *  param=[
     *  [0,    1000, 100],
     *  [3000, 1000, 100],
     *  [6000, 2000, 150],
     *  [9000, 3000, 200]
     * ]
     * 
     * smoother.setup(data,theta,phi,param)
     * res=smoother.smooth();
     * res.saveAs(path+'testRca.fdm')
     * 
     * \endcode
     * 
     * 
     * 
     */
    class RcaSmooth {
    public:
        
        /// Dip field unit
        enum RcaUnits { radian, degree, meter };
        
        /**
         * 
         * @param input the volume to be smoothed
         * @param theta theta volume or dipx if isDip=True
         * @param phi   phi volume or dipy if isDip=True
         * @param params smooth parameters is a list of (z, x-y radius, z radius)
         * @param isDip  optional, if True, the xVol,yVol is the dip not angle
         * @param ftype  optional, default is :attr:`RcaUnits.radian`, if useDip=True, set your own unit
         */
         void setup(Volume input, Volume theta, Volume phi,
                boost::python::list params, bool isDip=false, RcaUnits ftype=radian);  
         
         /** Compute and returns the smoothed volume */
         Volume smooth();

    };

}

    

#endif /* RCASMOOTH_HPP */

