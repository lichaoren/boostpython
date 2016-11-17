/* 
 * File:   vpVolumeAPI.cpp
 * Author: Dominique DOLEDEC
 * 
 * Created on May 26, 2016, 3:57 PM
 */
#include <boost/python.hpp>

#include "FSConstants.hpp"
#include "FS.hpp"
#include "GenericPath.hpp"

#include "vlFdmCube.hpp"
#include "vlSurveyRegion.hpp"
#include "vlPythonInterpreter.hpp"
#include "vlVolume.hpp"
#include "vlVGCube.hpp"
#include "VGCubeStorage.hpp"

#include "vmGridDef.hpp"


#include "vpVolumeAPI.hpp"
#include "vpVolumeProxy.hpp"
#include "vpMathAPI.hpp"


        
extern "C" {
  char* FS_scratchname(const char*, char *, size_t);
};




#define DEFAULT_BLOCK_SIZE 5   // default chunk size in Go

namespace vpVolumeAPI {

    /**
     *  the limited support of Volume arithmic operations
     * Note: these independent functions will be built to python
     * Volume object to make it "looks like" the Volume built in
     * methods
     * @param self operands
     * @param var operands
     * @return new volume (python manage its memory)
     */
    template<typename T>
    Volume* VolumeAdd(Volume* self, T var) {
        vpMathAPI math;
        return math.plus<Volume*, T>(self, var);
    }

    template<typename T>
    Volume* VolumeRadd(Volume* self, T var) {
        vpMathAPI math;
        return math.plus<T, Volume*>(var, self);
    }

    template<typename T>
    Volume* VolumeSub(Volume* self, T var) {
        vpMathAPI math;
        return math.minus<Volume*, T>(self, var);
    }

    template<typename T>
    Volume* VolumeRsub(Volume* self, T var) {
        vpMathAPI math;
        return math.minus<T, Volume*>(var, self);
    }

    template<typename T>
    Volume* VolumeMul(Volume* self, T var) {
        vpMathAPI math;
        return math.multiply<Volume*, T>(self, var);
    }

    template<typename T>
    Volume* VolumeRmul(Volume* self, T var) {
        vpMathAPI math;
        return math.multiply<T, Volume*>(var, self);
    }

    template<typename T>
    Volume* VolumeDiv(Volume* self, T var) {
        vpMathAPI math;
        return math.divide<Volume*, T>(self, var);
    }

    template<typename T>
    Volume* VolumeRdiv(Volume* self, T var) {
        vpMathAPI math;
        return math.divide<T, Volume*>(var, self);
    }

    template<typename T>
    Volume* VolumeMod(Volume* self, T var) {
        vpMathAPI math;
        return math.mod<Volume*, T>(self, var);
    }

    template<typename T>
    Volume* VolumeRmod(Volume* self, T var) {
        vpMathAPI math;
        return math.mod<T, Volume*>(var, self);
    }



    /** A couple of helpers
     * \code
     * help (Volume.save)
     * \endcode
     */
    static const char* helpVolume();
    static const char* helpSave();
    static const char* helpSaveAs();
    static const char* helpLoad();
    static const char* helpSub();
    static const char* helpGetSize();
    static const char* helpSetValue();

    /**
     * With boost we can add methods to the base class Volume and avoid overloading.
     */
    static Volume* cloneVolume(Volume* vol);
    static Volume* createVolume(const BaseGeometry& geom);
    static int vmVolumeAPI_load(Volume* vol, const char* path);
    static BaseGeometry* getGeometry(Volume* vol, const char* path = 0);
    static bool loadSubsetRegion(Volume* vol, const char* path, const SurveyRegion& sr);
    static bool loadVolumeSubset(Volume* vol, const char*path, Vect2i xl, Vect2i il);
    static boost::python::list getTrace(Volume* vol, const int il, const int xl);
    static bool setTrace(Volume* vol, const int il, const int xl, const boost::python::list& tpl);
    
       
    static  boost::python::list chunkVolume(vString inputName, vString outputName, int nbXline);
    static bool chunkGeoVoxel(vString inputName, vString outputName, const SurveyRegion& sr);
    
