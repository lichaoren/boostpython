#include <boost/python.hpp> 
#include "vpHorizonTrimmer.hpp"
#include "vlHorizonGroup.hpp"

vpHorizonTrimmer::vpHorizonTrimmer()
{
    InsertSaltBase::setupCut(true, CUT_PAIR, 0);
}

void vpHorizonTrimmer::exposePython()
{
    using namespace boost::python;
    
    enum_<InsertSaltBase::CutMode>("CutMode")
    .value("CUT_BASE", InsertSaltBase::CUT_BASE)
    .value("CUT_PAIR", InsertSaltBase::CUT_PAIR);
    
    class_<vpHorizonTrimmer>("HorizonTrimmer")
    .def("setCutMode", &InsertSaltBase::setCutDir)
    .def("setOutputPath", &InsertSaltBase::setCutPath)
    .def("setSaltHrzPairsByName", &vpHorizonTrimmer::setSaltHrzPairsByName)
    .def("setSaltHrzPairsByIndex", &vpHorizonTrimmer::setSaltHrzPairsByIndex)
    .def("setSedimentHrzPairsByName", &vpHorizonTrimmer::setSediHrzPairsByName)
    .def("setSedimentHrzPairsByIndex", &vpHorizonTrimmer::setSediHrzPairsByIndex)
    .def("setHorizonGroup", &vpHorizonTrimmer::setHorizonGroup)
    .def("exe", &vpHorizonTrimmer::exe)
    .def("setRadius", &InsertSaltBase::setRadius)
    .def("setAlwaysMatchPair", &InsertSaltBase::setAlwaysMatchPair);
}

void vpHorizonTrimmer::setHorizonGroup(vgHorizonGroup * hg)
{
    InsertSaltBase::setHorizons(hg, false);
}

void vpHorizonTrimmer::exe()
{
    InsertSaltBase::cutBase();
    InsertSaltBase::saveCutHrzGrp();
    
}

void vpHorizonTrimmer::setSaltHrzPairsByName(boost::python::list& lst){
    Py_ssize_t length = boost::python::len(lst);
    vector<pair<int,int> > seg;
    // loop for list
    for (Py_ssize_t i = 0; i < length; i++)
    {
        Py_ssize_t len_each = boost::python::len(lst[i]);
        // need to make sure the parameter list is right
        if (len_each != 2)
            continue;
        string top, btm;
        top = boost::python::extract<string>(lst[i][0]);
        btm = boost::python::extract<string>(lst[i][1]);
        int index_top = top.empty() ? -1 : myHG->getHorizonIndexByName(top.c_str());
        int index_btm = btm.empty() ? -1 : myHG->getHorizonIndexByName(btm.c_str());
        if (!top.empty() && index_top < 0)
        {
            cerr << "Your input horizon " << top << " is a wrong name" << endl;
            exit(-1);
        }
        if (!btm.empty() && index_btm < 0)
        {
            cerr << "Your input horizon " << btm << " is a wrong name" << endl;
            exit(-1);
        }

        cout << "The pair [" << top << "," << btm << "] corresponding to [" << index_top << ","
                << index_btm << "] has been added to insert salt horizons" << endl;

        seg.push_back(pair<int, int>(index_top, index_btm));
    }
    
    InsertSaltBase::setSaltPairs(seg);
}

