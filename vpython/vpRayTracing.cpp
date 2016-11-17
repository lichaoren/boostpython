#include "vpRayTracing.hpp"
#include "vlPythonInterpreter.hpp"
// ----------------------------------------------------------------------
vpRayTracing::vpRayTracing() :
    mySamRayTrace( 0 ),
    myVol_v( 0 ),
    myVol_d( 0 ),
    myVol_e( 0 ),
    myVol_t( 0 ),
    myVol_p( 0 ),
    myCMapId( -1 ),
    myUp( true ),
    myAcceptRays( 1 ),
    myMaxVerticalAngle( 80.0 ),
    myReflectHrz( 0 ),
    myRecordHrz( 0 ),
    myPlot( true ),
    myPlotAll( false ),
    myWaterBottom(NULL),
    myNumberOfThreads(1),
    myFirstx(0),
    myFirsty(0),
    myLastx(0),
    myIncx(0),
    myLasty(0),
    myIncy(0),
    myRadius(0),
    myRecvDepth(0)
{
    myReflectorOffsetMin.clear();
    myReflectorOffsetMax.clear();
    myReflectorOffsetAttrId.clear();
    mySourceOffsetMin.clear();
    mySourceOffsetMax.clear();
    mySourceOffsetAttrId.clear();
    myReceiverOffsetMin.clear();
    myReceiverOffsetMax.clear();
    myReceiverOffsetAttrId.clear();
    // create a timer

    myNx_mask = myNy_mask = 0;
    myMinAX = -40;
    myMaxAX = 40;
    myNumAX = 17;
    myMinAY = 0;
    myMaxAY = 0;
    myNumAY = 1;
    myMinAAz = 0;
    myMaxAAz = 0;
    myNumAAz = 1;
    myReflectorAngleX = 0;
    myReflectorAngleY = 0;
    myRoseGeom_xl = 500;
    myRoseGeom_sl = 600;
    myFoldGeom_xl = 82.021;
    myFoldGeom_sl = 82.021;
}

vpRayTracing::~vpRayTracing() 
{
    delete myFP;
    delete myOFP;
}


void vpRayTracing::setMethod(int mode)
{
    myMethod = mode;
}

void vpRayTracing::setVelocityPath(vString vel, vString delta, vString epilson, vString theta, vString phi)
{
    myVelPath = vel;
    myDelPath = delta;
    myEpiPath = epilson;
    myThePath = theta;
    myPhiPath = phi;
}

void vpRayTracing::setRefHorizon(vString hrzPath, vString hrzName)
{
    myHrzPath = hrzPath;
    myRefHrz = hrzName;
}

void vpRayTracing::setWaterBottom(vString waterBottomHrzPath, vString hrzName)
{
    myWBPath = waterBottomHrzPath;
    myWBHrz = hrzName;
}

void vpRayTracing::setRayAngle(float minX, float maxX, int numX, float minY, float maxY, int numY, 
                               float minAzimuth, float maxAzimuth, int numAzimuth, int MaxAngleNum)
{
    myMinAX = minX;
    myMaxAX = maxX;
    myNumAX = numX;
    myMinAY = minY;
    myMaxAY = maxY;
    myNumAY = numY;
    myMinAAz = minAzimuth;
    myMaxAAz = maxAzimuth;
    myNumAAz = numAzimuth;
    myMaxRayAngle = MaxAngleNum;
}


void vpRayTracing::setAcceptAllRays()
{
    myAcceptRays = 0;
}


void vpRayTracing::setAcceptCompleteRays()
{
    myAcceptRays = 1;
}


void vpRayTracing::setAcceptRaysOnSurfaceLine( int depth, float radius, int receiverMode, float firstX, float lastX, 
                                              float firstY, float lastY, float IncX, float IncY)
{
    myRadius = radius;
    myAcceptRays = 2;
    mySurface = receiverMode;
    myFirstx = firstX;
    myLastx = lastX;
    myFirsty = firstY;
    myLasty = lastY;
    myIncx = IncX;
    myIncy = IncY;
    myRecvDepth = depth;
}

void vpRayTracing::setShotUp()
{
    myUp = true;
}


void vpRayTracing::setShotDown()
{
    myUp = false;
}


void vpRayTracing::setDepthDipAsSupplied(int dipx, int dipy)
{
    myDepthDipMode = 0;
    myDipInX = dipx;
    myDipInY = dipy;
}

void vpRayTracing::setDepthDipAsHorizon(vString dipHrz)
{
    myDepthDipMode = 1;
    myDipHrz = dipHrz;
}

void vpRayTracing::setMultiSourceFile(vString filePath)
{
    mySrcMode = 0;
    mySrcFile = filePath;
}


void vpRayTracing::setMultiSourceInLine(float posX, float posY, float posZ, 
                                        float incX, float incY, int numOfshot, 
                                        float SrcLineIncX, float SrcLineIncY, int SrcLineNum)
{
    mySrcMode = 1;
    myX = posX;
    myY = posY;
    myZ = posZ;
    myShotXInc = incX;
    myShotYInc = incY;
    myShotNum = numOfshot;
    myLineXInc = SrcLineIncX;
    myLineYInc = SrcLineIncY;
    myLineNum = SrcLineNum;
}


void vpRayTracing::setMultiSourceInDirLine(float posX, float posY, float posZ, 
                                           float dirLineSpace, float dirShotSpace, float dirLineAngle)
{
    mySrcMode = 2;
    myX = posX;
    myY = posY;
    myZ = posZ;
    myLineSpacing = dirLineSpace;
    myShotSpacing = dirShotSpace;
    myLineAngle = dirLineAngle;
}


void vpRayTracing::setMultiSourceSeisFile(vString filePath, int radius, vString grpName, 
                                          vString shotX, vString shotY, vString shotZ, 
                                          vString recX, vString recY, vString recZ)
{
    mySrcMode = 3;
    mySrcFile = filePath;
    mySeisGrp = grpName;
    mySeisSx = shotX;
    mySeisSy = shotY;
    mySeisSz = shotZ;
    mySeisRx = recX;
    mySeisRy = recY;
    mySeisRz = recZ;
    myRadius = radius;
}


void vpRayTracing::setMultiSourceSrcRecFile(vString filePath, int radius)
{
    mySrcMode = 4;
    mySrcFile = filePath;
    myRadius = radius;
}

void vpRayTracing::setRoseDiagram(vString filePath, float geom_xl, float geom_sl)
{
    myRoseOutPath = filePath;
    myRoseGeom_xl = geom_xl;
    myRoseGeom_sl = geom_sl;
}

  
void vpRayTracing::setShotFold(vString filePath, float geom_xl, float geom_sl)
{
    myFoldOutPath = filePath;
    myFoldGeom_xl = geom_xl;
    myFoldGeom_sl = geom_sl;
}


void vpRayTracing::setRayPath(vString filePath)
{
    myOutPath = filePath;
}

 
void vpRayTracing::setSummary(vString filePath, bool angleInfo, bool saltTime, bool InitialAngle)
{
    mySummaryFile = filePath;
    myAngleInfo = angleInfo;
    myExcludeSalt = saltTime;
    myInitialVector = InitialAngle;
}


void vpRayTracing::setThreadNum(int threadNum)
{
    myNumberOfThreads = threadNum;
}

void vpRayTracing::onSave()
{
  if (myOutPath.size() > 1)  
  {
    fprintf(stdout,"save ray path to %s\n", myOutPath.c_str());
    g_app.getModel().getSamRayTracePlot()->save((char*)myOutPath.c_str());
  }
  else fprintf(stderr,"Will not save ray path file\n");
}