    // Remove all ambiguity/overloads
    int (Volume::*VolumeSaveAsWrap)(const char*) = &Volume::saveAs;
    bool (Volume::*loadVolumeHeader)(const vString&) = &Volume::loadHeader;
    Vect4d(Volume::*lin2logReturn)(const Vect4d&) const = &Volume::lin2log;
    double (Volume::*getValByInd)(const int, const int, const int) const = &Volume::getValue;
    double (Volume::*getValByIndArr)(const Vect4i&) const = &Volume::getValue;
    double (Volume::*getValByLog)(const Vect4d&) const = &Volume::getValue;

    void (Volume::*setValByInd)(const int, const int, const int, const double) = &Volume::setValue;
    void (Volume::*setValByIndArr)(const Vect4i&, const double) = &Volume::setValue;
    void (Volume::*setValByLog)(const Vect4d&, const double) = &Volume::setValue;

    BOOST_PYTHON_FUNCTION_OVERLOADS(vgVolume_geometry_overloads, getGeometry, 1, 2)


    void exposePython() {
        using namespace boost::python;

        // It was defined as a function (I add it available from the class also)
        def("createVolume", createVolume, return_value_policy<manage_new_object>());
        def("chunkVolume", chunkVolume);
        def("chunkGeoVoxel", chunkGeoVoxel);
        

        class_<Volume,  auto_ptr<Volume>, boost::noncopyable>("Volume")
                .def("loadSubset", loadVolumeSubset)
                .def("createVolume", createVolume, return_value_policy<manage_new_object>())
                .def("clone", cloneVolume, return_value_policy<manage_new_object>())
                .def("load", vmVolumeAPI_load, helpLoad())
                .def("save", &Volume::save, helpSave())
                .def("saveAs", VolumeSaveAsWrap, helpSaveAs())
                .def("unload", &Volume::unload)
                .def("getGeometry", getGeometry, vgVolume_geometry_overloads()[return_value_policy<manage_new_object>()])
                .def("loadSubset", loadVolumeSubset)
                .def("loadSubsetRegion", loadSubsetRegion)
                .def("loadHeader", loadVolumeHeader)
                .def("getStep", &Volume::getStep)
                .def("setStep", &Volume::setStep)
                .def("getMin", &Volume::getMin)
                .def("setMin", &Volume::setMin)
                .def("getMax", &Volume::getMax)
                .def("setMax", &Volume::setMax)
                .def("getInc", &Volume::getInc)
                .def("setInc", &Volume::setInc)
                .def("getSize", &Volume::getSize)
                .def("setSize", &Volume::setSize)
                .def("grd2log", &Volume::grd2log)
                .def("grd2lin", &Volume::grd2lin)
                .def("lin2grd", &Volume::lin2grd)
                .def("lin2log", lin2logReturn)
                .def("log2lin", &Volume::log2lin)
                .def("log2grd", &Volume::log2grd)
                .def("wld2log", &Volume::wld2log)
                .def("log2wld", &Volume::log2wld)
                .def("getOrigin", &Volume::getOrigin)
                .def("setOrigin", &Volume::setOrigin)

                .def("getValue", getValByInd)
                .def("getValue", getValByIndArr)
                .def("getValue", getValByLog)
                .def("setValue", setValByInd)
                .def("setValue", setValByIndArr)
                .def("setValue", setValByLog)
                
                .def("getTrace", getTrace)
                .def("setTrace", setTrace)
                
                .def("getAttributeType", &Volume::getAttrName)
                .def("setAttributeType", &Volume::setAttrName)

                .def("__add__", VolumeAdd<Volume*>, return_value_policy<manage_new_object>())
                .def("__add__", VolumeAdd<float>, return_value_policy<manage_new_object>())
                .def("__radd__", VolumeRadd<Volume*>, return_value_policy<manage_new_object>())
                .def("__radd__", VolumeRadd<float>, return_value_policy<manage_new_object>())
                .def("__sub__", VolumeSub<Volume*>, return_value_policy<manage_new_object>())
                .def("__sub__", VolumeSub<float>, return_value_policy<manage_new_object>())
                .def("__rsub__", VolumeRsub<Volume*>, return_value_policy<manage_new_object>())
                .def("__rsub__", VolumeRsub<float>, return_value_policy<manage_new_object>())
                .def("__mul__", VolumeMul<Volume*>, return_value_policy<manage_new_object>())
                .def("__mul__", VolumeMul<float>, return_value_policy<manage_new_object>())
                .def("__rmul__", VolumeRmul<Volume*>, return_value_policy<manage_new_object>())
                .def("__rmul__", VolumeRmul<float>, return_value_policy<manage_new_object>())
                .def("__div__", VolumeDiv<Volume*>, return_value_policy<manage_new_object>())
                .def("__div__", VolumeDiv<float>, return_value_policy<manage_new_object>())
                .def("__rdiv__", VolumeRdiv<Volume*>, return_value_policy<manage_new_object>())
                .def("__rdiv__", VolumeRdiv<float>, return_value_policy<manage_new_object>())
                .def("__mod__", VolumeMod<Volume*>, return_value_policy<manage_new_object>())
                .def("__mod__", VolumeMod<float>, return_value_policy<manage_new_object>())
                .def("__rmod__", VolumeRmod<Volume*>, return_value_policy<manage_new_object>())
                .def("__rmod__", VolumeRmod<float>, return_value_policy<manage_new_object>());

    }

