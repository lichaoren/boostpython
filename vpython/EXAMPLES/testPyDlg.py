'''Test Horizon save, load and some basic interfaces and interpolations'''
from base import *
from vision import vision # warning, do not run with -script, will crash

#use vision package to get the current working horizon
hrz=vision.getCurrentHorizon()

#use Horizon Interface to do update
size=hrz.getSize()

for i in range(size[0]):
    for j in range(size[1]):
        # return two, found is boolean, val is value for Depth
        # if found value, val will be updated
        val=hrz.getValue(i,j,'DEPTH')
        #only update those without values
        if ( val!=None):
            hrz.setValue(i,j,'DEPTH',200)


# now test the current attribute
attr=vision.getCurrentAttribute()
size=attr.getSize()
for i in range(size[0]/2,size[0]):
    for j in range(size[1]/2, size[1]):
        for k in range(size[2]/2,size[2]):
            val=attr.getValue(i,j,k)
            attr.setValue(i,j,k,val/2)

# now test the current seismic
seis=vision.getCurrentSeismic()
size=seis.getSize()
for i in range(size[0]/2,size[0]):
    for j in range(size[1]/2, size[1]):
        for k in range(size[2]/2,size[2]):
            val=seis.getValue(i,j,k)
            seis.setValue(i,j,k,val*2)

