/* 
 * File:   vpReportFileParameters.hpp
 * Author: ddoledec
 *
 * Created on February 12, 2016, 2:54 PM
 */

#ifndef VPREPORTFILEPARAMETERS_HPP
#define	VPREPORTFILEPARAMETERS_HPP
#include <boost/python.hpp>

#include "vmReportFileParameters.hpp"

class vpReportFileParameters : public vmReportFileParameters, 
        public boost::python::wrapper<vmReportFileParameters> {
public:
    vpReportFileParameters();
    vpReportFileParameters(const vpReportFileParameters& orig);
    virtual ~vpReportFileParameters();
    
    /** Set the tokens from a python list */
    void setNaming(boost::python::list& lst);
    
private:
    

};

#endif	/* VPREPORTFILEPARAMETERS_HPP */

