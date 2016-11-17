#include "vpVolumeProxy.hpp"
#include <netinet/in.h>
#include "FS.hpp"
#include "GenericPath.hpp"

#include "vlAppModel.hpp"
#include "vlCapiAttribNames.hpp"
#include "vlCube.hpp"
#include "vlDataPort.hpp"
#include "vlFileAccess.hpp"
#include "vlFdmCube.hpp"
#include "NoValue.hpp"

#include "vmGridDef.hpp"

#include "vpVox2Fdm.hpp"
#include "vlPythonInterpreter.hpp"
#include "vpPythonContext.hpp"


size_t GEGA = (size_t)1024*(size_t)1024*(size_t)1024;


void process_mem_usage(double& vm_usage, double& resident_set) {
    using std::ios_base;
    using std::ifstream;
    using std::string;

    vm_usage = 0.0;
    resident_set = 0.0;

    // 'file' stat seems to give the most reliable results
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
            >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
            >> utime >> stime >> cutime >> cstime >> priority >> nice
            >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}

void printMem(string msg) {
    double vm_usage, resident_set;
    process_mem_usage(vm_usage, resident_set);
    cout << msg << " VM: " << vm_usage / 1024. << " RES:" << resident_set / 1024. << endl;
}

/** Capi need to have a query to proper load a subset */
bool eligibleForQuery(const vString& path)
{
    GenericPath gp(path); // We are using the first path as a representative to decide whether all the files specified by myPaths are valid for SQL or not
    PRJTYPE projType = gp.getProjectType();
    if (projType == ProjectType::TANGO || 
            projType == ProjectType::SAGE || 
            projType == ProjectType::VEGA || 
            projType == ProjectType::GEO || 
            projType == ProjectType::UDS || 
            projType == ProjectType::ACORN ) 
    {
        return true;
    }
    
    return false;
}

/** Check if @gridDef can be found in @path.
 *  It validates @path first, then compare ranges of @dridDef with
 *  the geometry of @path.
 *  Returns @path if succeeds.
 */
vString getQuery(const vString& path, const vmGridLineDef& gridDef )
{
    vString query = path;
    // The SQL query can only be applied to a few formats of the files
    if (eligibleForQuery(path)) 
    {
        vString filter;
        vlAppModel& model = vlAppModel::getRef();
        vHandle<Store> store = model.openStore(path);
        
        
        const char* const subl[] = {"SUBLINE", ""};
        bool hasSubline = hasAttrName(*store, subl);
        store->close();

        filter.erase(); // Just in case
        vString sublineKey = "INLINE = ";
        if (hasSubline) 
            sublineKey="SUBLINE = ";
        
        filter << "CROSSLINE = " << gridDef.myCrlRange[0] << " : " << gridDef.myCrlRange[1] << " : " << gridDef.myCrlRange[2];
        filter <<  " AND ";
        filter << sublineKey << gridDef.myInlRange[0] << " : " << gridDef.myInlRange[1] << " : " << gridDef.myInlRange[2];
            
        query << "+select * from '" << path << "' where " << filter;
    }
    
    return query;
}

vpVolumeProxy::vpVolumeProxy() :
myInitGeometry(false),
mySubVolSize(0),
myTaper(0),
myFileNum(0),
myMarchVolume(0),
myMarchIndex(0),
mySlicePerChunk(-1),
myComputedSlicePerChunk(-1) {
    myVol = new Volume;
    myTmpFileNames.clear();
    myRealLines.clear();
    mySplitLines.clear();
    myMergeLines.clear();
    myDirection = ILINE;
    myMultipleRatio[0] = 1;
    myMultipleRatio[1] = 1;
    myMultipleRatio[2] = 1;
}

vpVolumeProxy::vpVolumeProxy(BaseGeometry geom) :
myInitGeometry(true),
mySubVolSize(0),
myTaper(0),
myFileNum(0),
myMarchVolume(0),
myMarchIndex(0),
mySlicePerChunk(-1),
myComputedSlicePerChunk(-1) {
    myVol = new Volume;
    myTmpFileNames.clear();
    myRealLines.clear();
    mySplitLines.clear();
    myMergeLines.clear();
    myDirection = ILINE;
    myMultipleRatio[0]=1;
    myMultipleRatio[1]=1;
    myMultipleRatio[2]=1;
    
    setPredictedGeometry(geom);

    split();
}


