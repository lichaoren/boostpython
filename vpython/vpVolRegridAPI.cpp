#include <boost/python.hpp>

#include "vpVolRegridAPI.hpp"

#include "vpVolumeProxy.hpp"
#include "vlPythonInterpreter.hpp"


using namespace std;


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpVolumeRegridsetValueType, vpVolumeRegrid::setValueType, 1, 2)


void vpVolumeRegrid::exposePython() {
    namespace bp = boost::python;

    bp::enum_<vpVolumeRegrid::ValType>("RegridValueType")
            .value("VALUE", vpVolumeRegrid::VALUE)
            .value("SLOWNESS", vpVolumeRegrid::SLOWNESS)
            .value("ANGLE", vpVolumeRegrid::ANGLE)
            .value("TTCOMP", vpVolumeRegrid::TTCOMP);


    // TODO: no implemented! (but provided in the documentation / source code)
    // I keep it, but no documentation is provided
    bp::enum_<vpVolumeRegrid::RInterpMode>("RegridInterpType")
            .value("TRIANGULATION", vpVolumeRegrid::RLINEAR)
            .value("NATURAL_NEIGHBOR", vpVolumeRegrid::RNATURAL_NEIGHBOR)
            .value("MINIMUM_CURVATURE", vpVolumeRegrid::RMINIMUM_CURVATURE)
#ifdef USE_ALSO_GPL_DELTA
            .value("DELTA_TRIANGULATION", vpVolumeRegrid::RLINEAR_DELTA)
#endif
            .value("NEAREST_NEIGHBOR", vpVolumeRegrid::RNEAREST_NEIGHBOR)
            .value("FOUR_POINT_LINEAR", vpVolumeRegrid::R_DEFINE);

//     TODO: remove, now we need it for backward compatibility!
    bp::enum_<vpVolumeRegrid::OutputType>("RegridOutputType")
            .value("FDM", vpVolumeRegrid::FDM)
            .value("VOXEL", vpVolumeRegrid::VOXEL);
    
    bp::enum_<vpVolumeRegrid::ResampleMode>("RegridResampleType")
            .value("LINEAR", vpVolumeRegrid::LINEAR)
            .value("BLOCKY", vpVolumeRegrid::BLOCKY)
            .value("LINEAR_TTS", vpVolumeRegrid::LINEAR_TTS);

    bp::scope local_scope = bp::class_<vpVolumeRegrid>("VolumeRegrid")
            .def("setInput", &vpVolumeRegrid::setInput)
            .def("setOutput", &vpVolumeRegrid::setOutput)
            .def("regrid", &vpVolumeRegrid::regrid)
            .def("useLine", &vpVolumeRegrid::setUseLine)
            .def("setValueType", &vpVolumeRegrid::setValueType, vpVolumeRegridsetValueType())
            .def("setResampleType", &vpVolumeRegrid::setResampleMode)
            .def("setInterpType", &vpVolumeRegrid::setInterpMode)            
            .def("setGeometry", &vpVolumeRegrid::setGeometry)
            ;
}




vpVolumeRegrid::vpVolumeRegrid() {
    setWithGui(false); // always disable any gui
    setUseLastSample(false, 0); // no use last sample
    setOutPutType(vmVolRegridModel::FDM); // only output fdm
    setLinearId((int) R_DEFINE);
    setInterpMode(R_DEFINE); // only for 4 point linear
}

vpVolumeRegrid::~vpVolumeRegrid() {
}


    
void vpVolumeRegrid::setValueType( ValType regridMode, 
        AngleConv::Type angleType) {
    setValType(regridMode);
    

    if (regridMode == ANGLE) {
        AngleConv angleConverter;
        angleConverter.setType(angleType);
        setAngleConv(angleConverter);
    }    
}



void vpVolumeRegrid::setInput(string input) {
    myInputFile = input;
}

