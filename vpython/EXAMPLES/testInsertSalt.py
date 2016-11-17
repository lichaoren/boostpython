from algorithm import InsertSalt
import base

path="/data2/devtest/tornado/test_scripting/"
inserter = InsertSalt();
inserter.setHorizonGroup(path+'hrz_gb.hrz')
inserter.setInputVolume(path+'xp_M_a2_regrid.fdm')
inserter.setTargetVolume(path+'delete.fdm')
inserter.setOrigSed(path+'xp_M_a_regrid.fdm')
#inserter.setSaltVelocity(4840);
horizon_pairs=[['Hod','Herring'],
               ['Top_Balder','Top_Chalk']]
inserter.setHorizonPairs(horizon_pairs)
inserter.insertSalt();