void vpVolumeProxy::init() {
    if (myVol)
        delete myVol;
    myVol = new Volume;
    mySubVolSize = 0;
    myTaper = 0;
    myFileNum = 0;
    myTmpFileNames.clear();
    myRealLines.clear();
    mySplitLines.clear();
    myMergeLines.clear();
    myDirection = ILINE;
    myMarchVolume=0;
    myMarchIndex=0;
}

vpVolumeProxy::~vpVolumeProxy() {
    cleanUp();

    if (myVol)
        delete myVol;
    if (myMarchVolume)
        delete myMarchVolume;
        
    myVol = 0;
}

#define DEFAULT_PROCESS_MEMORY 8.0

/**
 * Size of the chunk between [0-8] giga.
 * It is considered that by default we allocate the memory 
 * for a process in this scope (actual settings of the clusters).
 * Therefore, if the hardware have more memory you may set the amount of
 * memory in the context, in this way all the chunk will scale to this new
 * limit ( memScaleRatio = context.MAX_MEMORY/8).
 * 
 * @param size in Giga
 * @param fixed set it to true if you don't want to scale according to the memory available. 
 */
void vpVolumeProxy::setSubVolSize(float size, bool fixed)
{

    if (fixed == false)
    {
        vpPythonContext& pyContext = vlPythonInterpreter::getInstance().getContext();
        size *= pyContext.getAvailableMemory()/DEFAULT_PROCESS_MEMORY;

    }
    mySubVolSize = size;
}

Volume* vpVolumeProxy::getVolume(int chunkIdx, bool fromChunk) {
    if (!myInitGeometry) {
        split();
    }

    Vect4i size = myPredictedGeometry.getSize();
    Vect2i grdMin;
    Vect2i grdInc;
    Vect2i grdLen;

    if (myDirection == ZLINE) {
        int beg = mySplitLines[chunkIdx][0];
        int len = mySplitLines[chunkIdx][1];
        char* name = const_cast<char*> (myTmpFileNames[chunkIdx].c_str());
        return loadSubsetInZ(myVol, name, beg, len);
    }

    if (myDirection == ILINE) {
        grdMin = Vect2i(0, mySplitLines[chunkIdx][0]);
        grdInc = Vect2i(1, 1);
        grdLen = Vect2i(size[0], mySplitLines[chunkIdx][1]);
    } else if (myDirection == XLINE) {
        grdMin = Vect2i(mySplitLines[chunkIdx][0], 0);
        grdInc = Vect2i(1, 1);
        grdLen = Vect2i(mySplitLines[chunkIdx][1], size[1]);
    }


    vmGridDef gd(grdMin, grdInc, grdLen);
    Vect2i crlrg;
    crlrg[0] = myPredictedGeometry.grdToLin(0, grdMin[0]);
    crlrg[1] = myPredictedGeometry.grdToLin(0, grdMin[0] + grdLen[0]);

    Vect2i inlrg;
    inlrg[0] = myPredictedGeometry.grdToLin(1, grdMin[1]);
    inlrg[1] = myPredictedGeometry.grdToLin(1, grdMin[1] + grdLen[1]);

    vmGridLineDef gridDef(inlrg, crlrg);
    vString path = getQuery(myOriginPath, gridDef);

    Volume* dummy = new Volume;
    dummy->setCreateHint(Volume::eHintLSUB);

    // try to load local chunk
    if (fromChunk) {
        if (dummy->loadHeader(myTmpFileNames[chunkIdx].c_str()) &&
                dummy->load()) {
            return dummy;
        }
    }

    if (dummy->loadHeader(path)) {
        // Check limits
        Vect4i input_size = dummy->getSize();
        int grdXlMax = input_size[0] - gd.myMin[0];
        int grdIlMax = input_size[1] - gd.myMin[1];
        if (gd.myNum[0] > grdXlMax)
            gd.myNum[0] = grdXlMax;
        if (gd.myNum[1] > grdIlMax)
            gd.myNum[1] = grdIlMax;

        dummy->loadSubset(gd, 0);
        dummy->setPath(this->myTmpFileNames[chunkIdx].c_str());

        return dummy;
    } else {

        // If no chunk and no origin ... must be a new cube
        if (myToSaveAs.size() != 0) {
            FdmCube* zCube = new FdmCube();
            BaseGeometry geom = *dummy;
            zCube->create(geom, this->myTmpFileNames[chunkIdx].c_str());

            delete dummy;
            dummy = new Volume(zCube);

            dummy->setPath(this->myTmpFileNames[chunkIdx].c_str());
            return dummy;
        }


    }
    exit(-1);
    delete dummy;
    return 0;
}

