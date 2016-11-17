/* 
 * File:   vpReportDataParameters.cpp
 * Author: ddoledec
 * 
 * Created on January 15, 2016, 5:41 PM
 */

#include "vpReportDataParameters.hpp"

#include "vlBase.hpp"

#include "vmApplication.hpp"
#include "vmDataGroup.hpp"
#include "vmModel.hpp"
#include "vmView3D.hpp"


/*
    DATA_SEISMIC,
    DATA_ATTRIBUTE,
    DATA_HORIZONGROUP,
    DATA_HORIZON,
    DATA_GATHERSET,
    DATA_GATHERPICKS,
    DATA_CIGPICK,
    DATA_WELLLOG,
 */

class AutomaticVolumeLoading {
public:
    AutomaticVolumeLoading(Volume* vol, DATA_TYPE type);
    ~AutomaticVolumeLoading();

    void removeFromTornado();
    void addToTornado();
    bool isMatching(DATA_TYPE t, Volume* d)
    {
        return (myType == t && myVol == d);
    }

private:
    //python should take care of pointers
    //we don't want to delete volumes on Tornado side    
    Volume* myVol;
    DATA_TYPE myType;
    bool myVolumeLoadedInModel;
};

AutomaticVolumeLoading::AutomaticVolumeLoading(Volume* vol, DATA_TYPE type) :
myVol(vol),
myType(type),
myVolumeLoadedInModel(false) {
    addToTornado();
}

AutomaticVolumeLoading::~AutomaticVolumeLoading() {
    removeFromTornado();
}

void AutomaticVolumeLoading::addToTornado() {
    vmView3D& view3D = g_app.getView3D();
    vmModel& model = g_app.getModel();

    if (myVol != 0) {
        int sel = -1;

        if (myType == DATA_SEISMIC) {
            sel = model.getSeismicIdxFromVol(*myVol);
            if (sel < 0) {
                Volume *vol = model.getSeismic(myVol->getPath());
                sel = vol == 0 ? -1 : model.getSeismicIdxFromVol(*vol);
            }

            if (sel < 0)
                model.addSeismic(*myVol, model.getDomain());

        } else if (myType == DATA_ATTRIBUTE) {
            sel = model.getAttributeIdxFromVol(*myVol);
            if (sel < 0) {
                Volume *vol = model.getAttribute(myVol->getPath());
                sel = vol == 0 ? -1 : model.getAttributeIdxFromVol(*vol);
            }

            if (sel < 0)
                model.addAttribute(*myVol, myVol->getName(), model.getDomain());
        }


        if (sel >= 0) {
            myVolumeLoadedInModel = true;
            return;
        }

        Send(model, model.VolumeChanged, myVol);
        view3D.requestRender();
    }
}

void AutomaticVolumeLoading::removeFromTornado() {
    vmView3D& view3D = g_app.getView3D();
    vmModel& model = g_app.getModel();
    if(!myVolumeLoadedInModel)
    {
        if (myVol != 0) {
            int sel = -1;

            if (myType == DATA_SEISMIC) {
                sel = model.getSeismicIdxFromVol(*myVol);
                if (sel >= 0)
                    model.unloadSeismicWithoutDelete(sel, model.getDomain());

            } else if (myType == DATA_ATTRIBUTE) {
                sel = model.getAttributeIdxFromVol(*myVol);
                if (sel >= 0)
                    model.unloadAttributeWithoutDelete(sel, model.getDomain());
            }

            view3D.requestRender();
        }
    }
}



vpReportDataParameters::vpReportDataParameters()
{
}

vpReportDataParameters::vpReportDataParameters(const vpReportDataParameters& orig)
{
}

vpReportDataParameters::~vpReportDataParameters()
{
    myClearData();
}

void vpReportDataParameters::myClearData()
{
    while(!ptr.empty()) delete ptr.front(), ptr.pop_front();
    ptr.clear();
}

void vpReportDataParameters::myRemoveData(DATA_TYPE type, Volume* data)
{
    std::list< AutomaticVolumeLoading* >::iterator it = ptr.begin();
    while( it != ptr.end() )
    {
        AutomaticVolumeLoading* avl = *it;
        if( avl->isMatching(type,data) )
        {
            it = ptr.erase(it);
            delete avl;
        }
        ++ it;
    }
}

vpReportDataParameters& vpReportDataParameters::operator=(const vpReportDataParameters& src)
{
    if(this == &src)
        return *this;

    vmReportDataParameters::operator=( *(dynamic_cast<const vmReportDataParameters*>(&src)) );
    myClearData();
    for(int i = 0; i < src.ptr.size(); ++i)
    {
        ptr.push_back(src.ptr.front()+i);
    }
    return *this;
}

void vpReportDataParameters::clear()
{
    vmReportDataParameters::clear();
    myClearData();
}

void vpReportDataParameters::addSeismic(Volume* data, int idx) {
     add( SEISMIC,  data, idx);
     ptr.push_back( new AutomaticVolumeLoading(data, DATA_SEISMIC) );
}

void vpReportDataParameters::removeSeismic(Volume* data, int idx) {
     remove( SEISMIC,  data, idx);
     myRemoveData(DATA_SEISMIC, data);
}

void vpReportDataParameters::addAttribute(Volume* data, int idx) {
    add( ATTRIBUTE,  data , idx);
    ptr.push_back( new AutomaticVolumeLoading(data, DATA_ATTRIBUTE) );
}

void vpReportDataParameters::removeAttribute(Volume* data, int idx) {
    remove( ATTRIBUTE,  data , idx);
    myRemoveData(DATA_ATTRIBUTE, data);
}

void vpReportDataParameters::addGather(Volume* data, int idx) {
    add( GATHER,  data, idx);
//    ptr.push_back( new AutomaticVolumeLoading(data, DATA_GATHER) );
}

void vpReportDataParameters::removeGather(Volume* data, int idx) {
    remove( GATHER,  data, idx);
}

void vpReportDataParameters::add(DataType type, Volume* data, int idx)
{
    if(!data)
    {
        fprintf(stderr,"WARNING: cannot add python script data!\n");
        return;
    }
    
    while (idx >= size())
    {
        vmDataGroup dataGroup;
        addDataGroup(dataGroup);
    }

    try
    {
        vmDataGroup& dataGroup = getDataGroup(idx);

        if (type == SEISMIC)
        {
            dataGroup.setSeismic(data);
        } else if (type == ATTRIBUTE)
        {
            dataGroup.setAttribute(data);
        } else if (type == GATHER)
        {
            dataGroup.setGatherVol(data);
        }
        return;

    } catch (...)
    {
        cerr << "Catch Unable to Insert data in Parameters" << endl;
    }
    cerr << "Unable to Insert data in Parameters" << endl;
}

void vpReportDataParameters::remove(vpReportDataParameters::DataType type, Volume* data, int idx)
{
    if(!data || idx < 0 || idx >= size())
    {
        fprintf(stderr,"WARNING: cannot remove python script data!\n");
        return;
    }
    
    try
    {
        vmDataGroup& dataGroup = getDataGroup(idx);

        if (type == SEISMIC && dataGroup.getSeismic() == data)
        {
            dataGroup.setSeismic(0);
        } else if (type == ATTRIBUTE && dataGroup.getAttribute() == data)
        {
            dataGroup.setAttribute(0);
        } else if (type == GATHER && dataGroup.getGatherVol() == data)
        {
            dataGroup.setGatherVol(0);
        }
        return;

    } catch (...)
    {
        cerr << "Catch Unable to Remove data from Parameters" << endl;
    }
    cerr << "Unable to Remove data from Parameters" << endl;
}


