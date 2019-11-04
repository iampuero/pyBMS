# Code Bin

#BMS correlations for stationary case
# Not to apply now
# Based in Pranas
# Suposes stationarity
def pairwiseTimeCorrelations(self,Tmax, fUnit,sUnit): #STATIONARY
	T = self.raster.shape[1]
	maxTime = 2*Tmax+1
	times = [np.where(self.raster[fUnit]==1)[0],np.where(self.raster[sUnit]==1)[0]]
	r = [times[0].shape[0]/T,times[1].shape[0]/T] #spikeRates
	norm = np.sqrt((r[0]-r[0]**2)*(r[1]-r[1]**2))
	Corr = np.zeros(maxTime)
	for m in range(-Tmax,Tmax+1): #m -> [-Tmax,Tmax] #PARAL
		llim = max(0,m)
		rlim = min(T-1,T+m-1)
		for n in times[0]:
			time = n+m
			if not llim<=time<=rlim:
				continue
			Corr[m+Tmax]-=r[1]
			if time in times[1]:
				Corr[m+Tmax]+=1
		for n in times[1]:
			if not llim<=n<=rlim:
				continue
			Corr[m+Tmax]-=r[0]
		Corr[m+Tmax]+=(rlim-llim-1)*r[0]*r[1]
		Corr[m+Tmax]/=(T*norm)
	return Corr
def AvgPairwiseTimeCorrel(self,Tmax): #STATIONARY
	N=self.raster.shape[0]
	Corr = np.array([self.pairwiseTimeCorrelations(Tmax,i,j) for i in range(N) for j in range (i+1,N)])
	return sum(Corr)/Corr.shape[0] #Average and return