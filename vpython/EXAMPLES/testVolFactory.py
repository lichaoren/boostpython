from base import *

geomLoaded=Geometry()
path="/data2/devtest/tornado/yanhliu/rca/testGeometry.geom"
geomLoaded.load(path)

vol=createVolume(geomLoaded)

size=vol.getSize()
                            
for i in xrange(size[0]):
    for j in xrange(size[1]):
        for k in xrange(size[2]):
            vol.setValue(i,j,k,3.14159)

vol.saveAs("/data2/devtest/tornado/yanhliu/rca/testCreate.fdm")
