#include <boost/python.hpp>
#include "vpVolumeDipCreator.hpp"

#include "vlAmiAttr.hpp"
#include "vlFdmCube.hpp"
#include "vlPythonInterpreter.hpp"

#include "vmCubeOperator.hpp"

#include "vpVolumeProxy.hpp"
#include "vqMemoryInfo.hpp"


BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(outputPathOverload, vpVolumeDipCreator::setOutputPath, 1, 2)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(dipOutputOverload, vpVolumeDipCreator::setDipOutput, 1, 3)
BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(aztetaOutputOverload, vpVolumeDipCreator::setAzThetaOutput, 1, 3)


void vpVolumeDipCreator::exposePython() {
    namespace bp = boost::python;

    bp::enum_<vpVolumeDipCreator::FillGapMode>("FillGapMode")
            .value("None", vpVolumeDipCreator::None)
            .value("Linear", vpVolumeDipCreator::Linear)
            .value("MinCurvature", vpVolumeDipCreator::MinCurvature);

    bp::enum_<vpVolumeDipCreator::VolumeDipUNIT>("VolumeDipUNIT")
            .value("RADIANS", vpVolumeDipCreator::RADIANS)
            .value("DEGREES", vpVolumeDipCreator::DEGREES)
            .value("METERS", vpVolumeDipCreator::METERS);

    bp::scope local_scope = bp::class_<vpVolumeDipCreator>("VolumeDipCreator")
            .def("run", &vpVolumeDipCreator::run)
            .def("setFilter", &vpVolumeDipCreator::setFilter)
            .def("setSubsampling", &vpVolumeDipCreator::setSubsamplingStep)
            .def("setNumVolSize", &vpVolumeDipCreator::setNumVolSize)

            .def("setOutputPath", &vpVolumeDipCreator::setOutputPath, outputPathOverload())
            .def("setDipOutput", &vpVolumeDipCreator::setDipOutput, dipOutputOverload())
            .def("setAzThetaOutput", &vpVolumeDipCreator::setAzThetaOutput, aztetaOutputOverload())

            .def("setFillGap", &vpVolumeDipCreator::fillGap)
            ;

    // Reference Enum to the class rather than the module
    local_scope.attr("None") = vpVolumeDipCreator::None;
    local_scope.attr("Linear") = vpVolumeDipCreator::Linear;
    local_scope.attr("MinCurvature") = vpVolumeDipCreator::MinCurvature;

    local_scope.attr("RADIANS") = vpVolumeDipCreator::RADIANS;
    local_scope.attr("DEGREES") = vpVolumeDipCreator::DEGREES;
    local_scope.attr("METERS") = vpVolumeDipCreator::METERS;
}



vpVolumeDipCreator::vpVolumeDipCreator() :
    myHorFilter(false),
    myChunkSize(2.0f),
    myOutputFileX("dipx"),
    myOutputFileY("dipy"),
    myOutputAzm("azim"),
    myOutputTheta("theta"),
    myDipUnit(vpVolumeDipCreator::DEGREES),
    myAzThetaUnit(vpVolumeDipCreator::NONE),
    myFillGapMode(vpVolumeDipCreator::None)
{
    setFilter(1, 1, 1);
    setSubsamplingStep(2, 2, 2);
}

void vpVolumeDipCreator::setSubsamplingStep(int xl, int il, int z) {
    mySubsampling[0] = xl;
    mySubsampling[1] = il;
    mySubsampling[2] = z;
}

void vpVolumeDipCreator::setFilter(int xl, int il, int z) {
    // Set from window size to 1/2 win size
    mySmoothSize[0] = (int) (xl - 1) / 2;
    mySmoothSize[1] = (int) (il - 1) / 2;
    mySmoothSize[2] = (int) (z - 1) / 2;
}

void vpVolumeDipCreator::setNumVolSize(float num) {
    vString message;
    double totalMemoryUsed = vqMemoryInfo::getTotalUsedMemory();
    double totalMemoryAvailable = vqMemoryInfo::getTotalMemory();

    myChunkSize = (totalMemoryAvailable - totalMemoryUsed) / num;

    message = vString("totalMemory ") + vString::number(totalMemoryAvailable - totalMemoryUsed)
              + vString(" setChunkSize ") + vString::number(myChunkSize);
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);

}