void vpVolumeRegrid::setOutput(string output) {
    myOutputFile = output;
}

bool vpVolumeRegrid::regrid() {
    //error checking, we only port a few options, others return error
    if (getValType() != vmVolRegridModel::VALUE && 
            getValType() != vmVolRegridModel::SLOWNESS && 
            getValType() != vmVolRegridModel::ANGLE ) {
         
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                "Type of regridding unknown (VALUE/SLOWNESS/ANGLE)" );
        return false;
    }
    
    if (getInterpMode() != 5) {         
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                "Now only support Interpolation Methods of 4 points linear" );
        return false;
    }

    if ( myInputFile.empty() ) {         
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                "No input file provided " );
        return false;
    }
      
    if ( myOutputFile.empty() ) {         
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                "No output file provided " );
        return false;
    }
    
    

    /********************************************************************
     * Setup phase
     ********************************************************************/
    Vect4i minL = this->objGeom.getMin();
    Vect4i maxL = this->objGeom.getMax();
    Vect4d org = objGeom.getOrigin();
    Vect4d step = objGeom.getStep();
    Vect4i inc = objGeom.getInc();
    Vect4i num = objGeom.getSize();

    bool isUseLine = this->getUseLine();
    vector<double> geo_input;
    if (isUseLine) {
        // by line
        geo_input.push_back((double) minL[0]);
        geo_input.push_back((double) inc[0]);
        geo_input.push_back((double) maxL[0]);
        geo_input.push_back((double) num[0]);
        geo_input.push_back((double) minL[1]);
        geo_input.push_back((double) inc[1]);
        geo_input.push_back((double) maxL[1]);
        geo_input.push_back((double) num[1]);
        geo_input.push_back(org[2]);
        geo_input.push_back(step[2]);
        geo_input.push_back(org[2]+(num[2] - 1) * step[2]);
        geo_input.push_back((double) num[2]);
    } else {
        // xy coordinate
        geo_input.push_back(org[0]);
        geo_input.push_back((double) inc[0]);
        geo_input.push_back(step[0]);
        geo_input.push_back((double) num[0]);
        geo_input.push_back(org[1]);
        geo_input.push_back((double) inc[1]);
        geo_input.push_back(step[1]);
        geo_input.push_back((double) num[1]);
        geo_input.push_back(org[2]);
        geo_input.push_back(step[2]);
        geo_input.push_back(org[2]+(num[2] - 1) * step[2]);
        geo_input.push_back((double) num[2]);
    }
    this->setGeomDataFromVect(geo_input);

    //get input geometry
    Volume* dummy = new Volume;
    dummy->setCreateHint(Volume::eHintLSUB);
    if (!dummy->loadHeader(myInputFile.c_str())) {
        vString message;
        message << "Cannot open your provided volume: " << myInputFile.c_str();
        delete dummy;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message );
    }
    BaseGeometry inputGeom = *dummy;
    delete dummy;
    dummy = 0;

    // from now on, we do not care about the output file size
    // and we will directly write data into disk
    return myOnDiskRegrid(inputGeom);
}

bool vpVolumeRegrid::myOnDiskRegrid(BaseGeometry& inputGeom) {
    // will update myGD, myGD is the object geometry info
    if (deriveGeomData() == false) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, "Unable to deriveGeomData"  );
        return false;
    }
    // test if use blocky
    setUseBlocky();
    GeomData gd_input; //gd_input is the original data geometry info
    BaseGeometry geom = inputGeom;
    getGeoAndGD(geom, gd_input);
    GeomData targetGD = getGD(); //targetGD is the geometry we want to regrid to
    if (getWeight(targetGD, gd_input) == false)
        return false;

    return myOnDiskRegridHelper(targetGD, gd_input, geom);
}

// this is another version of regrid2 for direct IO purpose
// if you think it is dup, feel free to refactor it

