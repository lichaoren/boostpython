
from base import Volume 
from algorithm import unit, RcaSmooth

path="/data2/devtest/tornado/yanhliu/rca/"
data, theta, phi = Volume(), Volume(), Volume()
data.load(path+'titusrg_test_start_0922.fdm_p')
theta.load(path+'titusrg_scn10_hgd.fdm_theta')
phi.load(path+'titusrg_scn10_hgd.fdm_phi')
param=[
[0,    1000, 100],
[3000, 1000, 100],
[6000, 2000, 150],
[9000, 3000, 200]
]

rcsmoother = RcaSmooth()
rcsmoother.setup(data,theta,phi,param,False,unit.degree)
res = rcsmoother.smooth()
res.saveAs(path+'testRca2.fdm')
