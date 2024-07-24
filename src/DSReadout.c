#include "DSReadout.h"
#include "DSFunc.h"
#include "TROOT.h"
#include "TSystem.h"
#include <TGMsgBox.h>

extern DigitizerConfig_t Dcfg;
extern Histograms_t Histo;
extern ReadoutConfig_t Rcfg;

void CalcRate(uint64_t &PrevRateTime){
	char CName[100];
	uint64_t CurrentTime, ElapsedTime, DAQTime;
	
	CurrentTime = get_time( );
    ElapsedTime = CurrentTime - PrevRateTime;
	DAQTime = (CurrentTime - Rcfg.StartTime) / 1000;
			
		if (Rcfg.fTimer && DAQTime > Rcfg.timer_val)
			Rcfg.loop = 0;
					        	
		if (ElapsedTime > 1000) { 
			sprintf(CName,"T: %li s",  DAQTime );
			Rcfg.TLabel->SetText(CName);
			gSystem->ProcessEvents(); 
				
           	if (Rcfg.TrgCnt != 0){
				sprintf(CName," TrgCnt. %.2f Hz ", (float)Rcfg.TrgCnt*1000.f/(float)ElapsedTime );
				Rcfg.StatusBar->SetText(CName, 0);
			}
			else{
				sprintf(CName, "No data...");
				Rcfg.StatusBar->SetText(CName, 0);
			}
			
			//for (int i=0; i<4; i++){
			//	sprintf(CName, "Tr[%i] %f", i, Histo.trace[i]->GetBinContent(1366) );
			//	Rcfg.StatusBar->SetText(CName, i+1);
			//}
			
			Rcfg.TrgCnt = 0;
	                       		
        PrevRateTime = CurrentTime;
	   	}
	
}



void InitHisto( ){
	char CName[30];
	Histo.PSD_BIN = 2;
	sprintf(Histo.h2Style, "%s", (char*)"");
	Histo.k = 1;
	Histo.b = 0;
	sprintf(Histo.fFormula, "%s", (char*)"            Hr wll b smthng         ");
	Histo.fCalib = false;
	
	Histo.WF_XMIN = 0; Histo.WF_XMAX = Dcfg.ns * b_width;  
	Histo.WF_YMIN = -500; Histo.WF_YMAX = 10000;
	
	Histo.ALBound = 0, Histo.ARBound = Dcfg.ns * b_width; //WDcfg.RecordLength * b_width;
	Histo.ILBound = 0, Histo.IRBound = Dcfg.ns * b_width; //WDcfg.RecordLength * b_width;
	
	Histo.NPad = 1;
	Histo.BL_CUT = 20;
	Histo.fBL = true;
	Histo.fTrace = true;
	
	sprintf(Histo.h2Style, "COLZ");
	
	for (int i = 0; i < 4; i++){ //4 trace types AT1 AT2 DT1 DT2
		Histo.fDraw[i] = true;
		sprintf(CName, "h_trace%i", i);
		Histo.trace[i]= new TH1D(CName, CName, Dcfg.ns, 0, Dcfg.ns * b_width); // ns
	}
	
	
	Histo.ampl = new TH1D("h_ampl", "h_ampl", 16384, 0, 16384); // 2^14 bit ADC 
	Histo.detH = new TH1D("h_detH", "h_detH", 16384, 0, 16384); // 2^14 bit Histo 
	Histo.detHE = new TH1D("h_detHE", "h_detHE", 16384, 0, 2500); // 2.5 MeV I hope enough // xmax shold be recalculated every time when you do calibration
	Histo.integral = new TH1D("h_integral", "h_integral", 10000, 0, 20000000); // 1000 0 20000
	
	
	Histo.psd_ampl = new TH2D("h_psd_ampl", "h_psd_ampl", 1000, 0, 16384, 1000, -2, 2);
	Histo.int_ampl = new TH2D("h_int_ampl", "h_int_ampl", 1000, 0, 16384, 1000, 0, 20000000);
	
	//Histo.psd_ampl = new TH2D("h_psd_ampl", "h_psd_ampl", Histo.psd_ampl_set.xbins, Histo.psd_ampl_set.xmin, Histo.psd_ampl_set.xmax, // 1000 0 2000
	//						  							  Histo.psd_ampl_set.ybins, Histo.psd_ampl_set.ymin, Histo.psd_ampl_set.ymax); // 1000 -2 2
	
	//Histo.int_ampl = new TH2D("h_int_ampl", "h_int_ampl", Histo.ampl_set.xbins, Histo.ampl_set.xmin, Histo.ampl_set.xmax, // 1000 0 2000
	//						  							  Histo.integral_set.xbins, Histo.integral_set.xmin, Histo.integral_set.xmax); // 1000 -2 2

}

