

namespace vision {

    /** 
     * @ingroup snapshot
     */

    /**
     * In order to reproduce the same images most of the windows can save the display 
     * parameters into a file. The script provide a facility to reload those parameters.
     * 
     * @brief Display parameters 
     */
    class CaptureDisplayParameters {
    public:
         
        /** Get filename used to save the parameters in tornado
         */   
        string getPath();
        /** Set filename used to save the parameters in tornado
         */
        void setPath(string path);
    };

}



