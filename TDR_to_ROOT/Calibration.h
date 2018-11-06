#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <FileReader.h>

extern double CalibrateEnergy(const word_t &detector);

extern double CalibrateTime(const word_t &detector);

extern bool CheckTimeGateAddback(const double &timediff);


#endif // CALIBRATION_H
