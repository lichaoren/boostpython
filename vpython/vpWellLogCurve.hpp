/* 
 * File:   vpWellLogCurve.hpp
 * Author: Dominique DOLEDEC
 *
 * Created on May 6, 2016, 4:25 PM
 */

#ifndef VPWELLLOGCURVE_HPP
#define VPWELLLOGCURVE_HPP
#include <boost/python/list.hpp>

#include "vlLasWellLog.hpp"

class vpWellLogCurve  {
public:
    vpWellLogCurve();
    vpWellLogCurve(WellLogCurve *logCurve);
    vpWellLogCurve(const vpWellLogCurve& orig);
    virtual ~vpWellLogCurve();

    static void exposePython();
    
    /** Standard python list access */
    boost::python::list getData() const;
    void setData(const boost::python::list& tpl);

    /** Numpy array access */
     boost::python::object getNPyData() const;
    void setNPyData(const boost::python::object& tpl);
    
    vString getName() const ;

    int getType() const ;
    void setType(int type);
    
private:
    WellLogCurve *myLogCurve;
};

#endif /* VPWELLLOGCURVE_HPP */

