/* 
 * File:   vpVolumeAPI.hpp
 * Author: Dominique DOLEDEC
 *
 * Created on May 26, 2016, 3:57 PM
 */

#ifndef VPVOLUMEAPI_HPP
#define VPVOLUMEAPI_HPP

/**
 * It's really an API (for once) in the way that we don't want to use a wrapper 
 * on top of the Volume (previous implementation).
 * Before, we had to deal with 2 kind of objects in Python Volume an VolumeAPI
 * where methods are available ... or not according to the instance.
 * 
 * ## Example of code that was not working in the prev version
 * 
 * \code{.py}
 * vol1 = Volume()
 * vol1.load('dipx.fdm')
 * vol2 = Volume()
 * vol2.load('dipx.fdm')
 * vol2.getTrace(1,1)
 * 
 * vol3 = vol1*vol2
 * # now vol3 is a Volume and we have no access to sgetTrace (only member of API)
 * vol3.getTrace(1,1)
 * \endcode
 * 
 */

#include "vlVolume.hpp"


namespace vpVolumeAPI {
   void  exposePython();
}


#endif /* VPVOLUMEAPI_HPP */

