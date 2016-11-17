from algorithm import HorizonMisite
from algorithm import InterpMode

from base import *
from vision import *
from vision import PointAnnotation
import time

# Change the flag if you want to run with torpy or from command line
interactiveMode=3 # 1:interactive + data loaded  2:interactive load data in script 3:batch

# Change the flag if you want to change computation method (hrz/tops  vs checkshots)
checkShotsOption=True

###########################################################
#   Data (input/output)
###########################################################
#output file for mistie process
miscOutputFile = '/tmp/miscResult'

if (interactiveMode==1) :
	hg = vision.getCurrentHorizonGroup()
	velocity  = vision.getCurrentAttribute() # velocity for checkshots
        #Here we select only the wells with Tops
        wellList = vision.getWellList(WellSelectionMode.WITH_TOP)
else:
	hg_path='/data2/devtest/tornado/welltest/data/hrz_gb.hrz'
	velocity_path='/data2/devtest/tornado/welltest/data/xp_M_a.fdm'
	
	# Need to load the horizon group
	hg = HorizonGroup()
	hg.load( hg_path )
	
	velocity  = Volume()
	velocity.load(velocity_path)
	# Need to load the wells from a well list


###########################################################
#   MISTIE Process
###########################################################

processMisite = HorizonMisite()
# Output annotation File
processMisite.setAnnotationFile(miscOutputFile)

# Uncomment lines to change default
# Horizontal Interpolation: Triangulation, NatNeighbor, MinCurvature, Delta, Nearest
# default:       processMisite.setInterpolationMode(InterpMode.MINIMUM_CURVATURE, 0.2, 0 )
# example:     processMisite.setInterpolationMode(InterpMode.LINEAR)

# Vertical Interpolation for Missing tops  (interpolation , output interpolated misite)
# default: processMisite.interpolateMissingTops( False )
# example:     processMisite.interpolateMissingTops( True, True )

#Structure Based
# default:  processMisite.structureBased(False)

#output attribute name 
# default: processMisite.setOutputAttrName('MISTIE')

# z in time Time or depth 
# default: 'DEPTH' if attr exist or 'TWT'
# default: processMisite.setDomain('DEPTH')

#By default the process is using all the wells
processMisite.setWellList(wellList)


if (checkShotsOption==False) :
	# Process
	# With Top horizon Pairs
	topHorizonPair =[ ['Top_Balder','Top_Balder'],
                            ['Top_Chalk','Top_Rattray'],
                            ['Base_Rattray','Base_Rattray']]
	
	processMisite.run(hg, topHorizonPair)

else :   
	# CheckShot Option
	# OR  from Checkshots and FDM
	horizons = ['Top_Balder','Top_Chalk','Herring']
	#TODO processMisite.run(hg, wellList, velocity, horizons )
	processMisite.run(hg,  horizons, velocity )


if(interactiveMode==2):
  vision.addHorizonGroup(hg)
  vision.addAttribute(velocity,'toto.fdm')


###########################################################
#   QC the data in the graphic application
#         Not for TORPY !!!!!!!!!!!!!!!!!
###########################################################
if(interactiveMode==1 or interactiveMode==2):
	# Horizons to QC
	h_toQC = ['Top_Balder','Top_Chalk','Herring','Base_Rattray']
	
	# Load Annotations created by the process
	annotations = PointAnnotation()
	annotations.loadDisplaySettings('/tmp/settingsAnnotations.xml') # Settings saved from the dialog
	# As definied in the dialog: name, wellID, topName, hrzName, pos, depth, mistie, mistie_perc
	# Default: 'depth','mistie','mistie_perc'
	#annotations.fields(['wellID','depth','mistie','mistie_perc']) 
	
	# Just an exemple to display for all the tops the misties
	#annotations.load(miscOutputFile,0)    #  extract info from the current hrz 
	#time.sleep(2) # delay 2 seconds
	vision.showWell(WellSelectionMode.WITH_TOP)
	
	# For each horizon display the values of the misties
	for hName in h_toQC :
	  annotations.unload()                           # Remove all annotations
	  vision.setCurrentHorizon(hName)         # Set current horizon
	  annotations.load(miscOutputFile,1)    #  extract info from the current hrz 
	  time.sleep(2) # delay 2 seconds


