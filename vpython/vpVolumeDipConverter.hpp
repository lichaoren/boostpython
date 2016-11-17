/* 
 * File:   vpVolumeDipConverter.hpp
 * Author: Dominique DOLEDEC
 *
 * Created on May 23, 2016, 4:17 PM
 */

#ifndef VPVOLUMEDIPCONVERTER_HPP
#define VPVOLUMEDIPCONVERTER_HPP

#include "vlVolumeDipConverter.hpp"

class vpVolumeDipConverter : public vlVolumeDipConverter {
public:
    vpVolumeDipConverter();
    virtual ~vpVolumeDipConverter();
    
     
    /** Expose Methods to Python */
    static void exposePython();

    FnOK convert(vlVolumeDipConverter::VolumeDipType inputType, vlVolumeDipConverter::VolumeDipUNIT inputUnit,
        vlVolumeDipConverter::VolumeDipType outputType, vlVolumeDipConverter::VolumeDipUNIT outputUnit);    
    
private:
    FnOK convertWithChunk(vlVolumeDipConverter::VolumeDipType inputType, vlVolumeDipConverter::VolumeDipUNIT inputUnit,
        vlVolumeDipConverter::VolumeDipType outputType, vlVolumeDipConverter::VolumeDipUNIT outputUnit);

        
};

#endif /* VPVOLUMEDIPCONVERTER_HPP */

