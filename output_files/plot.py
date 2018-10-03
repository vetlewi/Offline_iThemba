from numpy import *
import matplotlib.pyplot as plt

infile = open("Calib_data_140818.txt", "r")
lines = infile.readlines()
infile.close()

ringNo = []
NiGS = []

for line in lines:
	info = line.split()

	if info[0] == "63Ni_0M":
		if int(info[1]) == 0:
			ringNo.append(int(info[2]))
			NiGS.append(float(info[3]))

plt.plot(NiGS, ringNo, "o")
plt.show()