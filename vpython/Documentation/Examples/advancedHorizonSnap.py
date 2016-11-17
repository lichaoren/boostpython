### please contact tornado support to share your python code in production ####
### Authors of this code: Beng Ong & Ravi Maranganti ####
### Version:  2016.04.15
### Documentation: \\int.cggveritas.com\shares\nasa\hou\groups\USI_Proprietary\rmaranga\09152014_TOS1_SelectiveSnap.pptx

from base import Volume
from base import HorizonGroup
from base import Horizon
from algorithm import VolumeExtractAttribute
from algorithm import VolumeMath
from algorithm import VolumeSmooth
from algorithm import HorizonSmooth
from algorithm import HorizonSnap
from vision import vision

############### DEFINING INPUT ###########################################

FLAG_interactive = 0
if (FLAG_interactive == 0):
    path = '/data9/d3359stat/03_modelbuilding/16_T3B3/80_tornado_script/03_fun_with_python/01_files_to_play/'
    tos1  = path + 'BOS1_for_selective_snap_simple.hrz'
    vol  =  path + 'prdstfd_0412stk_vx.vol.gz'
    hg = HorizonGroup()
    hg.load( tos1 )
    orig = hg.getHorizonByName('TOS1')
    seismic = Volume()
    seismic.load(vol)
else:
    hg       = vision.getCurrentHorizonGroup()
    orig     = vision.getCurrentHorizon()
    seismic  = vision.getCurrentSeismic()

############### DEFINING OUTPUT ##########################################
path2 = '/data9/d3359stat/03_modelbuilding/16_T3B3/80_tornado_script/04_experiment/'

sel_tos1 = path2 + 'BOS1_out4_selective_snap_bongtrial3.hrz'

############### SCRIPTS ##################################################
### Assume that you have starting TOS1 (interpolated/smoothed)
### Optional: includes TOS1_snap which is TOS1 that has been snap to peak
### User has an idea of what value to RMS value to clip

## user options
RMS_window      = 5	    ### radius, in sample 
RMS_threshold   = 2         ### this is also used as max RMS
Smoothing       = 3         ### (radius = 3 sample)
FLAG_snap_exist = 0         ### 1 = TOS1_snap exist and skip the snapping, 0 = do the snapping
snap_window     = 5         ### not sure sample or meter yet (mostly likely sample)

#########################################################################

hg.addAttribute('RMS')
hg.addAttribute('normalized_rms')

size = hg.getSize()

step = orig.getStep()
inc_x = step[0]
inc_y = step[1]

######### Extract RMS amplitude from volume ###########################
extractAttr = VolumeExtractAttribute()
extractAttr.windowSizeAbove = RMS_window  #number of samples above
extractAttr.windowSizeBelow = RMS_window  #number of samples below
extractAttr.mode = VolumeExtractAttribute.RMS
extractAttr.run(seismic, hg, orig, 'RMS') 
print '******** Task 1: RMS extraction complete !!!!'

###### To snap or not to snap is the question ########################
snap_name = orig.getName() + '_snap'
snap      = hg.getHorizonByName(snap_name)
if (FLAG_snap_exist != 1):
    if (snap == None):
        snap = hg.createHorizon(snap_name)
    snap.copy(orig)
    z_index = orig.getAttrIndex('DEPTH')
    snapper = HorizonSnap()
    snapper.setWindowSize(snap_window)       ##not sure sample or meter yet
    snapper.setMode(HorizonSnap.POSITIVE_PEAK)  ## 0 = mean, 2 = median
    snapper.run(seismic, snap, z_index)
print '******** Task 2: Snap of TOS1 obtained !!!!'
####################################################################

tos  = hg.createHorizon( 'tos1_selected' )
#####  Clipping and normalizing the RMS attribute in HorizonGroup ####
for i in xrange(size[0]):
    for j in xrange(size[1]):
        z_orig   = orig.getValue(i, j,'DEPTH')
        if ( z_orig == None ):
            continue
        rms_orig = orig.getValue(i, j, 'RMS')
        #### clipping and normalizing in one step
        normalized_rms = min(rms_orig, RMS_threshold) / RMS_threshold
        tos.setValue(i,j,'RMS', normalized_rms)
print '******** Task 3: Normalization and Clipping of RMS attribute done !!!!!'

##### Smoothing the RMS attribute in the horizon ########
rms_idx  = tos.getAttrIndex('RMS')
smoother = HorizonSmooth()
smoother.setRadiusX(Smoothing * inc_x)
smoother.setRadiusY(Smoothing * inc_y)
smoother.setMode(0)               ## 0 = mean, 2 = median
smoother.run(tos, rms_idx)
print '******** Task 4: Smoothing of RMS attribute is done !!!!'

##### Build Selective TOS1 #############################
nb_undefSnap = 0
for i in xrange(size[0]):
    for j in xrange(size[1]):
        z_snap  = snap.getValue(i, j,'DEPTH')
        if (z_snap == None):
            line = orig.grd2lin([i,j,0,0])
            nb_undefSnap += 1
            if (  nb_undefSnap  < 200 ):
                print 'z_snap = None: XL, IL', line[0], line[1]
            continue
        z_orig  = orig.getValue(i, j,'DEPTH')
        normalized_rms = tos.getValue(i, j, 'RMS')
        z_sel = z_snap * normalized_rms + (1 - normalized_rms) * z_orig
        tos.setValue(i,j,'DEPTH', z_sel)
print '******** Task 5: Selective TOS1 is built !!!!!'
hg.setPath( sel_tos1 )
hg.save()
print '******** All tasks complete !!!!!!'





