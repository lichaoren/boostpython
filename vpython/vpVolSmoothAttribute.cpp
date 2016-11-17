/* 
 * File:   vpVolSmoothAttribute.cpp
 * Author: ddoledec
 * 
 * Created on April 28, 2015, 2:53 PM
 */
#include <boost/python.hpp> 
#include <string> 
#include <algorithm>

#include "vpVolSmoothAttribute.hpp"

#include "vpVolumeProxy.hpp"
#include "vpPythonContext.hpp"
#include "vlPythonInterpreter.hpp"

#include "vlHorizon.hpp"

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(vpVolSmoothAttribute_overloads, setDepthVarying, 1, 2)

void vpVolSmoothAttribute::exposePython() {
    namespace bp = boost::python;
    
    void (vpVolSmoothAttribute::*setWinList)(boost::python::list&) = &vpVolSmoothAttribute::setSmoothWindows;
    void (vpVolSmoothAttribute::*setWindow)(double, double, double) = &vpVolSmoothAttribute::setSmoothWindow;
    // Expose methods
    bp::scope vol_smooth_scope = bp::class_<vpVolSmoothAttribute>("VolumeSmooth")
            .def("run", &vpVolSmoothAttribute::run)
            .def("runChunk", &vpVolSmoothAttribute::runChunk)
            .def("setMethod", &vpVolSmoothAttribute::setUseMethod)
            .def("setFastMethod", &vpVolSmoothAttribute::setUseFast)
            .def("setDoublePrecision", &vpVolSmoothAttribute::setDoublePrecision)
            .def("setMedianPrecision", &vpVolSmoothAttribute::setAppMedianPrecision)
            .def("setDataType", &vpVolSmoothAttribute::setType)
            .def("setDataMedium", &vpVolSmoothAttribute::setMode)
            .def("setSmoothWindow", setWinList)
            .def("setSmoothWindow", setWindow)
            .def("setDepthVarying", &vpVolSmoothAttribute::setDepthVarying, vpVolSmoothAttribute_overloads())
            .def("setAlpha", &vpVolSmoothAttribute::setAlpha);

    // Enums used
    bp::enum_<vlVolSmooth::SmoothMethod> ("SmoothMethod")
            .value("MEAN", vlVolSmooth::MEAN)
            .value("MEDIAN", vlVolSmooth::MEDIAN)
            .value("ALPHA_TRIM", vlVolSmooth::ALPHA_TRIM)
            .value("B_SPLINE", vlVolSmooth::B_SPLINE)
            .value("MEAN_EXCLUDE", vlVolSmooth::MEAN_EXCLUDE)
            .value("COMPRESS", vlVolSmooth::COMPRESS)
            .value("MEAN_DVK", vlVolSmooth::MEAN_DVK)
            .value("MEAN_DIRECTION", vlVolSmooth::MEAN_DIRECTION)
            .value("MEAN_SINGLE_FAULT", vlVolSmooth::MEAN_SINGLE_FAULT)
            .value("MEAN_MULTI_FAULTS", vlVolSmooth::MEAN_MULTI_FAULTS)
            .value("APPROX_MEDIAN", vlVolSmooth::APPROX_MEDIAN);
    
    // Reference Enum to the class rather than the module
    vol_smooth_scope.attr("MEAN") = vlVolSmooth::MEAN;
    vol_smooth_scope.attr("MEDIAN") = vlVolSmooth::MEDIAN;
    vol_smooth_scope.attr("ALPHA_TRIM") = vlVolSmooth::ALPHA_TRIM;
    vol_smooth_scope.attr("B_SPLINE") = vlVolSmooth::B_SPLINE;
    vol_smooth_scope.attr("MEAN_EXCLUDE") = vlVolSmooth::MEAN_EXCLUDE;
    vol_smooth_scope.attr("COMPRESS") = vlVolSmooth::COMPRESS;
    vol_smooth_scope.attr("MEAN_DVK") = vlVolSmooth::MEAN_DVK;
    vol_smooth_scope.attr("MEAN_DIRECTION") = vlVolSmooth::MEAN_DIRECTION;
    vol_smooth_scope.attr("MEAN_SINGLE_FAULT") = vlVolSmooth::MEAN_SINGLE_FAULT;
    vol_smooth_scope.attr("MEAN_MULTI_FAULTS") = vlVolSmooth::MEAN_MULTI_FAULTS;
    vol_smooth_scope.attr("APPROX_MEDIAN") = vlVolSmooth::APPROX_MEDIAN;
    
    // Horizon AREA
    /* Not yet available 
    bp::enum_<vlVolSmooth::SmoothHorizonRule> ("SmoothHorizonRule")
            .value("WITHIN_HORIZON", vlVolSmooth::WITHIN_HORIZON)
            .value("WITHIN_HRZEDGE", vlVolSmooth::WITHIN_HRZEDGE)
            .value("AROUND_BOUNDARY", vlVolSmooth::AROUND_BOUNDARY);

    vol_smooth_scope.attr("WITHIN_HORIZON") = vlVolSmooth::WITHIN_HORIZON;   
    vol_smooth_scope.attr("WITHIN_HRZEDGE") = vlVolSmooth::WITHIN_HRZEDGE;   
    vol_smooth_scope.attr("AROUND_BOUNDARY") = vlVolSmooth::AROUND_BOUNDARY;   
    */    
    
    // Data type
    bp::enum_<vlVolSmooth::SmoothDataType> ("SmoothDataType")
            .value("MISC", vlVolSmooth::MISC)
            .value("VELOCITY", vlVolSmooth::VELOCITY);
    vol_smooth_scope.attr("MISC") = vlVolSmooth::MISC;   
    vol_smooth_scope.attr("VELOCITY") = vlVolSmooth::VELOCITY;   

    
    // Medium mode
    bp::enum_<vlVolSmooth::SmoothDataMedium> ("SmoothDataMedium")
            .value("ISOTROPIC", vlVolSmooth::ISOTROPIC)
            .value("VTI", vlVolSmooth::VTI)
            .value("TTI", vlVolSmooth::TTI);
    vol_smooth_scope.attr("ISOTROPIC") = vlVolSmooth::ISOTROPIC;   
    vol_smooth_scope.attr("VTI") = vlVolSmooth::VTI;   
    vol_smooth_scope.attr("TTI") = vlVolSmooth::TTI;   
}



