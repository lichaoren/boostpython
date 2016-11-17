
class base::HorizonGroup;

namespace algorithm {

    using namespace base;
    /**
     * This script show how to compute misties (hrz/tops or checkshots) and 
     * display the result in Tornado (For each horizon display the values of the misties).
     * 
     * @example testHorizonMisite.py
     * 
     */
    

    /**
     * @brief Compute Well Misties
     * 
     * 
     * Between:
     *  - top/hrz pairs 
     *  - checkshots ()
     * 
     * An script example:
     * @ref testHorizonMisite.py 
     *      
     */
    class HorizonMisite {
    public:
        HorizonMisite();

        void setAnnotationFile(string& path);

        /** Compute misties between hrz and tops
         * \code{.py}
        # With Top horizon Pairs
        topHorizonPair =[ ['Top_Balder','Top_Balder'],
                            ['Top_Chalk','Top_Rattray'],
                            ['Base_Rattray','Base_Rattray']]
        processMisite.run(hg, topHorizonPair)
         * \endcode
         * 
         * 
         */
        bool run(HorizonGroup hg, list < pair< string, string> > horizonWellPair);

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
        bool run(HorizonGroup hg,
                list<string> horizonList,
                Volume vol);

        /**
         * Name of the attribute (horizon) where the result is saved
         * @param name default is 'MISTIE'
         * 
         * \code{.py}
         * #output attribute name
         * processMisite.setOutputAttrName('mistie_try1')
         * \endcode
         * 
         */
        void setOutputAttrName(string name);

        /** Time or depth domain
         * @param name 'DEPTH' if attr exist or 'TWT'  
         */
        void setDomain(string name);

        void setStructureBased(bool flag);


        /** Vertical Interpolation for Missing tops (interpolation , output interpolated misite)
         *     default processMisite.interpolateMissingTops( False )
         * @param verticalInterpolation default = false
         * @param outputInterpolatedMisite default = false
         */
        void interpolateMissingTops(bool verticalInterpolation,
                bool outputInterpolatedMisite = false);

        /**
         * Horizontal interpolation between misties
         * @param method  interpolation method
         * @param tension MinCurvature tension
         * @param level   MinCurvature level
         */
        void setInterpolationMode(GridInterp::Mode method, float tension = 0, int level = 0);


        /** 
         * @note if not defined the process will take all the wells loaded in tornado
         * @brief Define the list of well to use for the process.
         * @param wellList @see vision.getWellList()
         */
        void setWellList(list wellList);

    };
}