void vpVolumeProxy::cleanUp() {
    for (int i = 0; i<this->myFileNum; i++) {
        remove(this->myTmpFileNames[i].c_str());
    }
    init();
}


bool vpVolumeProxy::getVolumeStep(const string& pathName, double &rx, double &ry, double &rz)
{
    vString path = vString(pathName.c_str());
//    FileAccess fa;
    // make sure we can open it
//    bool res = fa.open(pathName.c_str(), "r");
//    if (res)
//        fa.close();
//    else
//        cerr<<"Cannot open file " <<endl;
//        //dd tango    return false;
    Volume vol;
    vol.setCreateHint(Volume::eHintLSUB);
    if (!vol.loadHeader(path.c_str())) {
        vString message;
        message << "Cannot load " << path ;        
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message);
        return false;
    }
    Vect4d steps= vol.getStep();
    rx=steps[0];
    ry=steps[1];
    rz=steps[2];
    
    return true;
}



bool vpVolumeProxy::isToSplit(string Origpath) {
    vString path = vString(Origpath.c_str());

    Volume *vol= new Volume;
    vol->setCreateHint(Volume::eHintLSUB);
    if (!vol->loadHeader(path.c_str())){
        vString message;
        message << "Failed to load the path: " << path;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message);
        delete vol;
        return false;
    }
    Vect4i tmpSize= vol->getSize();
    size_t size =sizeof(float)*(size_t)tmpSize[0]*(size_t)tmpSize[1]*(size_t)tmpSize[2];
    delete vol;
    vpVox2Fdm converter;
    if (converter.isVoxel(Origpath.c_str()))
        return size * 4 > mySubVolSize * GEGA;
    else
        return size>this->mySubVolSize*GEGA;
}

extern "C" {
  char* FS_scratchname(const char*, char *, size_t);
};

void vpVolumeProxy::generateFileNames() {
    cout << "original data:" << myOriginPath << " will be splitted to be " << myFileNum << " pieces "
            << endl;

    int i;
    const char preffix[] = "test";
    //const char b[] = "VOLUME";
    char tempFileName[1024];
    for (i = 0; i < myFileNum; ++i) {
        this->myTmpFileNames.push_back(string( FS_scratchname(preffix, tempFileName, 1024)));
    }

}

void vpVolumeProxy::split() {

    if (!myInitGeometry) {
        // load the header to get data info
        myVol->setCreateHint(Volume::eHintLSUB);
        myVol->loadHeader(this->myOriginPath.c_str());
        setPredictedGeometry(*this->myVol);
    }
    splitLines();
}

