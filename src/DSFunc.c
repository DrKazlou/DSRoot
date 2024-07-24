#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <sys/types.h>
#include <unistd.h>

#include "DSFunc.h"

#include <TSystem.h>
#include "TH1D.h"
#include "TROOT.h"
#include "TGMsgBox.h"

#include "CAENDPPLib.h"
#include "CAENDPPLibTypes.h"

extern ReadoutConfig_t Rcfg;

long get_time()
{
    long time_ms;
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;

    return time_ms;
}

int32_t DefineAutosetParameters(CAENDPP_DgtzParams_t *Params) {
    int32_t ch;

    for (ch = 0; ch < 1; ch++) { //single channel for DT5770
        // Channel Parameters
        Params->DCoffset[ch] = AUTOSET_VALUE; // DC Offset (DAC Offset)
        Params->PulsePolarity[ch] = (CAENDPP_PulsePolarity_t)(AUTOSET_VALUE); // Pulse Polarity
        
        // DPP - PHA Parameters
        Params->DPPParams.M[ch] = AUTOSET_VALUE; // Signal Decay Time Constant
        Params->DPPParams.m[ch] = AUTOSET_VALUE; // Trapezoid Flat Top
        Params->DPPParams.k[ch] = AUTOSET_VALUE; // Trapezoid Rise Time
        Params->DPPParams.ftd[ch] = AUTOSET_VALUE; // Flat Top Delay
        Params->DPPParams.a[ch] = AUTOSET_VALUE; // Trigger Filter smoothing factor
        Params->DPPParams.b[ch] = AUTOSET_VALUE; // Input Signal Rise time
        Params->DPPParams.thr[ch] = AUTOSET_VALUE; // Trigger Threshold
        Params->DPPParams.nsbl[ch] = AUTOSET_VALUE; // Number of Samples for Baseline Mean
        Params->DPPParams.nspk[ch] = AUTOSET_VALUE; // Number of Samples for Peak Mean Calculation
        Params->DPPParams.pkho[ch] = AUTOSET_VALUE; // Peak Hold Off
        Params->DPPParams.blho[ch] = AUTOSET_VALUE; // Base Line Hold Off
        Params->DPPParams.trgho[ch] = AUTOSET_VALUE; // Trigger Hold Off
        Params->DPPParams.dgain[ch] = AUTOSET_VALUE; // Digital Probe Gain
        Params->DPPParams.enf[ch] = AUTOSET_VALUE; // Energy Nomralization Factor
        Params->DPPParams.decimation[ch] = AUTOSET_VALUE; // Decimation of Input Signal
    }
    return 0;
}

void InitReadoutConfig(ReadoutConfig_t *Rcfg){
	
	Rcfg->fPrint = false; // common print flag for debug
	Rcfg->fStoreTraces = false; // flag to store traces
	Rcfg->fTimer = false; // flag for timer usage
	
	Rcfg->loop = -1; // ReadoutLoop flag
	Rcfg->DrawTime = 0.5; // time between histograms drawing in sec
	Rcfg->TrgCnt = 0;
	
}


