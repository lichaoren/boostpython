/* 
 * File:   vpReportDataParameters.hpp
 * Author: ddoledec
 *
 * Created on January 15, 2016, 5:41 PM
 */

#ifndef VPREPORTDATAPARAMETERS_HPP
#define	VPREPORTDATAPARAMETERS_HPP

#include <boost/python.hpp>
#include "vmReportDataParameters.hpp"

class vlResource;
class Volume;
class AutomaticVolumeLoading;

class vpReportDataParameters : public vmReportDataParameters, public boost::python::wrapper<vmReportDataParameters> {
public:

    vpReportDataParameters();
    vpReportDataParameters(const vpReportDataParameters& orig);
    virtual ~vpReportDataParameters();
    virtual vpReportDataParameters& operator=(const vpReportDataParameters&);
    
    void clear();

    void addSeismic(Volume*  data, int idx = 0);
    void removeSeismic(Volume*  data, int idx = 0);
    void addAttribute(Volume* data, int idx = 0);
    void removeAttribute(Volume* data, int idx = 0);
    void addGather(Volume*  data, int idx = 0);
    void removeGather(Volume*  data, int idx = 0);

protected:
    enum DataType {
        SEISMIC,
        ATTRIBUTE,
        GATHER,
        WELL
    };
    void add(DataType type, Volume* data, int idx);
    void remove(DataType type, Volume* data, int idx);

private:
    std::list< AutomaticVolumeLoading* > ptr;
    void myClearData();
    void myRemoveData(DATA_TYPE type, Volume* data);
};

#endif	/* VPREPORTDATAPARAMETERS_HPP */

