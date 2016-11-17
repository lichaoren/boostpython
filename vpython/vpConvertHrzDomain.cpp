#include <boost/python.hpp> 
#include "vpConvertHrzDomain.hpp"
#include "vlFdmCube.hpp"
#include "vlHorizonGroup.hpp"
#include "vlVolume.hpp"
#include "vmHGDomainConvertor.hpp"


#include "vpVolumeProxy.hpp"
#include "vpPythonContext.hpp"
#include "vlPythonInterpreter.hpp"

#include <iomanip>
#include <locale>
#include <sstream>

vpConvertHrzDomain::vpConvertHrzDomain() :
myInputHG(0),
myConvertType(DEPTH2TIME),
myVelocityOpt(BLOCKY)
{
    
}

vpConvertHrzDomain::~vpConvertHrzDomain() 
{
    if(myInputHG)
        delete myInputHG;
}

void vpConvertHrzDomain::exe()
{
    if(!myInputHG)
    {
        fprintf(stderr, "Invalid inputs. Conversion failed.");
        return;
    }
    myConvertor.setInputHG(myInputHG);
    
    Volume *vel = new Volume, * uvel = 0;
    vel->load(myVelPath.c_str());
    if (!vel)
    {
        cerr << "ERROR: cannot load data: " << myVelPath << endl;
        exit(-1);
    }
    
    if(myConvertType == DEPTH2DEPTH)
    {
        uvel = new Volume;
        uvel->load(myUpdatedVelPath.c_str());
        if (!uvel)
        {
            cerr << "ERROR: cannot load data: " << myUpdatedVelPath << endl;
            exit(-1);
        }
    }
    
    if(process(vel, uvel))
        myConvertor.save(myOutputPath.c_str());
    else
        fprintf(stderr, "Horizon Domain Converting failed !");
    
    delete vel;
    if(uvel)
        delete uvel;
}

/**
 * exe for chunk, save the functions for future development
 */