void vpRayTracing::onPrint()
{
 
  std::cout<<"Print parameter for ray tracing..."<<std::endl; 
  std::cout<<"myMethod      "<<myMethod<<std::endl; 
  std::cout<<"myVelPath     "<<myVelPath<<std::endl;
  if (myMethod != 0)
  { 
    std::cout<<"myDelPath     "<<myDelPath<<std::endl; 
    std::cout<<"myEpiPath     "<<myEpiPath<<std::endl; 
    if (myMethod != 1)
    {
      std::cout<<"myThePath     "<<myThePath<<std::endl; 
      std::cout<<"myPhiPath     "<<myPhiPath<<std::endl; 
    }
  }
  std::cout<<"myHrzPath     "<<myHrzPath<<std::endl;
  std::cout<<"myRefHrz      "<<myRefHrz<<std::endl;

  std::cout<<"myMinAX       "<<myMinAX<<std::endl; 
  std::cout<<"myMaxAX       "<<myMaxAX<<std::endl; 
  std::cout<<"myNumAX       "<<myNumAX<<std::endl; 
  std::cout<<"myMinAY       "<<myMinAY<<std::endl; 
  std::cout<<"myMaxAY       "<<myMaxAY<<std::endl; 
  std::cout<<"myNumAY       "<<myNumAY<<std::endl; 
  std::cout<<"myMinAAz      "<<myMinAAz<<std::endl; 
  std::cout<<"myMaxAAz      "<<myMaxAAz<<std::endl; 
  std::cout<<"myNumAAz      "<<myNumAAz<<std::endl; 
  std::cout<<"myMaxRayAngle "<<myMaxRayAngle<<std::endl;

  std::cout<<"myAcceptRays  "<<myAcceptRays<<std::endl;
  if (myAcceptRays == 2)
  { 
    std::cout<<"mySurface     "<<mySurface<<std::endl;
    std::cout<<"myFirstx      "<<myFirstx<<std::endl;
    std::cout<<"myLastx       "<<myLastx<<std::endl;
    std::cout<<"myIncx        "<<myIncx<<std::endl;
    std::cout<<"myFirsty      "<<myFirsty<<std::endl;
    std::cout<<"myLasty       "<<myLasty<<std::endl;
    std::cout<<"myIncy        "<<myIncy<<std::endl;
    std::cout<<"myRadius      "<<myRadius<<std::endl;
    std::cout<<"myRecvDepth   "<<myRecvDepth<<std::endl;
  }

  std::cout<<"myUp          "<<myDepthDipMode<<std::endl;
  std::cout<<"myDepthDipMode"<<myDepthDipMode<<std::endl;
  if (!myDepthDipMode)
  {
    std::cout<<"myDipInX      "<<myDipInX<<std::endl;
    std::cout<<"myDipInY      "<<myDipInY<<std::endl;
  }
  else std::cout<<"myDipHrz      "<<myDipHrz<<std::endl;
  std::cout<<"mySrcMode     "<<mySrcMode<<std::endl;

  if (mySrcMode  == 1)
  {
    std::cout<<"myShotXInc    "<<myShotXInc<<std::endl;
    std::cout<<"myShotYInc    "<<myShotYInc<<std::endl;
    std::cout<<"myShotNum     "<<myShotNum<<std::endl;
    std::cout<<"myLineXInc    "<<myLineXInc<<std::endl;
    std::cout<<"myLineYInc    "<<myLineYInc<<std::endl;
    std::cout<<"myLineNum     "<<myLineNum<<std::endl;
    std::cout<<"myX           "<<myX<<std::endl;
    std::cout<<"myY           "<<myY<<std::endl;
    std::cout<<"myZ           "<<myZ<<std::endl;
  }
  else 
  {
    std::cout<<"mySrcFile     "<<mySrcFile<<std::endl;
    std::cout<<"myRadius      "<<myRadius<<std::endl;
    if (mySrcMode ==3)
    {
      std::cout<<"mySeisGrp     "<<mySeisGrp<<std::endl;
      std::cout<<"mySeisSx      "<<mySeisSx<<std::endl;
      std::cout<<"mySeisSy      "<<mySeisSy<<std::endl;
      std::cout<<"mySeisSz      "<<mySeisSz<<std::endl;
      std::cout<<"mySeisRx      "<<mySeisRx<<std::endl;
      std::cout<<"mySeisRy      "<<mySeisRy<<std::endl;
      std::cout<<"mySeisRz      "<<mySeisRz<<std::endl;
    }
  }
  std::cout<<"myOutPath     "<<myOutPath<<std::endl;
  std::cout<<"mySummaryFile "<<mySummaryFile<<std::endl;
  std::cout<<"myNumberThread"<<myNumberOfThreads<<std::endl;
}

static FdmDiskCube *loadFdmDisk (const char *path, TaskStatus *prog)
{
  FdmDiskCube *cube = new FdmDiskCube ();
  cube->setPath (path);
  cube->setProgressor (prog);
  if (cube->loadCube () == false)
  {
    fprintf (stderr, "Error in loading %s\n", path);
    delete cube;
    return NULL;
  }
  cube->buildDataLimit();
  return cube;
}

static bool loadAttr (const vString &inp_path, const vString &attr,
                      const vString &geo_path, int dom, bool disk, TaskStatus *prog)
{
  vmModel &m = g_app.getModel();
  if (! disk) {
    if(!m.loadAttribute(inp_path, attr, geo_path, dom, prog)) 
    {
      vString errMsg = "ERROR: failed to load input model:";
      errMsg += inp_path;
      vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
      return false;
    }
    return true;
  }
  FdmDiskCube *fcube = loadFdmDisk (inp_path.c_str(), prog);
  if (fcube == NULL) 
  {
    vString errMsg = "ERROR: failed to load input model:";
    errMsg += inp_path;
    vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
    return false;
  }
  Volume* vol = new Volume;
  vol->setCube (fcube);

  m.addAttribute (*vol, attr, dom);
  return true;
}