void vpVolumeProxy::splitLines() {

    int i = 0, totalNumOfLines = 0;
    size_t sliceSize = 0;

    Vect4i size = myPredictedGeometry.getSize();
    if (myDirection == ILINE) {
        sliceSize = sizeof(float) * size[0] * size[2];
        totalNumOfLines = size[1];
    } else if (myDirection == XLINE) {
        sliceSize = sizeof(float) * size[1] * size[2];
        totalNumOfLines = size[0];
    } else {
        sliceSize = sizeof(float) * size[0] * size[1];
        totalNumOfLines = size[2];
    }

    int nbOfSlices = (int) round((mySubVolSize * GEGA) / sliceSize);

    if (myTaper > 0 && (nbOfSlices - myTaper * 2) > 0) {  // what if not true here?
        nbOfSlices -= (myTaper * 2);
    }

    int nbOfChunks =
        (nbOfSlices == 0) ? 1 : (int) ceil((float) totalNumOfLines / (float) nbOfSlices);
    int numOfLines = (int) ceil((float) totalNumOfLines / (float) nbOfChunks);
    if (mySlicePerChunk != -1) {
        numOfLines = mySlicePerChunk;
        nbOfChunks = (int) ceil((float) totalNumOfLines / (float) numOfLines);
    }

    if (myDirection == ILINE && (numOfLines % myMultipleRatio[1] != 0)) {
        numOfLines -= numOfLines % myMultipleRatio[1];
        nbOfChunks = (int) ceil((float) totalNumOfLines / (float) numOfLines);
    }

    myFileNum = nbOfChunks;
    generateFileNames();

    myComputedSlicePerChunk = numOfLines;
    for (i = 0; i < myFileNum; ++i) {
        int len = numOfLines;
        if (i == myFileNum - 1) {
            len = totalNumOfLines - i * numOfLines;
        }
        int beg = i * numOfLines;

        int real_beg = beg;
        if (i > 0)
            real_beg = beg - myTaper;  // taper on the left

        int real_len = len + 2 * myTaper;  // taper on right
        if (i == 0 || i == myFileNum - 1)
            real_len = len + myTaper;  // only one taper at left/right

        if (myFileNum == 1)
            real_len = len;

        Vect2i realLine(beg, len);
        Vect2i splitLine(real_beg, real_len);
        int mergeBeg = 0;
        if (i != 0)
            mergeBeg = this->myTaper;
        Vect2i mergeLine(mergeBeg, len);

        this->myRealLines.push_back(realLine);
        this->mySplitLines.push_back(splitLine);
        this->myMergeLines.push_back(mergeLine);
    }
}


// this method use multi reader to read, which will avoid too much fseek to block
// second version, the first version is slow
void vpVolumeProxy::mergeZLine() {
    FileAccess fa;
    writeHeader(fa);
    Vect4i oSize = myPredictedGeometry.getSize();
    
    float *data= new float[(size_t)oSize[0]*(size_t)oSize[2]];
    float *dummy= new float[(size_t)oSize[0]*(size_t)oSize[2]];
    
    vector<FileAccess*> file_readers;
    // loop each file, open and skip  the header   
    int i;
    for ( i = 0; i<this->myFileNum; ++i) {
        FileAccess *reader = new FileAccess;
        reader->open(myTmpFileNames[i].c_str(), "r");
        reader->seek(sizeof(float)*128, SEEK_SET);
        file_readers.push_back(reader);    
    }
    cerr<<"Merging the final fdm"<<endl;    
    size_t buffer_size=sizeof(float)*(size_t)oSize[0]*(size_t)oSize[2];
    for (int y=0; y<oSize[1]; y++){ 

        memset(data, 0, buffer_size);                
        for (i=0;i<myFileNum; i++){
            size_t s= (size_t)mySplitLines[i][1]*(size_t)oSize[0]*sizeof(float);
            file_readers[i]->read(dummy, s);
            for (int x=0; x <oSize[0]; x++ ){
                size_t dummy_offset= (size_t)x*mySplitLines[i][1]+(size_t)myMergeLines[i][0];
                size_t offset=(size_t)x*(size_t)oSize[2]+(size_t)myRealLines[i][0];
                memcpy(data+offset,dummy+dummy_offset, sizeof(float)*myMergeLines[i][1]);
                offset+=myMergeLines[i][1];
            }
        }
        fa.write((char*) data, buffer_size);
    }
    fa.close();
    delete [] data;
    delete [] dummy;
    for (i=0; i<myFileNum; i++){
        file_readers[i]->close();
        delete file_readers[i];
    }
}
template<typename T>
static void printVect(const T& v, const char* info){
    cout<<info<<": [";
    cout<<v[0]<<" , "<<v[1]<<" , "<<v[2]<<"]"<<endl;
}


