/* 
 * File:   vpWellTieMap.hpp
 * Author: ddoledec
 *
 * Created on April 21, 2016, 5:17 PM
 */

#ifndef VPHORIZONWELLMISITE_HPP
#define VPHORIZONWELLMISITE_HPP

#include "vlWellTieMap.hpp"

class vgHorizonGroup;
class Volume;

/**
 * Compute Well Misties between:
 *  - top/hrz pairs 
 *  - checkshots ()
 */
class vpWellTieMap : public vlWellTieMap {
public:
    vpWellTieMap();
    virtual ~vpWellTieMap();

    /** Expose Boost methods */
    static void exposePython();

    /**
     * Horizontal interpolation between misties
     * @param method  interpolation method
     * @param tension MinCurvature tension
     * @param level   MinCurvature level
     */
    void setInterpolationMode(GridInterp::Mode method, float tension = 0, int level = 0);


    /** Vertical Interpolation for Missing tops (interpolation , output interpolated misite)
     *  default processMisite.interpolateMissingTops( False )
     * @param verticalInterpolation default = false
     * @param outputInterpolatedMisite default = false
     */
    void interpolateMissingTops(bool verticalInterpolation,
            bool outputInterpolatedMisite = false);

    /** Compute misties between hrz and tops
     * \code{.py}
    # With Top horizon Pairs
    topHorizonPair =[ ['Top_Balder','Top_Balder'],
                        ['Top_Chalk','Top_Rattray'],
                        ['Base_Rattray','Base_Rattray']]
    processMisite.run(hg, topHorizonPair)
     * \endcode
     * 
     */
    bool run(vgHorizonGroup* hg,
            boost::python::list& horizonWellPair);

    /** Compute misties for checksots 
     * 
     * @param hg Horizon group 
     * @param horizonList names of horizons to use
     * \code{.py}
    # OR  from Checkshots and FDM
    horizons = ['Top_Balder','Top_Chalk','Herring']
    processMisite.run(hg,  horizons, velocity )
     * \endcode
     * 
     */
    bool run(vgHorizonGroup* hg,
            boost::python::list& horizonList,
            Volume* vol);

    /** 
     * @note if not defined the process will take all the wells loaded in tornado
     * @brief Define the list of well to use for the process.
     * @param wellList @see vision.getWellList()
     */
    void setWellList(boost::python::list wellList);


private:
    /** Wells used to apply process*/
    vector<LasWellLog*> getSelectedWells();

    /** Set couples of (horizons/well tops) from python  (hrz/tops option)*/
    bool setHorizonWellPairs(const vgHorizonGroup& myHG, boost::python::list& lst);

    /** Set horizon list from python (checkshot option) */
    bool setHorizons(const vgHorizonGroup& myHG, boost::python::list& lst);

    /** Check&Initialize the horizon group (attribute, domain..)*/
    bool initHorizonGroup(vgHorizonGroup& hg);

    // List of well to process
    vector<LasWellLog*> myWellList;
};

#endif /* VPHORIZONWELLMISITE_HPP */

