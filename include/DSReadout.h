#include "TH1D.h"
#include "TCanvas.h"
#include "TColor.h"

//#include <../include/CAENDigitizerType.h>
//#include "CAENDigitizer.h"


void CalcRate(uint64_t &PrevRateTime);

void InitHisto();
void FillHisto(int16_t *AT1, int16_t *AT2, uint8_t *DT1, uint8_t *DT2, uint32_t ns); 
void DrawHisto(TCanvas *c1);

int32_t ReadoutLoop(int handle);
int32_t DataAcquisition(int handle);