/* 
 * File:   vpConvertHrzDomain.hpp
 * Author: ruwu
 *
 * Created on January 27, 2016, 3:30 PM
 */

#ifndef VPCONVERTHRZDOMAIN_HPP
#define	VPCONVERTHRZDOMAIN_HPP

#include "vString.hpp"
#include "vmHGDomainConvertor.hpp"

class Volume;
class vgHorizonGroup;

class vpConvertHrzDomain {
public:

    enum ConvertType {
        DEPTH2TIME = 0,
        TIME2DEPTH = 1,
        DEPTH2DEPTH = 2,
        TIME_SHIFT = 3
    };

    enum VelocityOpt {
        BLOCKY = 0,
        LINEAR = 1
    };

    vpConvertHrzDomain();
    ~vpConvertHrzDomain();
    static void exposePython();

    void exe();

    void setConvertType(int ConvertType) {
        this->myConvertType = ConvertType;
    }

    void setVelocityOpt(int VelocityOpt) {
        this->myVelocityOpt = VelocityOpt;
    }

    void setInputHG(vString path);

    void setOutputPath(vString myOutputPath) {
        this->myOutputPath = myOutputPath;
    }

    void setUpdatedVelPath(vString myUpdatedVelPath) {
        this->myUpdatedVelPath = myUpdatedVelPath;
    }

    void setVelPath(vString myVelPath) {
        this->myVelPath = myVelPath;
    }

private:
    bool process(Volume * Vel, Volume * updatedVel);
    bool processChunk(Volume * Vel);

    vgHorizonGroup * myInputHG;
    vString myOutputPath, myVelPath, myUpdatedVelPath;

    int myConvertType;
    int myVelocityOpt;
    
    vmHGDomainConvertor  myConvertor;

};

#endif	/* VPCONVERTHRZDOMAIN_HPP */

