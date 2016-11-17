from base import Horizon
from base import HorizonGroup 
from base import Volume

#Load data   
hrg= HorizonGroup()
hrg.load('hrz_gb.hrz')
fdm = Volume()
fdm.load('xp_M_a.fdm') 

#Add attribute 'FDM'
hrg.addAttribute('FDM')

 
num_h=hrg.getNumHorizon()
print 'Number of Horizons ', num_h

#Loop on all horizons
for hor_index in xrange(num_h):
    hr=hrg.getHorizonByIndex(hor_index)
    hr_size = hr.getSize() 
    hr_name = hr.getName()
    print 'Name of Horizon --> ',hr_name, ' and its size --> ', hr_size
    for i in xrange(hr_size[0]):
        for j in xrange(hr_size[1]):
            # Extract z value from horizon
            found, val = hr.getAttrByGrid(i,j,"DEPTH") 
            if( found == False):
                continue
            log_hr_pos = hr.grd2log([i,j,0,0])
            log_hr_pos[2]=val
            volume_grd=map(int,fdm.log2grd(log_hr_pos)) 
            # At z get FDM value
            fdm_val=fdm.getValue(log_hr_pos) 

            # Set extracted value in the horizon attribute 
            hr.setAttrByLog(log_hr_pos[0], log_hr_pos[1],   fdm_val , 'FDM' )

ave Horizon
hrg.setPath('/tmp/hrz_gb_sid.hrz')
hrg.save()
