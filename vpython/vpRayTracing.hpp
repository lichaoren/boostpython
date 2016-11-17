#include <stdio.h>
#include <stdlib.h>
#include "vmApplication.hpp"
#include "vmModel.hpp"
#include "vlGenericPath.hpp"
#include "vlFdmDiskCube.hpp"
#include "utilB.hpp"
#include "vThread.hpp"
#include "vlUIUtils.hpp"
#include "vmSamRayTracePlot.hpp"
#include "vmSamRayTrace.hpp"
#include "vlFixColumnWidths.hpp"
#include "vmAttrRayThread.hpp"
#include "vlSamRayTraceQueue.hpp"
#include "vlSamRayTraceThread.hpp"
#include "vlThreadingStatus.hpp"
#include "vlCapiAttribNames.hpp"

class vpRayTracing {

public:
  vpRayTracing();
  ~vpRayTracing();

public:
  void    onShootRays();
  void    onParse();
  void    onPrint();
  void    onSave();
  void    onTest();

  /** 
   * @brief Specifing ray-tracing Method.
   *
   * @param mode Mode, ISO 0, VTI 1, TTI2  
   */
  void setMethod(int mode);

  /** 
  * @brief Setup velocities path.
  *
  * @param vel Velocity Path.  
  * @param delta Delta Path.
  * @param epilson Epislon Path.
  * @param theta Theta Path.
  * @param phi Phi Path. 
  */
  void setVelocityPath(vString vel, vString delta, vString epilson, vString theta, vString phi);


  /** 
  * @brief Setup reflected horzion.
  *
  * @param hrzPath Horizon path.
  * @param hrzName Horizon name.
  */
  void setRefHorizon(vString hrzPath, vString hrzName);

  /** 
  * @brief Setup water bottom horzion.
  *
  * @param waterBottomHrzPath WaterBottom path.
  * @param hrzName Horizon name.
  */
  void setWaterBottom(vString waterBottomHrzPath, vString hrzName);

  /** 
  * @brief Setup ray angle(Incident wavefront angle) in degree.
  *
  * @param minX Minimum angle for X.  
  * @param maxX Maximum angle for X.
  * @param numX Number of rays in X direction.
  * @param minY Minimum angle for Y.  
  * @param maxY Maximum angle for Y.
  * @param numY Number of rays in Y direction.
  * @param minAzimuth Minimum angle for Azimuth.  
  * @param maxAzimuth Maximum angle for AzimuthY.
  * @param numAzimuth Number of rays in Azimuth direction.
  * @param MaxAngleNum Maxmimum number of rays.
  */
  void setRayAngle(float minX, float maxX, int numX, float minY, float maxY, int numY, float minAzimuth, float maxAzimuth, int numAzimuth, int MaxAngleNum);


  /** 
  * @brief To accept all rays.
  */
  void setAcceptAllRays();


  /** 
  * @brief To accept only the completed rays.
  */
  void setAcceptCompleteRays();


  /** 
  * @brief To accept the ray recored on surface line.
  *
  * @param depth Surface's depth.
  * @param receiverMode Mode for receiver. 0 = lines, 1 = region
  * @param firstX X Line start line number.
  * @param lastX X Line end line number.
  * @param IncX X direction increment.
  * @param firstY Y Line start line number.
  * @param lastY Y Line end line number.
  * @param IncY Y direction increment. 
  */
  void setAcceptRaysOnSurfaceLine(int depth, float radius,  int receiverMode, float firstX, float lastX, float firstY, float lastY, float IncX, float IncY);


  /** 
  * @brief Setup shooting up.
  */
  void setShotUp();


  /** 
  * @brief Setup shooting up.
  */
  void setShotDown();

  /** 
  * @brief Setup depth and dip as supplied(Dip of up-going rays in degree).
  *
  * @param dipx Dip in X.
  * @param dipy Dip in Y.
  */
  void setDepthDipAsSupplied(int dipx, int dipy);


  /** 
  * @brief Setup depth and dip using horizon, need to load horizon via shotsel::setRefHorizon().
  *
  * @param dipHrz horizon name.
  */
  void setDepthDipAsHorizon(vString dipHrz);