void FillHisto(int16_t *AT1, int16_t *AT2, uint8_t *DT1, uint8_t *DT2, uint32_t ns){ 
	
	Double_t BL_mean = 0, integral = 0;
	Int_t ampl = 0, m_stamp;
	Double_t psd_val =0, psd_sum1 = 0, psd_sum2 = 0;
	Int_t p = Dcfg.DParams.PulsePolarity[0] == CAENDPP_PulsePolarityPositive ? 1 : -1;  
	
	int16_t Arr_bl[ns];
		
	for (int j = 0; j<4; j++)
		Histo.trace[j]->Reset("ICESM");
					
	for (int i = 0; i<ns; i++){ //number of samples 
		//Histo.trace[0]->Fill(i, AT1[i]);
		Histo.trace[1]->Fill(i * b_width, AT2[i]);
		Histo.trace[2]->Fill(i * b_width, DT1[i] * 2000); // to be able to see that signals without zooming
		Histo.trace[3]->Fill(i * b_width, DT2[i] * 2000); // to be able to see that signals without zooming
	}
			
	for (int i=0; i<ns; i++){
		Arr_bl[i] = AT1[i];
		if (i<Histo.BL_CUT)
			BL_mean += AT1[i];	
	}
	BL_mean /= Histo.BL_CUT;	
			
	for ( int i=0; i<ns; i++){
		Arr_bl[i] -= BL_mean;
			
		if (Arr_bl[i] * p > ampl && i * b_width> Histo.ALBound && i * b_width < Histo.ARBound){ //* b_width
			ampl = Arr_bl[i] * p;
			m_stamp = i;
		}
			
		if (i * b_width> Histo.ILBound && i * b_width< Histo.IRBound) //* b_width
			integral += Arr_bl[i] * p;
				
		Histo.trace[0]->Fill( i * b_width, Histo.fBL ? Arr_bl[i] : AT1[i]); //* b_width
	}
			
	//PSD part
	
	if (Histo.fPSD_ampl){
		for (int j=m_stamp; j<ns; j++){
			if (j<(m_stamp + Histo.PSD_BIN) )
				psd_sum1 = psd_sum1 + p * Arr_bl[j];
			if (j>(m_stamp + Histo.PSD_BIN) )
				psd_sum2 = psd_sum2 + p * Arr_bl[j];
		}
		psd_val = 1 - (psd_sum1/(psd_sum1 + psd_sum2));
		Histo.psd_ampl->Fill(ampl, psd_val);	
	}
			
	Histo.integral->Fill(integral);
	Histo.ampl->Fill(ampl);
	Histo.int_ampl->Fill(ampl, integral);
		
					
	if (Rcfg.fPrint)
		printf(" Ampl(%i) Int(%6.0f)  \n", ampl, integral);
	
	
	//if (Rcfg.fStoreTraces){
	//	Rcfg.tree->Fill();	
	//	Histo.v_out.clear();
	//}	
}

