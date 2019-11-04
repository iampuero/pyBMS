
import numpy as np
from os import system
from time import sleep
import matplotlib.pyplot as plt

EPS = 10**-10

class BMS:
	def __init__(self,units,leak,noise,threshold=1,randomstart=0):
		self.units = units
		self.leak = leak
		self.unitThreshold = np.array([threshold for n in range(units)])
		self.unitVoltage = np.zeros(units)
		self.weights = np.zeros((units,units))
		self.raster = None
		self.rasterLog = None
		self.v0 = np.random.random(units)*randomstart
		self.noise = noise

	def randomW(self,sigma):
		self.weights=np.random.choice([-1,1],(self.units,self.units))*np.random.random((self.units,self.units))*sigma

	def barStimulus(self,a,delta,v,offset,T):
		return np.array([[amplitude if -a<=i*delta-v*t<=a else 0 for i in range(self.units)] for t in range(T)])

	def new_raster(self,timesteps,current=None):
		if type(current)==type(None):
			current=np.zeros(self.units)
		raster=np.zeros((self.units,timesteps))
		oldV = self.v0
		newV = np.zeros(self.units)
		vCurrent = len(current.shape)==1 #vector
		for t in range(timesteps):
			for i in range(self.units):
				if vCurrent:
					newV[i]=current[i]
				else:
					newV[i]=current[t][i]
				newV[i]+=np.random.random()*self.noise*np.random.choice([-1,1])
				for j in range(self.units):
					newV[i]+=self.weights[i][j]*int(oldV[j]>self.unitThreshold[j])
				if oldV[i]>=self.unitThreshold[i]:
					raster[i][t]=1
				else:
					newV[i]+=oldV[i]*self.leak
			newV,oldV = np.copy(oldV),np.copy(newV)
		self.raster=raster
		return raster

	def new_rasterLog(self,timesteps,current=None):
		if type(current)==type(None):
			current=np.zeros(self.units)
		raster=np.zeros((self.units,timesteps))
		vLog = []
		oldV = self.v0
		newV = np.zeros(self.units)
		vCurrent = len(current.shape)==1 #vector
		for t in range(timesteps):
			for i in range(self.units):
				if vCurrent:
					newV[i]=current[i]
				else:
					newV[i]=current[t][i]
				newV[i]+=np.random.random()*self.noise*np.random.choice([-1,1])
				for j in range(self.units):
					newV[i]+=self.weights[i][j]*int(oldV[j]>self.unitThreshold[j])
				if oldV[i]>=self.unitThreshold[i]:
					raster[i][t]=1
				else:
					newV[i]+=oldV[i]*self.leak
			vLog.append(newV)
			newV,oldV = np.copy(oldV),np.copy(newV)
		self.raster=raster
		self.rasterLog = np.stack(vLog)
		return raster,np.stack(vLog)


	def pairwiseCorrelation(self,fUnit,sUnit,interval,tau): #External C code
		T = self.raster.shape[1]
		times = [np.where(self.raster[fUnit]==1)[0],np.where(self.raster[sUnit]==1)[0]]
		with open("Files/n{0}.txt".format(fUnit),"w") as O1:
			with open("Files/n{0}.txt".format(sUnit),"w") as O2:
				for n in range(len(times[0])):
					O1.write(str(int(times[0][n]))+"\n")
				for n in range(len(times[1])):
					O2.write(str(int(times[1][n]))+"\n")
		system("./a.out {0} {1} {2} {3} {4}".format(fUnit,sUnit,tau,T,interval))
		sleep(0.5)
		with(open("Files/{0}-{1}-{2}.txt".format(fUnit,sUnit,tau))) as O:
			C=[]
			for x in O:
				t,c,o = list(map(float,x.strip().split("\t")))
				C.append(c)
		return C
		plt.figure(figsize=(16,4))
		plt.xlim(0,T)
		plt.ylim(-1,1)
		plt.plot(range(len(C)),C)
		plt.plot([0,T],[0,0],"k--",alpha=0.2)

	def PWC(self,fUnit,sUnit,interval,tau): #Centered Window
		vec1 = self.raster[fUnit]
		vec2 = self.raster[sUnit]
		T=len(vec1)
		if tau==0:
			vec3 = vec1*vec2
		else:
			vec3 = np.array([1.0 if e>0 else 0.0 for e in 
	                vec1*sum(np.array([np.concatenate([np.zeros(abs(min(s,0))),
	                	vec2[max(s,0):len(vec2)-abs(min(s,0))],
	                	np.zeros(max(s,0))]) for s in range(-tau,tau+1)]))])
		phi = np.array([np.sum(vec3[int(max(0,t-interval/2)):int(min(t+interval/2,T))])for t in range(int(T))])/interval
		sp1 = np.array([np.sum(vec1[int(max(0,t-interval/2)):int(min(t+interval/2,T))]) for t in range(int(T))])/interval
		sp2 = np.array([np.sum(vec2[int(max(0,t-interval/2)):int(min(t+interval/2,T))]) for t in range(int(T))])/interval
		#print(phi>sp1)
		C = (phi-sp1*sp2)/np.sqrt(sp1*sp2)
		C = [C[i] if sp2[i]>0 and sp1[i]>0 else 0.0 for i in range(len(C))]
		#print ( [(i,C[i], phi[i],sp1[i],sp2[i])  for i in range(T-interval) if C[i]>1] )
		return C


