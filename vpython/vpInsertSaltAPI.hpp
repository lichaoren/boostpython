#ifndef VPINSERTSALTAPI_HPP
#define	VPINSERTSALTAPI_HPP

#include <boost/python.hpp> 
#include <vlInsertSaltBase.hpp>
using namespace std;

class vmSaltDefinition;

/**
 */
class vpInsertSaltAPI {
public:

    vpInsertSaltAPI();
    ~vpInsertSaltAPI();
 
    void setHorizonGroup(string horizonGroup);
    void setInputVolume(string inputVolume);
    void setTargetVolume(string targetVolume);
    void setOrigSed(string sed);
    void setSaltVelocity(float);
    void setCheckSaltBoundary(bool);
    void setForceLinear(bool);
    void setFloodMax(bool);
    void addSegs(Vect2i tp);
    void addHorizonPairs(boost::python::list& lst);
    void setTopHrzIdx(boost::python::list& lst);
    void setBtmHrzIdx(boost::python::list& lst);
    
    /** Add tuple of (hrz-list, salt-def) 
     * salt-def = float: salt velocity
     * salt-def = string: dirty-salt or sediment cube
     */
//    void addSalt(boost::python::tuple& lst);

    void addSalt(boost::python::list& hList, float velocity) ;
    void addSalt(boost::python::list& hList, const string& velocity);
    
    /** Run the process */
    void insertSalt();
 
    /** Expose Methods to Python */
    static void exposePython();

protected:
    bool shiftHorizons(vmSaltDefinition& saltDefinition, bool apply=true);
    vgHorizonGroup* getHorizonGrp() const;
      
    void addHorizonPairsToDefinition(boost::python::list& lst, vmSaltDefinition& saltDef);

    /** Definition for only one salt layer */  
    vmSaltDefinition& getSingleLayerDef();
    
    /** Definition for a list of salt layers */  
    list<vmSaltDefinition>& getMultiLayerDef();    
private:
    /** Chunk the volumes to save memory */
    void insertSaltNoChunk();
    
    /** Load all the volumes into memory */
    void insertSaltChunk();    
    vector<int> list2Vector(boost::python::list& lst);

    InsertSaltBase myInserter;
    string myInput;
    string myTarget;
 
    list<vmSaltDefinition>  mySaltDefinition;
    bool                    mySingleLayerDefinition;

    vgHorizonGroup * myHG;
};

#endif	/* VPINSERTSALTAPI_HPP */


