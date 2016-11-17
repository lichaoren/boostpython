from base.region import *
from base import Volume
from algorithm import ReplaceMode as mode
from algorithm import ReplaceRangeType as tp
from algorithm import ReplaceRegionType as rt
from algorithm import AttrReplace


#load multiple regions at one run
aRegion=MultiPolygon2D()
aRegion.load("/data2/devtest/tornado/test_scripting/testRgn2.rgn")
npoly=aRegion.numPolygons()
print "number of regions: ",npoly


#load the data
data=Volume()
data.load('/data2/devtest/tornado/test_scripting/test_attr_replace_region.fdm')

# get the replacer
replacer=AttrReplace()
replacer.replaceInf(True)
replacer.replaceNAN(True)
replacer.setReplaceType(mode.FIXED_VALUE, 0.)
replacer.setRange(tp.MinMax, 0.,5000.)
replacer.setAttribute(data)

for poly in range(npoly):
    aSurveyRegion = SurveyRegion()
    aSurveyRegion.setPolygonCoordinates(aRegion.polygon(poly))
    replacer.setRegion(aSurveyRegion, rt.Current)
    replacer.doReplace()

data.saveAs('/data2/devtest/tornado/test_scripting/test_attr_replace_region_out.fdm')
