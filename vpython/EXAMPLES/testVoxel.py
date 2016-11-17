from base import Volume 
from algorithm import RegridValueType as vt
from algorithm import RegridInterpType as it
from algorithm import RegridOutputType as ot
from algorithm import RegridResampleType as st
from algorithm import AngleType 
from algorithm import VolumeRegrid
from base import Geometry 

path="/data2/devtest/tornado/test_scripting/"
data=Volume()
data.load(path+'wwaz_sc15b3_0126_rtm.vol.gz')
angleGeom=Geometry()
angleGeom.load(path+'testGeometry.geom')
regrid=VolumeRegrid()
regrid.setGeometry(angleGeom)
regrid.setInput(data)
regrid.setValueType(vt.ANGLE)
regrid.setAngleType(AngleType.DegreeSigned)
regrid.setInterpType(it.TRIANGULATION)
output=regrid.regrid()
output.saveAs(path+'testVoxelRegrid.fdm')
