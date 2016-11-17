from base import Geometry

geom=Geometry()

min_geom=[3693,8003,0,1]
max_geom=[4577,8883,0,1]
inc=[4,4,1,1]

geomSize=[222,221,481,1]
geomOrigin=[92300., 200050., 0., 0.]
geomStep=[100., 100., 25., 0.]

geom.setMin(min_geom)
geom.setMax(max_geom)
geom.setInc(inc)
geom.setOrigin(geomOrigin)
geom.setStep(geomStep)
geom.setSize(geomSize)

#print out
print "Min Lines:", geom.getMin()
print "Max Lines:", geom.getMax()
print "Line Increment per Grid:", geom.getInc()
print "Logical Origin:" ,geom.getOrigin()
print "Logical increment per Grid:",geom.getStep()
print "Number of grid:",geom.getSize()

print "*"*20
print "test geometry loading"
print "*"*20

geomLoaded=Geometry()
path="/data2/devtest/tornado/test_scripting/testGeometry.geom"
geomLoaded.load(path)
print "Min Lines:", geomLoaded.getMin()
print "Max Lines:", geomLoaded.getMax()
print "Line Increment per Grid:", geomLoaded.getInc()
print "Logical Origin:" ,geomLoaded.getOrigin()
print "Logical increment per Grid:",geomLoaded.getStep()
print "Number of grid:",geomLoaded.getSize()

