#include  <Python.h>
# include <boost/python/errors.hpp>


#include <QStringList>
#include <QDirIterator>

#include "vlPythonInterpreter.hpp"
#include "vlDirectInterpreter.hpp"
#include "vString.hpp"


static QString vPYTHON_PATH = ":/vpython/";
static QString vPYTHON_EXTENSION = ".py";

vlPythonInterpreter* vlPythonInterpreter::myPyInstance=0;


vlPythonInterpreter& vlPythonInterpreter::getInstance()
{
  if (!myPyInstance)
    myPyInstance= new vlPythonInterpreter;
  return (*myPyInstance);
}


vlPythonInterpreter::vlPythonInterpreter() : myPyFileName(0), myLastReturnStatus(true)
{
    myInterpreter = new vlDirectInterpreter();
}


vlPythonInterpreter::~vlPythonInterpreter()
{
    Py_Finalize();
}

/** Set Python file to run */
bool vlPythonInterpreter::setFile(const char* fileName)
{
    if (myPyFileName)
        free(myPyFileName);

    myPyFileName = strdup(fileName);
    
    // may test existence...
    return true;
}

bool vlPythonInterpreter::run()
{
    if (getContext().isAvailable() == false)
        return false;
    
    if (myPyFileName == 0)
        return false;
    
    myLastReturnStatus = myInterpreter->run(myPyFileName);

    return myLastReturnStatus;
}


void vlPythonInterpreter::reset()
{
  if (myPyFileName)
    {
#if (PY_MAJOR_VERSION>2) || (PY_MINOR_VERSION>=5)
              Py_DECREF(myPyFileName);
#else
#warning Change Py_DECREF
#endif
        myPyFileName = 0;
    }
}

bool vlPythonInterpreter::runCMD(const char* cmd)
{
    if (getContext().isAvailable()==false)
        return false;
    
    return myInterpreter->runCMD(cmd);
}
/** List of predefined scripts (not the full path but a "simplifications")*/
QStringList vlPythonInterpreter::getScriptNames() const
{
    Q_INIT_RESOURCE(vpython);
    QStringList list;
    QDirIterator it(vPYTHON_PATH, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QString fullPath = it.next();

#if __cplusplus <= 199711L
        if (fullPath.contains(vPYTHON_EXTENSION, Qt::CaseInsensitive))
#else
        if (fullPath.contains(vPYTHON_EXTENSION, Qt::CaseSensitivity::CaseInsensitive))
#endif
            
        {
            fullPath.remove(vPYTHON_PATH);
            fullPath.remove(vPYTHON_EXTENSION);
            list << fullPath;
        }
    }
    return list;
}


/** Qt resource file path */
QString vlPythonInterpreter::getScriptPath(const QString& name) const
{
    QString fullPath = vPYTHON_PATH+name+vPYTHON_EXTENSION;
    
    QFile file(fullPath);
    
    return file.exists()?fullPath:"";
}

void vlPythonInterpreter::setError(vpErrorLevel level, const vString& message) {
    cerr << "Tornado: " << message.c_str() << endl;

    vpPythonContext& ctxt = getContext();
    if (ctxt.getMode() == vpPythonContext::BATCH) {
        if (level == Warning || level == Message) {
            return;
        }

        if (level == Error || level == Critical) {
            PyErr_SetString(PyExc_RuntimeError, message.c_str());
            exit(-1);
        }
    } else {
        if (level == Warning || level == Message ) {
//            PyErr_SetString(PyExc_UserWarning, message.c_str());
//            PyErr_Clear();
            return; 
        } else {
            PyErr_SetString(PyExc_RuntimeError, message.c_str());
            boost::python::throw_error_already_set();
        }
    }

}

