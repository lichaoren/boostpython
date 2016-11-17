'''test volume load/save'''
from base import Volume

save_path="/auto/neo/home1/yanhliu/test.txt"
f=open(save_path,'w')
f.write("XL   IL   X             Y             Z           VAL\n")
#load fdm
path1="/data2/devtest/tornado/yanhliu/rca/"
fdm=Volume()
fdm.load(path1+'titusrg_scn10_hgd_dipxsm.fdm')

size=fdm.getSize()
min_val=58
max_val=59
print size
val=0
for i in xrange(size[0]):
    for j in xrange(size[1]):
        for k in xrange(size[2]):
            
            val=fdm.getValue(i,j,k)
            if (val<min_val or val>max_val):
                continue
            line=fdm.grd2lin([i,j,k,0])
            logic=fdm.grd2log([i,j,k,0])
            f.write("%i %i %f %f %f %f\n" % (line[0],line[1],logic[0],logic[1],logic[2], val))


f.close()
