


namespace vision {
 
    /**
     * @example snapshotBookmarks.py
     */
        
    /** 
     * @ingroup snapshot
     */
    
    /**
     * Snapshot - Display parameters from a bookmark
     * To be able to apply the colormap, size, position ... to other data
     *
     * # Examples
     * @ref snapshotBookmarks.py
     * 
     * @brief Display parameters loaded from a bookmark
     * 
     */
    class BookmarkDisplay : public CaptureDisplayParameters {
    public:

        enum ColorMapMode {
            /// Apply the range from the bookmark
            BOOKMARK_COLORS,
            /// Don't use the range from the bookmark
            CURRENT_COLORS
        };

        enum DataType {
            HORIZON = 0x8,
            WELL = 0x4,
            ATTRIBUTE = 0x2,
            SEISMIC = 0x1,
            ALL = HORIZON | WELL | ATTRIBUTE | SEISMIC,
        };


        /** Select the names of the bookmark to apply*/
        void selectBookmark(const vector<string> bkList);

        /** Select the name of the bookmark to apply*/
        void selectBookmark(const string bkName);

        /** For a type of data set colormap behavior */
        void setColormap(int dataType, ColormapType flag);

        /**
         * Replace a value in the xml file.
         * i.e: setXML("bkm1","ATTRIBUTE_COLORMAP.SPECTRUM.RANGE","-0.500000 0.900000")
         * @param bkmrkName  name of the bookmark 
         * @param xmlString  tags separated by "." identifying the property to replace
         * @param value      replaced value
         */
        bool setXML(const vString& bkmrkName, const vString& field, const vString& value);

        /** Bookmark file path */
        void load(const string& path);
    };


}