void vpVolumeProxy::writeHeader(FileAccess& fa, BaseGeometry* geom) {
    vString path = vString(this->myToSaveAs.c_str());
    GenericPath gp(path);
    string mode = "wb";
    
    if (!fa.open(path.c_str(), mode.c_str())) {
        fprintf(stderr, "Can't open %s\n", path.c_str());
        exit(-1);
    }
    BaseGeometry currGeom = myPredictedGeometry;
    if (geom) {
        currGeom = *geom;
    }
    
    FdmCube fdmCube;
    if (!myAttrName.isEmpty())
    fdmCube.setAttrName(myAttrName);
    
    Float32 header[128];
    fdmCube.fillHeader(currGeom, header);

#ifdef AVOID_TO_DO_THAT    
    
     
    Vect4d myOrigin = currGeom.getOrigin();
    Vect4d myStep = currGeom.getStep();
    Vect4i mySize = currGeom.getSize();
    Vect4i myMin = currGeom.getMin();
    Vect4i myMax = currGeom.getMax();
    Vect4i myInc = currGeom.getInc();   

//    Vect4d myOrigin = this->myPredictedGeometry.getOrigin();
//    Vect4d myStep = this->myPredictedGeometry.getStep();
//    Vect4i mySize = this->myPredictedGeometry.getSize();
//    Vect4i myMin = this->myPredictedGeometry.getMin();
//    Vect4i myMax = this->myPredictedGeometry.getMax();
//    Vect4i myInc = this->myPredictedGeometry.getInc();

    cout<<"Write header for "<<myToSaveAs<<endl;
    printVect(myOrigin, "Origin");
    printVect(myStep, "Step");
    printVect(mySize,"Size");
    printVect(myMin, "Min Lines");
    printVect(myMax, "Max Lines");
    printVect(myInc, "Increment");
    // fill the header

    float header[128];
    memset(header, 0, sizeof (float)*128);
    header[0] = myOrigin[0];
    header[1] = (Float32) mySize[0];
    header[2] = myStep[0];

    header[3] = myOrigin[1];
    header[4] = (Float32) mySize[1];
    header[5] = myStep[1];

    header[6] = myOrigin[2];
    header[7] = (Float32) mySize[2];
    header[8] = myStep[2];

    // format unused
    header[9] = getEndianMagicFloat32();
    header[10] = 0.0;
    header[11] = 0.0;

    header[14] = myMin[0];
    header[15] = myMin[1];

    header[16] = myInc[0];
    header[17] = myInc[1];
#endif
    fa.bfwrite((char*) header, sizeof (float)*128);
}

// due to the limitation of ZFILE, each trunk is limited below 4G. For reason
// check ZFILE header
void vpVolumeProxy::mergeILine() {

    FileAccess fa;
    writeHeader(fa);
    Volume* vol = 0;

    for (int i = 0; i<this->myFileNum; ++i) {
        vol = new Volume;
        string theFile = myTmpFileNames[i];
        vol->load(theFile.c_str());
        Cube* c = vol->getCube();
        Vect4i size = vol->getSize();
        float* data = (float*) c->getData();

        size_t dataSize = (size_t) size[0]*(size_t) size[2];
        dataSize *= (size_t)this->myMergeLines[i][1];
        size_t offset = (size_t)this->myMergeLines[i][0]*(size_t) size[0] *(size_t)size[2];
        fa.bfwrite((char*) (data + offset), dataSize * sizeof (float));
        delete vol;
    }
    fa.close();
}

BaseGeometry mergeGeometry(const BaseGeometry& geom1,const BaseGeometry& geom2) {
    BaseGeometry geom;
    int nDim=2;
    Vect4i min1 = geom1.getMin();
    Vect4i min2 = geom2.getMin();
    for(int i=0;i<=nDim;i++)
        min1[i]=MIN(min1[i],min2[i]);
 
    Vect4i max1 = geom1.getMax();
    Vect4i max2 = geom2.getMax();
    for(int i=0;i<=nDim;i++)
        max1[i]=MAX(max1[i],max2[i]);

    
    Vect4i inc1 = geom1.getInc();
    Vect4i inc2 = geom2.getInc();
    for(int i=0;i<=nDim;i++)
        inc1[i]=MAX(inc1[i],inc2[i]);
    
    Vect4d orig1 = geom1.getOrigin();
    Vect4d orig2 = geom2.getOrigin();
    for(int i=0;i<=nDim;i++)
        orig1[i]=MIN(orig1[i],orig2[i]);
        
    Vect4d step1 = geom1.getStep();
    Vect4d step2 = geom2.getStep();
    for(int i=0;i<=nDim;i++)
        step1[i]=MAX(step1[i],step2[i]);
    

    // Recompute the size
    Vect4i size = geom1.getSize();
    for (int i = 0; i <= nDim; i++)
        size[i] = (int) (max1[i] - min1[i]) / inc1[i] + 1;

    // And confirm the max
    for (int i = 0; i <= nDim; i++)
        max1[i] = (int) (size[i] - 1) * inc1[i] + min1[i];
     
    geom.setMin(min1);
    geom.setInc(inc1);
    geom.setMax(max1);
    geom.setSize(size);
    geom.setOrigin(orig1);
    geom.setStep(step1);
    
    
    return geom;    
}