void vpHorizonTrimmer::setSaltHrzPairsByIndex(boost::python::list& lst){
    Py_ssize_t length = boost::python::len(lst);
    vector<pair<int,int> > seg;
    // loop for list
    for (Py_ssize_t i = 0; i < length; i++)
    {
        Py_ssize_t len_each = boost::python::len(lst[i]);
        // need to make sure the parameter list is right
        if (len_each != 2)
            continue;
        
        int index_top = boost::python::extract<int>(lst[i][0]) - 1;
        int index_btm = boost::python::extract<int>(lst[i][1]) - 1;
        
        
        int nHrz = myHG->getNumHorizon();
        if (index_top >= nHrz || index_top < 0)
        {
            cerr << "Your input horizon " << index_top + 1 << " is a wrong index" << endl;
            exit(-1);
        }
        if (index_btm >= nHrz || index_btm < 0)
        {
            cerr << "Your input horizon " << index_btm + 1 << " is a wrong index" << endl;
            exit(-1);
        }
        
        if(!myHG->getHorizonByIndex(index_top) || !myHG->getHorizonByIndex(index_btm))
        {
            cerr << "Cannot get the horizons." << endl;
            exit(-1);
        }
        vString top = myHG->getHorizonByIndex(index_top)->getName();
        vString btm = myHG->getHorizonByIndex(index_btm)->getName();
        cout << "The pair [" << top.c_str() << "," << btm.c_str() << "] corresponding to [" << index_top+1 << ","
                << index_btm+1 << "] has been added to insert salt horizons" << endl;

        seg.push_back(pair<int, int>(index_top, index_btm));
    }
    
    InsertSaltBase::setSaltPairs(seg);
}


void vpHorizonTrimmer::setSediHrzPairsByName(boost::python::list& lst){
    Py_ssize_t length = boost::python::len(lst);
    vector<pair<int,int> > seg;
    // loop for list
    for (Py_ssize_t i = 0; i < length; i++)
    {
        Py_ssize_t len_each = boost::python::len(lst[i]);
        // need to make sure the parameter list is right
        if (len_each != 2)
            continue;
        string top, btm;
        top = boost::python::extract<string>(lst[i][0]);
        btm = boost::python::extract<string>(lst[i][1]);
        int index_top = top.empty() ? -1 : myHG->getHorizonIndexByName(top.c_str());
        int index_btm = btm.empty() ? -1 : myHG->getHorizonIndexByName(btm.c_str());
        if (!top.empty() && index_top < 0)
        {
            cerr << "Your input horizon " << top << " is a wrong name" << endl;
            exit(-1);
        }
        if (!btm.empty() && index_btm < 0)
        {
            cerr << "Your input horizon " << btm << " is a wrong name" << endl;
            exit(-1);
        }

        cout << "The pair [" << top << "," << btm << "] corresponding to [" << index_top << ","
                << index_btm << "] has been added to insert sediment horizons" << endl;

        seg.push_back(pair<int, int>(index_top, index_btm));
    }
    
    InsertSaltBase::setSediPairs(seg);
}

void vpHorizonTrimmer::setSediHrzPairsByIndex(boost::python::list& lst){
    Py_ssize_t length = boost::python::len(lst);
    vector<pair<int,int> > seg;
    // loop for list
    for (Py_ssize_t i = 0; i < length; i++)
    {
        Py_ssize_t len_each = boost::python::len(lst[i]);
        // need to make sure the parameter list is right
        if (len_each != 2)
            continue;
        
        int index_top = boost::python::extract<int>(lst[i][0]) - 1;
        int index_btm = boost::python::extract<int>(lst[i][1]) - 1;
        
        
        int nHrz = myHG->getNumHorizon();
        if (index_top >= nHrz || index_top < 0)
        {
            cerr << "Your input horizon " << index_top + 1 << " is a wrong index" << endl;
            exit(-1);
        }
        if (index_btm >= nHrz || index_btm < 0)
        {
            cerr << "Your input horizon " << index_btm + 1 << " is a wrong index" << endl;
            exit(-1);
        }
        
        if(!myHG->getHorizonByIndex(index_top) || !myHG->getHorizonByIndex(index_btm))
        {
            cerr << "Cannot get the horizons." << endl;
            exit(-1);
        }
        vString top = myHG->getHorizonByIndex(index_top)->getName();
        vString btm = myHG->getHorizonByIndex(index_btm)->getName();
        cout << "The pair [" << top.c_str() << "," << btm.c_str() << "] corresponding to [" << index_top+1 << ","
                << index_btm+1 << "] has been added to insert sediment horizons" << endl;

        seg.push_back(pair<int, int>(index_top, index_btm));
    }
    
    InsertSaltBase::setSediPairs(seg);
}
