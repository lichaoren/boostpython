from base import *
from algorithm import RegridValueType as vt
from algorithm import RegridInterpType as it
from algorithm import RegridOutputType as ot
from algorithm import RegridResampleType as st
from algorithm import AngleType 
from algorithm import VolumeRegrid

#load data 
path="/data2/devtest/tornado/yanhliu/Script_test/"

geomLoaded=Geometry()
geomLoaded.load(path+'testBigVolRegrid.geom')
print "Min Lines:", geomLoaded.getMin()
print "Max Lines:", geomLoaded.getMax()
print "Line Increment per Grid:", geomLoaded.getInc()
print "Logical Origin:" ,geomLoaded.getOrigin()
print "Logical increment per Grid:",geomLoaded.getStep()
print "Number of grid:",geomLoaded.getSize()

regrid=VolumeRegrid()
regrid.setGeometry(geomLoaded)
regrid.setValueType(vt.SLOWNESS) #VALUE, SLOWNESS,ANGLE,TTCOMP
regrid.setInterpType(it.FOUR_POINT_LINEAR)
regrid.setResampleType(st.LINEAR_TTS)
output=regrid.regridModule(path+'example_model_0331.fdm', path+'test_example_regrid.fdm', 10)

