
#ifndef VLPYTHONINTERPRETER_H
#define VLPYTHONINTERPRETER_H

#include "vpPythonContext.hpp"

class vlInterpreter;
class QStringList;
class QString;
class vString;

/*
  python interpreter embeded in tornado
 */
class vlPythonInterpreter {
public:
    enum vpErrorLevel { 
        Message,
        Warning,
        Error,
        Critical
    };
    
    
    /** Set Python file to run */
    bool setFile(const char* fileName);
    /* run the python script*/
    bool run();
    /* singleton get method*/
    static vlPythonInterpreter& getInstance();
    /* after the run of script, reset the state*/
    void reset();
    bool runCMD(const char*);

    /** List of predefined scripts (not the full path but a "simplifications")*/
    QStringList getScriptNames() const;

    /** Qt resource file path */
    QString getScriptPath(const QString& name) const;

    /** Information about Application running Python (batch/interactive..) */
    vpPythonContext& getContext() {return myContext;}
    
    /** Does the last run success */
    bool getLastReturnStatus() const { return myLastReturnStatus; }
    
    /** According to the severity can exit */
    void setError(vpErrorLevel level, const vString& message );
    
    
protected:
    /*hide the constructors*/
    vlPythonInterpreter();
    virtual ~vlPythonInterpreter();
private:
    char* myPyFileName;
    static vlPythonInterpreter* myPyInstance;
    vlInterpreter* myInterpreter;
    vpPythonContext myContext;
    bool myLastReturnStatus;
};

#endif


