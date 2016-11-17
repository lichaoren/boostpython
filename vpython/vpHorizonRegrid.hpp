/* 
 * File:   vpHorizonRegrid.hpp
 * Author: ruwu
 *
 * Created on Oct 23, 2015
 */

#ifndef VPHORIZONREGRED_HPP
#define VPHORIZONREGRED_HPP

#include "vlHorizonRegridProcess.hpp"

class vgHorizonGroup;

class vpHorizonRegrid : public vlHorizonRegridProcess
{
public:
    enum ExtendDir{
        EXTEND_ALONG_XL = 0,
        EXTEND_ALONG_IL = 1,
        NO_EXTEND = 2
    };
    
    vpHorizonRegrid();
    ~vpHorizonRegrid();
    
    static void exposePython();
    void setExtend(int dir);
    vgHorizonGroup * doRegrid();
};

#endif