def PWC(vec1,vec2,interval,T,tau): #with sprates 0 and 1
	times = [np.where(vec1==1)[0],np.where(vec2==1)[0]]
	with open("Files/n0.txt","w") as O1:
		with open("Files/n1.txt","w") as O2:
			for n in range(len(times[0])):
				O1.write(str(int(times[0][n]))+"\n")
			for n in range(len(times[1])):
				O2.write(str(int(times[1][n]))+"\n")
	system("./a.out {0} {1} {2} {3} {4}".format(0,1,tau,T,interval))
	sleep(0.5)
	with(open("Files/{0}-{1}-{2}.txt".format(0,1,tau))) as O:
		with(open("Files/{0}-{1}-firing1.txt".format(0,1))) as FR1:
			with(open("Files/{0}-{1}-firing2.txt".format(0,1))) as FR2:
				C=[]
				for x in O:
					t,c,o = list(map(float,x.strip().split("\t")))
					C.append(c)
				F1=[]
				for x in FR1:
					t,c,o = list(map(float,x.strip().split("\t")))
					F1.append(c)
				F2=[]
				for x in FR2:
					t,c,o = list(map(float,x.strip().split("\t")))
					F2.append(c)
	return C,F1,F2
	
def PWC2(vec1,vec2,interval,T,tau):
	if tau==0:
		vec3=vec1*vec2
	else:
		vec3 = np.array([1.0 if e>0 else 0.0 for e in vec1*sum(np.array([np.concatenate([np.zeros(abs(min(s,0))),vec2[max(s,0):len(vec2)-abs(min(s,0))],np.zeros(max(s,0))]) for s in range(-tau,tau+1)]))])
	phi = np.array([np.sum(vec3[int(max(0,t-interval/2)):int(min(t+interval/2,T))])for t in range(int(T-interval/2))])/interval
	sp1 = np.array([np.sum(vec1[int(max(0,t-interval/2)):int(min(t+interval/2,T))]) for t in range(int(T-interval/2))])/interval
	sp2 = np.array([np.sum(vec2[int(max(0,t-interval/2)):int(min(t+interval/2,T))]) for t in range(int(T-interval/2))])/interval
	#print(phi>sp1)
	C = (phi-sp1*sp2)/np.sqrt(sp1*sp2)
	C = [C[i] if sp2[i]>0 and sp1[i]>0 else 0.0 for i in range(len(C))]
	print ( [(i,C[i], phi[i],sp1[i],sp2[i])  for i in range(T-interval) if C[i]>1] )
	return C


### pythonize tau,pi,pi'/pi,pi'/1-pi,deltaX?
def pi(x):
    return 0.5 * (1 - erf(x / np.sqrt(2.0)))
def piPrime(x):
    return 0
def getX(x):
    return 0
def getIe(x):
    return 0
def getEta(x):
    return 0
def getSigma(x):
    return 0
def getTau(x):
    return 0

