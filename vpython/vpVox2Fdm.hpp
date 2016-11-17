/* 
 * File:   vpVox2Fdm.hpp
 * Author: yanhliu
 *
 * Created on April 20, 2015, 5:43 PM
 */

#ifndef VPVOX2FDM_HPP
#define	VPVOX2FDM_HPP
#include "vlVolume.hpp"
#include "vlBaseGeometry.hpp"
#include "FS.hpp"
#include "vlDataPort.hpp"
class vpVox2Fdm {
public:
    vpVox2Fdm();
    bool isVoxel(const char* path);
    void convert2Fdm(const char* path);
    string getTempFile(){return myLocalFdmName;};
    void cleanUp();
    virtual ~vpVox2Fdm();
private:
    size_t fdmOffset(int i, int j, int k, int nx, int ny, int nz);
    bool myIsVoxel;
    BaseGeometry myGeom;
    string myLocalFdmName;
    float myValueTable[256];
};

#endif	/* VPVOX2FDM_HPP */

