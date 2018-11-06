#include "Calibration.h"

#include "experimentsetup.h"

static double gain_labr3x8[NUM_LABR_3X8_DETECTORS] = {0.574817, 0.709093};
static double shift_labr3x8[NUM_LABR_3X8_DETECTORS] = {-3.432099, -6.136491};

static double gain_labr2x2_fs[NUM_LABR_2X2_DETECTORS] = {0.787, 1.054, 0.907, 0.948, 0.724, 0.750};
static double shift_labr2x2_fs[NUM_LABR_2X2_DETECTORS] = {13.003, -8.962, 9.454, -3.280, 26.419, 36.128};

static double gain_labr2x2_ss[NUM_LABR_2X2_DETECTORS] = {1.222, 1.943, 1.426, 1.769, 1.151, 1.213};
static double shift_labr2x2_ss[NUM_LABR_2X2_DETECTORS] = {0.596, -4.586, -3.441, -2.070, 5.438, 9.871};

static double gain_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS] =
{
    {0.408000,  0.381000,   0.414000,   0.414000},
    {0.169000,  0.162000,   0.165000,   0.166000},
    {0.369651,	0.463323,	0.394758,	0.398068},
    {0.384589,	0.392890,	0.562113,	0.539415},
    {0.381208,	0.371987,	0.359224,	0.359925},
    {0.592566,	0.390425,	0.376041,	0.592191},
    {1.000000,  1.000000,   1.000000,   1.000000},
    {0.368001,	0.381710,	0.590218,	0.523783},
    {0.161583,	0.166419,	0.159687,	0.162319},
    {1.000000,  1.000000,   1.000000,   1.000000}
};
static double shift_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS] =
{
    {3.220,		1.925,		2.442,		2.772},
    {0.485,		0.680,		0.399,		0.509},
    {3.184580,	-2.89467,	0.335753,	3.89079},
    {0.548623,	 2.68867,	1.118820,	-0.0218},
    {0.158211,	 2.82344,	1.607980,	3.47200},
    {0.263676,	 3.62043,	1.894920,	0.81708},
    {0,          0,         0,          0},
    {2.4162, 	6.05897,	0.81121,	-0.290962},
    {0.797847,	0.384532,	-0.0086,	0.5429770},
    {0,          0,         0,          0}
};

static double gain_de_ring[NUM_SI_DE_RING] = {
    1.927446385220,	1.947499935140,	1.969706845890,	1.917352646550,	1.913833076780,	1.929450032580,	1.955578996480,	1.918397590590,
    1.928669425100,	1.960767759930,	1.958049896720,	1.927431440230,	1.939622601790,	1.988305037890,	1.969151998000,	1.969653344450,
    0.800896571678,	0.487099501418,	0.814150974546,	0.490928142697,	0.784970551647,	0.486158935709,	0.783503807659,	0.483423988004,
    0.757375708366,	0.496961119321,	0.806021529207,	0.500804844907,	0.793005735327,	0.494395656889,	0.774392144790,	0.470892645040,
    0.801067535052,	0.488226660695,	0.842021701101,	0.484125537629,	0.709889781601,	0.485499958055,	0.692294205630,	0.482375757873,
    0.822716460620,	0.475156564812,	0.748419018422,	0.500327250274,	0.764656020831,	0.512041915151,	0.790298575693,	0.479945040747};
static double shift_de_ring[NUM_SI_DE_RING] = {
    149.327294032,	152.580881931,	149.867455993,	269.265482626,	265.539401586,	147.95796958,	142.172950490,	142.709080851,
    144.246508167,	138.973374246,	140.363640468,	140.385937986,	129.557129459,	143.062851372,	131.801565474,	146.500427294,
    173.565397307,	175.731732592,	172.716760891,	172.491752385,	167.251144819,	170.602215561,	165.835452191,	168.599277919,
    737.864135469,	163.915648047,	158.067411423,	167.314894707,	158.333057699,	160.045352892,	152.835502093,	628.848345497,
    153.310294679,	156.822162623,	150.767058434,	149.621599273,	911.732799344,	151.400395741,	149.591292357,	144.038564396,
    144.481138223,	568.551815758,	371.417307599,	144.995204431,	373.123665719,	141.484046146,	138.653722372,	137.009020692};

static double gain_de_sect[NUM_SI_DE_SECT] = {
    2.217464,	2.223131,	2.191638,	2.253810,	1.986610,	2.012854,	1.855940,	1.935060,
    1.893695,	1.791707,	1.927650,	1.891382,	2.104209,	1.957175,	2.245551,	2.324817};
static double shift_de_sect[NUM_SI_DE_SECT] = {
    -183.177114,	-160.785636,	-184.383246,	-219.802098,	-27.131935,	-44.305029,	-53.59617,	-108.952255,
    -93.117626,	35.42837309,	-8.624059,	-52.083574,	-154.007666,	64.333827,	-183.060563,	-351.885766};

static double gain_e[NUM_SI_E_DET] = {
    2.3046448600,   2.77444486785,	2.72122708980, 	2.69128936778,	2.71962254892,	2.74162376399,	2.75803326366,	2.71911276403,
    2.7880329599,	2.75374065208,	2.73251871636,	2.69361141339,	2.78490497062,	2.70462555204,	2.75669077904,	2.74493030682};