#include "FS.hpp"
void vpRayTracing::onShootRays()
{

    // update threading options

    bool iondisk = false;
    TaskStatus* progress = NULL;

    vmModel& m = g_app.getModel();

    Volume* vol_v = 0;
    Volume* vol_d = 0;
    Volume* vol_e = 0;
    Volume* vol_t = 0;
    Volume* vol_p = 0;


    printf("Loading velocity volume:%s\n",myVelPath.c_str());
    if(!loadAttr(myVelPath, "VINT", "", DOMAIN_DEPTH, iondisk, progress))
    {
        vString errMsg = "ERROR: failed to load velocity model:";
        errMsg += myVelPath;
        vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
    }

    vol_v = m.getCurAttribute();

    if (myMethod != 0)
    {
        printf("Loading delta volume:%s\n",myDelPath.c_str());
        if(!loadAttr(myDelPath, "VINT", "", DOMAIN_DEPTH, iondisk, progress))
        {
          vString errMsg = "ERROR: failed to load delta model:";
          errMsg += myDelPath;
          vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
          return;
        }

        vol_d = m.getCurAttribute();

        printf("Loading epislon volume:%s\n",myEpiPath.c_str());
        if(!loadAttr(myEpiPath, "VINT", "", DOMAIN_DEPTH, iondisk, progress))
        {
          vString errMsg = "ERROR: failed to load epison model:";
          errMsg += myEpiPath;
          vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
          return;
        }

        vol_e = m.getCurAttribute();

        if (myMethod != 1)
        {
          printf("Loading theta volume:%s\n",myThePath.c_str());
          if(!loadAttr(myThePath, "VINT", "", DOMAIN_DEPTH, iondisk, progress))
          {
            vString errMsg = "ERROR: failed to load theta model:";
            errMsg += myThePath;
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
            return;
          }

          vol_t = m.getCurAttribute();

          printf("Loading Phi volume:%s\n",myPhiPath.c_str());
          if(!loadAttr(myPhiPath, "VINT", "", DOMAIN_DEPTH, iondisk, progress))
          {
            vString errMsg = "ERROR: failed to load Phi model:";
            errMsg += myPhiPath;
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
            return;
          }

          vol_p = m.getCurAttribute();
        }
    }

    if (!myUp)
    {
        myReflectorAngleX = 0.0;
        myReflectorAngleY = 0.0;
    }

    vmSamRayTracePlot* rayPlot = g_app.getModel().getSamRayTracePlot();
    if (rayPlot == 0) 
    {
        fprintf(stderr,"Invalid ray plot!");
        return;
    }

    myPlot = true;
    myPlotAll = false; // exclude failed ray

    bool RecLineIsChecked = false;

    if (myNumberOfThreads > 1)
    {
        myPlot = false;
        myPlotAll = false;
    }

    rayPlot->setIsLine(RecLineIsChecked);
    rayPlot->setRecFirstX(myFirstx);
    rayPlot->setRecFirstY(myFirsty);
    rayPlot->setRecLastX(myLastx);
    rayPlot->setRecLastY(myLasty);
    rayPlot->setRecRad(myRadius);
    rayPlot->setRecIncX(myIncx);
    rayPlot->setRecIncY(myIncy);
    rayPlot->setAngleGather(myAngleInfo);
    rayPlot->setExtrudeSalt(myExcludeSalt);
    rayPlot->setIniAngCheck(myInitialVector);
    rayPlot->setPairCheck(false);
    if (myFirstx > myLastx || myFirsty > myLasty || myIncx < 0 || myIncy < 0) 
    {
        fprintf(stderr,"Invalid Receiver setting!");
        return;
    }

// Incident angle is set to 0 90
    rayPlot->setMinIncidentAngle(Deg2Rad(0));
    rayPlot->setMaxIncidentAngle(Deg2Rad(90));

    bool** recMask = 0;
    myNx_mask = 0, myNy_mask = 0;
    if (myAcceptRays == 2 && mySurface ==2)
    {
        if (myIncx == 0 || myIncy == 0) 
        {
            fprintf(stderr,"Inc X and Inc Y cannot be 0!");
            return;
        }
        myNx_mask = int((myLastx - myFirstx)/myIncx + 1.5);
        myNy_mask = int((myLasty - myFirsty)/myIncy + 1.5);

        new2d(recMask, myNx_mask, myNy_mask);
    }


    myReflectHrz = 0;
    myRecordHrz = 0;
    if ( !myHrzPath.isBlank() )
    {
        printf("Load horizon file: %s\n", myHrzPath.c_str());
        if(!m.loadHorizonGroup(myHrzPath,progress))
        {
            vString errMsg = "ERROR: failed to load horizon:";
            errMsg += myHrzPath;
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
            return;
        }

        vgHorizonGroup* hg = g_app.getModel().getCurHorizonGroup();
        if (hg == 0)
        {
            fprintf(stderr,"Horizon group not found!");
            return;
        }

        if (hg->getAttrIndex(ATTR_NAME_DEPTH) == -1)
        {
            fprintf(stderr,"Horizon does not have DEPTH!");
            return;
        }

        // FIXME !!! Index is hard coded
        int refHrzIdx = hg->getHorizonIndexByName(myRefHrz.c_str());
        myReflectHrz = hg->getHorizonByIndex(refHrzIdx);
        if (!myReflectHrz)
        {
            fprintf(stderr,"Horizon chosen as reflector not found!");
        }
        else
        {
            fprintf(stdout,"Reflecting horizon = %s\n", myReflectHrz->getName().c_str());
            myRecordHrz = myReflectHrz;
        }

    }
  
    myWaterBottom = 0;
    if(myWBPath != "")
    {
        printf("Load water bottom horizon file: %s\n", myWBPath.c_str());
        if(!m.loadHorizonGroup(myWBPath, progress))
        {
            vString errMsg = "ERROR: failed to load horizon:";
            errMsg += myHrzPath;
            vlPythonInterpreter::getInstance().setError(vlPythonInterpreter::Error, errMsg.c_str());
            return;
        }

        vgHorizonGroup* hg = g_app.getModel().getCurHorizonGroup();
        if (hg == 0)
        {
            fprintf(stderr,"Horizon group not found!");
            return;
        }

        if (hg->getAttrIndex(ATTR_NAME_DEPTH) == -1)
        {
            fprintf(stderr,"Horizon does not have DEPTH!");
            return;
        }

        // FIXME !!! Index is hard coded
        int wbHrzIdx = hg->getHorizonIndexByName(myWBHrz.c_str());
        myWaterBottom = hg->getHorizonByIndex(wbHrzIdx);
        if (!myWaterBottom)
        {
            fprintf(stderr,"Horizon chosen as water bottom not found!");
        }
        else
        {
            fprintf(stdout,"Water bottom horizon = %s\n", myWaterBottom->getName().c_str());
        }

        if (myWaterBottom != 0)
        {
            g_app.getModel().getSamRayTracePlot()->setWatBotHor(myWaterBottom);
        }

    }

    if (myUp && myReflectHrz!=0)
    {
        fprintf(stderr,"Reflecting up-going rays not yet implemented!");
        return;
    }

    bool ok = false;
    ok = myRayInit(vol_v, vol_d, vol_e, vol_t, vol_p);
    if (!ok)
    {
        fprintf(stderr,"Problem initializing ray-tracing!");
        return;
    }

    mySamRayTrace->setAngleMax(myMaxRayAngle);


    //Make sure the destination file can be opened for writing
    myFP = new FileAccess();
    myOFP = new FileAccess();
    vString finalSummaryFile;
    if (mySummaryFile.isBlank())
    {
        fprintf(stderr,"mySummaryFile has not been set!\n");
    }
    else
    {
        finalSummaryFile = mySummaryFile;
        std::cout<<"MyOFP "<<mySummaryFile<<std::endl;
        myOFP->open(mySummaryFile.c_str(), "w+");
        if ( myOFP && !myOFP->isOpened() ) 
        {
            fprintf(stderr,"Error opening file %s!\n", mySummaryFile.c_str());
            return;
        }

        //Save to a temporary file first before writing the ray pairs
        vString btok, atok;
        mySummaryFile.splitAtLastToken( btok, atok, "/" );
        mySummaryFile = FS::scratchDirectory();
        mySummaryFile += "tmp_";
        mySummaryFile << atok;
        //Write the file to scratch directory first before adding pairs' info.
        std::cout<<"MyFP "<<mySummaryFile<<std::endl;
        if( !myFP->open(mySummaryFile.c_str(), "w+") )
        {
            fprintf(stderr,"Error opening file!");
            return;
        }
    }

    vector<float> vectorX, vectorY, vectorZ, vectorAngleX, vectorAngleY;
    vector< vector<Receiver> > vectorReceivers;

  //Multiple source
    if(true) 
    {

        if (mySrcMode == 1) 
        {

            float x_line = myX;
            float y_line = myY;
            float z      = myZ;

            for (int i_line=0; i_line<myLineNum; ++i_line)
            {
                fprintf (stderr, "\n");
                fprintf (stderr, "INFO: Working on Shot-line %d out of %d\n", i_line+1, myLineNum);
                myX = x_line;
                myY = y_line;

                for (int i_shot=0; i_shot<myShotNum; ++i_shot) {
                  vectorX.push_back( myX );
                  vectorY.push_back( myY );
                  
                  myX += myShotXInc;
                  myY += myShotYInc;
                }

                x_line += myLineXInc;
                y_line += myLineYInc;
            }
        }
        else if (mySrcMode == 0)
        {
            FileAccess fp;

            if ( !fp.open(mySrcFile.c_str(), "r") )
            {
                fprintf(stderr,"Error opening source file!");
                return;
            }

            char buf[MAX_LINE_LENGTH];
            while ( !fp.is_eof() )
            {
                if (fp.gets(buf, MAX_LINE_LENGTH) == NULL)
                    continue;

                if (sscanf(buf, "%f %f %f", &myX, &myY, &myZ) == 3)
                {
                  vectorX.push_back( myX );
                  vectorY.push_back( myY );
                  vectorZ.push_back( myZ );
                }
            }

            fp.close();
        }
        else if (mySrcMode == 2)
        {
            if(!myGetShots(vol_v, myX, myY, myShotSpacing, myLineSpacing, myLineAngle, vectorX, vectorY))
            {
                fprintf(stderr,"Failed to get shot locations!");
                return;
            }
        }
        else if (mySrcMode ==4)
        {
            FileAccess fp;

            if( !fp.open(mySrcFile.c_str(), "r" ) )
            {
                fprintf(stderr,"Error opening source-receiver file!");
                return;
            }
            char buf[MAX_LINE_LENGTH];
            int idx, nrec;
            int rcvIdx = 0;
            while ( !fp.is_eof() ) 
            {
                vector<Receiver> receivers;
                if ( fp.gets(buf, MAX_LINE_LENGTH) == NULL )
                  continue;

                if (sscanf(buf, "%f %f %f %d", &myX, &myY, &myZ, &nrec) == 4) 
                {
                    vectorX.push_back( myX );
                    vectorY.push_back( myY );
                    vectorZ.push_back( myZ );
                    vectorAngleX.push_back(myReflectorAngleX);
                    vectorAngleY.push_back(myReflectorAngleY);
                }
                else 
                {
                    continue;
                }

                for (int i = 0; i < nrec; ++ i) 
                {
                    if ( fp.gets(buf, MAX_LINE_LENGTH) == NULL ) 
                    {
                        fprintf(stderr,"Failed to read receiver!");
                        return;
                    }
                    float xloc, yloc, zloc;
                    if (sscanf(buf, "%f %f %f", &xloc, &yloc, &zloc) == 3) 
                    {
                        receivers.push_back(Receiver(xloc, yloc, zloc, rcvIdx++));
                    }
                }

                vectorReceivers.push_back(receivers);
            }
            fp.close();
            myVectorReceivers.clear();
            myVectorX.clear();
            myVectorY.clear();
            myVectorZ.clear();
            myVectorReceivers = vectorReceivers;
            myVectorX = vectorX;
            myVectorY = vectorY;
            myVectorZ = vectorZ;
        }
        else
        {
            vHandle<Store> store = g_app.getModel().openStore(mySrcFile);
            if (store.isEmpty()) 
            {
                fprintf(stderr,"Failed to open store!");
                return;
            }
            Attribute attr_grp, attr_shotx, attr_shoty, attr_shotz, attr_revx, attr_revy, attr_revz;
       
            const char* const grpNames[] = {mySeisGrp.c_str(),""}; 
            const char* const szNames[] = {mySeisSz.c_str(),""};  
            const char* const rzNames[] = {mySeisRz.c_str(),""}; 
            const char* const sxNames[] = {mySeisSx.c_str(), ""};
            const char* const syNames[] = {mySeisSy.c_str(), ""};
            const char* const rxNames[] = {mySeisRx.c_str(), ""};
            const char* const ryNames[] = {mySeisRy.c_str(), ""};
            if (!queryAndSetAttr(*store, grpNames, attr_grp) || !queryAndSetAttr(*store, sxNames, attr_shotx) || !queryAndSetAttr(*store, syNames, attr_shoty)
              || !queryAndSetAttr(*store, szNames, attr_shotz) || !queryAndSetAttr(*store, rxNames, attr_revx) || !queryAndSetAttr(*store, ryNames, attr_revy)
              || !queryAndSetAttr(*store, rzNames, attr_revz) )
            {
                fprintf(stderr,"Failed to find attributes!");
                return;
            }

            Int64 num_ins = store->getNInstances();
            Int64 first,last, g1, g2=0;
            bool start = false;
            vector<Receiver> receivers;
            int rcvIdx = 0;

            for ( first = 1; first <= num_ins; first += 5000 ) 
            {
                last = min( first+5000-1, num_ins );
                if ( last > num_ins ) last = num_ins;

                store->read( attr_grp, first, last );
                store->read( attr_shotx, first, last );
                store->read( attr_shoty, first, last );
                store->read( attr_shotz, first, last );
                store->read( attr_revx, first, last );
                store->read( attr_revy, first, last );
                store->read( attr_revz, first, last );

                g1 = (int)attr_grp.getValue(1);

                if (start && g1 != g2 ) 
                {
                    vectorReceivers.push_back(receivers);
                    receivers.clear();
                    vectorX.push_back( attr_shotx.getValue(1) );
                    vectorY.push_back( attr_shoty.getValue(1) );
                    vectorZ.push_back( attr_shotz.getValue(1) );
                    vectorAngleX.push_back(myReflectorAngleX);
                    vectorAngleY.push_back(myReflectorAngleY);
                }

                if (!start) 
                {
                    vectorX.push_back( attr_shotx.getValue(1) );
                    vectorY.push_back( attr_shoty.getValue(1) );
                    vectorZ.push_back( attr_shotz.getValue(1) );
                    vectorAngleX.push_back(myReflectorAngleX);
                    vectorAngleY.push_back(myReflectorAngleY);
                    start = true;
                    receivers.clear();
                }

                receivers.push_back(Receiver(attr_revx.getValue(1), attr_revy.getValue(1), attr_revz.getValue(1), rcvIdx++));
                    
                for ( Int64 i=2; i<=last-first+1; ++i ) 
                {
                    g2 = (int)attr_grp.getValue( i );
                    if (g1 != g2) 
                    {   //another group start
                        vectorReceivers.push_back(receivers);
                        receivers.clear();
                        vectorX.push_back( attr_shotx.getValue(i) );
                        vectorY.push_back( attr_shoty.getValue(i) );
                        vectorZ.push_back( attr_shotz.getValue(i) );
                        vectorAngleX.push_back(myReflectorAngleX);
                        vectorAngleY.push_back(myReflectorAngleY);
                        g1 = g2;
                    }
                    receivers.push_back(Receiver(attr_revx.getValue(i), attr_revy.getValue(i), attr_revz.getValue(i), rcvIdx++));
                }
            }
            vectorReceivers.push_back(receivers);
        }
    }

    if (mySrcMode == 3 || mySrcMode ==4) 
    {
        if ( myNumberOfThreads == 1 ) 
        {
            myRayShoot(vectorX, vectorY, vectorZ, vectorReceivers);
        }
        else
        {
            // if using multithreads, use another method
            shootRaysThread(vectorX, vectorY, vectorZ, vectorReceivers);
            if (!mySummaryFile.isBlank())
            {
                if ( ! vlFixColumnWidths(mySummaryFile.c_str()) )
                {
                    fprintf(stderr, "ERROR: couldn't normalise the "
                        "columns of file %s.\n", (char*) mySummaryFile.c_str());
                }
            }
            return;
        }
    }
    else
    {
        int i_shot;
        for (i_shot = 0; i_shot < vectorX.size(); ++ i_shot) 
        {
            myX = vectorX[i_shot];
            myY = vectorY[i_shot];
            if (myDepthDipMode == 1) 
            {
                vgHorizonGroup* hg = g_app.getModel().getCurHorizonGroup();
                if(hg != 0) 
                {
                    int dipHrzIdx = hg->getHorizonIndexByName(myDipHrz.c_str());
                    vgHorizon *hrz = hg->getHorizonByIndex(dipHrzIdx);
                    if (hrz)
                    {
                        int id_z = hrz->getAttrIndex( ATTR_NAME_DEPTH );
                        double z;
                        if (!hrz->borehole(myX, myY, id_z, z))
                          continue;

                        myZ = (float) z;
                        hrz->getDipHorizon_dip(id_z, myX, myY, myReflectorAngleX, myReflectorAngleY);
                        myReflectorAngleX = -Rad2Deg(atan(myReflectorAngleX));
                        myReflectorAngleY = -Rad2Deg(atan(myReflectorAngleY));
                    }
                }
            }
            else if (vectorZ.size() == vectorX.size()) 
            {
              myZ = vectorZ[i_shot];
            }

            if (myNumberOfThreads == 1)
                myRayShoot(recMask);

            if (vectorZ.size() == vectorX.size()) 
            {
                vectorZ[i_shot] = myZ;
            }
            else 
            {
                vectorZ.push_back(myZ);
            }
            vectorAngleX.push_back(myReflectorAngleX);
            vectorAngleY.push_back(myReflectorAngleY);
        }
        if (myNumberOfThreads > 1 ) 
        {
            // if using multithreads, use another method
            shootRaysThread(vectorX, vectorY, vectorZ, vectorAngleX, vectorAngleY, vectorReceivers);
            if (!mySummaryFile.isBlank())
            {
                if ( ! vlFixColumnWidths(mySummaryFile.c_str()) )
                {
                    fprintf(stderr, "ERROR: couldn't normalise the "
                        "columns of file %s.\n", (char*) mySummaryFile.c_str());
                }
            }
            return;
        }
    }

    // normalise column for myFP
    if (!mySummaryFile.isBlank())
    {
        if ( ! vlFixColumnWidths(mySummaryFile.c_str()) )
        {
            fprintf(stderr, "ERROR: couldn't normalise the "
                "columns of file %s.\n", (char*) mySummaryFile.c_str());
        }
    }

    //convert result from myFP(local) to myOFP(remote)
    outputFp2Ofp(myFP, myOFP);

    if ( myFP && myFP->isOpened() )
        myFP->close();
    if ( myOFP && myOFP->isOpened() )
        myOFP->close();


    if (myRecvDepth != 0 && myOutPath.size() > 1)
    {
        calFreeDepth(finalSummaryFile, myOutPath);
    }
    if (recMask != 0) 
    {
        del2d(recMask);
        myNx_mask = myNy_mask = 0;
    }

    fprintf(stdout,"Finished Raytracing!!!\n");
}

