from base import Volume
from algorithm import VolumeSmooth
from algorithm import VolumeMath
from vision import *

data=Volume()
data.load('/s0/scr/DATA/essaiCreateModel.fdm')

data_b=Volume(data)
data_c=Volume(data)

# Initialize parameters for smoothing
volSmoother = VolumeSmooth()

# The Type of data is an important parameter # i.e: use VELOCITY to smooth in slowness # Choices: MISC, VELOCITY
volSmoother.setDataType(VolumeSmooth.VELOCITY)

# Set the interpolation method
# Choices: MEAN,MEDIAN,ALPHA_TRIM,B_SPLINE,MEAN_EXCLUDE,COMPRESS,MEAN_DVK,
#          MEAN_DIRECTION,MEAN_SINGLE_FAULT,MEAN_MULTI_FAULTS,APPROX_MEDIAN
volSmoother.setMethod(VolumeSmooth.MEAN)

# Window size in meter/depth unit
volSmoother.setSmoothWindow(200,200,4)

# Run the smoothing
volSmoother.run(data_b)
#data_b.saveAs('/data05/sp258pp/PS_VA/swi/zzhou_edit/r47_60vsedit_sm200_4.fdm')


# Initialize parameters for smoothing
volSmoother = VolumeSmooth()
volSmoother.setDataType(VolumeSmooth.VELOCITY)
volSmoother.setMethod(VolumeSmooth.MEAN)
volSmoother.setSmoothWindow(400,400,8)
volSmoother.run(data_c)
#data_c.saveAs('/data05/sp258pp/PS_VA/swi/zzhou_edit/r47_60vsedit_sm400_8.fdm')


#data_d=data+data_c-data_b
data_d=data_c-data_b
#data_d.saveAs('/data05/sp258pp/PS_VA/swi/zzhou_edit/r47_60vsedit_smsub.fdm')

vision.addAttribute(data,'origine')
vision.addAttribute(data_b,'smooth_200')
vision.addAttribute(data_c,'smooth_400')
vision.addAttribute(data_d,'difference')


