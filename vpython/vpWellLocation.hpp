/* 
 * File:   vpWellLocation.hpp
 * Author: ddoledec
 *
 * Created on January 14, 2016, 11:00 AM
 */

#ifndef VPWELLLOCATION_HPP
#define	VPWELLLOCATION_HPP
    
#include <boost/python.hpp> 
#include "vmDataLocation.hpp"


class vpWellLocation : public vmWellLocation {
public:
    vpWellLocation();
    vpWellLocation(const vpWellLocation& orig);
    virtual ~vpWellLocation();
    
    /** Add well (ID) from a python list */
    void addWell(boost::python::list& lst);
    
private:
};

#endif	/* VPWELLLOCATION_HPP */