/* Copy fp to ofp */
void vpRayTracing::outputFp2Ofp(FileAccess* fp, FileAccess* ofp)
{
    char buf[MAX_LINE_LENGTH];
    size_t buffLgth = MAX_LINE_LENGTH*sizeof(char);
    fp->rewind();

    while(!fp->is_eof()) {
        int nByteRead = fp->read(buf, buffLgth);
        if ( nByteRead<=0 ) {
            fprintf(stdout, "The file is empty or is unable to read local rays result!\n");
            return;
        }
        if (ofp->write(buf, nByteRead) != nByteRead ) {
            fprintf(stdout, "The file is empty or is unable to write remote rays result!\n");
            return;
        }
    }
}


bool vpRayTracing::myRayInit(Volume* vol_v, Volume* vol_d, Volume* vol_e, Volume* vol_t, Volume* vol_p)
{
    if (mySamRayTrace != 0)
    {
        if (vol_v==myVol_v && vol_d==myVol_d && vol_e==myVol_e && vol_t==myVol_t && vol_p==myVol_p)
            return true;

        mySamRayTrace->clear();
        delete mySamRayTrace;
        mySamRayTrace = 0;
    }

    if (vol_v == 0)
    {
        fprintf(stderr,"No velocity volume found!");
        return false;
    }

    fprintf( stderr, "Initializing Ray-Tracer...\n");

    int nx = vol_v->getSize()[0];
    int ny = vol_v->getSize()[1];
    int nz = vol_v->getSize()[2];
    float dx = vol_v->getStep()[0];
    float dy = vol_v->getStep()[1];
    float dz = vol_v->getStep()[2];
    float ox = vol_v->getOrigin()[0];
    float oy = vol_v->getOrigin()[1];
    float oz = vol_v->getOrigin()[2];
    g_app.getModel().getSamRayTracePlot()->setZ0(oz);

    float* data_v = 0;
    float* data_d = 0;
    float* data_e = 0;
    float* data_t = 0;
    float* data_p = 0;

    FdmCube* fdm_v = dynamic_cast<FdmCube*>(vol_v->getCube());
    if (fdm_v == 0)
    {
        fprintf(stderr,"Velocity data not found!");
        return false;
    }

    data_v = (float*)fdm_v->getData();

    if (vol_d != 0)
    {
        if (vol_d->getSize()[0]!=nx || vol_d->getSize()[1]!=ny || vol_d->getSize()[2]!=nz)
        {
            fprintf(stderr,"Delta geometry is inconsistent with velocity");
            return false;
        }

        FdmCube* fdm_d = dynamic_cast<FdmCube*>(vol_d->getCube());
        if (fdm_d == 0)
        {
            fprintf(stderr,"Delta data not found!");
            return false;
        }

        data_d = (float*)fdm_d->getData();
    }

    if (vol_e != 0)
    {
        if (vol_e->getSize()[0]!=nx || vol_e->getSize()[1]!=ny || vol_e->getSize()[2]!=nz)
        {
            fprintf(stderr,"Epsilon geometry is inconsistent with velocity");
            return false;
        }

        FdmCube* fdm_e = dynamic_cast<FdmCube*>(vol_e->getCube());
        if (fdm_e == 0)
        {
            fprintf(stderr,"Epsilon data not found!");
            return false;
        }

        data_e = (float*)fdm_e->getData();
    }

    if (vol_t != 0)
    {
        if (vol_t->getSize()[0]!=nx || vol_t->getSize()[1]!=ny || vol_t->getSize()[2]!=nz)
        {
            fprintf(stderr,"Theta geometry is inconsistent with velocity");
            return false;
        }

        FdmCube* fdm_t = dynamic_cast<FdmCube*>(vol_t->getCube());
        if (fdm_t == 0)
        {
            fprintf(stderr,"Theta data not found!");
            return false;
        }

        data_t = (float*)fdm_t->getData();
    }

    if (vol_p != 0)
    {
        if (vol_p->getSize()[0]!=nx || vol_p->getSize()[1]!=ny || vol_p->getSize()[2]!=nz)
        {
            fprintf(stderr,"Phi geometry is inconsistent with velocity");
            return false;
        }

        FdmCube* fdm_p = dynamic_cast<FdmCube*>(vol_p->getCube());
        if (fdm_p == 0)
        {
            fprintf(stderr,"Phi data not found!");
            return false;
        }

        data_p = (float*)fdm_p->getData();
    }

    mySamRayTrace = new vmSamRayTrace(nx, ny, nz, dx, dy, dz, ox, oy, oz);

    myVol_v = vol_v;

    g_app.getModel().getSamRayTracePlot()->setModel(vol_v);

    if (vol_d==0 || vol_e==0)
    {
        myVol_d = 0;
        myVol_e = 0;
        myVol_t = 0;
        myVol_p = 0;

        mySamRayTrace->setModel(data_v);
    }
    else
    {
        myVol_d = vol_d;
        myVol_e = vol_e;

        if (vol_t==0 || vol_p==0)
        {
            myVol_t = 0;
            myVol_p = 0;

            mySamRayTrace->setModel(data_v, data_d, data_e);
        }
        else
        {
            myVol_t = vol_t;
            myVol_p = vol_p;

            mySamRayTrace->setModel(data_v, data_d, data_e, data_t, data_p);
        }
    }

    return true;
}

