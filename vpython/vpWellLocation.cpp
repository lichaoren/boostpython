/* 
 * File:   vpWellLocation.cpp
 * Author: ddoledec
 * 
 * Created on January 14, 2016, 11:00 AM
 */

#include "vpWellLocation.hpp"
#include "vlAppModel.hpp"

vpWellLocation::vpWellLocation()
{
}

vpWellLocation::vpWellLocation(const vpWellLocation& orig)
{
}

vpWellLocation::~vpWellLocation()
{
}

void vpWellLocation::addWell(boost::python::list& lst)
{
    Py_ssize_t length = boost::python::len(lst);
    vlAppModel& model = getAppModelRef();

    std::vector<LasWellLog*> wellList;
    for (Py_ssize_t i = 0; i < length; i++)
    {
        std::string wellName = boost::python::extract<string>(lst[i]);
        vString wellID(wellName.c_str());
        LasWellLog* wellLog = model.getWellLog(wellID);


        if (wellLog)
        {
            wellList.push_back(wellLog);
        } else
        {
            cerr << "ERROR input well " << wellName << endl;
        }

    }

    addLocation(wellList);
}