bool vpVolumeRegrid::myOnDiskRegridHelper(const GeomData& gd, const GeomData& gd_old,
        const BaseGeometry& outputGeom) {

    // firstly write down the geometry
    vpVolumeProxy proxy;
    proxy.setToSaveAs(this->myOutputFile.c_str());
    proxy.setPredictedGeometry(outputGeom);
    FileAccess fa;
    proxy.writeHeader(fa);

    // now prepare for another proxy to handle input data
    size_t inputFileSize = sizeof (float)*(size_t) gd_old.nx * (size_t) gd_old.ny * (size_t) gd_old.nz;
    
    // set the default chunk size
    float subVolSize = 8.;
    if (inputFileSize > subVolSize * float(GEGA)) {
        subVolSize = subVolSize;
    } else {
        // trunk it to 2 pieces at least
        subVolSize = .5 *  float(inputFileSize) / float(GEGA);
    }
    vpVolumeProxy inProxy;
    inProxy.setOriginPath(this->myInputFile.c_str());
    inProxy.setSubVolSize(subVolSize);
    inProxy.setTaper(4);
    inProxy.split();


    // now we will loop to write each trace
    const int w_size = myW_idxs.size();
    vector<float> data_buf;
    data_buf.resize(gd_old.nz * (w_size + 1) + gd.nz);

    vector<float*> data_in_v;
    for (int iw = 0; iw < w_size; iw++)
        data_in_v.push_back(&data_buf[iw * gd_old.nz]);

    float* data_out0 = &data_buf[gd_old.nz * w_size];
    float* data_out1 = &data_buf[gd_old.nz * (w_size + 1)];


    cout<<"According to inline, the total number: "<<gd.ny<<endl;
    
    for (int iy = 0; iy < gd.ny; ++iy) {
        if (iy%100==0){
            cout<< "Inline "<< iy <<" is done" <<endl;
        }
        for (int ix = 0; ix < gd.nx; ++ix) {

            size_t idx = iy * gd.nx + ix;

            for (int iw = 0; iw < w_size; iw++) {
                int n_i = myW_idxs[iw][idx];
                int ix_wi = n_i / gd_old.ny;
                int iy_wi = n_i % gd_old.ny;
                memcpy(data_in_v[iw], inProxy.getTrace(ix_wi, iy_wi), sizeof(float)*gd_old.nz);
            }

            int iz;
            for (iz = 0; iz < gd_old.nz; ++iz) {
                data_out0[iz] = 0;
                if ((int) myValueType == 2) {
                    float c_v = 0;
                    float s_v = 0;
                    for (int iw = 0; iw < w_size; iw++) {
                        const float val = data_in_v[iw][iz];
                        float v = myAngConv.convertTo(val, AngleConv::RadSigned);
                        c_v += myWi[iw][idx] * cos(v);
                        s_v += myWi[iw][idx] * sin(v);
                    }
                    data_out0[iz] = atan2(s_v, c_v);
                } else {
                    for (int iw = 0; iw < w_size; iw++){
                        data_out0[iz] += myWi[iw][idx] * data_in_v[iw][iz];
                    }
                }

                if ((int) myValueType == 1 || (int) myValueType == 3)
                    if(data_out0[iz]!=0)
                        data_out0[iz] = 1 / data_out0[iz];
            }

            if (!regridLinearZ(gd, gd_old, data_out0, data_out1)) {
                return false;
            }

            for (iz = 0; iz < gd.nz; ++iz) {
                if ((int) myValueType == 1 || (int) myValueType == 3) {
                    if (data_out1[iz] != 0)
                        data_out1[iz] = 1 / data_out1[iz];
                }
                if ((int) myValueType == 2) {
                    data_out1[iz] = myAngConv.convertFrom(AngleConv::RadSigned,
                            data_out1[iz]);
                }
            }
            fa.bfwrite((char*)data_out1, sizeof(float)*(size_t)gd.nz);
        }
    }
    fa.close();
    return true;
}



