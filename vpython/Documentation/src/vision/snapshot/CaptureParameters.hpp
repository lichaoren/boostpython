



namespace vision {
    /** 
     * @ingroup snapshot
     */

    /**
     * Parameters needed to create images.
     * Contains: 
     *            - Output file description
     *            - Data to use (as a toggle list)
     *            - Location (where the data need to be in the scene)
     * 
    Usage:
     * - Create the parameter
     * \code{.py}
     * param = CaptureParameters()
     * \endcode
     * 
     * - Set file description
     * \code{.py}
     * param.setFileParameters()
     * \endcode
     * 
     * 
     * - Set data
     * \code{.py}
     * param.setDataParameters()
     * \endcode
     * 
     * - Set location
     * \code{.py}
     * param.setLocations()
     * \endcode
     * 
     * - Set Display parameters
     * \code{.py}
     * param.setDisplayParameters()
     * \endcode
     * 
     * @brief Encompass all the parameters to create images.
     * 
     */
    class CaptureParameters {
    public:
        CaptureParameters();

        /** Get the settings related to the output file */
        CaptureFileParameters getFileParameters();
        
        /** Set the settings related to the output file */
        void setFileParameters(CaptureFileParameters parameters);

        /** Get the settings for the display */
        CaptureDisplayParameters getDisplayParameters();
        /** Set the settings for the display */
        void setDisplayParameters(CaptureDisplayParameters parameters);

        /** get the data to use for the screenshot*/
        ReportDataParameters getDataParameters();
        /** define the data to use for the screenshot*/
        void setDataParameters(ReportDataParameters parameters);

        /** 
         * get the location: where the data need to be moved before screen-shot.
         * i.e: Move to a well position, inline, subline, gather ...
         */
        DataLocation getLocations();
        
        /** 
         * define the location: where the data need to be moved before screen-shot.
         * i.e: Move to a well position, inline, subline, gather ...
         */
        void setLocations(DataLocation location);
    };

}


