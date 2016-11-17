/* 
 * File:   vpPythonContext.cpp
 * Author: ddoledec
 * 
 * Created on April 22, 2015, 5:49 PM
 */

#include "vpPythonContext.hpp"

vpPythonContext::vpPythonContext() :
 myAvailableMemory(8.0),
 myIsAvailable (true)
{
    #if __cplusplus <= 199711L
    myMode = vpPythonContext::BATCH;
#else
    myMode = ModuleMode::BATCH;
#endif 
}

vpPythonContext::~vpPythonContext()
{
}

vpPythonContext::ModuleMode vpPythonContext::getMode() const
{
    return myMode;
}

void vpPythonContext::setMode(ModuleMode mode)
{
    this->myMode = mode;
}

bool vpPythonContext::isInteractiveMode() {
#if __cplusplus <= 199711L
        return (myMode==vpPythonContext::INTERACTIVE);
#else
        return (myMode==ModuleMode::INTERACTIVE);
#endif    
    
}
bool vpPythonContext::isBatchMode() {
    
#if __cplusplus <= 199711L
        return myMode==vpPythonContext::BATCH;
#else
        return myMode==ModuleMode::BATCH;
#endif  
}

bool vpPythonContext::isAvailable() const
{
    return myIsAvailable;
}

void vpPythonContext::setAvailable(bool available)
{
    this->myIsAvailable = available;
}