void vpRayTracing::myRayShoot(bool** recMask)
{

    if (recMask != 0 && myNx_mask > 0 && myNy_mask > 0) 
    {
        for (int i = 0; i < myNx_mask; ++ i) 
        {
            for (int j = 0; j < myNy_mask; ++ j) 
            {
                recMask[i][j] = false;
            }
        }
    }

    int count = 0;
    float angle_x_min_ok = 0.0, angle_x_max_ok = 0.0, angle_y_min_ok = 0.0, angle_y_max_ok = 0.0;

    if (myReflectHrz != 0)
    {
        g_app.getModel().getSamRayTracePlot()->setReflect(myReflectHrz, myRecordHrz, myReflectorOffsetMin, myReflectorOffsetMax, myReflectorOffsetAttrId, mySourceOffsetMin, mySourceOffsetMax, mySourceOffsetAttrId, myReceiverOffsetMin, myReceiverOffsetMax, myReceiverOffsetAttrId);
    }

    if (myWaterBottom != 0)
    {
        g_app.getModel().getSamRayTracePlot()->setWatBotHor(myWaterBottom);
    }

    long pos_in = 0, pos_out = 0;
    if ( myFP->isOpened() )
       pos_in = myFP->ftell();
    if (myOFP->isOpened() )
        pos_out = myOFP->ftell();

    mySamRayTrace->setValidLoc(true);

    for (int ix=0; ix<myNumAX; ++ix)
    {
        float angle_x = 0.5*(myMaxAX+myMinAX);
        if (myNumAX > 1)
            angle_x = myMinAX + (myMaxAX-myMinAX)*(float)ix/(float)(myNumAX-1);

        angle_x = Deg2Rad(angle_x);

        for (int iy=0; iy<myNumAY; ++iy)
        {
            float angle_y = 0.5*(myMaxAY+myMinAY);
            if (myNumAY > 1)
                angle_y = myMinAY + (myMaxAY-myMinAY)*(float)iy/(float)(myNumAY-1);

            angle_y = Deg2Rad(angle_y);

            for (int iaz=0; iaz<myNumAAz; ++iaz)
            {
                float azimuth = 0.5*(myMinAAz+myMaxAAz);
                if (myNumAAz > 1)
                    azimuth = myMinAAz + (myMaxAAz-myMinAAz)*(float)iaz/(float)(myNumAAz-1);

                azimuth = Deg2Rad(azimuth);
                /* Rotate to defined azimuth on Reflector */
                float rot_angle_x = atan(cos(azimuth)*tan(angle_x) - sin(azimuth)*tan(angle_y));
                float rot_angle_y = atan(sin(azimuth)*tan(angle_x) + cos(azimuth)*tan(angle_y));
                //Incident wavefront angles
                float dipx = tan(rot_angle_x);
                float dipy = tan(rot_angle_y);

                /* Add on reflector to angle */
                rot_angle_x += Deg2Rad(myReflectorAngleX);
                rot_angle_y += Deg2Rad(myReflectorAngleY);

                float dip_x = tan(rot_angle_x);
                float dip_y = tan(rot_angle_y);
                if (g_app.getModel().getSamRayTracePlot()->trace(mySamRayTrace, myX, myY, myZ, dip_x, dip_y, myUp, myAcceptRays, myPlot, myFP, myPlotAll, recMask, myNx_mask, myNy_mask))
                {
                    if (count == 0)
                    {
                        angle_x_min_ok = angle_x;
                        angle_x_max_ok = angle_x;
                        angle_y_min_ok = angle_y;
                        angle_y_max_ok = angle_y;
                    }
                    else
                    {
                        if (angle_x < angle_x_min_ok)
                            angle_x_min_ok = angle_x;
                        if (angle_x > angle_x_max_ok)
                            angle_x_max_ok = angle_x;
                        if (angle_y < angle_y_min_ok)
                            angle_y_min_ok = angle_y;
                        if (angle_y > angle_y_max_ok)
                            angle_y_max_ok = angle_y;
                    }
                    ++count;
                }
            }
        }
    }
 
    // g_app.getModel().getSamRayTracePlot()->findAndWritePairs(myFP, pos_in, pairs, myOFP, pos_out);

    if (myReflectHrz != 0)
    {
        g_app.getModel().getSamRayTracePlot()->clearReflect();
    }

    g_app.getModel().getSamRayTracePlot()->refresh();

    fprintf( stderr, "  Source x=%g y=%g z=%g  Rays shot:%d ok:%d\n", myX, myY, myZ, myNumAX*myNumAY, count);
    
    if(!mySamRayTrace->isSrcValidLoc())
    {
        fprintf( stderr, "    Warning!! source invalid location!(outside of geometry)\n");
    }

    if (count > 0)
    {
      fprintf( stderr, "   angles: x_min:%g x_max:%g y_min:%g y_max:%g\n", Rad2Deg(angle_x_min_ok), Rad2Deg(angle_x_max_ok), Rad2Deg(angle_y_min_ok), Rad2Deg(angle_y_max_ok));
    }

}