static double shift_e[NUM_SI_E_DET] = {
    2645.16135000,	150.124886446,	150.286646252,	147.998651892,	153.907183610,	171.815558362,	159.058541061,	158.243823384,
    158.349829867,	155.633017400,	168.706496987,	165.206755975,	166.710546253,	166.904345054,	156.500918683,	163.624161882};

static double shift_t_labr3x8[NUM_LABR_3X8_DETECTORS] = {-45.5, -39.5};
static double shift_t_labr2x2_fs[NUM_LABR_2X2_DETECTORS] = {0.000, -1.84708, -2.14505, -16.4156, -22.3857, -2.10594};
static double shift_t_labr2x2_ss[NUM_LABR_2X2_DETECTORS] = {-163.667, -166.404,	-62.5705, -181.022, -174.5, -176.636};

static double shift_t_clover[NUM_CLOVER_DETECTORS][NUM_CLOVER_CRYSTALS] = {
    {-535.846,	-538.616,	-509.510,	-531.509},
    {-569.531,	-584.247,	-562.057,	-554.698},
    {-532.845,	-530.896,	-520.164,	-538.613},
    {-543.447,	-536.339,	-545.352,	-515.460},
    {-530.627,	-541.043,	-531.836,	-549.665},
    {-531.821,	-537.466,	-545.932,	-535.603},
    {-0.00000,	-0.00000,	-0.00000,	-0.00000},
    {-541.746,	-543.434,	-534.803,	-528.564},
    {-486.948,	-484.200,	-476.324,	-480.749},
    {-0.00000,	-0.00000,	-0.00000,	-0.00000}};

static double shift_t_de_ring[NUM_SI_DE_RING] = {
    -190.266,	-190.795,	-190.501,	-190.328,	-190.768,	-190.626,	-190.916,	-192.510,
    -190.808,	-190.383,	-189.306,	-191.137,	-190.145,	-190.303,	-190.132,	-190.439,
    -192.927,	-236.192,	-193.325,	-232.990,	-194.144,	-231.995,	-194.522,	-232.593,
    -193.437,	-232.054,	-194.219,	-233.491,	-194.242,	-231.763,	-200.022,	-231.886,
    -195.280,	-231.897,	-195.727,	-232.515,	-193.532,	-233.193,	-196.241,	-233.038,
    -202.351,	-232.545,	-193.816,	-233.568,	-234.500,	-234.035,	-195.526,	-233.976};

static double shift_t_de_sect[NUM_SI_DE_SECT] = {
    -189.045,	-188.668,	-189.095,	-188.855,	-188.916,	-190.939,	-195.419,	-189.538,
    -190.075,	-190.626,	-189.183,	-189.411,	-190.932,	-190.930,	-188.426,	-188.195};

static double shift_t_e[NUM_SI_E_DET] = {
    -261.175,	-260.892,	-260.827,	-261.034,	-259.259,	-259.522,	-261.394,	-259.957,
    -260.040,	-266.265,	-259.761,	-265.653,	-260.488,	-264.456,	-260.842,	-266.389};

static double clover_addback_gate[2] = {-75.0, 75.0};

double CalibrateEnergy(const word_t &detector)
{
    DetectorInfo_t dinfo = GetDetector(detector.address);
    switch (dinfo.type) {
        case labr_3x8 :
            return gain_labr3x8[dinfo.detectorNum]*detector.adcdata + shift_labr3x8[dinfo.detectorNum];
        case labr_2x2_ss :
            return gain_labr2x2_ss[dinfo.detectorNum]*detector.adcdata + shift_labr2x2_ss[dinfo.detectorNum];
        case labr_2x2_fs :
            return gain_labr2x2_fs[dinfo.detectorNum]*detector.adcdata + shift_labr2x2_fs[dinfo.detectorNum];
        case clover :
            return gain_clover[dinfo.detectorNum][dinfo.telNum]*detector.adcdata + shift_clover[dinfo.detectorNum][dinfo.telNum];
        case deDet_ring :
            return gain_de_ring[dinfo.detectorNum]*detector.adcdata + shift_de_ring[dinfo.detectorNum];
        case deDet_sect :
            return gain_de_sect[dinfo.detectorNum]*detector.adcdata + shift_de_sect[dinfo.detectorNum];
        case eDet :
            return gain_e[dinfo.detectorNum]*detector.adcdata + shift_e[dinfo.detectorNum];
        default :
            return detector.adcdata;
    }
}

double CalibrateTime(const word_t &detector)
{
    DetectorInfo_t dinfo = GetDetector(detector.address);
    switch (dinfo.type) {
        case labr_3x8 :
            return detector.cfdcorr + shift_t_labr3x8[dinfo.detectorNum];
        case labr_2x2_ss :
            return detector.cfdcorr + shift_t_labr2x2_ss[dinfo.detectorNum];
        case labr_2x2_fs :
            return detector.cfdcorr + shift_t_labr2x2_fs[dinfo.detectorNum];
        case clover :
            return detector.cfdcorr + shift_t_clover[dinfo.detectorNum][dinfo.telNum];
        case deDet_ring :
            return detector.cfdcorr + shift_t_de_ring[dinfo.detectorNum];
        case deDet_sect :
            return detector.cfdcorr + shift_t_de_sect[dinfo.detectorNum];
        case eDet :
            return detector.cfdcorr + shift_t_e[dinfo.detectorNum];
        default :
            return detector.cfdcorr;
    }
}

bool CheckTimeGateAddback(const double &timediff)
{
   return (timediff >= clover_addback_gate[0] && timediff <= clover_addback_gate[1]) ? true : false;
}
