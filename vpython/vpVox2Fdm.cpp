/* 
 * File:   vpVox2Fdm.cpp
 * Author: yanhliu
 * 
 * Created on April 20, 2015, 5:43 PM
 */

#include "vpVox2Fdm.hpp"
#include "GenericPath.hpp"
#include "vlCubeFileCategory.hpp"
#include "vlVGCube.hpp"

vpVox2Fdm::vpVox2Fdm() {
    myIsVoxel = false;
}

vpVox2Fdm::~vpVox2Fdm() {
}

bool vpVox2Fdm::isVoxel(const char* path) {
    const char *capi = strchr(path, '!');
    const char *ext = strrchr(path, '.');
    vLCString lcExt(ext);
    GenericPath gp(path);
    PRJTYPE pt = gp.getProjectType();

    bool flag = false;
    // check by extension
    if (pt == ProjectType::REMOTEFILE) { // remote file
        if (strstr(path, ".vol") != (char*) NULL || strstr(path, ".VOL") != (char*) NULL) {
            flag = true;
        }
    } else if (lcExt == ".vol") { // local
        flag = true;
    } else if (isVGFile(path)) { // direct decide
        flag = true;
    }
    this->myIsVoxel = flag;
    return flag;
}

size_t vpVox2Fdm::fdmOffset(int i, int j, int k, int nx, int ny, int nz) {
    return (size_t) (j * nx + i)*(size_t) nz + k;
}

void vpVox2Fdm::convert2Fdm(const char* path) {
    Volume* dummy = new Volume;
    // load the header
    dummy->setCreateHint(Volume::eHintLSUB);
    if (!dummy->loadHeader(path)) {
        cerr << "Failed to load " << path << endl;
        assert(false);
    }
    // get the range of data
    Vect2d range = dummy->getRange();
    double myMinVal = range[0];
    double myMaxVal = range[1];
    // use the min/max to build up the value table
    memset(this->myValueTable, 0, sizeof (myValueTable));

    for (int i = 1; i < 256; i++) {
        double val = i * (myMaxVal - myMinVal) / 255.0 + myMinVal;
        myValueTable[i] = val;
    }
    myValueTable[0] = myMinVal;


    //grep geometry from vgCube
    BaseGeometry predictedGeometry = *dummy;

    delete dummy;
    // generate temporary files
    vString tmpFileName;
#if 1
    vString scratch = "";
#endif

#if 0
    vString scratch = "/auto/mntpnt/data2/devtest/tornado/yanhliu/Script_test/";
#endif

    tmpFileName = FS::getTempFilename("FDM");
    // will be scratch_path/+tmpfileName
    string full_path = string(scratch.c_str()) + tmpFileName.c_str() + ".fdm";
    this->myLocalFdmName = full_path;

    FileAccess fa;
    string mode = "wb";
    if (!fa.open(full_path.c_str(), mode.c_str())) {
        fprintf(stderr, "Can't open %s\n", full_path.c_str());
        assert(0);
    }

    Vect4d myOrigin = predictedGeometry.getOrigin();
    Vect4d myStep = predictedGeometry.getStep();
    Vect4i mySize = predictedGeometry.getSize();
    Vect4i myMin = predictedGeometry.getMin();
    Vect4i myMax = predictedGeometry.getMax();
    Vect4i myInc = predictedGeometry.getInc();
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

    //    header[12] = myRange[0];
    //    header[13] = myRange[1];

    header[14] = myMin[0];
    header[15] = myMin[1];

    header[16] = myInc[0];
    header[17] = myInc[1];

    fa.bfwrite((char*) header, sizeof (float)*128);

    // finished the header writting, now the hard work comes

    FileAccess fac;
    if (fac.open(path, "rb") == false) {
        fprintf(stderr, "ERROR: failed to open volume file: %s\n", path);
        assert(false);
    }
    // skip the header
    if (fac.seek(sizeof (struct VV_volume), SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: read error!\n");
        fac.close();
        assert(false);
    }
    size_t size_of_header = sizeof (float)*128;


    // try to load the data by 4 Gega at a time
    
    // calculate the size of one slice
    size_t slice_size = sizeof (unsigned char) * (size_t) mySize[0]*(size_t) mySize[1];
    // define the max size we can load at a time, for truncation 
    size_t max_trunk_size = (size_t) 4 * (size_t) ( 1024*(size_t)1024*size_t(1024));
    // calculate the number of slices for each load based on our memory budget
    int num_slice_per_trunk = (int)(ceil((float) max_trunk_size / (float) slice_size));
    // calculate how many truncation we can do 
    int num_of_trunk = (int)(ceil((float) mySize[2] / (float) num_slice_per_trunk));
    // give one trace the max budget
    float *trace = new float[ mySize[2]];
    cerr<<path<< "will be truncated to "<< num_of_trunk<<" pieces"<<endl;
    // loop to truncate
    for (int itrunk = 0; itrunk < num_of_trunk; itrunk++) {
        unsigned char *trunk; // truncation buffer
        int length; // for each truncation trace, how long it will be
        int firstZ; // the first slice number for each truncation
        size_t sizeOfTrunk;
        if (itrunk != num_of_trunk - 1) {
            length = num_slice_per_trunk;
            sizeOfTrunk = num_slice_per_trunk * mySize[0] * mySize[1];
            firstZ =mySize[2]- (itrunk+1)*num_slice_per_trunk;
        } else {
            length = mySize[2]-(num_of_trunk - 1) * num_slice_per_trunk;
            sizeOfTrunk = length * mySize[0] * mySize[1];
            firstZ = 0;
        }
        trunk = new unsigned char [sizeOfTrunk];
        size_t rd = fac.read(trunk, sizeOfTrunk);
        if (rd < sizeOfTrunk) {
            cerr << "Bad vg cube data" << endl;
            assert(-1);
        }

        // voxel geo is a weried data design, need to reverse the order when writing
        // the trace
        for (int ind = 0; ind < mySize[0] * mySize[1]; ind++) {
            int i = ind % mySize[0]; // translate to i, j
            int j = ind / mySize[0];
            size_t offset = fdmOffset(i, j, firstZ, mySize[0], mySize[1], mySize[2]);
            size_t res = fa.seek(sizeof (float)*offset + size_of_header, SEEK_SET);
            assert(res == 0);
            for (int tr = 0; tr < length; tr++) {

                size_t n = (size_t) i + (size_t) j * (size_t) mySize[0] +
                        (size_t) tr * (size_t) mySize[0] * (size_t) mySize[1];
                trace[length-tr-1] = myValueTable[trunk[n]]; // reverse the trunk trace
            }
            fa.write(trace, length * sizeof (float));
        }
        delete trunk;
        cerr<<"The truncation "<<itrunk+1<<" of " <<num_of_trunk<<" is done"<<endl;
    }
    delete trace;
    fa.close();
    fac.close();
    
    cerr<<"The original voxel volume "<<path<<" has been truncated" <<endl;
    cerr<<"Proceed to the next step..."<<endl;
}

void vpVox2Fdm::cleanUp() {
    if (!this->myLocalFdmName.empty()) {
        string cmd = "rm -rf " + myLocalFdmName;
        system(cmd.c_str());
    }
    memset(this->myValueTable, 0, sizeof (myValueTable));
    myLocalFdmName = "";
}