void vpRayTracing::shootRaysThread(const vector<float>& vectorX, const vector<float>& vectorY, const vector<float>& vectorZ,
                                   const vector< vector<Receiver> >& vectorReceivers)
{
    vector<float> vectorAngleX, vectorAngleY;
    shootRaysThread(vectorX, vectorY, vectorZ, vectorAngleX, vectorAngleY, vectorReceivers);
}

void vpRayTracing::shootRaysThread(const vector<float>& vectorX, const vector<float>& vectorY, const vector<float>& vectorZ, const vector<float>& vectorAngleX, const vector<float>& vectorAngleY, const vector< vector<Receiver> >& vectorReceivers)
{
    if (myReflectHrz != 0)
    {
        g_app.getModel().getSamRayTracePlot()->setReflect(myReflectHrz, myRecordHrz, myReflectorOffsetMin, myReflectorOffsetMax, myReflectorOffsetAttrId, mySourceOffsetMin, mySourceOffsetMax, mySourceOffsetAttrId, myReceiverOffsetMin, myReceiverOffsetMax, myReceiverOffsetAttrId);
    }

    SamRayTraceQueue taskQueue;
    SamRayTraceTask task;

    int numworker = myNumberOfThreads;
    vector<SamRayTraceThread> workers;
    workers.resize( numworker);
    int u;

    myThreadingStatus.reset();

    for( u = 0; u<numworker; ++u )
    {
        workers[u].init( &taskQueue, u+1, &myThreadingStatus);
        workers[u].setShootingAngles( myMinAX, myMaxAX, myNumAX,
                                      myMinAY, myMaxAY, myNumAY,
                                      myMinAAz, myMaxAAz, myNumAAz);
        workers[u].setUpFlag( myUp );
        workers[u].setAcceptFlag( myAcceptRays );
        workers[u].setReflectHrz( myReflectHrz );
        workers[u].setSamRayTrace( mySamRayTrace );
        workers[u].setSamRayTracePlot( g_app.getModel().getSamRayTracePlot() );
        workers[u].setPlotFlags( myPlot, myPlotAll );
        workers[u].setFilePointer( myOFP );
        workers[u].setRecMaskDim( myNx_mask, myNy_mask );
    }

    vector<vThread*> workerThreads;
    workerThreads.resize( numworker );
    for( u=0; u<numworker; ++u ) 
    {
        workerThreads[u] = new vThread( workers[u] );
    }

    bool fromSourceReceiverFile = (vectorAngleX.size() == 0);

    for (int i_shot = 0; i_shot < vectorX.size(); ++ i_shot) 
    {
        task.myX = vectorX[i_shot];
        task.myY = vectorY[i_shot];
        task.myZ = vectorZ[i_shot];
        task.myUp = myUp;
        if (fromSourceReceiverFile) 
        {
            task.myReflectorAngleX = myReflectorAngleX;
            task.myReflectorAngleY = myReflectorAngleY;
            task.myReceivers = vectorReceivers[i_shot];
        }
        else 
        {
            task.myReflectorAngleX = vectorAngleX[i_shot];
            task.myReflectorAngleY = vectorAngleY[i_shot];
        }

        taskQueue.putTask( task );

    }
    for( u=0; u<numworker; ++u ) 
    {
      workerThreads[u]->start();
    }

    for( u=0; u<numworker; ++u ) 
    {
        workerThreads[u]->join();
    }

    for( u=0; u<numworker; ++u ) 
    {
        delete workerThreads[u];
        workerThreads[u] = NULL;
    }

    workers.clear();
    workerThreads.clear();

    if (myFP && myFP->isOpened())
    {
        myFP->close();
    }

    if (myOFP && myOFP->isOpened())
    {
        myOFP->close();
    }

    if (myReflectHrz != 0)
    {
        g_app.getModel().getSamRayTracePlot()->clearReflect();
    }
    else if(!vectorReceivers.empty())
    {
        g_app.getModel().getSamRayTracePlot()->filterRays( -1 );
    }

    g_app.getModel().getSamRayTracePlot()->refresh();
    bool refresh_view = false;
    if (myReflectHrz != 0 && myRecordHrz != 0)
    {
        for (int i_ref=0; i_ref<myReflectorOffsetAttrId.size(); ++i_ref)
        {
            myRecordHrz->updateAttrMinMax( myReflectorOffsetAttrId[i_ref] );
            double mn, mx;
            g_app.getModel().getCurHorizonGroup()->getAttrRange( myReflectorOffsetAttrId[i_ref], mn, mx );
        }
        for (int i_source=0; i_source<mySourceOffsetAttrId.size(); ++i_source)
        {
            myRecordHrz->updateAttrMinMax( mySourceOffsetAttrId[i_source] );
            double mn, mx;
            g_app.getModel().getCurHorizonGroup()->getAttrRange( mySourceOffsetAttrId[i_source], mn, mx );
        }
        for (int i_rec=0; i_rec<myReceiverOffsetAttrId.size(); ++i_rec)
        {
            myRecordHrz->updateAttrMinMax( myReceiverOffsetAttrId[i_rec] );
            double mn, mx;
            g_app.getModel().getCurHorizonGroup()->getAttrRange( myReceiverOffsetAttrId[i_rec], mn, mx );
        }
    }
}

