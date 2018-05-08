from numpy import *

outfile = open("auto_setup.c", "w")

outfile.write("#include experimentsetup.h\n")
outfile.write("\n\n\n\n")

outfile.write("DetectorInfo_t pDetector[] =\n{\n")

num_channels = 180

labr1_mod = 0
labr2_mod = 1
dE1_2_mod = 2
dE3_4_mod = 3
dE5_6_mod = 4
dE7_8_mod = 5
E_mod = 6

"""labr1_mod = 6
labr2_mod = 5
dE1_2_mod = 4
dE3_4_mod = 3
dE5_6_mod = 2
dE7_8_mod = 1
E_mod = 0"""

labr1_start = 32+16*labr1_mod
labr1_stop = 32+16*(labr1_mod+1)

labr2_start = 32+16*labr2_mod
labr2_stop = 32+16*(labr2_mod+1)

dE1_2_start = 32+16*dE1_2_mod
dE1_2_stop = 32+16*(dE1_2_mod+1)

dE3_4_start = 32+16*dE3_4_mod
dE3_4_stop = 32+16*(dE3_4_mod+1)

dE5_6_start = 32+16*dE5_6_mod
dE5_6_stop = 32+16*(dE5_6_mod+1)

dE7_8_start = 32+16*dE7_8_mod
dE7_8_stop = 32+16*(dE7_8_mod+1)

E_start = 32+16*E_mod
E_stop = 32+16*(E_mod+1)

count = 0
back_mod = 0

listOfDetectors = empty(num_channels, dtype=list)

# Set everything to "zero"
for i in range(0, num_channels):
	listOfDetectors[i] = [i, "f000MHz", "unused", 0, 0]

# Set LaBr channels (we will figure out about this in more detail later)
for i in range(labr1_start, labr1_stop):
	listOfDetectors[i] = [i, "f500MHz", "labr", i-labr1_start, 0]

for i in range(labr2_start, labr2_stop):
	listOfDetectors[i] = [i, "f500MHz", "labr", i-labr2_start+16, 0]

for i in range(dE1_2_start, dE1_2_stop):
	count += 1
	if count > 8:
		count = 0
		back_mod += 1
	listOfDetectors[i] = [i, "f250MHz", "deDet", i-dE1_2_start, back_mod]

back_mod += 1
count = 0
for i in range(dE3_4_start, dE3_4_stop):
	count += 1
	if count > 8:
		count = 0
		back_mod += 1
	listOfDetectors[i] = [i, "f250MHz", "deDet", i-dE3_4_start+16, back_mod]

back_mod += 1
count = 0
for i in range(dE5_6_start, dE5_6_stop):
	count += 1
	if count > 8:
		count = 0
		back_mod += 1
	listOfDetectors[i] = [i, "f250MHz", "deDet", i-dE5_6_start+32, back_mod]

back_mod += 1
count = 0
for i in range(dE7_8_start, dE7_8_stop):
	count += 1
	if count > 8:
		count = 0
		back_mod += 1
	listOfDetectors[i] = [i, "f250MHz", "deDet", i-dE7_8_start+32+16, back_mod]

#for i in range(E_start, E_stop):
#	listOfDetectors[i] = [i, "f250MHz", "eDet", i-E_start]

for i in range(0, 8):
	if i < 4:
		listOfDetectors[2*i+E_start] = [2*i+E_start, "f250MHz", "ppac", i, i]
	else:
		listOfDetectors[2*i+E_start] = [2*i+E_start, "f250MHz", "eGuard", i, i]
	listOfDetectors[2*i+E_start+1] = [2*i+E_start+1, "f250MHz", "eDet", i, i]


for i in range(0, num_channels):
	det = listOfDetectors[i]
	outfile.write("\t{%d, %s, %s, %d, %d},\n" % (det[0], det[1], det[2], det[3], det[4]) )
outfile.write("\t{%d, %s, %s, %d, %d}\n" % (num_channels, "f000MHz", "unused", 0, 0))
outfile.write("};\n")

outfile.close()

