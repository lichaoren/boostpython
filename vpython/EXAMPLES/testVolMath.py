from base import *
from algorithm import VolumeMath


path="/data2/devtest/tornado/yanhliu/train_data/"
data=Volume()
data.load(path+'test_base.fdm.gz')

#test the basic operators + - * / %
data2=(100+data-300)*(data/20)%data
save_path="/data2/devtest/tornado/yanhliu/Script_test/"
data2.saveAs(save_path+'testPlus1.fdm')

#test the math module with constraint
#load hg
hg=HorizonGroup()
hg.load(path+'test_hg.hrz')
hrzs=[]
hrzs.append(hg.getHorizonByIndex(hg.getHorizonIndexByName("TOS1")))

myMath=VolumeMath(hg,hrzs,True)
data3=myMath.d_dx(data)
data3.saveAs(save_path+'testPlus2.fdm')
