from base import *
from vision import *
import time
import glob
import os
import glob


## Snapshot
param = CaptureParameters()

fileParameters = CaptureFileParameters()
##  Set file parameters
fileParameters.setPrefix('iter1')
fileParameters.setPath('/tmp/IMAGES')
fileParameters.setFormat("png")
#fileParameters.setNaming(['Prefix','BookmarkName','WindowName','SeismicName','AttrName','GatherName','WellName','GridNum'])

param.setFileParameters(fileParameters)

bkmDisplayParameters = BookmarkDisplay()
bkmDisplayParameters.load('/tmp/horGroupBkm.xml')
bkmDisplayParameters.setColormap( DataType.SEISMIC,ColorMapMode.CURRENT_COLORS)
bkmDisplayParameters.selectBookmark(['bk2'])

bkmDisplayParameters.setXML('bk1','ATTRIBUTE_COLORMAP.SPECTRUM.RANGE','-0.500000 0.900000')
bkmDisplayParameters.setXML('bk1','ATTRIBUTE_COLORMAP.SPECTRUM.COLORMAP_INDEX','1')
bkmDisplayParameters.setXML('bk1','ATTRIBUTE_COLORMAP.SPECTRUM.LABEL','LouLou')
bkmDisplayParameters.setXML('bk1','ATTRIBUTE_COLORMAP.SPECTRUM.TIMES','2')

param.setDisplayParameters(bkmDisplayParameters)


# Location from IL,XL
volLocation=VolumeLocation()
volLocation.setIL(1601)
volLocation.setXL(1900, 2000, 2000)
param.setLocations(volLocation)


fdm_folder="/data2/devtest/tornado/welltest/data/"
fdm_list = glob.glob( fdm_folder + '*.fdm')
pos=0
for fdm_file in fdm_list :
    print( fdm_file)
    ## Add the data as a bookmark (seismic,attibute)
    dataParam = CaptureDataParameters()
    attribute1= Volume()
    attribute1.load(fdm_file)
    dataParam.addAttribute(attribute1,pos)
    param.setDataParameters(dataParam)
    # For each bookmarks capture the MAIN WINDOW
    captureImage(Window.MAIN_VIEW).capture(param)

param=0
dataParam=0

   













