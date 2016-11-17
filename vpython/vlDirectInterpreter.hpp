/* 
 * File:   vlDirectInterpreter.hpp
 * Author: ddoledec
 *
 * Created on January 29, 2015, 2:38 PM
 */

#ifndef VLDIRECTINTERPRETER_HPP
#define	VLDIRECTINTERPRETER_HPP

#include "vlInterpreter.hpp"

class vlDirectInterpreter : public vlInterpreter {
public:
    vlDirectInterpreter();
    virtual ~vlDirectInterpreter();

    /* run the python script*/
    bool run(const char* fileName);
    bool runCMD(const char* cmd);
};

#endif	/* VLDIRECTINTERPRETER_HPP */

