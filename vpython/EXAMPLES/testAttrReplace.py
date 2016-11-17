from base import Volume
from algorithm import ReplaceMode as mode
from algorithm import ReplaceRangeType as tp
from algorithm import AttrReplace
from algorithm import AngleType

path="/data2/devtest/tornado/test_scripting/"
data=Volume()
data.load(path+'titusrg_test_start_0922.fdm_p')

#before the attribute replace, test the vect converter
print data.getSize()
val=data.grd2log([1,2,3,4])
print val
#end of testing converter
replacer=AttrReplace()
replacer.replaceInf(True)
replacer.replaceNAN(True)
replacer.setReplaceType(mode.NEAREST_NEIGHBOR)
replacer.setRange(tp.MinMax, 1000.,3000.)
replacer.setAttribute(data)
replacer.doReplace()
data.saveAs(path+'testReplace.fdm')


######################################
#now test the angle conversion

data2_path=path+'titusrg_scn10_hgd_dipxsm.fdm'
data2=Volume()
data2.load(data2_path)
replacer.setAngleType(AngleType.DegreeSigned)
replacer.setRange(tp.MinMax,-10,10.)
replacer.setAttribute(data2)
replacer.setReplaceType(mode.LINEAR)
replacer.doReplace()
data2.saveAs(path + "testAngleReplace.fdm")
