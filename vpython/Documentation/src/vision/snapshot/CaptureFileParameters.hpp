


namespace vision {

    /** 
     * @ingroup snapshot
     */

    /**
     * Description of the output file(s). Include the file naming or the description of the format(jpeg, png...)
     * 
     * \code{.py}
       from vision import *
              
       #######  Create the parameters
       param = CaptureParameters()
       
       #######  File settings
       fileParameters = CaptureFileParameters()
       fileParameters.setPrefix("xxxx")
       # Output directory
       fileParameters.setPath("/tmp/")
       # Image format: png, jpg, png
       fileParameters.setFormat("png")
       # Name of the file according to the data & position
       # Available parameters:  Prefix, Suffix, GridNum, BookmarkName, SeismicName, 
       #                                          GatherName, WellName, AttrlName, WindowName
       fileParameters.setNaming(['Prefix','WindowName','SeismicName','AttrName','GridNum'])
       
       param.setFileParameters(fileParameters)

     * \endcode
     * 
     * @brief  File description for output images 
     */
    class CaptureFileParameters {
    public:

        /** Image format (png,jpeg, bmp)*/
        void setFormat(string imageFormat);
        
        /** directory path for the output images  */
        void setPath(string imagePath);


        /** Set the tokens from a python list.
         * 
         * Set the tokens to use (ordered) to generate the file names of the images: 
         * Prefix, Suffix, GridNum, BookmarkName, SeismicName, 
         * GatherName, WellName, AttrlName, WindowName
         * 
         * \code{.py}
         * fileParameters.setNaming(['Prefix','WindowName','SeismicName','AttrName','GridNum'])
         * \endcode
         *  
         */
        void setNaming(list<string> lst);


        /**   Set the prefix to generate the file names of the images */
        void setPrefix(string imagePrefix);

    };



}


