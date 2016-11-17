
namespace algorithm {

    /*!
     * In an existing velocity model (fdm) insert velocity (Salt) between horizons defined as top and bottoms
     * 
     * 
     * ## Snippet: 
     * example job to run G2 job script:
     * 
     * \code{.py}
       import base

       #specify the folder your data locates
       path="REMOTEFILE!houdat002:/data2/devtest/tornado/yanhliu/insertSalt/"

       inserter = InsertSalt();

       #add the horizon group you want to use
       inserter.setHorizonGroup(path+'SC18_041715.hrz');

       #setup the input volume
       inserter.setInputVolume(path+'sediment_0402_4x4_cre_trn_regrid.fdm_p');

       #setup the output file name
       inserter.setTargetVolume(path+'delete.fdm');

     * #Salt 1
     * inserter.addSalt( [['TOS1_prod','REGIONAL_BOS1_MRG']],
                             path+'variable_salt_vel_with_dslt_inft.fdm')
     * 
     * #Salt 2
     * inserter.addSalt([['SALT_PATCH_T1DEEP','SALT_PATCH_B1DEEP'],['T_DEEP_SALT','B_DEEP_SALT']],
                           14698)
   
       #insert
       inserter.insertSalt();
     * \endcode
     * 
     * @brief Insert velocity between top/bot horizons
     * 
     */

    
    class InsertSalt {
    public:

        InsertSalt();

        /**
         * @param horizonGroup Path of input horizon group
         * @warning For G2 job
         * @warning           add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning           Only supports uncompressed FDM file
         */
        void setHorizonGroup(string horizonGroup);

        /**
         * @param path input Volume path
         * @warning           add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning           Only supports uncompressed FDM file
         */
        void setInputVolume(string path);

        /**
         * Set the output volume path
         * @param target_path path of the output
         * @warning For G2 job
         * @warning           add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning           Only supports uncompressed FDM file
         */
        void setTargetVolume(string target_path);


        /**
         * 
         * @param flag
         */
        void setCheckSaltBoundary(bool flag);

        /**
         * vertical interpolation.
         * Instead of the default insertion using slowness, you can force it to use linear 
         */
        void setForceLinear(bool flag);

        void setFloodMax(bool flag);


        /**
         * Set the pair of horizon (top/bottom) to inset salt
         * @param hList list of horizon pairs. 
         *  One pair is represented as [’top1’, ‘btm1’] or [’top’,’‘]. 
         *  The first pair is a pair between horizon top1 and btm1, and 
         *  the second one is a pair between top and base. Finally, one
         *  list is represented as [[’top1’,‘btm1’],[’top’,’‘]]
         * @deprecated better use @ref addSalt() 
         */
        void setHorizonPairs( list hList);

        /**
         * If the salt velocity is described by a fdmf
         * @param sed_path path to the dirty salt
         * @warning For G2 job
         * @warning           add 'REMOTEFILE!houdat0XX:' before your path for Cluster accessing data island.
         * @warning           Only supports uncompressed FDM file
         * @deprecated better use @ref addSalt() 
         */
        void setOrigSed(string sed_path);

        /**
         * @param velocity salt velocity value
         * @warning This function is eclusive with @ref setOrigSed .If both are set, will use the salt velocity to insert
         * @deprecated better use @ref addSalt() 
         */
        void setSaltVelocity(float velocity);

        /**
         * @brief Add a constant velocity between each top/bottom pairs
         * This method is cumulative (each call append a salt definition)
         * 
         * \code{.py}
         * inserter.addSalt([['SALT_PATCH_T1DEEP','SALT_PATCH_B1DEEP'],['T_DEEP_SALT','B_DEEP_SALT']],
                           14698)   
         * \endcode 
         * 
         * @param hList list of top/bottom pairs
         * @param velocity constant velocity
         */
        void addSalt( list hList, float velocity);

        /**
         * Add a velocity defined by a cube between each top/bottom pairs
         * 
         * This method is cumulative (each call append a salt definition)
         * 
         * 
         * \code{.py}
         * inserter.addSalt( [['TOS1_prod','REGIONAL_BOS1_MRG']],
                             path+'variable_salt_vel_with_dslt_inft.fdm') 
         * \endcode    
         * 
         * A more advanced way is to 'virtualy' shift the horizons during the process
         * i.e: to shift down (-100m) the first top
         * 
         * \code{.py}
         * inserter.addSalt( [[('TOS1_prod',-100),'REGIONAL_BOS1_MRG']],
                             path+'variable_salt_vel_with_dslt_inft.fdm') 
         * \endcode    
         *  
         * 
         * @param hList hList list of top/bottom pairs
         * @param velocity_path path of the velocity model to insert between each top/bottom pairs
         */
        void addSalt(list hList, string velocity_path);

        
        /** Run the process - do the salt insertion according to your previous setup. */
        void run();

        /** 
         * Run the process - do the salt insertion according to your previous setup.
         * @deprecated better use @ref run() 
         */
        void insertSalt();


        
// This was never documented (used???) and it's deprecated by addsalt
#ifdef NEVER_DOCUMENTED

        /**
         * 
         * @param hList
         */
        void addSegs(python::list hList);

        /**
         * @param hList
         */
        void setTopHrzIdx(python::list& lst);

        /**
         * @param hList
         */
        void setTopHrzIdx(python::list& lst);

        /**
         * @param hList
         */
        void setBtmHrzIdx(python::list& lst);

#endif


    };
};