void vpVolumeDipCreator::setOutputPath(const std::string& path, std::string prefix) {
    myOutputPath = path.c_str();
    if (myOutputPath.isEmpty() == false && myOutputPath[myOutputPath.size() - 1] != '/')
        myOutputPath += '/';

    myOutputPrefix = prefix.c_str();
}

void vpVolumeDipCreator::setDipOutput(VolumeDipUNIT unit, std::string dipxName,
                                      std::string dipyName) {
    myDipUnit = unit;
    if (!dipxName.empty())
        myOutputFileX = dipxName.c_str();
    if (!dipyName.empty())
        myOutputFileY = dipyName.c_str();
}

void vpVolumeDipCreator::setAzThetaOutput(VolumeDipUNIT unit, std::string azName,
                                          std::string tetaName) {

    myAzThetaUnit = unit;
    if (!azName.empty())
        myOutputAzm = azName.c_str();
    if (!tetaName.empty())
        myOutputTheta = tetaName.c_str();
}

vString vpVolumeDipCreator::getDipXFileName() const {
    return myOutputPath + myOutputPrefix + myOutputFileX + ".fdm";
}

vString vpVolumeDipCreator::getDipYFileName() const {
    return myOutputPath + myOutputPrefix + myOutputFileY + ".fdm";
}

vString vpVolumeDipCreator::getAzFileName() const {
    if (myAzThetaUnit == vpVolumeDipCreator::NONE)
        return "";

    return myOutputPath + myOutputPrefix + myOutputAzm + ".fdm";
}

vString vpVolumeDipCreator::getTetaFileName() const {
    if (myAzThetaUnit == vpVolumeDipCreator::NONE)
        return "";
    return myOutputPath + myOutputPrefix + myOutputTheta + ".fdm";
}

void vpVolumeDipCreator::fillGap(FillGapMode mode) {
    myFillGapMode = mode;
}

vpVolumeDipCreator::~vpVolumeDipCreator() {
}

bool vpVolumeDipCreator::run(const std::string& volName) {
    vString message;
    bool status = true;

    vString outputFileX = getDipXFileName();
    vString outputFileY = getDipYFileName();
    vString outputFileAz = getAzFileName();
    vString outputFileTheta = getTetaFileName();

    if (outputFileX.isEmpty() || outputFileY.isEmpty()) {
        message = vString("Output files for dipx and dipy is requested !!!");
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message);
        return false;
    }

    // Process Parameters    
    bool runHorFilter = myHorFilter;

    bool in_degrees = myAzThetaUnit == DEGREES ? true : false;
    bool computeAzTheta = (!outputFileAz.isEmpty() && !outputFileTheta.isEmpty()) ? true : false;

    bool to_fill_gap = (myFillGapMode == vpVolumeDipCreator::None) ? false : true;
    int interp_mode = (myFillGapMode == vpVolumeDipCreator::Linear) ? 0 : 1;

    int dip_unit = myDipUnit == DEGREES ? 0 : myDipUnit == RADIANS ? 1 : 2;
    // End Process Parameters 

    // validate input path
    Volume* inp = new Volume();
    if (inp->load(volName.c_str()) == 0) {
        message = vString("The input is not found");
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning, message);
        return false;
    }

    vpVolumeProxy inputProxy;
    myInitInputVolume(inputProxy, volName);

    int n = inputProxy.getNumVolume();
    message = vString("The job has been divided to ") + vString::number(n) + vString(" pieces");
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);

    vmCubeOperator dipGenerator;
    dipGenerator.setPath("/");

    // Proxy Setup for output
    vpVolumeProxy outputProxy[4];
    int nbOutputs = 2;

    outputProxy[0].setToSaveAs(outputFileX.c_str());
    outputProxy[0].setAttrName(ATTR_NAME_DIPX);

    outputProxy[1].setToSaveAs(outputFileY.c_str());
    outputProxy[1].setAttrName(ATTR_NAME_DIPY);

    if (computeAzTheta) {
        outputProxy[2].setToSaveAs(outputFileAz.c_str());
        outputProxy[2].setAttrName(ATTR_NAME_AZIMUTH);

        outputProxy[3].setToSaveAs(outputFileTheta.c_str());
        outputProxy[3].setAttrName(ATTR_NAME_THETA);

        nbOutputs += 2;
    }

    for (int i = 0; i < nbOutputs; i++) {
        myInitOutputVolume(outputProxy[i], inputProxy);
    }

    vector<string> dipxFileNames = outputProxy[0].getChunkFileNames();
    vector<string> dipyFileNames = outputProxy[1].getChunkFileNames();
    vector<string> azFileNames;
    vector<string> thetaFileNames;
    if (computeAzTheta) {
        azFileNames = outputProxy[2].getChunkFileNames();
        thetaFileNames = outputProxy[3].getChunkFileNames();
    }

    for (int i = 0; i < n; i++) {
        Volume* input_vol = inputProxy.getVolume(i); // new() creating, needs to be deleted later

        string fileX = dipxFileNames[i];
        string fileY = dipyFileNames[i];

        string fileAz;
        string fileTheta;
        if (computeAzTheta) {
            fileAz = azFileNames[i];
            fileTheta = thetaFileNames[i];
        }

        // start offset for running dip on chunks, only apply for i > 0
        Vect4d offset;
        int dir = inputProxy.getDirection();
        if (i > 0)
            offset[dir] = double(mySmoothSize[dir]);
        status = dipGenerator.dipCube(input_vol, mySmoothSize, mySubsampling, fileX.c_str(),
                                      fileY.c_str(), runHorFilter,
                                      (fileAz.size() <= 0) ? 0 : fileAz.c_str(),
                                      (fileTheta.size() <= 0) ? 0 : fileTheta.c_str(), in_degrees,
                                      to_fill_gap, interp_mode, dip_unit, true,
                                      offset);

        delete input_vol;

        if (status == false) {
            message = vString("Job failed processing chunk #") + vString::number(i + 1);
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, message);
            return false;
        }

        message = vString("Chunk ") + vString::number(i + 1) + vString("/") + vString::number(n)
                  + vString(" processed !");
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message, message);
    }

    for (int i = 0; i < nbOutputs; i++) {
        outputProxy[i].globalMerge();  // merge();
        outputProxy[i].cleanUp();
    }

    return status;
}

