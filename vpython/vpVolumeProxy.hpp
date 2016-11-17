#ifndef VPVOLUMEPROXY_HPP
#define	VPVOLUMEPROXY_HPP

#include "vlVolume.hpp"
#include <fstream>

#if 1
void process_mem_usage(double& vm_usage, double& resident_set);

void printMem(string msg);
#endif
/// One giga 
extern size_t GEGA;


class vString;

/**
 *  This class is only used for MODULE mode of tornado scripting functionality
 *  1 When you add any feature, try to use printMem to trace the memory change
 *  to make sure the memory is below an acceptable level, usually a cluster machine
 *  only have 16-32 GB, leave some overhead to protect 
 *  2 Try to distribute assert() call in you FILE operations, do not worry it may 
 *  abort, we need to on purposely abort it to issue error to user in module 
 *  3  You may be faced with splitting many cubes, then you need to do it by hand
 *  to calculate the budget
 *  4. Do not forget to call  cleanUp once you are done to make sure you have cleaned
 *  the disk space (IMPORTAT!)
 */

class vpVolumeProxy {
public:
    
    enum Direction{
        XLINE=0,
        ILINE,
        ZLINE
    };
    
    
    vpVolumeProxy();
    
    /// Create a proxy (write mode)
    vpVolumeProxy(BaseGeometry geom);

    /** Attribute Name (only for write)*/
    void setAttrName(const vString& attr);
    
    virtual ~vpVolumeProxy();
    
    void split(); // split the data
    
    /**
     * @param i index (chunk number)
     * @param fromChunk will load data from disk-chunk if available
     */
    Volume* getVolume(int i, bool fromChunk=false); //get the ith volume
    
    void merge(); // merge the splitted files
    void globalMerge(); // merge result taking into account a sub-sampling (initial/final))

    void cleanUp(); // clean up the temporary files
    void init(); // re initialize the chunker
    bool isToSplit(string path); // check if one data is good to split
    /**
     * @param fromChunk will load data from chunk if available
     */
    float* getTrace(int i, int j, float* buf=0, bool fromChunk=false);
    void writeHeader(FileAccess& fa, BaseGeometry* geom=0); // write the given geometry as a fdm header
    
    static bool getVolumeStep(const string& Origpath, double &rx,double &ry, double &rz); //Get the steps dx, dy, dz of the volume
    
    int getDirection() const { return static_cast<int>(myDirection); }
    void setDirection(int dir){ myDirection=static_cast<Direction>(dir); };
    
        
    /// Get full geometry 
     const BaseGeometry& getGeometry() const;
     /// Define the geometry of the new cube
     void setPredictedGeometry(const BaseGeometry& geom); // set the geometry 

    /**
     * Size of the chunk between [0-8] giga.
     * It is considered that by default we allocate the memory 
     * for a process in this scope (actual settings of the clusters).
     * Therefore, if the hardware have more memory you may set the amount of
     * memory in the context, in this way all the chunk will scale to this new
     * limit ( memScaleRatio = context.MAX_MEMORY/8).
     * 
     * @param size in Giga
     * @param set it to true if you don't want to scale according to the memory available. 
     */
    void setSubVolSize(float size, bool fixed = false);
        
    /** Rather than defining the size set the number of lines for the chunk.
     * This will disable the size set in Giga, or the size-multiple.
     * @param nbOfSlices number of il, xl or time
     */
    void setChunkSize(int nbOfSlices );
    int getChunkSize() const;

    /**
     * When you want the chunk honor a size (il/xl/z) multiple of a step. 
     * Useful during a resampling process.
     * 
     * @param xscale 
     * @param yRatio
     */
    void setSubVolMultiple(int xscale, int yRatio, int zRatio);

    
    void setOriginPath(vString path) {myOriginPath=path;}
    void setToSaveAs(string path) {myToSaveAs=path;}
    
    string getSaveAs() const { return myToSaveAs;}
    
    int getTaper() const { return myTaper; }
    void setTaper(int tp){ myTaper = tp;}
    int getNumVolume(){return myFileNum;}
    vector<Vect2i> getSplitLines(){ return mySplitLines; }
    
    vector<string> getChunkFileNames() const { return myTmpFileNames;}

private:
    Volume *myVol;
    vString myAttrName;
    
    float mySubVolSize;
    int myTaper;
    int myFileNum;
    vString myOriginPath;
    
    BaseGeometry myPredictedGeometry;
    Direction myDirection;
    string myToSaveAs;
    int myMarchIndex;
    Volume* myMarchVolume;

    vector<string> myTmpFileNames;
    vector<Vect2i> myRealLines; // where you want to split and how long
    vector<Vect2i> mySplitLines; // Where your really split with taper and how long;
    vector<Vect2i> myMergeLines; // when merge, where to start and how long

    void splitLines();
    void mergeILine();
    void mergeXLine();
    void mergeZLine();
    void generateFileNames();
    Volume* loadSubsetInZ(Volume* vol, char* name, int beg, int len);

    bool myInitGeometry; // check if the geometry has been setup
    Vect3i myMultipleRatio;
    int mySlicePerChunk;               // Asked
    int myComputedSlicePerChunk;       // Computed
};

#endif	/* VPVOLUMEPROXY_HPP */

