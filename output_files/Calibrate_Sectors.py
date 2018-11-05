from numpy import *
import matplotlib.pyplot as plt

infile = open("sector_peaks.txt", "r")
lines = infile.readlines()
infile.close()

ringNo = []
NiGS = []

sec_no_nr = array(range(16))
Ni63_gs = zeros(16)
Ni62_gs = zeros(16)

for line in lines:
	info = line.split()
	sec_no = int(info[1])
	if info[0] == "63Ni_0M":
		Ni63_gs[sec_no] = float(info[4])
	elif info[0] == "62Ni_0M":
		Ni62_gs[sec_no] = float(info[4])

e_63 = 3121.088647
e_62 = 4530.508970

a = (e_62 - e_63)/(Ni62_gs - Ni63_gs)
b = e_63 - Ni63_gs*a

for i in range(len(sec_no_nr)):
	print "%2.6f %2.6f" % (a[i], b[i])

# Sector 6 only
ch_63 = 1.76976e+03
ch_62 = 2.56082e+03
en_63 = 3230.972830
en_62 = 4699.133007
a = (en_62 - en_63)/(ch_62 - ch_63)
b = en_63 - a*ch_63
print a, " ", b

# Sector 9 only
ch_63 = 1.78352e+03
ch_62 = 2.60294e+03
a = (en_62 - en_63)/(ch_62 - ch_63)
b = en_63 - a*ch_63
print a, " ", b

plt.plot(sec_no_nr, Ni63_gs, "o")
plt.show()