void DrawHisto(TCanvas *c1){
	Color_t color[4] = {kBlack, kBlue, kRed, kGreen}; 	
	
	int FirstToDraw = 0;
	
	if (Histo.fTrace){
		c1->cd(1);
		for (int j = 0; j<4; j++){
			if (Histo.fDraw[j]){
				Histo.trace[j]->SetLineColor(color[j]);
				if (j == Histo.FirstToDraw){
					Histo.trace[j]->Draw("HIST");
					Histo.trace[j]->GetYaxis()->SetRangeUser(Histo.WF_YMIN, Histo.WF_YMAX);
					Histo.trace[j]->GetYaxis()->SetTitleOffset(1.1);
					Histo.trace[j]->GetYaxis()->SetTitle("ADC, lsb");
					Histo.trace[j]->GetXaxis()->SetRangeUser(Histo.WF_XMIN, Histo.WF_XMAX);
					Histo.trace[j]->GetXaxis()->SetTitle(b_width == 1 ? " Bins, b_width = 6.4 ns" : " Time, ns"); // Time, ns
				}	
				else 
					Histo.trace[j]->Draw("HIST SAME");
			}
		}
	}
	
	if (Histo.fAmpl){
		c1->cd(Histo.cAmpl);
		Histo.ampl->SetLineColor(kBlue);
		Histo.ampl->Draw("HIST");
	}
		
	if (Histo.fDetH){
		c1->cd(Histo.cDetH);
		if (!Histo.fCalib){
			Histo.detH->SetLineColor(kBlue);
			Histo.detH->Draw("HIST");
			Histo.detH->GetXaxis( )->SetTitle("  Channels, N"); 
		}
		else{
			Histo.detHE->Reset("ICESM");
			for (int i = 0; i<Histo.detH->GetNbinsX( ); i++ )
				Histo.detHE->Fill( Histo.k * i + Histo.b, Histo.detH->GetBinContent(i) );
			Histo.detHE->SetLineColor(kBlue);
			Histo.detHE->Draw("HIST");
			Histo.detHE->GetXaxis( )->SetTitle(" Energy, keV "); 
		}
	}
		
	if (Histo.fInt){		
		c1->cd(Histo.cInt);
		Histo.integral->SetLineColor(kBlue);
		Histo.integral->Draw("HIST");
	}
		
	if (Histo.fIA){
		c1->cd(Histo.cIA);
		Histo.int_ampl->SetMarkerStyle(21);
		Histo.int_ampl->SetMarkerSize(0.4);
		Histo.int_ampl->SetMarkerColor(kBlue);
		Histo.int_ampl->Draw(Histo.h2Style);
	}		
	
	if (Histo.fPSD_ampl){
		c1->cd(Histo.cPSD_ampl);
		Histo.psd_ampl->SetMarkerStyle(21);
		Histo.psd_ampl->SetMarkerSize(0.4);
		Histo.psd_ampl->SetMarkerColor(kBlue);
		Histo.psd_ampl->Draw(Histo.h2Style); //COLZ
	}		
		
	c1->Update();
	
}