  /** 
  * @brief Setup sources.
  *
  * @param filePath Multi-sources file path.
  */
  void setMultiSourceFile(vString filePath);


  /** 
  * @brief Specify sources using lines parameters.
  *
  * @param posX Source first X position.
  * @param posY Source first Y position.
  * @param posZ Source first Z position.
  * @param incX X Increment along line.
  * @param incY Y Increment along line.
  * @param numOfShot Shot Number along first X/Y direction.
  * @param SrcLineIncX X Increment between line.
  * @param SrcLineIncY Y Increment between line.
  * @param SrcLineNum Line Number along first X/Y direction.
  */
  void setMultiSourceInLine(float posX, float posY, float posZ, float incX, float incY, int numOfShot, float SrcLineIncX, float SrcLineIncY, int SrcLineNum);


  /** 
  * @brief Specify sources using directional lines parameters.
  *
  * @param posX Source first X position.
  * @param posY Source first Y position.
  * @param posZ Source first Z position.
  * @param dirLineSpace Line spacing.
  * @param dirShotSpace Shot spacing.
  * @param dirLineAngle Line angle in degree.
  */
  void setMultiSourceInDirLine(float posX, float posY, float posZ, float dirLineSpace, float dirShotSpace, float dirLineAngle);


  /** 
  * @brief Specify sources and receivers using seismic file(dataset).
  *
  * @param filePath Seismic file path(Dataset).
  * @param radius Receivers radius.
  * @param grpName Group name attribute specified in dataset.
  * @param shotX ShotX name attribute specified in dataset.
  * @param shotY ShotY name attribute specified in dataset.
  * @param shotZ ShotZ name attribute specified in dataset.
  * @param recX  RecX name attribute specified in dataset.
  * @param recY  RecY name attribute specified in dataset.
  * @param recZ  RecZ name attribute specified in dataset.
  */
  void setMultiSourceSeisFile(vString filePath, int radius, vString grpName, 
                              vString shotX, vString shotY, vString shotZ, 
                              vString recX, vString recY, vString recZ);


  /** 
  * @brief Specify sources and receivers using flat text file.
  *
  * @param filePath Text file path.
  * @param radius Receivers radius..
  */
  void setMultiSourceSrcRecFile(vString filePath, int radius);


  /** 
  * @brief Specify rose diagram parsing parameters.
  *
  * @param filePath Output file path.
  * @param geom_xl Cross line geometry.
  * @param geom_sl In-line geometry.
  */
  void setRoseDiagram(vString filePath, float geom_xl, float geom_sl);


  /** 
  * @brief Specify shot fold parsing parameters.
  *
  * @param filePath Output file path.
  * @param geom_xl Cross line geometry.
  * @param geom_sl In-line geometry.
  */       
  void setShotFold(vString filePath, float geom_xl, float geom_sl);


  /** 
  * @brief Specify rays output file path.
  *
  * @param filePath Output file path.
  */ 
  void setRayPath(vString filePath);


  /** 
  * @brief Setup summary output info.
  *
  * @param filePath Output file path.
  * @param angleInfo Boolean factor to specify whether to output reflector incident/azimuth angle.
  * @param saltTime Boolean factor to specify whether to output time info of salt and water bottom.
  * @param InitialAngle Boolean factor to specify whether to output initial incident azimuth angle.
  */ 
  void setSummary(vString filePath, bool angleInfo, bool saltTime, bool InitialAngle);


  /** 
  * @brief Specify thread number.
  *
  * @param threadNum Thread number.
  */ 
  void setThreadNum(int threadNum);