vpVolSmoothAttribute::vpVolSmoothAttribute()
{
    setType(VELOCITY);
    enableUndo (false); // save memory
}
vpVolSmoothAttribute::~vpVolSmoothAttribute()
{
}

// Need to write vlVolSmooth copy ... 
//vpVolSmoothAttribute::vpVolSmoothAttribute(const vpVolSmoothAttribute& orig)
//{
//}

void vpVolSmoothAttribute::setSmoothWindow(double rx, double ry, double rz) {
	vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Warning,
			"==== Deprecated === Rather use addSmoothWIndows()\n"
					"Smooth windows is cleared and set");
	mySmoothWindows.clear();
	mySmoothWindows.push_back(Vect3d(rx, ry, rz));
	setIsVariableRadius(false);
}

void vpVolSmoothAttribute::setSmoothWindows(boost::python::list& lst) {
	mySmoothWindows.clear();
	pylistToVec3d(lst, mySmoothWindows);
	setIsVariableRadius(false);
}

void vpVolSmoothAttribute::setDepthVarying(boost::python::list params, vgHorizon* hrz) {     
    vector<vlVolSmooth::ZRadius>& varyingDpeth = zRadius() ;
    varyingDpeth.clear();
        
    // Use horizon to limit smoothing
    setIsHrzLimited(hrz==0?false:true);
    setLimitedHrz(hrz, 0);
    
    Py_ssize_t length = boost::python::len(params);
    setIsVariableRadius(true);
    
    for (Py_ssize_t i = 0; i < length; i++)
    {
        // loop for each list 
        Py_ssize_t len_each = boost::python::len(params[i]);

        // need to make sure the parameter list is right
        if (len_each != 5)
            continue;

        float z=boost::python::extract<double>(params[i][0]);
        float rx = boost::python::extract<double>(params[i][1]);
        float ry = boost::python::extract<double>(params[i][2]);
        float rz = boost::python::extract<double>(params[i][3]);
        bool toInterp = boost::python::extract<double>(params[i][4])== 1 ? true : false;
        
        ZRadius radius( z, rx, ry, rz, toInterp  );

        varyingDpeth.push_back(radius);
    }
    
    setIsVariableRadius(true);
}


