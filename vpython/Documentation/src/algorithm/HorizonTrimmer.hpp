
#ifndef HORIZONTRIMMER_HPP
#define HORIZONTRIMMER_HPP

#include <list>
#include <string>

using namespace std;

namespace algorithm {


    /**
     * @brief Trim Horizon
     * 
     * In Tornado (Main Window: Attribute -> Update -> Advanced Salt Insertion)
     * "Cut horizon(s) is one crosses above the other" section)
     * @image html HorizonTrimmer.png
     * 
     * \code{.py}
        from base import HorizonGroup
        from algorithm import CutMode as cm
        from algorithm import HorizonTrimmer

        pairs_name = [['tos1_0824', 'bos1_0824'], ['tos2_0824', 'bos2_0824']]
        pairs_index = [[1,2], [3,4]]
        #load the horizon group
        hg= HorizonGroup()
        hg.load('/data2/devtest/tornado/ruwu/horizon_cut/k2_b3_0826_start.hrz')

        HrzTrimmer = HorizonTrimmer()
        HrzTrimmer.setHorizonGroup(hg)
        
        # set the horizon pairs by name, 
        # or by index (which starts from 1) using HrzTrimmer.setHrzPairsByIndex(pairs_index).
        HrzTrimmer.setHrzPairsByName(pairs_name)

        HrzTrimmer.setRadius(0) #If you do not set it, it is 0 by default.
        
        # cm.CUT_PAIR or cm.CUT_BASE. If you do not set it, it is cm.CUT_PAIR by default.
        HrzTrimmer.setCutMode(cm.CUT_PAIR) 
        HrzTrimmer.setOutputPath('/data2/devtest/tornado/ruwu/horizon_cut/trim1.hrz')

        HrzTrimmer.exe()
     * \endcode
     * 
     */

    class HorizonTrimmer {
    public:

        /**set whether horizons will be modified to cut base of salt horizon
         * where it crosses above the top of salt horizon
         */
        enum CutMode {
            CUT_BASE, CUT_PAIR, NUM_CUT
        };

        /** @brief cut base only (CUT_BASE) or cut both top and base (CUT_PAIR) */
        void setCutMode(CutMode mode);

        /** @brief set the output path for the cut horizon group */
        void setOutputPath(const string path);

        /** 
         * @brief set the Salt horizon pairs by their names
         * @param lst One pair is represented as ['top1', 'btm1']
         */
        void setSaltHrzPairsByName(list< string , string > lst);
        
        /** 
         * @brief set the Salt horizon pairs by their names
         * @param list is represented as [['top1', 'btm1'],['top2', 'btm2']]         
         */
        void setSaltHrzPairsByName( list< list< string , string > > lst);

        /** 
         * @brief set the <b>Salt<\b> horizon pairs by their locations in input horizon group (location starts from 1)
         * @param lst One pair is represented as [1, 2]. Note: index starts from 1.
         */
        void setSaltHrzPairsByIndex(list< int , int > lst);
        
        /** 
         * @brief set the Salt horizon pairs by their locations in input horizon group (location starts from 1)
         * @param list is represented as [[1, 2],[3, 4]]. Note: index starts from 1.
         */                
        void setSaltHrzPairsByIndex(list< list< int , int > > lst);        

        /** 
         * @brief set the Sediment horizon pairs by their names
         * @param lst One pair is represented as ['top1', 'btm1']
         */
        void setSediHrzPairsByName(list< string, string > lst);

        /** 
         * @brief set the Sediment horizon pairs by their names
         * @param list is represented as [['top1', 'btm1'],['top2', 'btm2']]         
         */
        void setSediHrzPairsByName(list< list< string, string > > lst);

        /** 
         * @brief set the Sediment horizon pairs by their locations in input horizon group (location starts from 1)
         * @param lst One pair is represented as [1, 2]. Note: index starts from 1.
         */
        void setSediHrzPairsByIndex(list< int, int > lst);

        /** 
         * @brief set the Sediment horizon pairs by their locations in input horizon group (location starts from 1)
         * @param list is represented as [[1, 2],[3, 4]]. Note: index starts from 1.
         */
        void setSediHrzPairsByIndex(list< list< int, int > > lst);
        

        /** @brief set the input horizon group */
        void setHorizonGroup( HorizonGroup hg);

        /** @brief run the cutting process and save the output horizon to disk */
        void exe();

        /** @brief set the radius for extension (default value: 0) */
        void setRadius(int radius);

        /** @brief */
        void setAlwaysMatchPair(bool alwaysMatchPair);

    };

}

#endif /* HORIZONTRIMMER_HPP */