    int vmVolumeAPI_load(Volume* vol, const char* path) {
        return vol->load(path, NULL);
    }

    /**
     * Factory to create a volume based on geometry
     */
    Volume* createVolume(const BaseGeometry& geom) {
        Volume *vol = new Volume();
        if (vol == NULL)
            return NULL;
        vol->setGeometry(geom);
        FdmCube *f = new FdmCube();
        f->create(geom, "temp.fdm");
        vol->setCube(f);
        return vol;
    }

    bool loadVolumeSubset(Volume* vol, const char* path, Vect2i xl, Vect2i il) {

        if (vol->loadHeader(path)) {
            Vect2i gridmin, gridinc(1, 1), gridnum;
            Vect4i inc = vol->getInc();

            gridnum[0] = (xl[1] - xl[0]) / inc[0] + 1;
            gridnum[1] = (il[1] - il[0]) / inc[1] + 1;

            Vect4i gmin = vol->getMin();
            gridmin[0] = (xl[0] - gmin[0]) / inc[0];
            gridmin[1] = (il[0] - gmin[1]) / inc[1];

            vmGridDef *gd = new vmGridDef(gridmin, gridinc, gridnum);
            return vol->loadSubset(*gd, 0);
        }
        return false;
    }

    /**
     * List of samples in float. (10 time faster rather than accessing the volume with getValue(i,j,k) )
     * @param il, xl index. First trace is (0,0)
     */
    boost::python::list getTrace(Volume* vol, const int il, const int xl) {
        boost::python::list pyArray;
        Cube* cube = vol->getCube();
        if (cube) {
            Vect4i cubeSize = vol->getSize();
            float * dummy = new float[cubeSize[2]];
            float* array = cube->getTrace(il, xl, dummy);

            for (int i = 0; i < cubeSize[2]; ++i) {
                pyArray.append(array[i]);
            }
            if (dummy)
                delete [] dummy;
        }
        return pyArray;
    }

    /**
     * List of samples in float. (10 time faster rather than accessing the volume with getValue(i,j,k) )
     * @param il, xl index. First trace is (0,0)
     */
    bool setTrace(Volume* vol, const int il, const int xl, const boost::python::list& tpl) {
        Cube* cube = vol->getCube();
        if (!cube)
            return false;

        Vect4i cubeSize = vol->getSize();
        if (cubeSize[2] != boost::python::len(tpl)) {
            cerr << "Length of the tuple don't respect the trace length" << endl;
            return false;
        }
        float * dummy = new float[cubeSize[2]];
        float * array = cube->getTrace(il, xl, dummy);
        if (!array) {
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, " Failed in setTrace.");
            if (dummy)
                delete [] dummy;
            return false;
        }
        for (int i = 0; i < cubeSize[2]; ++i)
            array[i] = boost::python::extract<float>(tpl[i]);

        cube->setTrace(il, xl, array);
        if (dummy)
            delete [] dummy;