void vpVolumeProxy::globalMerge() {
    // Compute the final geometry of all cubes
    BaseGeometry originalGeom = myPredictedGeometry;

    for (int i = 0; i < this->myFileNum; ++i) {
        Volume vol;
        string theFile = myTmpFileNames[i];
        vol.loadHeader(theFile.c_str());
        originalGeom = mergeGeometry(originalGeom, vol);
    }

    FileAccess fa;
    writeHeader(fa, &originalGeom);  // write header (it's most likely wrong but help to allocate size at the beginning of the file)

    Vect4i final_size = originalGeom.getSize();
    size_t traceSize = (size_t) final_size[2];
    float NOVALUE = 0;
    // vUtl::setNoValue( NOVALUE );

    float dummy[final_size[2]];
    for (int i = 0; i < final_size[2]; i++)
        dummy[i] = NOVALUE;

    float trace[final_size[2]];
    memset(dummy, 0, sizeof(float) * final_size[2]);

    // load from chunk
    for (int il = 0; il < final_size[1]; il++) {
        for (int xl = 0; xl < final_size[0]; xl++) {
            float *T = getTrace(xl, il, trace, true);
            fa.bfwrite((char*) T == 0 ? dummy : T, traceSize * sizeof(float));
        }
    }

    fa.close();
}



void vpVolumeProxy::mergeXLine() {
    FileAccess fa;
    writeHeader(fa);
    Volume* vol = 0;
    size_t prev_nx = myPredictedGeometry.getSize()[0];
    for (int i = 0; i<this->myFileNum; ++i) {
        vol = new Volume;
        string theFile = myTmpFileNames[i];
        vol->load(theFile.c_str());
        Cube* c = vol->getCube();
        Vect4i size = vol->getSize();
        for (int x = myMergeLines[i][0]; x<myMergeLines[i][0]+myMergeLines[i][1]; x++) {
            for (int y = 0; y < size[1]; y++) {
                // get each trace
                float* oneTr = c->getTrace(x, y, 0);
                // calculate new offset in the file
                size_t offset = sizeof(float)*(size_t) ((size_t)y * (size_t)prev_nx + 
                                                        (size_t)x+(size_t)mySplitLines[i][0])*(size_t) size[2] 
                  + sizeof (float)*128;
                fa.seek(offset, SEEK_SET);
                fa.bfwrite((char*) oneTr, sizeof (float)*(size_t)size[2]);
            }
        }
        delete vol;
    }
    fa.close();
}

// always save as fdm

void vpVolumeProxy::merge() {
    if (myDirection == ILINE) {
        mergeILine();
    } else if (myDirection == XLINE) {
        mergeXLine();
    } else {
        mergeZLine();
    }
    cleanUp();
}


