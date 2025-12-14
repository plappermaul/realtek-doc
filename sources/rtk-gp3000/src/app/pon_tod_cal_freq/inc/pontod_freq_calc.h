#ifndef PONTOD_FREQ_CALC_H
#define PONTOD_FREQ_CALC_H

void pontod_freq_cal_ratio(PonToDNLData_t* item);
void pontod_freq_cal_intr_ratio(PonToDNLData_t* item);
void pontod_freq_cal_auto_ratio(PonToDNLData_t* item);

void pontod_freq_set_cal_ratio(void);
void pontod_freq_set_cal_reinit(void);

#endif
