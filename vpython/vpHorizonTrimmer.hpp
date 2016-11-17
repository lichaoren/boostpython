/* 
 * File:   vpHorizonTrimmer.hpp
 * Author: ruwu
 *
 * Created on January 13, 2016, 2:19 PM
 */

#ifndef VPHORIZONTRIMMER_HPP
#define	VPHORIZONTRIMMER_HPP

#include "vlInsertSaltBase.hpp"

class vgHorizonGroup;

class vpHorizonTrimmer : public InsertSaltBase
{
    public:
                
        vpHorizonTrimmer() ;
        ~vpHorizonTrimmer() {}
        
        static void exposePython();
        
        void setHorizonGroup(vgHorizonGroup * hg);
        
        void setSaltHrzPairsByName(boost::python::list& lst);
        void setSaltHrzPairsByIndex(boost::python::list& lst);
        
        void setSediHrzPairsByName(boost::python::list& lst);
        void setSediHrzPairsByIndex(boost::python::list& lst);
        
        void exe();
        
        
};

#endif	/* VPHORIZONTRIMMER_HPP */

