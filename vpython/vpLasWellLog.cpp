/* 
 * File:   vpLasWellLog.cpp
 * Author: ddoledec
 * 
 * Created on May 6, 2016, 1:27 PM
 */
#include <boost/python/extract.hpp>

#include "vpLasWellLog.hpp"

#include "vString.hpp"
#include "vlLasWellLog.hpp"
#include "vlPythonInterpreter.hpp"

#include "vpWellLogCurve.hpp"
    

vpWellLogCurve* vpLasWellLog::addLog(LasWellLog* well, const vString& name) {
    if (!well) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                "addLog: Invalid Well");
        return 0;
    }
    
    WellLogCurve* wellLog = well->getCurve(well->getCurveIndex(name));

    // Don't create curve if already exist
    if (wellLog == 0) {
        if(well->getCurves().size()>0) { // need one curve ..
        well->addCurve(0, name);
        wellLog = well->getCurve(well->getCurveIndex(name));
        }
        if (!wellLog)
            return 0;
    }
    return new vpWellLogCurve(wellLog);
}

vpWellLogCurve* vpLasWellLog::getLog(LasWellLog* well, const vString& name) {
    if (!well) {
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error,
                "addLog: Invalid Well");
        return 0;
    }
    WellLogCurve* wellLog = well->getCurve(well->getCurveIndex(name));

    if (!wellLog)
        return 0;

    return new vpWellLogCurve(wellLog);
}
