/* 
 * File:   vpLasWellLog.hpp
 * Author: ddoledec
 *
 * Created on May 6, 2016, 1:27 PM
 */

#include <boost/python/list.hpp>

#ifndef VPLASWELLLOG_HPP
#define VPLASWELLLOG_HPP

class LasWellLog;
class vString;
class vpWellLogCurve;

class vpLasWellLog {
public:
    static vpWellLogCurve* addLog(LasWellLog* well, const vString& name);  
    static vpWellLogCurve* getLog(LasWellLog* well, const vString& name);  
private:

};

#endif /* VPLASWELLLOG_HPP */

