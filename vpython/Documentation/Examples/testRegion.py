from base import Geometry
from base.region import *


path_base="/data2/devtest/tornado/test_scripting/"
path_r1 = path_base+'test_region2.txt'


#load region from file
aRegion=MultiPolygon2D()
aRegion.load(path_r1)
numpoly = aRegion.numPolygons()
print "number polygons: ",numpoly
for i in range(0,numpoly):
    print "polygon: ",i," number points: ",aRegion.polygon(i).getNum()

#manipulate region directly?
print ""
print "----- add point to first polygon -----"

aPoly = aRegion.getPolygon(0)
print "aPoly num points before: ",aPoly.getNum()
aPoly.addPointSequence(450,450)
print "aPoly num points after: ",aPoly.getNum()
print ""


#adding point to original polygon
aRegion.polygon(0).addPointSequence(427,1542)
aRegion.polygon(0).addPointSequenceAt(5,397.56,1489.9)
aRegion.save(path_base+'test_region2_save.txt')
print "-----  changed original polygon  -----"
for i in range(numpoly):
    print "polygon: ",i," number points: ",aRegion.polygon(i).getNum()
print ""
print "saved modified region file to: "
print " ",(path_base+'test_region2_save.txt')


print ""
print "----- loading geometry -----"
aGeom = Geometry()
aGeom.load(path_base+'brtstk.geom')
gmin = aGeom.getMin()
gmax = aGeom.getMax()
ginc = aGeom.getInc()
gsize = aGeom.getSize()
print "Geom Min:", gmin
print "Geom Max:", gmax
print "Geom Inc:", ginc
print "Geom Size:", gsize
print ""
print "----- geometry from region -----"
aRGeom = aRegion.getGeometry()
gmin = aRGeom.getMin()
gmax = aRGeom.getMax()
ginc = aRGeom.getInc()
gsize = aRGeom.getSize()
print "RGeom Min:", gmin
print "RGeom Max:", gmax
print "RGeom Inc:", ginc
print "RGeom Size:", gsize
print ""

print "after set geometry: "
aRegion.setGeometry(aGeom)
aRGeom = aRegion.getGeometry()
gmin = aRGeom.getMin()
gmax = aRGeom.getMax()
ginc = aRGeom.getInc()
gsize = aRGeom.getSize()
print "RGeom Min:", gmin
print "RGeom Max:", gmax
print "RGeom Inc:", ginc
print "RGeom Size:", gsize
print ""

print "----- create survey region -----"
aSurveyRegion = SurveyRegion()
aSurveyRegion.setGeometry(aGeom)
aSurveyRegion.setPolygonCoordinates(aRegion.polygon(0))
print "region type (0=rectangle,1=polygon): ",aSurveyRegion.getType()
print "point [394,1346] is inside region? ",aSurveyRegion.logInside([394,1346])
print "point [968,1715] is inside region? ",aSurveyRegion.logInside([968,1715])


