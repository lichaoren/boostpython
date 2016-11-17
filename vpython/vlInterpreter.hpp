/* 
 * File:   vlInterpreter.hpp
 * Author: ddoledec
 *
 * Created on January 29, 2015, 2:53 PM
 */

#ifndef VLINTERPRETER_HPP
#define	VLINTERPRETER_HPP


class vlInterpreter {
public:
     virtual  bool run(const char* fileName)=0;
     virtual bool runCMD(const char* CMD){return true;};
};

#endif	/* VLINTERPRETER_HPP */

