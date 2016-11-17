from base import Volume
from algorithm import convertUnit, LengthUnit
#load fdm
path1="/data2/devtest/tornado/test_scripting/"
fdm=Volume()
fdm.load(path1+'titusrg_scn10_hgd_dipxsm.fdm')

origin=fdm.getOrigin()
step=fdm.getStep()
print "Origin:",fdm.getOrigin()
print "Step:",fdm.getStep()

m2f=convertUnit(LengthUnit.METER, LengthUnit.FEET)
print "Meter to feet constant:", m2f

#only convert the z component
origin[2]=origin[2]*m2f
step[2]=step[2]*m2f
print "Converting the z component of geometry:"


print "Converting the data"
fdm=fdm*m2f
fdm.setOrigin(origin)
fdm.setStep(step)
print "New Origin:",fdm.getOrigin()
print "New Step:", fdm.getStep()
path2="/data2/devtest/tornado/test_scripting/"
fdm.saveAs(path2+'attrUnitConvert.fdm')
