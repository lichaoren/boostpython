'''Test Horizon save, load and some basic interfaces and interpolations'''
from base import HorizonGroup
from base import Horizon
from algorithm import GridInterp as gp
from algorithm import InterpMode as mode
from base.region import *

f="/data2/devtest/tornado/yanhliu/rca/titus_test.hrz"
#test loading
hg=HorizonGroup()
hg.load(f)
#test basic interface for horizon
n=hg.getNumHorizon()
for i in range(n):
    hrz=hg.getHorizonByIndex(i)
    print hrz.getName()
    m=hrz.getNumAttr()
    for j in range(m):
        print "-"*5, hrz.getAttrName(j)
    print "*"*5, hrz.getAttrIndex("DEPTH")

#test interpolation
interpolator= gp()
interpolator.IsWallToWall=True
interpolator.Mode=mode.LINEAR # MINIMUM_CURVATURE #LINEAR # NEAREST_NEIGHBOR # NATRUAL_NEIGHBOR
interpolator.Radius=100

#loop the horizon group to interpolate the DEPTH attribute
for i in range(n):
    hrz=hg.getHorizonByIndex(i)
    print "get the horizon:",hrz.getName()
    m=hrz.getAttrIndex("DEPTH")
    if (m<0):
        continue
    interpolator.interp(hrz,m)
#test horizon save
hg.setPath(f.split('.')[0]+'_py.hrz')
hg.save(True)


aRegion=MultiPolygon2D()
aRegion.load("/data2/devtest/tornado/yanhliu/Script_test/testRgn.rgn")
print "aPoly num points before: ",aRegion.numPolygons()
aSurveyRegion = SurveyRegion()
aSurveyRegion.setPolygonCoordinates(aRegion.polygon(0))
interpolator.setRegion(aSurveyRegion)
hg2=HorizonGroup()
hg2.load('/data2/devtest/tornado/yanhliu/train_data/test_hg.hrz')
hrz=hg2.getHorizonByIndex(hg2.getHorizonIndexByName("BOS2"))
hrz2=hg2.getHorizonByIndex(hg2.getHorizonIndexByName("TOS2"))
m=hrz.getAttrIndex("DEPTH")
interpolator.interp(hrz,m)
interpolator.interp(hrz2,m)
hg2.setPath("/data2/devtest/tornado/yanhliu/Script_test/rgnInterpHrz.hrz")
hg2.save()