int32_t InitDgtzParams(CAENDPP_DgtzParams_t *Params) {
    int32_t ch;

    // set listMode default parameters
    Params->ListParams.enabled = FALSE;
    Params->ListParams.saveMode = CAENDPP_ListSaveMode_FileBinary;
    strcpy(Params->ListParams.fileName, "UNNAMED");
    Params->ListParams.maxBuffNumEvents = 0;
    Params->ListParams.saveMask = 0xF;

    // default board parameters
    Params->ChannelMask = 0; // it will be filled later
    Params->EventAggr = 0;
    Params->IOlev = CAENDPP_IOLevel_NIM;
    
    // Generic Writes to Registers
    Params->GWn = 0;                                    // Number of Generic Writes
    memset(Params->GWaddr, 0, MAX_GW*sizeof(uint32_t)); // List of addresses (length = 'GWn')
    memset(Params->GWdata, 0, MAX_GW*sizeof(uint32_t)); // List of datas (length = 'GWn')
    memset(Params->GWmask, 0, MAX_GW*sizeof(uint32_t)); // List of masks (length = 'GWn')

    // Waveform parameters default settings
    Params->WFParams.dualTraceMode = 1;
    Params->WFParams.vp1 = CAENDPP_PHA_VIRTUALPROBE1_Input;
    Params->WFParams.vp2 = CAENDPP_PHA_VIRTUALPROBE2_TrapBLCorr;
    Params->WFParams.dp1 = CAENDPP_PHA_DigitalProbe1_Peaking;
    Params->WFParams.dp2 = CAENDPP_PHA_DigitalProbe2_Trigger;
    Params->WFParams.recordLength = (int32_t)4096; // (8192);
    Params->WFParams.preTrigger = (int32_t)(2000);
    Params->WFParams.probeSelfTriggerVal = 150;
    Params->WFParams.probeTrigger = CAENDPP_PHA_PROBETRIGGER_MainTrig;
    
    // Channel parameters
    for (ch = 0; ch < 1; ch++) { // MAX_BOARD_CHNUM
        // Channel parameters
        //Params->DCoffset[ch] = 7864;
        Params->DCoffset[ch] = 5000;
        Params->PulsePolarity[ch] = CAENDPP_PulsePolarityPositive;
		//Params->PulsePolarity[ch] = CAENDPP_PulsePolarityNegative;
        
        // Coicidence parameters between channels
        Params->CoincParams[ch].CoincChMask = 0x0;
        Params->CoincParams[ch].CoincLogic = CAENDPP_CoincLogic_None;
        Params->CoincParams[ch].CoincOp = CAENDPP_CoincOp_OR;
        Params->CoincParams[ch].MajLevel = 0;
        Params->CoincParams[ch].TrgWin = 0;

        // DPP Parameters
        Params->DPPParams.M[ch] = 2000;        // Signal Decay Time Constant
        Params->DPPParams.m[ch] = 600;         // Trapezoid Flat Top
        Params->DPPParams.k[ch] = 5000;         // Trapezoid Rise Time
        Params->DPPParams.ftd[ch] = 600;       // Flat Top Delay
        Params->DPPParams.a[ch] = 2;            // Trigger Filter smoothing factor
        Params->DPPParams.b[ch] = 1000;          // Input Signal Rise time
        Params->DPPParams.thr[ch] = 50;       // Trigger Threshold
        Params->DPPParams.nsbl[ch] = 30;         // Number of Samples for Baseline Mean
        Params->DPPParams.nspk[ch] = 1;         // Number of Samples for Peak Mean Calculation
        Params->DPPParams.pkho[ch] = 0;         // Peak Hold Off
        Params->DPPParams.blho[ch] = 1500;      // Base Line Hold Off
        Params->DPPParams.dgain[ch] = 1;        // Digital Probe Gain
        Params->DPPParams.enf[ch] = 1.0;        // Energy Nomralization Factor
        Params->DPPParams.decimation[ch] = 0;   // Decimation of Input Signal   
        Params->DPPParams.trgho[ch] = 1300;      // Trigger Hold Off

        // Reset Detector
        Params->ResetDetector[ch].Enabled = 0;
        Params->ResetDetector[ch].ResetDetectionMode =
            CAENDPP_ResetDetectionMode_Internal;
        Params->ResetDetector[ch].thrhold = 100;
        Params->ResetDetector[ch].reslenmin = 2;
        Params->ResetDetector[ch].reslength = 2000;

        // Parameters for X770
        Params->DPPParams.X770_extraparameters[ch].CRgain = 0;
        Params->DPPParams.X770_extraparameters[ch].InputImpedance = CAENDPP_InputImpedance_50O;
		//Params->DPPParams.X770_extraparameters[ch].InputImpedance = CAENDPP_InputImpedance_1K;
        Params->DPPParams.X770_extraparameters[ch].TRgain = 0;
        Params->DPPParams.X770_extraparameters[ch].SaturationHoldoff = 300; //300
		Params->DPPParams.X770_extraparameters[ch].energyFilterMode = 0;
		Params->DPPParams.X770_extraparameters[ch].trigK = 300;
		Params->DPPParams.X770_extraparameters[ch].trigm = 10;
		Params->DPPParams.X770_extraparameters[ch].trigMODE = 0;

        Params->SpectrumControl[ch].SpectrumMode = CAENDPP_SpectrumMode_Energy;
        Params->SpectrumControl[ch].TimeScale = 1;
    }
    return 0;
}

