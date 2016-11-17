/* 
 * File:   vpPythonContext.hpp
 * Author: ddoledec
 *
 * Created on April 22, 2015, 5:49 PM
 */

#ifndef VPPYTHONCONTEXT_HPP
#define	VPPYTHONCONTEXT_HPP

/**
 * Information about Application running Python (batch/interactive..) 
 */
class vpPythonContext {
public:
    enum ModuleMode {
        INTERACTIVE, BATCH
    };

    vpPythonContext();
    virtual ~vpPythonContext();

    /**
     * The way it is used (batch, interactive...)
     */
    ModuleMode getMode() const;
    void setMode(ModuleMode mode);
    bool isInteractiveMode();
    bool isBatchMode();

    /** Memory in giga. Deault is set to 8Go  */
    float getAvailableMemory() const {
        return myAvailableMemory;
    }

    /** Memory in giga */
    void setAvailableMemory(float memory) {
        myAvailableMemory = memory;
    }

    /** If we allow to run it or if it's available.
        For some permission restriction the application
     *  is able to disable this functionality*/
    bool isAvailable() const ;
    void setAvailable(bool available);
    
    
private:
    ModuleMode myMode;
    float myAvailableMemory;
    bool myIsAvailable;
};

#endif	/* VPPYTHONCONTEXT_HPP */