bool vpRayTracing::myGetShots(Volume* vol_v, float x0, float y0, float shotSpacing, float lineSpacing, float lineAngle, vector<float>& vectorX, vector<float>& vectorY)
{
    vectorX.clear();
    vectorY.clear();

    float vol_x0 = vol_v->getLower()[0];
    float vol_x1 = vol_v->getUpper()[0];
    float vol_y0 = vol_v->getLower()[1];
    float vol_y1 = vol_v->getUpper()[1];

    const float Rad90 = Deg2Rad(90);
    float dx_line = sin(lineAngle)*lineSpacing;
    float dy_line = cos(lineAngle)*lineSpacing;

    float dx_shot = cos(lineAngle)*shotSpacing;
    float dy_shot = sin(lineAngle)*shotSpacing;
    if (lineAngle > Rad90) 
    {
    lineAngle = Rad90*2 - lineAngle;

    dx_line = cos(lineAngle)*shotSpacing;
    dy_line = sin(lineAngle)*shotSpacing;

    dx_shot = sin(lineAngle)*lineSpacing;
    dy_shot = cos(lineAngle)*lineSpacing;
    }

    float x = x0, y = y0;

    float x_int = 0.0, y_int = 0.0;
    Vect2f bound0, bound1;
    bool getBound0, getBound1;
    bool isUp = true;
    while (1) 
    { //line start point moving upwards
        getBound0 = true;
        if (fabs(dy_shot) < 1.0e-3) 
        {
            x_int = vol_x0;
            if (y < vol_y0 || y > vol_y1) 
                getBound0 = false;
            else 
                y_int = y;
        }
        else if (fabs(dx_shot) < 1.0e-3) 
        {

            if (x < vol_x0 || x > vol_x1) 
                getBound0 = false;
            else 
                x_int = x;

            y_int = vol_y0;
        }
        else 
        {
            x_int = ( (vol_y0-y)*(x+dx_shot) + (y+dy_shot-vol_y0)*x ) / dy_shot;

            if (x_int < vol_x0 || x_int > vol_x1) 
            {
                y_int = ( (vol_x0-x)*(y+dy_shot) + (x+dx_shot-vol_x0)*y ) / dx_shot;

                if (y_int < vol_y0 || y_int > vol_y1) 
                    getBound0 = false;
                else 
                    x_int = vol_x0;
            }
            else 
            {
                y_int = vol_y0;
            }
        }
        bound0 = Vect2f(x_int, y_int);
        getBound1 = true;
        if (fabs(dy_shot) < 1.0e-3) 
        {
            x_int = vol_x1;
            if (y < vol_y0 || y > vol_y1) 
                getBound1 = false;
            else 
                y_int = y;
        }
        else if (fabs(dx_shot) < 1.0e-3) 
        {

            if (x < vol_x0 || x > vol_x1) 
                getBound1 = false;
            else 
                x_int = x;

            y_int = vol_y1;
        }
        else 
        {
            x_int = ( (vol_y1-y)*(x+dx_shot) + (y+dy_shot-vol_y1)*x ) / dy_shot;

            if (x_int < vol_x0 || x_int > vol_x1) 
            {
                y_int = ( (vol_x1-x)*(y+dy_shot) + (x+dx_shot-vol_x1)*y ) / dx_shot;

                if (y_int < vol_y0 || y_int > vol_y1) 
                    getBound1 = false;
                else 
                    x_int = vol_x1;
            }
            else 
            {
                y_int = vol_y1;
            }
        }
        bound1 = Vect2f(x_int, y_int);

        if (!getBound0 || !getBound1) 
        {
            if (isUp) 
                isUp = false;
            else 
                break;
        }
        else 
        {
            if (x >= vol_x0 && x <= vol_x1 && y >= vol_y0 && y <= vol_y1) 
            {
                vectorX.push_back(x);
                vectorY.push_back(y);
            }

            float xx = x-dx_shot, yy = y-dy_shot;
            while (xx >= bound0[0] && yy >= bound0[1]) 
            {
                if (xx <= vol_x1 && yy <= vol_y1) 
                {
                    vectorX.push_back(xx);
                    vectorY.push_back(yy);
                }

                xx -= dx_shot;
                yy -= dy_shot;
            }

            xx = x+dx_shot;
            yy = y+dy_shot;
            while (xx <= bound1[0] && yy <= bound1[1]) 
            {
                if (xx >= vol_x0 && yy >= vol_y0) 
                {
                    vectorX.push_back(xx);
                    vectorY.push_back(yy);
                }

                xx += dx_shot;
                yy += dy_shot;
            }
        }

        if (isUp) 
        {
            x -= dx_line;
            y += dy_line;
        }
        else 
        {
            x += dx_line;
            y -= dy_line;
        }
    }

    return true;
 
}