int32_t InitDgtzStDParams(CAENDPP_DgtzParams_t *Params) {
    int32_t ch;

    // set listMode default parameters
    Params->ListParams.enabled = FALSE;
    Params->ListParams.saveMode = CAENDPP_ListSaveMode_FileBinary;
    strcpy(Params->ListParams.fileName, "UNNAMED");
    Params->ListParams.maxBuffNumEvents = 0;
    Params->ListParams.saveMask = 0xF;

    // default board parameters
    Params->ChannelMask = 0; // it will be filled later
    Params->EventAggr = 0;
    Params->IOlev = CAENDPP_IOLevel_NIM;
    
    // Generic Writes to Registers
    Params->GWn = 0;                                    // Number of Generic Writes
    memset(Params->GWaddr, 0, MAX_GW*sizeof(uint32_t)); // List of addresses (length = 'GWn')
    memset(Params->GWdata, 0, MAX_GW*sizeof(uint32_t)); // List of datas (length = 'GWn')
    memset(Params->GWmask, 0, MAX_GW*sizeof(uint32_t)); // List of masks (length = 'GWn')

    // Waveform parameters default settings
    Params->WFParams.dualTraceMode = 1;
    Params->WFParams.vp1 = CAENDPP_PHA_VIRTUALPROBE1_Input;
    Params->WFParams.vp2 = CAENDPP_PHA_VIRTUALPROBE2_TrapBLCorr;
    Params->WFParams.dp1 = CAENDPP_PHA_DigitalProbe1_Peaking;
    Params->WFParams.dp2 = CAENDPP_PHA_DigitalProbe2_Trigger;
    Params->WFParams.recordLength = (int32_t)4096; // (8192);
    Params->WFParams.preTrigger = (int32_t)(100);
    Params->WFParams.probeSelfTriggerVal = 150;
    Params->WFParams.probeTrigger = CAENDPP_PHA_PROBETRIGGER_MainTrig;
    
    // Channel parameters
    for (ch = 0; ch < 1; ch++) { // MAX_BOARD_CHNUM
        // Channel parameters
        //Params->DCoffset[ch] = 7864;
        Params->DCoffset[ch] = 32767;
        //Params->PulsePolarity[ch] = CAENDPP_PulsePolarityPositive;
		Params->PulsePolarity[ch] = CAENDPP_PulsePolarityNegative;
        
        // Coicidence parameters between channels
        Params->CoincParams[ch].CoincChMask = 0x0;
        Params->CoincParams[ch].CoincLogic = CAENDPP_CoincLogic_None;
        Params->CoincParams[ch].CoincOp = CAENDPP_CoincOp_OR;
        Params->CoincParams[ch].MajLevel = 0;
        Params->CoincParams[ch].TrgWin = 0;

        // DPP Parameters
        Params->DPPParams.M[ch] = 50000;        // Signal Decay Time Constant
        Params->DPPParams.m[ch] = 1000;         // Trapezoid Flat Top
        Params->DPPParams.k[ch] = 3000;         // Trapezoid Rise Time
        Params->DPPParams.ftd[ch] = 800;       // Flat Top Delay
        Params->DPPParams.a[ch] = 4;            // Trigger Filter smoothing factor
        Params->DPPParams.b[ch] = 200;          // Input Signal Rise time
        Params->DPPParams.thr[ch] = 50;       // Trigger Threshold
        Params->DPPParams.nsbl[ch] = 3;         // Number of Samples for Baseline Mean
        Params->DPPParams.nspk[ch] = 0;         // Number of Samples for Peak Mean Calculation
        Params->DPPParams.pkho[ch] = 0;         // Peak Hold Off
        Params->DPPParams.blho[ch] = 1000;      // Base Line Hold Off
        Params->DPPParams.dgain[ch] = 0;        // Digital Probe Gain
        Params->DPPParams.enf[ch] = 1.0;        // Energy Nomralization Factor
        Params->DPPParams.decimation[ch] = 0;   // Decimation of Input Signal   
        Params->DPPParams.trgho[ch] = 1300;      // Trigger Hold Off

        // Reset Detector
        Params->ResetDetector[ch].Enabled = 0;
        Params->ResetDetector[ch].ResetDetectionMode =
            CAENDPP_ResetDetectionMode_Internal;
        Params->ResetDetector[ch].thrhold = 100;
        Params->ResetDetector[ch].reslenmin = 2;
        Params->ResetDetector[ch].reslength = 2000;

        // Parameters for X770
        Params->DPPParams.X770_extraparameters[ch].CRgain = 0;
        Params->DPPParams.X770_extraparameters[ch].InputImpedance = CAENDPP_InputImpedance_50O;
		//Params->DPPParams.X770_extraparameters[ch].InputImpedance = CAENDPP_InputImpedance_1K;
        Params->DPPParams.X770_extraparameters[ch].TRgain = 0;
        Params->DPPParams.X770_extraparameters[ch].SaturationHoldoff = 300; //300
		Params->DPPParams.X770_extraparameters[ch].energyFilterMode = 0;
		Params->DPPParams.X770_extraparameters[ch].trigK = 30;
		Params->DPPParams.X770_extraparameters[ch].trigm = 10;
		Params->DPPParams.X770_extraparameters[ch].trigMODE = 0;

        Params->SpectrumControl[ch].SpectrumMode = CAENDPP_SpectrumMode_Energy;
        Params->SpectrumControl[ch].TimeScale = 1;
    }
    return 0;
}