int32_t ReadoutLoop(int handle){
	char CName[100];
	
	int32_t ret = 0;
	int32_t h_index, h_nbins, d32;
	uint32_t u32;
	
	uint64_t realTime, deadTime;
	uint64_t PrevRateTime = get_time( ), PrevDrawTime;
	CAENDPP_AcqStatus_t acqStatus;
	
	Rcfg.Nev = 0;
	Rcfg.TrgCnt = 0;
	
	//int16_t *AT1 = NULL, *AT2 = NULL;
	//uint8_t *DT1 = NULL, *DT2 = NULL;
	//	AT1 = (int16_t *)realloc(AT1, Dcfg.DParams.WFParams.recordLength*sizeof(int16_t));
	//	AT2 = (int16_t *)realloc(AT2, Dcfg.DParams.WFParams.recordLength*sizeof(int16_t));
	//	DT1 = (uint8_t *)realloc(DT1, Dcfg.DParams.WFParams.recordLength*sizeof(uint8_t));
	//	DT2 = (uint8_t *)realloc(DT2, Dcfg.DParams.WFParams.recordLength*sizeof(uint8_t));	
	
	int16_t	*AT1 = (int16_t *)realloc(AT1, Dcfg.DParams.WFParams.recordLength*sizeof(int16_t));
	int16_t	*AT2 = (int16_t *)realloc(AT2, Dcfg.DParams.WFParams.recordLength*sizeof(int16_t));
	uint8_t	*DT1 = (uint8_t *)realloc(DT1, Dcfg.DParams.WFParams.recordLength*sizeof(uint8_t));
	uint8_t	*DT2 = (uint8_t *)realloc(DT2, Dcfg.DParams.WFParams.recordLength*sizeof(uint8_t));
		
		
	uint32_t *h1 = (uint32_t *)calloc(MAX_HISTO_NBINS, sizeof(uint32_t));
		
	
	while(Rcfg.loop == 1 && !ret) {
						
		uint32_t ns = 0;
		double tsamples;	
	
		int ch = 0;
				
								//_GetWaveform(handle, ch, 1, AT1, AT2, DT1, DT2, &ns, &tsamples
		ret = CAENDPP_GetWaveform(handle, ch, 0, AT1, AT2, DT1, DT2, &ns, &tsamples);
		if (ret)
			sprintf(CName, "Can not read waveform ret = %i \n", ret);
		
		// Calculate throughput and trigger rate (every second) 		
		CalcRate(PrevRateTime);
		
		//Rcfg.Nev++;	 // should be checked by Peaking or Triggering in DT1 || DT2
		for (int i = 0; i < ns; i++)
			if (DT1[i]>0)
				Rcfg.TrgCnt++;
	
		gSystem->ProcessEvents( ); 
		
		if (ns>0)	{
			Rcfg.Nev++;	
			Rcfg.TrgCnt++;
				
			FillHisto(AT1, AT2, DT1, DT2, ns);	
		
			if (Rcfg.fPrint)
				printf("Evt [%i] H_AT1[%5.0f] ns [%i] tsamples[%2.2f] \n", Rcfg.Nev, Histo.trace[0]->GetBinContent(1366), ns, tsamples);
			
				
			if ( (get_time( ) - PrevDrawTime) >= Rcfg.DrawTime*1000){ // sec*1000 = ms  
				
				//ret = CAENDPP_GetTotalNumberOfHistograms(handle, 0, &d32);
				//printf("(ret = %i) Total number of histograms [%i] \n", ret, d32);
				
				ret = CAENDPP_GetCurrentHistogram(handle, 0, h1, &u32, &realTime, &deadTime, &acqStatus);
				if (ret)
					sprintf(CName, "Can not get current histogram ret = %i \n", ret);
								
				ret = CAENDPP_GetCurrentHistogramIndex(handle, 0, &h_index);
				ret = CAENDPP_GetHistogramSize(handle, 0, h_index, &h_nbins);
				if (Rcfg.fPrint)
					printf("Histo (%i bins) RealTime [%.3f s] DeadTime [%.0f ms] Good (%i CNT) \n", h_nbins, realTime / 1000000000.0, deadTime / 1000000.0, u32);
				
				float real_s = realTime / 1000000000.0;
				float dead_s = deadTime / 1000000000.0;
				
				sprintf(CName, "RealTime %.3f s", real_s);
				Rcfg.StatusBar->SetText(CName, 1);
				sprintf(CName, "DeadTime %.3f ms ", deadTime / 1000000.0);
				Rcfg.StatusBar->SetText(CName, 2);
				sprintf(CName, "Good (%i CNT)", u32);
				Rcfg.StatusBar->SetText(CName, 3);
				sprintf(CName, "Count Rate: %0.2f Hz", u32 / real_s);
				Rcfg.StatusBar->SetText(CName, 4);
				
				float dead_proc = (dead_s / (real_s + dead_s));
				sprintf(CName, "DeadTime : %0.2f %%", dead_proc * 100);
				Rcfg.StatusBar->SetText(CName, 5);
				
				Histo.detH->Reset("ICESM");				
				for (int n = 0; n< h_nbins; n++)
					Histo.detH->Fill(n , h1[n]);				
								
				DrawHisto(Rcfg.can);
				//Rcfg.can->Update(); // some shit happends on that step
				PrevDrawTime = get_time( );
			}	
		
		} // ns > 0 // number of samples | trace empty or not
				
    } // end of while readout loop		
				
	if(ret){
		printf("Error: %s\n", CName);
		new TGMsgBox(gClient->GetRoot(), Rcfg.main, "Error", CName, kMBIconStop, kMBOk);
	}
	
	return ret;
}


int32_t DataAcquisition(int handle){
	int32_t ret = 0;
	char CName[30];
	
	while(1) {
		gSystem->ProcessEvents(); 
		if (Rcfg.loop == 1){
			ret = CAENDPP_StartAcquisition(handle, 0); 
			
			if (ret) {
				sprintf(CName, "ERR_START_ACQUISITION %i \n", ret);
				new TGMsgBox(gClient->GetRoot(), Rcfg.main, "Error", CName, kMBIconStop, kMBOk);
				return ret;
			}
			ret = ReadoutLoop(handle);
		}
		
		if (Rcfg.loop == 0){
			ret = CAENDPP_StopAcquisition(handle, 0); 
			printf("Acquisition stopped \n");
			Rcfg.loop = -1;
		}
	}
	return ret;
}