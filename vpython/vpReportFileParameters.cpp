/* 
 * File:   vpReportFileParameters.cpp
 * Author: ddoledec
 * 
 * Created on February 12, 2016, 2:54 PM
 */

#include "vpReportFileParameters.hpp"

#include "vmFlexibleFileNaming.hpp"

vpReportFileParameters::vpReportFileParameters()
{
    /** Default Naming */
    vmFlexibleFileNaming& fileNaming = getFileNaming();
    fileNaming.addToken(vmFlexibleFileNaming::Prefix)
    .addToken(vmFlexibleFileNaming::BookmarkName)
    .addToken(vmFlexibleFileNaming::SeismicName)
    .addToken(vmFlexibleFileNaming::WellName)
    .addToken(vmFlexibleFileNaming::AttrlName)
    .addToken(vmFlexibleFileNaming::GatherName)
    .addToken(vmFlexibleFileNaming::GridNum)
    .addToken(vmFlexibleFileNaming::Suffix);
    
}

vpReportFileParameters::vpReportFileParameters(const vpReportFileParameters& orig)
{
}

vpReportFileParameters::~vpReportFileParameters()
{
}
void vpReportFileParameters::setNaming(boost::python::list& lst)
{
    Py_ssize_t length = boost::python::len(lst);
    vmFlexibleFileNaming& fileNaming = getFileNaming();
    fileNaming.resetTokens();

    for (Py_ssize_t i = 0; i < length; i++)
    {
        std::string token = boost::python::extract<string>(lst[i]);
        fileNaming.addToken(token);
    }
}

