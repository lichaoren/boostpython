from base import *
from vision import *
import time
import glob
import os


## Snapshot
param = CaptureParameters()

fileParameters = CaptureFileParameters()
##  Set file parameters
fileParameters.setPrefix('xxx')
fileParameters.setPath('/tmp/IMAGES')
fileParameters.setFormat("png")
fileParameters.setNaming(['Prefix','SeismicName','AttrName','GridNum'])
param.setFileParameters(fileParameters)

dataParam = CaptureDataParameters()
param.setDataParameters(dataParam)


## Data
raw_hrz_folder ="/data2/d3344exx/60_VMB/98_horizons_from_Exxon/09_06092016_Faulthorizons/01_raw_hrz/"

rawfiles = glob.glob( raw_hrz_folder + '0_mjw_1*')

## Basic loop to load data 
for fraw in rawfiles:
   print( fraw)
   hgraw=HorizonGroup()
   hgraw.load(fraw)
   vision.addHorizonGroup(hgraw)
   vision.setCurrentHorizonGroup(fraw)
   # Same name as the horizon
   base=os.path.basename(fraw)
   print (os.path.splitext(base)[0])
   fileParameters.setPrefix( os.path.splitext(base)[0])
   param.setFileParameters(fileParameters)
   captureImage(Window.MAIN_VIEW).capture(param)
   vision.removeHorizonGroup(fraw)

   