void vpVolumeDipCreator::myInitInputVolume(vpVolumeProxy& input, const std::string& volName) const {
    input.setOriginPath(volName.c_str());
    input.setSubVolSize(myChunkSize);
    input.setDirection(vpVolumeProxy::ILINE);
    input.setTaper(mySmoothSize[vpVolumeProxy::ILINE]);
    input.setSubVolMultiple(mySubsampling[0], mySubsampling[1], mySubsampling[2]);
//    input.myMakeChunkTable();
    input.split();
}

/* Initialize subsampled Geometry
 *
 */
void vpVolumeDipCreator::myInitSubSampleGeometry(BaseGeometry& geom,
                                                 const vpVolumeProxy& input) const {
    geom = input.getGeometry();

    // Computed the resulting geometry (sub-sampled))
    const int nDim = 3;
    Vect4i inc = geom.getInc();
    for (int i = 0; i < nDim; ++i)
        inc[i] *= mySubsampling[i];
    geom.setInc(inc);

    Vect4d step = geom.getStep();
    for (int i = 0; i < nDim; ++i)
        step[i] *= mySubsampling[i];
    geom.setStep(step);

    Vect4i min = geom.getMin();
    Vect4i max = geom.getMax();
    Vect4i size = geom.getSize();
    for (int i = 0; i < nDim; ++i)
        size[i] = (max[i] - min[i]) / inc[i] + 1;

    /* avoid zero data size */
    if (size[0] == 0)
        size[0] = 1;
    if (size[1] == 0)
        size[1] = 1;
    if (size[2] == 0)
        size[2] = 1;

    geom.setSize(size);
}

void vpVolumeDipCreator::myInitOutputVolume(vpVolumeProxy& output,
                                            const vpVolumeProxy& input) const {
    BaseGeometry geom;
    myInitSubSampleGeometry(geom, input);

    output.setDirection(input.getDirection());
    output.setTaper(0);
    output.setChunkSize(input.getChunkSize() / mySubsampling[vpVolumeProxy::ILINE]);
    output.setPredictedGeometry(geom);
    output.split();
}
