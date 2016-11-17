#include <boost/python.hpp> 

#include "vpHorizonRegrid.hpp"
#include "vlHorizonGroup.hpp"

vpHorizonRegrid::vpHorizonRegrid()
{

}

vpHorizonRegrid::~vpHorizonRegrid()
{

}

void vpHorizonRegrid::exposePython()
{
    using namespace boost::python;
    
    enum_<vpHorizonRegrid::ExtendDir>("ExtendDir")
    .value("EXTEND_ALONG_XL", vpHorizonRegrid::EXTEND_ALONG_XL)
    .value("EXTEND_ALONG_IL", vpHorizonRegrid::EXTEND_ALONG_IL)
    .value("NO_EXTEND", vpHorizonRegrid::NO_EXTEND);
    
    class_<vpHorizonRegrid>("HorizonRegrid")
    .def("doRegrid", &vpHorizonRegrid::doRegrid, return_value_policy<manage_new_object>())
    .def("setXLRange", &vlHorizonRegridProcess::setXLRange)
    .def("setILRange", &vlHorizonRegridProcess::setILRange)
    .def("setInc", &vlHorizonRegridProcess::setInc)
    .def("setOutputPath", &vlHorizonRegridProcess::setOutputPath)
    .def("setHorizonGroup", &vlHorizonRegridProcess::setHorizonGroup)
    .def("setExtend", &vpHorizonRegrid::setExtend);
}

void vpHorizonRegrid::setExtend(int dir)
{
    if(dir == EXTEND_ALONG_XL)
    {
        vlHorizonRegridProcess::setExtend(true);
        vlHorizonRegridProcess::setExtendXL();
    }
    else if(dir == EXTEND_ALONG_IL)
    {
        vlHorizonRegridProcess::setExtend(true);
        vlHorizonRegridProcess::setExtendIL();
    }
    else if(dir == NO_EXTEND)
    {
        vlHorizonRegridProcess::setExtend(false);
    }
    else
    {
        fprintf(stderr, "Parameter of setExtend should be EXTEND_ALONG_XL, EXTEND_ALONG_IL or NO_EXTEND.");
        vlHorizonRegridProcess::setExtend(false);
    }
}

vgHorizonGroup * vpHorizonRegrid::doRegrid()
{
    return vlHorizonRegridProcess::doRegrid(false);
}
