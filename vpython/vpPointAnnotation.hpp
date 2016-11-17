/* 
 * File:   vpPointAnnotation.hpp
 * Author: ddoledec
 *
 * Created on April 27, 2016, 10:50 AM
 */

#ifndef VPPOINTANNOTATION_HPP
#define VPPOINTANNOTATION_HPP
#include <string> 
#include <vector> 

class vmPointAnnotator;
class DevWellLog;

/**
 * Python override to display point annotations used as output
 * by the WellTieMap process
 */
class vpPointAnnotation {
public:
    vpPointAnnotation();
    vpPointAnnotation(const vpPointAnnotation& orig);
    virtual ~vpPointAnnotation();
    
    static void exposePython();
    
    /** DisplaySettings saved in Tornado */
    bool loadDisplaySettings(const std::string& settings);
 
    /** Load plot into the view3D*/
    bool load(const std::string& filename , int genMode);
        
    /** Remove plot from the view3D*/
    bool unload();
    
    /** Spread the labels to avoid any overlap */
    void spread(bool val=true);

    /** Change visibility */
    void setVisible(bool val=true, const std::string wellname=std::string() );
     
    /** List of fields displayed */
    void setSelectedFieldNames(boost::python::list& fields);
    
protected:
    vmPointAnnotator* getAnnotator();
    bool loadFromWells(const std::string& filename);
    bool loadFromHrz(const std::string& filename);

    
    std::vector<DevWellLog*> getSelectedWell();
    std::vector<int> getSelectedFieldNames() const;
    
private:
    /** List of fields extracted from the misc */
    std::vector<int> mySelectedFields;
};

#endif /* VPPOINTANNOTATION_HPP */

