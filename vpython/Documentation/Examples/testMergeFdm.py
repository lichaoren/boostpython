from base import Volume
from algorithm import MergeFdm
from algorithm import MergeCombineOption as MP
from base.region import *

path="/data2/devtest/tornado/yanhliu/Script_test/"
data1, data2= Volume(), Volume()
data1.load(path+'Tomerge1.fdm')
data2.load(path+'toMerge2.fdm')

vol_lst=[data1,data2]
file_lst=[path+'toMerge3.fdm']

merger=MergeFdm()
#do the setups
merger.setInMemData(vol_lst)
merger.setOnDiskData(file_lst)
merger.setNullValue(0)
merger.setCombineOption(MP.ByLine)
merger.setGeom([3693,8003,0,0],[4573,8883,1000,0],[4,4,1,0])
merger.setTargetName('/data2/devtest/tornado/yanhliu/Script_test/test2.fdm')
merger.setSaveToDisk(True)
merger.doMerge()


#now test with region method
merger=MergeFdm()

aRegion=MultiPolygon2D()
aRegion.load("/data2/devtest/tornado/yanhliu/Script_test/mergeRgn.rgn")
aSurveyRegion = SurveyRegion()
aSurveyRegion.setPolygonCoordinates(aRegion.polygon(0))


#do the setups
lst2=vol_lst[:2] # the first 2
merger.setInMemData(lst2)
merger.setNullValue(0)
merger.setToSmoothBoundary(True)
merger.setSmoothBoundaryRadius(6)
merger.setCombineOption(MP.ByLine)
merger.setGeom([3709,8195,0,0],[4121,8631,1000,0],[4,4,1,0])
merger.setTargetName('/data2/devtest/tornado/yanhliu/Script_test/test3.fdm')
merger.setSaveToDisk(True)
merger.setRegion(aSurveyRegion)
merger.doMerge()
