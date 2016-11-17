from algorithm import ShotSel
from base import *

shotsel = ShotSel()
shotsel.setMethod(0)
shotsel.setVelocityPath("REMOTEFILE!houdat002:/data2/devtest/tornado/welltest/data/xp_M_a.fdm", "", "", "", "")
shotsel.setRefHorizon("REMOTEFILE!houdat002:/data2/devtest/tornado/welltest/data/hrz_gb.hrz", "Herring")
shotsel.setRayAngle(-40, 40, 10, -40, 40, 10, 0, 0, 1, 80)
shotsel.setAcceptCompleteRays()
shotsel.setShotDown()
shotsel.setDepthDipAsSupplied(0, 0)
shotsel.setMultiSourceSrcRecFile("REMOTEFILE!houdat002:/data2/devtest/tornado/thsu/shotsel/sourceReceiver2.txt", 10000);

shotsel.setSummary("REMOTEFILE!houdat002:/data2/devtest/tornado/thsu/shotsel/output/test.sum", True, True, True);
shotsel.setThreadNum(4);

shotsel.start()
shotsel.save()  
