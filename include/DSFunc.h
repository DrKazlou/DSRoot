#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGLabel.h"
#include "TTree.h"
#include "TFile.h"
#include "TGStatusBar.h"
#include "TGWindow.h"
#include "TGFrame.h"

#include "CAENDPPLib.h"
#include "CAENDPPLibTypes.h"

// The maximum number of bits for the histograms
// 14 bits ->from bin 0 to bin 16383 (2^14-1)
#define MAX_HISTOBITS (14)

// The maximum number of bins of the histograms
// (depending on their maximum number of bits)
#define MAX_HISTO_NBINS (0x1 << (MAX_HISTOBITS-0))

#define AUTOSET_VALUE       (-1)    // The value used for Autoset of parameters

#define b_width 1    // 1 bin = 6.4 ns due to 150 MS/s sampling rate

typedef struct
{	
	bool fInit, fPrint, fTimer, fStoreTraces;
	
	int timer_val;

	short int port;
		
	int loop;
	TH1D *ampl;
	
	int Nev, TrgCnt;

	uint64_t StartTime;
	double DrawTime;
	
	TGLabel  *TLabel;
	TGStatusBar *StatusBar;
	TCanvas *can;

	TGMainFrame *main;
	TTree *tree;
	TFile *ff;
	
} ReadoutConfig_t;

typedef struct{
	
	CAENDPP_ConnectionParams_t connParam;
	CAENDPP_ConnectionType type;
	CAENDPP_AcqMode_t acqMode;
	CAENDPP_DgtzParams_t DParams;
	CAENDPP_InputRange_t InputRange;
	
	uint32_t ns; // number of samples
	//double temp;
	int32_t BID; // board ID
	
	
	CAENDPP_Info_t info;

}DigitizerConfig_t;

typedef struct{
	
	int xbins, xmin, xmax;
	int ybins, ymin, ymax;
	
}hist_settings_t;


typedef struct{
	
	
	int WF_XMIN, WF_XMAX, WF_YMIN, WF_YMAX;
	int ALBound, ARBound, ILBound, IRBound;
	
	int PSD_BIN;
	int BL_CUT;
	
	char h2Style[10];	
	int FirstToDraw;	
	
	int NPad;	
	
	double k, b, arr_calib[2][10]; // calibration fit parameters
	char fFormula[40];
	bool fCalib;
	
	int cAmpl, cDetH, cInt, cPSD_ampl, cPSD_int, cQsl, cIA;	
	bool fDraw[4], fBL, fTrace, fAmpl, fDetH, fInt, fdT, fPSD_ampl, fPSD_int, fQsl, fIA;  // flags for every time of histograms;
	
	TH1D *trace[4], *ampl, *detH, *detHE, *integral;
	TH2D *int_ampl, *psd_ampl, *psd_int, *qs_ql; 
			
	hist_settings_t ampl_set, integral_set, dt_set, psd_ampl_set;	
	
} Histograms_t;

long get_time();

int32_t DefineAutosetParameters(CAENDPP_DgtzParams_t *Params);

void InitReadoutConfig( ReadoutConfig_t *Rcfg );

int32_t InitDgtzParams(CAENDPP_DgtzParams_t *Params);

int32_t InitDgtzStDParams(CAENDPP_DgtzParams_t *Params);
	
int32_t InitialiseDigitizer(int32_t &handle, ReadoutConfig_t *Rcfg, DigitizerConfig_t *Dcfg);