/*
void vpConvertHrzDomain::exe()
{
    if(!myInputHG)
    {
        fprintf(stderr, "Invalid inputs. Conversion failed.");
        return;
    }
    myConvertor.setInputHG(myInputHG);
    
    vpVolumeProxy pxyVel; 
    if(myConvertType == DEPTH2DEPTH)
    {
        pxyVel.setSubVolSize(2); // 2 GB
       
    }
    else
    {
        pxyVel.setSubVolSize(4); // 4 GB
    }
    
    
    if (!pxyVel.isToSplit(myVelPath.c_str()))
    {
        Volume *vol= new Volume;
        vol->load(myVelPath.c_str());
        if (!vol)
        {
            cerr<<"ERROR: cannot load data: " << myVelPath << endl;
            exit(-1);
        }
        
        if (processChunk(vol))
            myConvertor.save(myOutputPath.c_str());
        else
            fprintf(stderr, "Horizon Domain Converting failed !");
        
        delete vol;
        return;
    }
    
    pxyVel.setOriginPath(myVelPath.c_str());
    pxyVel.split();
    int nv = pxyVel.getNumVolume();
    
    bool res = true;
    for (int i = 0; i < nv; i++)
    {
        Volume* velVol = pxyVel.getVolume(i);
        res = processChunk(velVol);
        delete velVol;

        string Result; // string which will contain the result

        ostringstream convert; // stream used for the conversion

        convert << i; // insert the textual representation of 'Number' in the characters in the stream

        Result = convert.str(); // set 'Result' to the contents of the stream

        vString np = myOutputPath + vString(Result.c_str());
        vString np2 = myOutputPath + vString(Result.c_str()) + "input";
        myConvertor.save(np.c_str());
        myInputHG->setPath(np2.c_str());
        myInputHG->save();
        
        if(!res)
            break;
    }

    if (res)
        myConvertor.save(myOutputPath.c_str());
    else
        fprintf(stderr, "Horizon Domain Converting failed !");

    pxyVel.cleanUp();
    
}

bool vpConvertHrzDomain::processChunk(Volume * Vel)
{
    vpVolumeProxy pxyUpdatedVel;
    bool isToSplit = false;
    if(myConvertType == DEPTH2DEPTH)
    {
        pxyUpdatedVel.setSubVolSize(2); // 2GB
        isToSplit = pxyUpdatedVel.isToSplit(myUpdatedVelPath.c_str());
    }
    
    Volume* uVel = 0;
    if (myConvertType == DEPTH2DEPTH)
    {
        if(!isToSplit)
        {
            uVel = new Volume;
            uVel->load(myUpdatedVelPath.c_str());
            if (!uVel)
            {
                cerr << "ERROR: cannot load data: " << myUpdatedVelPath << endl;
                exit(-1);
            }
            bool res = process(Vel, uVel);
            delete uVel;
            return res;
        }
        else
        {
            pxyUpdatedVel.setOriginPath(myUpdatedVelPath.c_str());
            pxyUpdatedVel.split();
            int nuv = pxyUpdatedVel.getNumVolume();
            bool res = true;
            for (int j = 0; j < nuv; j++)
            {
                uVel = pxyUpdatedVel.getVolume(j);
                res = process(Vel, uVel);
                delete uVel;
                
                if(!res)
                {
                    pxyUpdatedVel.cleanUp();
                    return false;
                }
            }
            
            pxyUpdatedVel.cleanUp();
        }
    }
    else
    {
         return process(Vel, uVel);
    }
    
    return true;
}
*/
bool vpConvertHrzDomain::process(Volume * Vel, Volume * updatedVel)
{
    if( !myInputHG || !Vel || (myConvertType == DEPTH2DEPTH && !updatedVel) )
    {
        fprintf(stderr, "Invalid inputs. Conversion failed.");
        return false;
    }
    if(!Vel->getCube())
    {
        fprintf(stderr, "Invalid inputs. Conversion failed.");
        return false;
    } 
    
    FdmCube * vcube = dynamic_cast<FdmCube*> (Vel->getCube());
    
    if ( vcube == 0 )
    {
        fprintf(stderr, "Conversion Failed");
        return false;
    }
    
    FdmCube * vcube2 = 0;
    if(myConvertType == DEPTH2DEPTH && updatedVel->getCube())
    {
        vcube2 = dynamic_cast<FdmCube*> (updatedVel->getCube());
        if (vcube2 == 0)
        {
            fprintf(stderr,"Updated velocity model is invalid!");
            return false;
        }
    }

    myConvertor.setVelModel(vcube, vcube2);
    myConvertor.setVelLinear(myVelocityOpt == LINEAR);
    bool rc;
    if (myConvertType == DEPTH2TIME)
        rc = myConvertor.d2t();
    else if (myConvertType == TIME2DEPTH)
        rc = myConvertor.t2d();
    else if (myConvertType == DEPTH2DEPTH)
        rc = myConvertor.d2d();
    else if (myConvertType == TIME_SHIFT)
        rc = myConvertor.applyShift();
    else
    {
        fprintf(stderr,"Conversion domain is invalid!");
        return false;
    }
    return rc;

    
}

void vpConvertHrzDomain::setInputHG(vString path)
{
    vgHorizonGroup *hg = new vgHorizonGroup;
    if (!hg->load(path.c_str())){
        cerr<<"Failed to load Horizon: "<<path<<endl;
        exit(-1);
    }
    myInputHG=hg;
}

void vpConvertHrzDomain::exposePython()
{
    using namespace boost::python;
    
    enum_<vpConvertHrzDomain::ConvertType>("ConvertType")
    .value("DEPTH_TO_TIME", vpConvertHrzDomain::DEPTH2TIME)
    .value("TIME_TO_DEPTH", vpConvertHrzDomain::TIME2DEPTH)
    .value("DEPTH_TO_DEPTH", vpConvertHrzDomain::DEPTH2DEPTH)
    .value("TIME_SHIFT", vpConvertHrzDomain::TIME_SHIFT);
    
    enum_<vpConvertHrzDomain::VelocityOpt>("VelocityOpt")
    .value("BLOCKY", vpConvertHrzDomain::BLOCKY)
    .value("LINEAR", vpConvertHrzDomain::LINEAR);
    
    class_<vpConvertHrzDomain>("ConvertHrzDomain")
    .def("setConvertType", &vpConvertHrzDomain::setConvertType)
    .def("setVelocityOpt", &vpConvertHrzDomain::setVelocityOpt)
    .def("setInputHorizonPath", &vpConvertHrzDomain::setInputHG)
    .def("setShiftVolume", &vpConvertHrzDomain::setVelPath)
    .def("setVelocityModelPath", &vpConvertHrzDomain::setVelPath)
    .def("setUpdatedVelocityModelPath", &vpConvertHrzDomain::setUpdatedVelPath)
    .def("exe", &vpConvertHrzDomain::exe)
    .def("setOutputPath", &vpConvertHrzDomain::setOutputPath);
}