int32_t InitialiseDigitizer(int32_t &handle, ReadoutConfig_t *Rcfg, DigitizerConfig_t *Dcfg) {
	
	char CName[200];
		
	Dcfg->acqMode = CAENDPP_AcqMode_Waveform; // Acquisition Mode (Histogram or Waveform)
	
	Dcfg->InputRange = CAENDPP_InputRange_10_0Vpp;
	
	int32_t ret = InitDgtzParams(&Dcfg->DParams);
	
	Dcfg->DParams.ChannelMask = 0x1;

	
	ret = CAENDPP_InitLibrary(&handle); // The handle will be used to command the library
    if (ret){
		sprintf(CName, "Init Library Error  \n ret = %i \n", ret);
		new TGMsgBox(gClient->GetRoot(), Rcfg->main, "Error", CName, kMBIconStop, kMBOk);
		return ret;
	}	
		
	Dcfg->connParam.LinkType = CAENDPP_ETH;
    Dcfg->connParam.LinkNum = 0;
    Dcfg->connParam.ConetNode = 0;
    Dcfg->connParam.VMEBaseAddress = 0x0;
	sprintf(Dcfg->connParam.ETHAddress, "134.176.17.127");
	
	printf(" Connection: LinkType[%i]  LinkNum[%i] Addr (%s) \n", Dcfg->connParam.LinkType, Dcfg->connParam.LinkNum, Dcfg->connParam.ETHAddress);
	ret = CAENDPP_AddBoard(handle, Dcfg->connParam, &Dcfg->BID);
	if (ret){
		sprintf(CName, "Add Board Error  \n ret = %i \n", ret);
		new TGMsgBox(gClient->GetRoot(), Rcfg->main, "Error", CName, kMBIconStop, kMBOk);
		return ret;
	}	
			
	ret = CAENDPP_GetDPPInfo(handle, Dcfg->BID, &Dcfg->info);
	if (ret){
		sprintf(CName, "GET_DPPInfo Error \n ret = %i \n", ret);
		new TGMsgBox(gClient->GetRoot(), Rcfg->main, "Error", CName, kMBIconStop, kMBOk);
		return ret;
	}	
	
	sprintf(CName, "Board %s_%u added succesfully (ret = %i) \n", Dcfg->info.ModelName, Dcfg->info.SerialNumber, ret);
	new TGMsgBox(gClient->GetRoot(), Rcfg->main, "Info", CName, kMBIconAsterisk, kMBOk);
		
	ret = CAENDPP_SetBoardConfiguration(handle, Dcfg->BID, Dcfg->acqMode, Dcfg->DParams);
	
	printf("Board is configurated (ret = %i): \n", ret);
	
	ret = CAENDPP_GetBoardConfiguration(handle, Dcfg->BID, &Dcfg->acqMode, &Dcfg->DParams);
	
	printf("Board config (ret = %i): \n \t \t Polarity %i  \n \t \t DCOffset %i \t \t RecordLength %i \n", ret, Dcfg->DParams.PulsePolarity[0], Dcfg->DParams.DCoffset[0], Dcfg->DParams.WFParams.recordLength);
		
	ret = CAENDPP_SetInputRange(handle, 0, Dcfg->InputRange);
	
	printf("Input range (ret = %i): %i \n ", ret, Dcfg->InputRange);
	
	//Let's try to set auto config
			
	Rcfg->fInit = true;

	return ret;
}
