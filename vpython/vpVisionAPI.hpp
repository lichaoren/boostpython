/* 
 * File:   vpVisionAPI.hpp
 * Author: yanhliu
 *
 * Created on February 2, 2015, 3:21 PM
 */

#ifndef VPVISIONAPI_HPP
#define	VPVISIONAPI_HPP
#include "boost/python/list.hpp"

#include <Vect2.hpp>
#include <Vect3.hpp>
#include <Vect4.hpp>

#include <string>
#include <vector>
#include <list>
#include <vString.hpp>

class LasWellLog;
class vgHorizonGroup;
class vgHorizon;
class Volume;


/**
 * BitMask used to select the list of wells
 * vpVisionAPI::getWellList(WELL_WITH_SONIC);
 */
enum WellSelectionMode {
    WELL_ALL = (1u << 0), // Main dip 
    WELL_WITH_SONIC = (1u << 1), // Normal to the surface
    WELL_WITH_TOPS = (1u << 2),
    WELL_WITH_CHECKSHOT = (1u << 3)
}; 


/**
 * Handle Tornado data & display 
 */
class vpVisionAPI {
public:
    
    void addAttribute(Volume* vol, std::string name="");
    void addHorizonGroup(vgHorizonGroup* hg );
    static vpVisionAPI& getInstance();
    
    Volume*     getCurrentAttribute();
    Volume*     getCurrentSeismic();
    vgHorizon*  getCurrentHorizon();
    vgHorizonGroup* getCurrentHorizonGroup();
        
    Volume* getAttribute(const char* attr);
    Volume* getSeismic(const char* seis);
    Volume* getGather(const char* name) const;
    vgHorizonGroup* getHorizonGroup(const char* hg);
    LasWellLog* getWell(const char* hg);
    
    /** List of wells loaded in Tornado. 
     * @param flag 
     * @return list of LasWellLog (references==same object as in Tornado)
     */
    boost::python::list getWellList(WellSelectionMode flag=WELL_ALL);
    
    /** @brief Show the list of well in view 3D */
    bool showWellList(boost::python::list wellList);

    /** @brief Show well according to a selection mode */
   bool showWell(WellSelectionMode flag=WELL_ALL);

   /** Change current displayed horizon.  */
    bool setCurrentHorizon(const vString& ehName);

    /** Change current displayed horizon.  */
   bool setCurrentHorizonGroup(const vString& ehName);
   
   /** Remove the horizon group from tornado */
   bool removeHorizonGroup(const vString& ehName);

   /** Change the limits and palette
    * @param dataType "HORIZON", "ATTIRBUTE", "SEISMIC","GATHER"
    * @param palette name of the palette as defined in tornado
    */
   void setCurrentSpectrum(const char* dataType, float min, float max, char* palette=0);
    
    void setCursorPos(const vUtl::Vect4d& survey, const vUtl::Vect2i& line);
    void setUserArg(const vUtl::Vect3d& args);
    vUtl::Vect3d getUserArg();
    vUtl::Vect4d getCursorLog();
    vUtl::Vect2i getCursorLine();
    void refresh();
    
protected:
    vpVisionAPI();
    ~vpVisionAPI();
private:
    static vpVisionAPI* myInstance;
    vUtl::Vect4d myCursorSuveyPos;
    vUtl::Vect2i myCursorLinePos;
    vUtl::Vect3d myUserArg;
};

#endif	/* VPVISIONAPI_HPP */