void vpRayTracing::myRayShoot(const vector<float>& vectorX, const vector<float>& vectorY, const vector<float>& vectorZ,
                              const vector< vector<Receiver> >& vectorReceivers)
{
    vmSamRayTracePlot* rayPlot = g_app.getModel().getSamRayTracePlot();
    rayPlot->setReflect(myReflectHrz, myRecordHrz, myReflectorOffsetMin, 
                        myReflectorOffsetMax, myReflectorOffsetAttrId,
                        mySourceOffsetMin, mySourceOffsetMax, mySourceOffsetAttrId, 
                        myReceiverOffsetMin, myReceiverOffsetMax, myReceiverOffsetAttrId);

    if(myWaterBottom!=0)
        rayPlot->setWatBotHor(myWaterBottom);

    long pos_in = 0, pos_out = 0;
    if ( myFP->isOpened() )
        pos_in = myFP->ftell();

    if ( myOFP->isOpened() )
        pos_out = myOFP->ftell();

    fprintf(stdout,"Total shot:%d",vectorX.size()); 
    for(int ishot = 0; ishot < vectorX.size(); ++ ishot) 
    {
        fprintf(stdout, "  shot no.%d, x=%f y=%f z=%f, num_rcv= %d \n", ishot+1, vectorX[ishot], vectorY[ishot], vectorZ[ishot], vectorReceivers[ishot].size());
        if (vectorReceivers[ishot].size() >0)
        {
            fprintf(stdout, "  first rcv(x=%f,y=%f,z=%f) \n", vectorReceivers[ishot][0].x, vectorReceivers[ishot][0].y, vectorReceivers[ishot][0].z);
        }
        fflush(stdout);
        mySamRayTrace->setValidLoc(true);
        for (int ix=0; ix<myNumAX; ++ix) 
        {
            float angle_x = 0.5*(myMaxAX+myMinAX);
            if (myNumAX > 1)
            {
                angle_x = myMinAX + (myMaxAX-myMinAX)*(float)ix/(float)(myNumAX-1);
            }

            angle_x = Deg2Rad(angle_x);

            for (int iy=0; iy<myNumAY; ++iy) 
            {

                float angle_y = 0.5*(myMaxAY+myMinAY);
                if (myNumAY > 1)
                    angle_y = myMinAY + (myMaxAY-myMinAY)*(float)iy/(float)(myNumAY-1);

                angle_y = Deg2Rad(angle_y);

                for (int iaz=0; iaz<myNumAAz; ++iaz) 
                {

                    float azimuth = 0.5*(myMinAAz+myMaxAAz);
                    if (myNumAAz > 1)
                        azimuth = myMinAAz + (myMaxAAz-myMinAAz)*(float)iaz/(float)(myNumAAz-1);

                    azimuth = Deg2Rad(azimuth);
                    /* Rotate to defined azimuth on Reflector */
                    float rot_angle_x = atan(cos(azimuth)*tan(angle_x) - sin(azimuth)*tan(angle_y));
                    float rot_angle_y = atan(sin(azimuth)*tan(angle_x) + cos(azimuth)*tan(angle_y));
                    //Incident wavefront angles
                    float dipx = tan(rot_angle_x);
                    float dipy = tan(rot_angle_y);

                    rot_angle_x += Deg2Rad(myReflectorAngleX);
                    rot_angle_y += Deg2Rad(myReflectorAngleY);        

                    float dip_x = tan(rot_angle_x);
                    float dip_y = tan(rot_angle_y);

                    rayPlot->trace(mySamRayTrace, vectorX[ishot], vectorY[ishot], vectorZ[ishot], 
                                   dip_x, dip_y, myUp, vectorReceivers[ishot], myAcceptRays, 
                                   myPlot, myFP, myPlotAll);
                }
            }
        }

        if(!mySamRayTrace->isSrcValidLoc())
        {
            fprintf( stderr, "    Warning!! source(x: %f y: %f z: %f ) invalid location!(outside of geometry)\n",  vectorX[ishot], vectorY[ishot], vectorZ[ishot]);
        }
    }

    if ( myReflectHrz != 0 ) 
        g_app.getModel().getSamRayTracePlot()->clearReflect();
   else if(!vectorReceivers.empty()) 
        rayPlot->filterRays( -1 );

    g_app.getModel().getSamRayTracePlot()->refresh();
}


void vpRayTracing::onParse()
{
    vmSamRayTracePlot* rayPlot = g_app.getModel().getSamRayTracePlot();
    if (rayPlot == 0) 
    {
        fprintf(stderr,"Invalid ray plot!");
        return;
    }
    
    if( !rayPlot->parse(myRoseGeom_xl, myRoseGeom_sl, myFoldGeom_xl, myFoldGeom_sl, myRoseOutPath, myFoldOutPath) )
    {
        fprintf(stderr,"Parse Error!");
        return;
    }
}

void vpRayTracing::calFreeDepth(const vString& fnam, const vString& ref)
{

  printf("Free surface is not implemented at this time!\n");
/*
    FILE* fpw = fopen(tmpFile.c_str(), "w");
    FILE* fpr = fopen(ref.c_str(),"r");
   
    vString nm,vs,ls; 
    float maxx, maxy, maxz, reft, srcx, recy, srcz, srct, recx, recy, recz, rect;
    char buf[128];
    char *pch;
    while (fgets(buf, sizeof(buf), fpr))
    {
      if (strcmp(buf, "RAY") == 0) 
      {
        pch = strtok(buf, " ");
        float x = atof(pch);
        pch = strtok(NULL, " ");
        float y = atof(pch);
        pch = strtok(NULL, " ");
        float z = atof(pch);
        pch = strtok(NULL, " ");
        float t = atof(pch);
    
      }

      fprintf(fpw,"%s\n",nm.c_str());
    }
    remove(fnam.c_str());
    rename(tmpFile.c_str(), fnam.c_str());
*/
}

void vpRayTracing::onTest()
{
    //TEST

}