        return true;
    }

    Volume* cloneVolume(Volume* vol) {
        Volume* tmp = new Volume();
        //(*tmp) = (*vol);

        const Cube* input_cube = dynamic_cast<const FdmCube*> (vol->getCube());
        if (input_cube == NULL) {
            std::cerr << "We currently only support FdmCube\n";
            return tmp;
        }

        Cube* new_cube = new FdmCube(*((Cube*) input_cube)); // limited support for fdm format
        tmp->setCube(new_cube);

        return tmp;
    }

    BaseGeometry* getGeometry(Volume* vol, const char* path) {
        BaseGeometry* geom = new BaseGeometry();
        if (path)
            vol->loadHeader(path);
        else
            vol->loadHeader();

        (*geom) = (*vol); // copy
        return geom;
    }

    bool loadSubsetRegion(Volume* vol, const char* path, const SurveyRegion& sr) {
        if (vol->loadHeader(path)) {
            SurveyRegion *r = new SurveyRegion();
            *r = sr;
            vol->loadSubsetRegion(*r);
            //use full grid, region will be used to limit
            //use grid , not linear coordinate system
            vmGridDef *gd = new vmGridDef(Vect2i(0, 0),
                    Vect2i(1, 1),
                    Vect2i(vol->getSize()[0], vol->getSize()[1]));
            if(vol->loadSubset(*gd, r))
                return true;
        }
 
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, " Unable to load subset region.");
       
        return false;
    }



