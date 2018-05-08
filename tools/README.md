# Tools


# How to calibrate time with the GetPeak.C & GetPeak_LaBr.C scripts.

The GetPeak.C & GetPeak_LaBr.C are ROOT scripts for identifying the prompt peak in the time spectrum and fitting these peaks to extract the alignment coefficients (calibrating the time spectra). Currently there isn't a script for PPACs, please do a pull request if you make a script doing the same for PPACs :)

To calibrate the time specra one should follow this 'recipe':

1) Set all time alginment coefficients to zero. This is done by pasting following lines into the batch file before the file you want to calibrate.

```
parameter shift_time_de = \
0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0

parameter shift_time_labr = \
0	0	0	0	0	0	0	0 0	0	0	0	0	0	0	0 \
0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0

parameter shift_time_ppac = \
0	0	0	0
```

2) Sort data.

3) Open the GetPeak.C script and edit the following line

```
TFile *file = TFile::Open("filename.root");
```
so that "filename.root" is the name of the root file you sort your data into.

4) Run the GetPeak.C script.

5) The GetPeak.C script will end by writing a block of numbers looking somewhat similar to this:
```
327.362	331.131	327.142	331.784	327.793	327.062	332.857	332.442	\
328.75	332.027	331.265	332.19	328.22	332.636	327.502	328.491	\
330.18	334.247	333.702	333.381	326	333.617	334.49	334.938	\
331.121	330.024	330.534	330.548	330.227	330.39	330.771	335	\
331.34	331.943	330.282	335.389	330.41	334.582	334.403	335.92	\
327.314	331.433	327.846	326.559	322.758	335.045	333.858	333.8	\
329.653	332.962	333.077	333.315	328.629	332.017	332.796	333.808	\
333.157	330.699	336.149	336.417	325.026	335.681	336.43	332.642
```
Now you should replace the zeros following "parameter shift_time_de = \" in the batch file with these numbers. **Congratulation, you have now align the Delta-E time!**

6) Sort data with the updated batch file.

7) Open the GetPeak_LaBr.C script and edit the line
```
TFile *file = TFile::Open("filename.root");
```
so that "filename.root" is the name of the root file you sort your data into. Now you can run the GetPeak.C script.

8) Run the GetPeak_LaBr.C script.

9) The GetPeak_LaBr.C will end by writing a block of numbers looking somewhat similar to this:
```
0 3.56841 5.82432 2.51612e+08 4.6032 7.71889 9.68854 -9.37518 -6.59023 4.6402 2.89241 -4.99057 -5.89974 2.37689 3.43747 3.5 \
3.86101 -3.23111 -3.5 -3.26796 -5.36303 -4 -6.34354 -3.01629 0.996446 9.98072 9.5 -2.81048 -2.55071 1.99404 -0.994219 -0.5
```
Relace the zeros following "parameter shift_time_labr = \" in the batch file with these numbers. **Congratulation, you have now align the LaBr time!**

10) Re-run the sorting program and enjoy all your perfectly align time spectra!!

### NOTE: These scripts are not perfect

# The Validate.cpp program
This is a small tool to check if the data aquisition was 'resynchronized' without the file being changed. Unfortunatly a 'resynchronized' will cause the time offsets of detectors to change. Luckily this effect is fairly small and are easily calibrated, but requires new alignment parameters each time there is such a 'resynchronization'. Since run files could potentially contain several occurences of such a 'resynchronization' we will need to split the file whenever this happends. The Validate.cpp is a short C++ program that checks run files if there are any such events. 

### How to compile
The program is compiled by running following:
```
>>g++ -O3 -o Validate Validate.cpp
```

### How to run
The program is ran by:
```
>>./Validate <run file #1> <run file #2> etc.
```
This will either check if the files needs to be split and split the file if needed (see below). If configured to split files then the resulting files will be written with a suffix 'A', 'B', 'C', etc. to the original filename.

### How to make it actually 'split' the files.
Change
``` C
#define SPLITFILES 0
```
to
``` C
#define SPLITFILES 1
```
in the Validate.cpp and recompile (see above).


### How to understand the output

The output will look something like this:
```
Results of file 'sirius-20180420-143428.data':
Total number of events: 4673451
Total number of ordered words: 4673451
Total number of unordered words: 0
--------------------------------------------------
```
and tells you that you need to split the file if "Total number of unordered words" is different from zero.