Volume* vpVolumeProxy::loadSubsetInZ(Volume* vol, char* name, int beg, int len){
    // in xxxCube, we do not have interfaces to load subset in z, implement
    // it here
    // record the old geometry
    BaseGeometry geom0= *vol;    
    Vect4i lmin = geom0.getMin();
    Vect4i linc = geom0.getInc();
    Vect4i lmax = geom0.getMax();
    Vect4i size = geom0.getSize();
    Vect4d orig = geom0.getOrigin();
    Vect4d step = geom0.getStep();
    // based on the input of begin and end prepare the new Geometry  
    Vect4i oSize=size;
    
    orig[2]+=step[2]*beg; // origin
    lmin[2]+=linc[2]*beg; // min line
    size[2]= len;
    lmax[2]= linc[2]*(size[2]-1)+lmin[2]; // max line
  
    // prepare the new geometry
    BaseGeometry geom;
    geom.setMin(lmin);
    geom.setInc(linc);
    geom.setMax(lmax);
    geom.setSize(size);
    geom.setOrigin(orig);
    geom.setStep(step);
    // create a Volume
    Volume* subVol= new Volume;
    subVol->createFdm(name, geom);
    
    // now start to read it
    // get the subvolume data
    float* data= (float*)subVol->getData();
    const char* path=vol->getPath();
    FileAccess fa;
    assert(fa.open(path, "r"));
    size_t headerSize= sizeof(float)*128;
    assert(fa.seek(headerSize, SEEK_SET)>=0);
    
    // chunk
    size_t chunk_size= oSize[0]*oSize[2];
    float *chunk= new float[chunk_size];
    
    for (int y=0; y<size[1]; y++){
        // read xz slice
        float *data_ptr=data+(size_t)y*(size_t)size[0]*(size_t)size[2];
        
        assert(sizeof(float)*chunk_size==fa.read((char*)chunk, sizeof(float)*chunk_size));
        for (int x=0; x<size[0]; x++){
            size_t orig_offset=(size_t)beg+x*(size_t)oSize[2];
            size_t cur_offset=(size_t) x * (size_t) size[2];
            memcpy(data_ptr+cur_offset, chunk+orig_offset, size[2]*sizeof(float));
        }
    }
    fa.close();
    delete [] chunk;
    return subVol;
}

/* note, we only handle j in the increase order, otherwise, big penalty on IO time
 be careful in using this API, the last volume we be freed by destructor
*/
float* vpVolumeProxy::getTrace(int i, int j, float* buf, bool fromChunk ){ 
    Vect4i size = myPredictedGeometry.getSize();
    if (myFileNum <= 0 || i < 0 || i >= size[0] || j<0 || j >= size[1] ){
        fprintf(stderr, FL("Query range out of predicted geometry"));
        return 0;
    }
    
    int curIndex=0;
    // find the best trunk, we use the last trace of real trunk to decide if we need to load
    while(curIndex < myFileNum && j > (myRealLines[curIndex][0] + myRealLines[curIndex][1] - 1) ){
        curIndex++;
    }
    
    if (curIndex >= myFileNum){
        cerr<<"ERROR: marching volume result in failure"<<endl;
        return 0;
    }

    if (curIndex != myMarchIndex)
            delete myMarchVolume;

    myMarchIndex = curIndex;
    myMarchVolume = this->getVolume(myMarchIndex,fromChunk);

    Cube* cube= myMarchVolume->getCube();
    if (cube == 0){
        cerr<<"ERROR: myMarchVolume is null"<<endl;
        return 0;
    }
    
    // DD- must convert as the geometry of the chunk can be different (no trace)
    int chunkXL = cube->linToGrd(SurveyGeometry::XLINE, myPredictedGeometry.grdToLin(SurveyGeometry::XLINE, i));
    int chunkIL = cube->linToGrd(SurveyGeometry::INLINE, myPredictedGeometry.grdToLin(SurveyGeometry::INLINE, j));
    
    // we are handling fdm, the third parameter dummy can be 0
   return cube->getTrace(chunkXL, chunkIL, buf);
}



const BaseGeometry& vpVolumeProxy::getGeometry() const {
    return myPredictedGeometry;
}

void vpVolumeProxy::setPredictedGeometry(const BaseGeometry& geom) {
    myInitGeometry = true;
    myPredictedGeometry = geom;
} // set the geometry 



void vpVolumeProxy::setSubVolMultiple(int xscale, int yRatio, int zRatio) {
    myMultipleRatio[0]=xscale;
    myMultipleRatio[1]=yRatio;
    myMultipleRatio[2]=zRatio;
}



void vpVolumeProxy::setChunkSize( int nbOfSlices ) {
    mySlicePerChunk=nbOfSlices;
    setSubVolMultiple(1,1,1);
}

int vpVolumeProxy::getChunkSize() const {
    if (myComputedSlicePerChunk != -1)
        return myComputedSlicePerChunk;
    
    return mySlicePerChunk;
}
       
void vpVolumeProxy::setAttrName(const vString& attr) {
    myAttrName=attr;
}