  int     myMethod; // ISO 0, VTI 1, TTI2
  int     myMaxRayAngle;
  vString myVelPath;
  vString myDelPath;
  vString myEpiPath;
  vString myThePath;
  vString myPhiPath;
  float   myFirstx;
  float   myFirsty;
  float   myLastx;
  float   myLasty;
  float   myIncx;
  float   myIncy;
  float   myRadius;
  bool    myAngleInfo;
  bool    myExcludeSalt;
  bool    myInitialVector;
  int     myRecvDepth;
  float   myShotXInc;
  float   myShotYInc;
  int     myShotNum;
  float   myLineXInc;
  float   myLineYInc;
  int     myLineNum;
  int     mySrcMode;
  vString mySrcFile;
  vString mySummaryFile;
  vString myHrzPath;
  vString myRefHrz;
  vString myWBPath;
  vString myWBHrz;
  vString myOutPath;
  vString myRoseOutPath;
  vString myFoldOutPath;
  float   myRoseGeom_xl;
  float   myRoseGeom_sl;
  float   myFoldGeom_xl;
  float   myFoldGeom_sl;
  
  int     myNumberOfThreads;
  ThreadingStatus myThreadingStatus;
  bool    myUp;
  int     myDepthDipMode;
  int     myDipInX;
  int     myDipInY;
  vString myDipHrz;
  float   myShotSpacing;
  float   myLineSpacing;
  float   myLineAngle;
 
  int     myAcceptRays;
  int     mySurface;
  float   myMinAX;
  float   myMaxAX;
  int     myNumAX;
  float   myMinAY;
  float   myMaxAY;
  int     myNumAY;

  float   myMinAAz;
  float   myMaxAAz;
  int     myNumAAz;
  float   myX;
  float   myY;
  float   myZ;
  vString mySeisGrp;
  vString mySeisSx; 
  vString mySeisSy; 
  vString mySeisSz; 
  vString mySeisRx; 
  vString mySeisRy; 
  vString mySeisRz; 


private:

  void    calFreeDepth(const vString& fnam, const vString& ref);
  bool    myRayInit(Volume* vol_v, Volume* vol_d, Volume* vol_e, Volume* vol_t, Volume* vol_p);
  void    myRayShoot(bool** recMask = 0);
  void    myRayShoot(const vector<float>& vectorX, const vector<float>& vectorY, const vector<float>& vectorZ,
                     const vector< vector<Receiver> >& vectorReceivers);
  void    shootRaysThread(const vector<float>& vectorX, const vector<float>& vectorY, const vector<float>& vectorZ,
                                      const vector< vector<Receiver> >& vectorReceivers);
  void    shootRaysThread(const vector<float>& vectorX, const vector<float>& vectorY, const vector<float>& vectorZ, const vector<float>& vectorAngleX, const vector<float>& vectorAngleY, const vector< vector<Receiver> >& vectorReceivers);
  bool    myGetShots(Volume* vol_v, float x0, float y0, float shotSpacing, float lineSpacing, float lineAngle, vector<float>& vectorX, vector<float>& vectorY);
  void    outputFp2Ofp(FileAccess* fp, FileAccess* ofp);
 
  float   myReflectorAngleX;
  float   myReflectorAngleY;
  float   myMaxVerticalAngle;

  vmSamRayTrace* mySamRayTrace;
  Volume* myVol_v;
  Volume* myVol_d;
  Volume* myVol_e;
  Volume* myVol_t;
  Volume* myVol_p;

  int     myCMapId;

  vgHorizon* myReflectHrz;
  vgHorizon* myRecordHrz;
  vgHorizon* myWaterBottom;
  vector<float> myReflectorOffsetMin;
  vector<float> myReflectorOffsetMax;
  vector<int>   myReflectorOffsetAttrId;
  vector<float> mySourceOffsetMin;
  vector<float> mySourceOffsetMax;
  vector<int>   mySourceOffsetAttrId;
  vector<float> myReceiverOffsetMin;
  vector<float> myReceiverOffsetMax;
  vector<int>   myReceiverOffsetAttrId;

  // following variables to store multiple sources of shot/rec from file
  vString myShotRecFileName;
  vector< vector<Receiver> > myVectorReceivers;
  vector<float> myVectorX;
  vector<float> myVectorY;
  vector<float> myVectorZ;

  bool       myPlot;
  bool       myPlotAll; // options to include unsucess ray in summary

  FileAccess*      myFP; //Temporary summary file without markers
  FileAccess*      myOFP;

  int myNx_mask, myNy_mask;
};