bool chunkGeoVoxel(vString inputName, vString outputName, const SurveyRegion& sr) {

        VGCubeStorage storage;
        VGCube cubeOrigin;
        cubeOrigin.setPath(inputName.c_str());
    
        vString tmpName = outputName;
        bool using_gz = tmpName.search(".gz") != strERROR;
        if (using_gz) {
            GenericPath gp(tmpName);
            PRJTYPE pt = gp.getProjectType();
            if (pt == ProjectType::REMOTEFILE) {
                char tempFileName[1024];
                tmpName = FS_scratchname("volume", tempFileName, 1024);
                tmpName += ".vol.gz";
            }
        }
        
        if (cubeOrigin.loadHeader() == false) {
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, " Unable to load target.");
            return false;
        }

        vmGridDef grid(Vect2i(0, 0),
                Vect2i(1, 1),
                Vect2i(cubeOrigin.getSize()[0], cubeOrigin.getSize()[1]));

        if (storage.writeSubset(cubeOrigin, tmpName,
                grid, &sr) == false) {
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, " Unable to write subset region.");
            return false;
        }

        if (using_gz) {
            vString rpath = outputName;
            rpath.replace("REMOTEFILE!","");
            
            if ( FS::cp(FS::FILE, vFilename(tmpName), vFilename(rpath)) != FS::SUCCESS ) {
                vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, " Unable to write to remote host.");
                return false;
            }
            remove(tmpName.c_str());
        }

        return true;
    }

    
    

    boost::python::list chunkVolume(vString inputName, vString outputName, int nbSubline) {

        boost::python::list listOfSubvolumes;
        vpVolumeProxy proxyIn;
        proxyIn.setSubVolSize(DEFAULT_BLOCK_SIZE, true);
        proxyIn.setOriginPath(inputName);
        proxyIn.setTaper(0);
        proxyIn.split();
        

        const BaseGeometry& outputGeom = proxyIn.getGeometry();

        Vect4i outSize = outputGeom.getSize();
        Vect2i chunkSize;
        chunkSize[0]= nbSubline; //dd1 outSize[0];
        chunkSize[1]= outSize[1];//dd1 nbXline;
        
        // simplify
//        if (chunkSize[0] != outSize[0]) {
//            return listOfSubvolumes;
//        }

        int nbSteps = (int) ceil(outSize[1] / (float) chunkSize[1]);

        for (int i = 0; i < nbSteps; i++ ) {
            BaseGeometry chunkGeom = outputGeom;
                
            Vect4i  size = chunkGeom.getSize();
            Vect4i  min  = chunkGeom.getMin();
            Vect4i  max =  chunkGeom.getMax();
            Vect4i  inc =  chunkGeom.getInc();
            Vect4d  orig = chunkGeom.getOrigin();
  
            // Limit
            if (i == nbSteps - 1) {
                int nbXlMax = (i+1)*chunkSize[1];
                size[1] = chunkSize[1] + (size[1]-nbXlMax);
                min[1] = min[1] + i * chunkSize[1]*inc[1];
                // max[1] = min[1] + size[1]*inc[1];
            } else {
                size[1] = chunkSize[1];
                min[1] = min[1] + i * chunkSize[1]*inc[1];
                max[1] = min[1] + chunkSize[1]*inc[1];
            }
              
            orig[1] = outputGeom.grdToLog(1, size[1]*i);

            chunkGeom.setSize(size);
            chunkGeom.setMin(min);
            chunkGeom.setMax(max);  
            chunkGeom.setOrigin(orig);  

            
            vString chunkName = outputName + "-" + vString::number(i) + ".fdm";
            vpVolumeProxy proxy;
            proxy.setToSaveAs(chunkName.c_str());
            proxy.setPredictedGeometry(chunkGeom);
            FileAccess fa;
            proxy.writeHeader(fa);

            int nx = size[0];
            int ny1 = chunkSize[1]*i;
            int ny2 = ny1+size[1];
            for (int iy = ny1; iy < ny2; ++iy) {
                if (iy % 100 == 0) {
                    cout << "Inline " << iy << " is done" << endl;
                }
                for (int ix = 0; ix < nx; ++ix) {
                    float* trace = proxyIn.getTrace(ix,iy);
                    if(!trace) {
                        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, 
                                " Failed to get Trace ");
                        boost::python::list emptyList;
                        return emptyList;
                    }
                   
                    fa.bfwrite((char*)trace, sizeof(float)*(size_t)size[2]);
                }
            }
            listOfSubvolumes.append(chunkName.c_str());
            fa.close();            
        }
        
        return listOfSubvolumes;
    }

    
    
    
    
    
    

    const char* helpVolume() {
        static std::string helpInfo = "Tornado-python Volume Documentation\n"
                "Object: Volume\n"
                "Methods: \n";
        static std::string example =
                "Example: create, load and save volume\n"
                "from Volume import Volume\n"
                "myVol=Volume()\n"
                "myVol.load('path_to_vol')\n"
                "myVol.saveAs('another_path_to_vol')\n"
                "\n";

        static std::string helpvolume = helpInfo
                + helpSave()
                + helpSaveAs()
                + helpLoad()
                + helpSub()
                + helpGetSize()
                + helpSetValue()
                + example;

        return helpvolume.c_str();
    }

    const char* helpSave() {
        static const char* helpInfo = "\tVolume.save:\n"
                "\t\tcategory: save method in Volume.Volume\n"
                "\t\tcall method: Volume.save()\n"
                "\t\tusage: to save data in Volume to disk/store where it is read from"
                "\n\n";
        return helpInfo;
    }

    const char* helpSaveAs() {
        static const char* helpInfo = "\tVolume.saveAs:\n"
                "\t\tcategory: saveAs method in Volume.Volume\n"
                "\t\tcall method: Volume.saveAs('path_to_vol')\n"
                "\t\tusage: to save data to  specified data path\n\n";
        return helpInfo;
    }

    const char* helpLoad() {
        static const char* helpInfo = "\tVolume.load:\n"
                "\t\tcategory: load method in Volume.Volume\n"
                "\t\tcall method: Volume.load('path_to_vol')\n"
                "\t\tusage: to load specified data path to python object\n\n";
        return helpInfo;
    }

    const char* helpSub() {
        static const char* helpInfo = "\tVolume.__sub__(MINUS operator)\n"
                "\t\tcategory: MINUS operator in Volume.Volume\n"
                "\t\tcall method: C=A-B\n"
                "\t\tusage: calculate the difference between Volume A-B\n"
                "\t\tnote: the MINUS operator will automatically return a Volume object\n"
                "\t\t      and there is no need to declare a C=Volume() at the beginning"
                "\n\n";
        return helpInfo;
    }

    const char* helpGetSize() {
        static const char* helpInfo = "\tVolume.getSize()\n"
                "\t\tcategory: get the 3 dims of the data contained in this volume\n"
                "\t\tcall method: (nx,ny,nz)=vol.getSize()\n"
                "\n\n";
        return helpInfo;
    }

    const char* helpSetValue() {
        static const char* helpInfo = "\tVolume.setValue(i,j,k,val)\n"
                "\t\tcategory: set val to the data at [i,j,k]\n"
                "\t\tcall method: vol.setValue(i,j,k,val)\n"
                "\n\n";
        return helpInfo;
    }

}



