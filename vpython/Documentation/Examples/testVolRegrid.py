from base import *
from algorithm import RegridValueType as vt
from algorithm import RegridInterpType as it
from algorithm import RegridOutputType as ot
from algorithm import RegridResampleType as st
from algorithm import AngleType 
from algorithm import VolumeRegrid

#load data 
path="/data2/devtest/tornado/yanhliu/rca/"
data=Volume()
data.load(path+'titusrg_test_start_0922.fdm_p')

#load geometry

path="/data2/devtest/tornado/yanhliu/Script_test/"
geomLoaded=Geometry()
geomLoaded.load(path+'regrid_geom.geom')
print "Min Lines:", geomLoaded.getMin()
print "Max Lines:", geomLoaded.getMax()
print "Line Increment per Grid:", geomLoaded.getInc()
print "Logical Origin:" ,geomLoaded.getOrigin()
print "Logical increment per Grid:",geomLoaded.getStep()
print "Number of grid:",geomLoaded.getSize()

regrid=VolumeRegrid()
regrid.setInput(data)
regrid.setGeometry(geomLoaded)
regrid.setValueType(vt.SLOWNESS) #VALUE, SLOWNESS,ANGLE,TTCOMP
regrid.setInterpType(it.FOUR_POINT_LINEAR)
regrid.setResampleType(st.LINEAR_TTS)

output=regrid.regrid()
output.saveAs(path+'testRegrid.fdm')



####################################################
# now test the angle cube regrid

data2=Volume()
angle_path="/data2/devtest/tornado/yanhliu/rca/"+'titusrg_scn10_hgd_dipxsm.fdm'
data2.load(angle_path)
angleGeom=Geometry()
angleGeom.load(path+'regrid_angle_geom.geom')
regrid=VolumeRegrid()
regrid.setGeometry(angleGeom)
regrid.setInput(data2)
regrid.setValueType(vt.ANGLE)
regrid.setAngleType(AngleType.DegreeSigned)
regrid.setInterpType(it.TRIANGULATION)
output=regrid.regrid()
output.saveAs(path+'testAngleRegrid.fdm')