bool vpVolSmoothAttribute::run( Volume& vol )
{
    setVelocity(&vol);
    for (int i = 0; i < mySmoothWindows.size(); ++i) {
        setRange(mySmoothWindows[i][0],mySmoothWindows[i][1],mySmoothWindows[i][2]);
        if (!advancedSmooth()) {
            vlPythonInterpreter::getInstance().setError(
                    vlPythonInterpreter::Error,
                    vString("failed to smooth volume "));
            return false;
        }
    }

    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Message,
            vString("Attribute Smoothing Done\n"));
    return true;
}

bool vpVolSmoothAttribute::runChunk(const std::string& inputAttrName, const std::string& outputAttrName)
{
    vpVolumeProxy proxySource;
    proxySource.setSubVolSize(4); // 4 GB

    // No chunking needed, run smooth on the entire volume
    if (!proxySource.isToSplit(inputAttrName.c_str()))
    {
        Volume *source_vol = new Volume;
        source_vol->load(inputAttrName.c_str());

        setVelocity(source_vol);
        if (isVariableRadius()) {
            if (advancedSmooth() == false) {
                vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                        vString("failed to smooth volume ") + inputAttrName.c_str());
                return false;
            }
        } else {
            for (unsigned int i = 0; i < mySmoothWindows.size(); ++i) {
                setRange(mySmoothWindows[i][0], mySmoothWindows[i][1], mySmoothWindows[i][2]);
                if (smooth() == false) {
                    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                            vString("failed to smooth volume ") + inputAttrName.c_str());
                    return false;
                }
            }
        }

        source_vol->saveAs(outputAttrName.c_str());
        delete source_vol;
        return true;
        
    } else {
        
        proxySource.setOriginPath(inputAttrName.c_str());
        proxySource.setToSaveAs(outputAttrName.c_str());
        
        setRunInBackground(true); // Multi thread

        /* in order to make sure the data used for taper were also updated,
         * extend taper width of each chunk to the summation of all window
         * width
         */
        double ry=0;
        for (unsigned int i = 0; i < mySmoothWindows.size(); ++i)
            ry += mySmoothWindows[i][1];
        
        double dx=0, dy=0, dz=0;
        proxySource.getVolumeStep(inputAttrName.c_str(), dx, dy, dz);

        int taper = (int) ceil(ry/dy);  // nb IL for the taper
        proxySource.setTaper(taper);

        proxySource.split();

        int n = proxySource.getNumVolume();
        for (int i = 0; i < n; i++)
        {            
            Volume* source_vol = proxySource.getVolume(i);       
            setVelocity(source_vol);

            if (isVariableRadius()) {
                if (advancedSmooth() == false) {
                    if (source_vol)
                        delete source_vol;
                    proxySource.cleanUp(); // remove files

                    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                            vString("ERROR during sub volume smoothing of") + inputAttrName.c_str());

                    return false; // Need something better ...                            
                }
            } else {
                for (int j = 0; j < mySmoothWindows.size(); ++j) {
                    setRange(mySmoothWindows[j][0], mySmoothWindows[j][1], mySmoothWindows[j][2]);
                    if (smooth() == false) {
                        if (source_vol)
                            delete source_vol;
                        proxySource.cleanUp(); // remove files

                        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                                vString("ERROR during sub volume smoothing of") + inputAttrName.c_str());

                        return false; // Need something better ...
                    }
                }
            }
            if (!source_vol->save()) {
                delete source_vol;
                vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                        vString("failed to save volume chunk"));
                return false;
            }
            delete source_vol;
        }
        
        proxySource.merge();
        proxySource.cleanUp();
        return true;
    }

    return false;
}


void pylistToVec3d(boost::python::list& lst, vector<Vect3d>& dst) {
	Py_ssize_t len = boost::python::len(lst);

	for (Py_ssize_t i = 0; i < len; ++i) {
		Py_ssize_t len_each = boost::python::len(lst[i]);
		if (len_each != 3) {
			vlPythonInterpreter::getInstance().setError(
					vlPythonInterpreter::Warning,
					"find window size != 3, skipped");
			continue;
		}
		dst.push_back(
				Vect3d(boost::python::extract<double>(lst[i][0]),
						boost::python::extract<double>(lst[i][1]),
						boost::python::extract<double>(lst[i][2])));
	}
}
