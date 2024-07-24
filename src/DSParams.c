#include "DSParams.h"
#include "DSFunc.h"

extern int32_t handle;
extern DigitizerConfig_t Dcfg;
extern ReadoutConfig_t Rcfg;

ParamsMenu::ParamsMenu(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, UInt_t options)
{
		
	char str[10];
	int j;

   fMain = new TGTransientFrame(p, main, w, h, options);
   fMain->Connect("CloseWindow()", "ParamsMenu", this, "CloseWindow()");
   fMain->DontCallClose(); // to avoid double deletions.

   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);

   fMain->ChangeOptions((fMain->GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

   f1 = new TGCompositeFrame(fMain, 400, 200, kHorizontalFrame); //| kFixedWidth
	
	//double Lmin[]	= {-10000, -50, -50,-50}; 	
	//double Lmax[]	= {10000, 10000, 10000,10000}; 	
	
	//Waveform Params	  
		
	TGGroupFrame *fGF_WF = new TGGroupFrame(f1, titlelabel[0], kVerticalFrame);
	fGF_WF->SetTextFont(gfFont);
	fGF_WF->SetTitlePos(TGGroupFrame::kCenter); 
	f1->AddFrame(fGF_WF, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));//
	
	//const Double_t wf_num[3] = { 8192, 100, 150}; // Dcfg.DParams.WFParams.recordLength , Dcfg.DParams.WFParams.preTrigger, Dcfg.DParams.WFParams.probeSelfTriggerVal
	const double wf_num[3] = { (double)Dcfg.DParams.WFParams.recordLength , (double)Dcfg.DParams.WFParams.preTrigger, (double)Dcfg.DParams.WFParams.probeSelfTriggerVal };
				
	for (int i = 0; i<8; i++)	{
		fHF_WF[i] = new TGHorizontalFrame(fGF_WF, 200, 30);
		fGF_WF->AddFrame(fHF_WF[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 4, 3));	
		
		fWFLabel[i] = new TGLabel(fHF_WF[i], wf_label[i]);
		fWFLabel[i]->SetTextFont(labelFont);
		fHF_WF[i]->AddFrame(fWFLabel[i], new TGLayoutHints(kLHintsCenterX, 0, 0, 4, 4));		
		// Virtual Probe comboboxies (type of traces to draw)
		if (i<4){
			fProbe[i] = new TGComboBox(fHF_WF[i], i);	// C0-3
			switch (i) { // change in future by 2 dim char vector
				case 0:
					for (int j = 0; j<(sizeof(List_vp1)/sizeof(*List_vp1) ); j++ )
						fProbe[i]->AddEntry(List_vp1[j], j);
					fProbe[i]->Select(Dcfg.DParams.WFParams.vp1); //0
					break;
				case 1:
					for (int j = 0; j<(sizeof(List_vp2)/sizeof(*List_vp2) ); j++ )
						fProbe[i]->AddEntry(List_vp2[j], j);
					fProbe[i]->Select(Dcfg.DParams.WFParams.vp2); //2
					break;
				case 2:
					for (int j = 0; j<(sizeof(List_dp1)/sizeof(*List_dp1) ); j++ )
						fProbe[i]->AddEntry(List_dp1[j], j);
					fProbe[i]->Select(Dcfg.DParams.WFParams.dp1); //4
					break;	
				case 3:
					for (int j = 0; j<(sizeof(List_dp2)/sizeof(*List_dp2) ); j++ )
						fProbe[i]->AddEntry(List_dp2[j], j);
					fProbe[i]->Select(Dcfg.DParams.WFParams.dp2); //0
					break;		
			}
		
			fProbe[i]->Resize(150, 20);
			fProbe[i]->Connect("Selected(Int_t)", "ParamsMenu", this, "DoComboBox()");
			fHF_WF[i]->AddFrame(fProbe[i], new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 2, 2, 2, 2));
		}			
	
	//recordLength & preTrigger numentry

		if (i == 4 || i == 5){
			fNumericEntries[0][i-4] = new TGNumberEntry(fHF_WF[i], wf_num[i-4], 8, i - 4, (TGNumberFormat::EStyle) 0); 
			fNumericEntries[0][i-4]->Connect("ValueSet(Long_t)", "ParamsMenu", this, "DoSetVal()");
			fNumericEntries[0][i-4]->Resize(55, fNumericEntries[0][i-4]->GetDefaultHeight( ) );
			fHF_WF[i]->AddFrame(fNumericEntries[0][i-4], new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 3, 3)); //kLHintsCenterY | kLHintsLeft  //new TGLayoutHints(0, 0, 0, 4, 4)
		}
		//probeTrigger combobox
		if (i == 6){
			fProbeTrigger = new TGComboBox(fHF_WF[i], 4); // C4
			for (int j = 0; j<(sizeof(List_probeTrigger)/sizeof(*List_probeTrigger) ); j++ )
				fProbeTrigger->AddEntry(List_probeTrigger[j], j);
			fProbeTrigger->Select(Dcfg.DParams.WFParams.probeTrigger);
			fProbeTrigger->Resize(150, 20);
			fProbeTrigger->Connect("Selected(Int_t)", "ParamsMenu", this, "DoComboBox()");
			fHF_WF[i]->AddFrame(fProbeTrigger, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 2, 2, 2, 2));
		}
		//probeSelfTrigger numentry
		if (i == 7){
			fNumericEntries[0][2] = new TGNumberEntry(fHF_WF[i], wf_num[2], 8, 2, (TGNumberFormat::EStyle) 0);  //N2
			fNumericEntries[0][2]->Connect("ValueSet(Long_t)", "ParamsMenu", this, "DoSetVal()");
			fNumericEntries[0][2]->Resize(55, fNumericEntries[0][2]->GetDefaultHeight( ) );
			fHF_WF[i]->AddFrame(fNumericEntries[0][2], new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 3, 3)); //kLHintsCenterY | kLHintsLeft  //new TGLayoutHints(0, 0, 0, 4, 4)
		}	
	}	
					
	fGF_WF->Resize( );			
	//fHF_WF[0]->Resize( );
		
	//Standart Params	  
		
	TGGroupFrame *fGF_StD = new TGGroupFrame(f1, titlelabel[1], kVerticalFrame);
	fGF_StD->SetTextFont(gfFont);
	fGF_StD->SetTitlePos(TGGroupFrame::kCenter); 
	f1->AddFrame(fGF_StD, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));//
	
	const Double_t std_num = Dcfg.DParams.DCoffset[0]; // 32767; // Dcfg.DParams.DCoffset[0]
		
	for (int i = 0; i<3; i++)	{
		fHF_StD[i] = new TGHorizontalFrame(fGF_StD, 200, 30);
		fGF_StD->AddFrame(fHF_StD[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 3, 3));	
		
		fStDLabel[i] = new TGLabel(fHF_StD[i], std_label[i]);
		fStDLabel[i]->SetTextFont(labelFont);
		fHF_StD[i]->AddFrame(fStDLabel[i], new TGLayoutHints(kLHintsCenterX, 0, 0, 4, 4));		
		
		switch (i){
		// DCOffset numentry
			case 0 :
				fNumericEntries[1][0] = new TGNumberEntry(fHF_StD[i], std_num, 8, 10+3, (TGNumberFormat::EStyle) 0);  // N3+10
				fNumericEntries[1][0]->Connect("ValueSet(Long_t)", "ParamsMenu", this, "DoSetVal()");
				fNumericEntries[1][0]->Resize(55, fNumericEntries[1][0]->GetDefaultHeight( ) );
				fHF_StD[i]->AddFrame(fNumericEntries[1][0], new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 5, 3)); //kLHintsCenterY | kLHintsLeft  //new TGLayoutHints(0, 0, 0, 4, 4)
				break;
		//PulsePolatiry combobox
		case 1 :
			fPolarity = new TGComboBox(fHF_StD[i], 5); // C5
			fPolarity->AddEntry("Positive", 0);
			fPolarity->AddEntry("Negative", 1);
			fPolarity->Select(Dcfg.DParams.PulsePolarity[0]);
			fPolarity->Resize(80, 20);
			fPolarity->Connect("Selected(Int_t)", "ParamsMenu", this, "DoComboBox()");
			fHF_StD[i]->AddFrame(fPolarity, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 2, 2, 4, 3));
			break;
		//InputRange combobox
		case 2 :
			fInputRange = new TGComboBox(fHF_StD[i], 6); // C6
			for (int j = 0; j<(sizeof(List_inputRange)/sizeof(*List_inputRange) ); j++ )
				fInputRange->AddEntry(List_inputRange[j], j);
			fInputRange->Select(3); // 10 Vpp
			fInputRange->Resize(80, 20);
			fInputRange->Connect("Selected(Int_t)", "ParamsMenu", this, "DoComboBox()");
			fHF_StD[i]->AddFrame(fInputRange, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 2, 2, 4, 3));
			break;	
		}
	}
	
	//DPP Params
			
	TGGroupFrame *fGF_DPP = new TGGroupFrame(f1, titlelabel[2], kVerticalFrame);
	fGF_DPP->SetTextFont(gfFont);
	fGF_DPP->SetTitlePos(TGGroupFrame::kCenter); 
	f1->AddFrame(fGF_DPP, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));//
	
	//const Double_t dpp_num[15] = { 50000, 1000, 3000, 800, 4, 200, 50, 3, 0, 0, 1000, 0, 1.0, 0, 1300}; // Dcfg.DParams.DPPParams.blho[0] 
	const double dpp_num[15] = {  (double)Dcfg.DParams.DPPParams.M[0],
														(double)Dcfg.DParams.DPPParams.m[0],
														(double)Dcfg.DParams.DPPParams.k[0],
        												(double)Dcfg.DParams.DPPParams.ftd[0],
        												(double)Dcfg.DParams.DPPParams.a[0],
														(double)Dcfg.DParams.DPPParams.b[0],
        												(double)Dcfg.DParams.DPPParams.thr[0],
        												(double)Dcfg.DParams.DPPParams.nsbl[0],
        												(double)Dcfg.DParams.DPPParams.nspk[0],
        												(double)Dcfg.DParams.DPPParams.pkho[0],
        												(double)Dcfg.DParams.DPPParams.blho[0],
        												(double)Dcfg.DParams.DPPParams.dgain[0],
        												(double)Dcfg.DParams.DPPParams.enf[0],
        												(double)Dcfg.DParams.DPPParams.decimation[0],
        												(double)Dcfg.DParams.DPPParams.trgho[0]
	};
	
	for (int i = 0; i<15; i++)	{
		fHF_DPP[i] = new TGHorizontalFrame(fGF_DPP, 200, 30);
		fGF_DPP->AddFrame(fHF_DPP[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 3, 3));	
						
		fNumericEntries[2][i] = new TGNumberEntry(fHF_DPP[i], dpp_num[i], 8, i+20, (TGNumberFormat::EStyle)  ( i != 12 ? 0 : 1));  // iStyle// N + 20 //( i == 12 ? 0 : 1)
		fNumericEntries[2][i]->Connect("ValueSet(Long_t)", "ParamsMenu", this, "DoSetVal()");
		fNumericEntries[2][i]->Resize(55, fNumericEntries[2][i]->GetDefaultHeight( ) );
		fHF_DPP[i]->AddFrame(fNumericEntries[2][i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 3, 3)); //kLHintsCenterY | kLHintsLeft  //new TGLayoutHints(0, 0, 0, 4, 4)
		
		fDPPLabel[i] = new TGLabel(fHF_DPP[i], dpp_label[i]);
		fDPPLabel[i]->SetTextFont(labelFont);
		fHF_DPP[i]->AddFrame(fDPPLabel[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 4, 4));		
	}
	
	//Reset Detector
			
	TGGroupFrame *fGF_Reset = new TGGroupFrame(f1, titlelabel[3], kVerticalFrame);
	fGF_Reset->SetTextFont(gfFont);
	fGF_Reset->SetTitlePos(TGGroupFrame::kCenter); 
	f1->AddFrame(fGF_Reset, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));//
	
	//const Double_t reset_num[4] = { 0, 100, 2, 2000}; // Dcfg.DParams.ResetDetector[0].thrhold
	const double reset_num[4] = { (double)Dcfg.DParams.ResetDetector[0].Enabled,
														(double)Dcfg.DParams.ResetDetector[0].thrhold,
														(double)Dcfg.DParams.ResetDetector[0].reslenmin,
														(double)Dcfg.DParams.ResetDetector[0].reslength };
	
	for (int i = 0; i<5; i++)	{
		fHF_Reset[i] = new TGHorizontalFrame(fGF_Reset, 200, 30);
		fGF_Reset->AddFrame(fHF_Reset[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 3, 3));	
		
		fResetLabel[i] = new TGLabel(fHF_Reset[i], reset_label[i]);
		fResetLabel[i]->SetTextFont(labelFont);
		fHF_Reset[i]->AddFrame(fResetLabel[i], new TGLayoutHints(kLHintsCenterX, 0, 0, 4, 4));		
		
		if (i == 0){
			fRDMode = new TGComboBox(fHF_Reset[i], 7); // C7
			fRDMode->AddEntry("Internal", 0);
			fRDMode->AddEntry("GPIO", 1);
			fRDMode->AddEntry("Both", 1);
			fRDMode->Select(Dcfg.DParams.ResetDetector[0].ResetDetectionMode);
			fRDMode->Resize(65, 20);
			fRDMode->Connect("Selected(Int_t)", "ParamsMenu", this, "DoComboBox()");
			fHF_Reset[i]->AddFrame(fRDMode, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 2, 2, 5, 3));
		}
		else{
			fNumericEntries[3][i-1] = new TGNumberEntry(fHF_Reset[i], reset_num[i-1], 8, i+40, (TGNumberFormat::EStyle) 0);  // N + 41
			fNumericEntries[3][i-1]->Connect("ValueSet(Long_t)", "ParamsMenu", this, "DoSetVal()");
			fNumericEntries[3][i-1]->Resize(55, fNumericEntries[3][i-1]->GetDefaultHeight( ) );
			fHF_Reset[i]->AddFrame(fNumericEntries[3][i-1], new TGLayoutHints(kLHintsCenterY | kLHintsRight, 2, 2, 3, 3)); //kLHintsCenterY | kLHintsLeft  //new TGLayoutHints(0, 0, 0, 4, 4)
		}
	}
	
	//Extra Parameters for X770
			
	TGGroupFrame *fGF_Extra = new TGGroupFrame(f1, titlelabel[4], kVerticalFrame);
	fGF_Extra->SetTextFont(gfFont);
	fGF_Extra->SetTitlePos(TGGroupFrame::kCenter); 
	f1->AddFrame(fGF_Extra, new TGLayoutHints(kLHintsTop | kLHintsLeft, 2, 2, 2, 2));//
			
	const double extra_num[7] = {	(double)Dcfg.DParams.DPPParams.X770_extraparameters[0].CRgain,
														(double)Dcfg.DParams.DPPParams.X770_extraparameters[0].TRgain, 
														(double)Dcfg.DParams.DPPParams.X770_extraparameters[0].SaturationHoldoff,
														(double)Dcfg.DParams.DPPParams.X770_extraparameters[0].energyFilterMode,
														(double)Dcfg.DParams.DPPParams.X770_extraparameters[0].trigK,
														(double)Dcfg.DParams.DPPParams.X770_extraparameters[0].trigm,
														(double)Dcfg.DParams.DPPParams.X770_extraparameters[0].trigMODE };
			
	for (int i = 0; i<8; i++)	{
		fHF_Extra[i] = new TGHorizontalFrame(fGF_Extra, 200, 30);
		fGF_Extra->AddFrame(fHF_Extra[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 3, 3));	
				
		if (i == 0){
			fImpedance = new TGComboBox(fHF_Extra[i], 8); // C8
			fImpedance->AddEntry("50O", 0);
			fImpedance->AddEntry("1K", 1);
			fImpedance->Select(Dcfg.DParams.DPPParams.X770_extraparameters[0].InputImpedance);
			fImpedance->Resize(60, 20);
			fImpedance->Connect("Selected(Int_t)", "ParamsMenu", this, "DoComboBox()");
			fHF_Extra[i]->AddFrame(fImpedance, new TGLayoutHints(kLHintsCenterY | kLHintsCenterX, 2, 2, 5, 3));
		}
		else{
			fNumericEntries[4][i-1] = new TGNumberEntry(fHF_Extra[i], extra_num[i-1], 8, i+50, (TGNumberFormat::EStyle) 0);  // N + 50
			fNumericEntries[4][i-1]->Connect("ValueSet(Long_t)", "ParamsMenu", this, "DoSetVal()");
			fNumericEntries[4][i-1]->Resize(55, fNumericEntries[4][i-1]->GetDefaultHeight( ) );
			fHF_Extra[i]->AddFrame(fNumericEntries[4][i-1], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 3, 3)); //kLHintsCenterY | kLHintsLeft  //new TGLayoutHints(0, 0, 0, 4, 4)
		}
		
		fExtraLabel[i] = new TGLabel(fHF_Extra[i], extra_label[i]);
		fExtraLabel[i]->SetTextFont(labelFont);
		fHF_Extra[i]->AddFrame(fExtraLabel[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 0, 0, 4, 4));		
	}
	
	fMain->AddFrame(f1, new TGLayoutHints(kLHintsTop | kLHintsRight, 2, 5, 10, 0) );
		
	fMain->MapSubwindows();
	fMain->Resize();

	fMain->CenterOnParent();

	fMain->SetWindowName("Digi params");

	fMain->MapWindow();
}



ParamsMenu::~ParamsMenu()
{

   fMain->DeleteWindow();  // deletes fMain
}

void ParamsMenu::CloseWindow()
{
   

   delete this;
}

void ParamsMenu::DoClose()
{
    //Handle Close button.
	
   CloseWindow();
}

void ParamsMenu::DoSetVal()
{
	TGNumberEntry *ne = (TGNumberEntry *) gTQSender;
	Int_t id = ne->WidgetId();
	
	printf("smth changed in %i  \n", id);
	
	//vector of void pointers, function to get type of Dcfg structure element to avoid that crap of huge switch
	switch (id){
		case 0:
			Dcfg.DParams.WFParams.recordLength = fNumericEntries[0][id]->GetNumber( );
			break;	
		case 1:
			Dcfg.DParams.WFParams.preTrigger = fNumericEntries[0][id]->GetNumber( );
			break;	
		case 2:
			Dcfg.DParams.WFParams.probeSelfTriggerVal = fNumericEntries[0][id]->GetNumber( );
			break;		
		case 13:
			Dcfg.DParams.DCoffset[0] = fNumericEntries[1][0]->GetNumber( );
			break;			
			
		case 20:
			Dcfg.DParams.DPPParams.M[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 21:
			Dcfg.DParams.DPPParams.m[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 22:	
			Dcfg.DParams.DPPParams.k[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
        case 23:
			Dcfg.DParams.DPPParams.ftd[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 24:
			Dcfg.DParams.DPPParams.a[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 25:
			Dcfg.DParams.DPPParams.b[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
        case 26:
			Dcfg.DParams.DPPParams.thr[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 27:	
        	Dcfg.DParams.DPPParams.nsbl[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 28:	
        	Dcfg.DParams.DPPParams.nspk[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
        case 29:
			Dcfg.DParams.DPPParams.pkho[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 30:
			Dcfg.DParams.DPPParams.blho[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
		case 31:	
			Dcfg.DParams.DPPParams.dgain[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
        case 32:
			Dcfg.DParams.DPPParams.enf[0] = fNumericEntries[2][id-20]->GetNumber( );
        	break;											
		case 33:
			Dcfg.DParams.DPPParams.decimation[0] = fNumericEntries[2][id-20]->GetNumber( );
			break;
        case 34:
			Dcfg.DParams.DPPParams.trgho[0] = fNumericEntries[2][id-20]->GetNumber( );	
			break;
			
		case 41:
			Dcfg.DParams.ResetDetector[0].Enabled = fNumericEntries[3][id-41]->GetNumber( );
			break;				
		case 42:
			Dcfg.DParams.ResetDetector[0].thrhold = fNumericEntries[3][id-41]->GetNumber( );
			break;					
		case 43:
			Dcfg.DParams.ResetDetector[0].reslenmin = fNumericEntries[3][id-41]->GetNumber( );
			break;					
		case 44:
			Dcfg.DParams.ResetDetector[0].reslength = fNumericEntries[3][id-41]->GetNumber( );
			break;						
		case 51:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].CRgain = fNumericEntries[4][id-51]->GetNumber( );
			break;							
		case 52:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].TRgain = fNumericEntries[4][id-51]->GetNumber( );
			break;								
		case 53:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].SaturationHoldoff = fNumericEntries[4][id-51]->GetNumber( );
			break;			
		case 54:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].energyFilterMode = fNumericEntries[4][id-51]->GetNumber( );
			break;								
		case 55:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].trigK = fNumericEntries[4][id-51]->GetNumber( );
			break;								
		case 56:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].trigm = fNumericEntries[4][id-51]->GetNumber( );
			break;								
		case 57:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].trigMODE = fNumericEntries[4][id-51]->GetNumber( );
			break;								
	}
	
	int32_t ret;	
	//int32_t ret = CAENDPP_IsBoardAcquiring(handle, Dcfg.BID, &isAcq);	 //doesn't work as expected, allways return 0
	//printf("IsBoardAcquiring (ret = %i ) isAcq %i \n", ret, isAcq);
	
	if (Rcfg.loop == -1){
		ret = CAENDPP_SetBoardConfiguration(handle, Dcfg.BID, Dcfg.acqMode, Dcfg.DParams);
		if (ret)
			printf("Error during SetBoardConfig (ret = %i ) \n", ret);
	}
	else{
		ret = CAENDPP_StopAcquisition(handle, 0);
		ret = CAENDPP_SetBoardConfiguration(handle, Dcfg.BID, Dcfg.acqMode, Dcfg.DParams);
		if (ret)
			printf("Error during SetBoardConfig (ret = %i ) \n", ret);
		ret = CAENDPP_StartAcquisition(handle, 0);
	}	
		printf("(ret = %i ) preTrigger %i  DCOffset %i \n", ret, Dcfg.DParams.WFParams.preTrigger, Dcfg.DParams.DCoffset[0]);
		
}

void ParamsMenu::DoCheckBox()
{
	TGCheckButton *chb = (TGCheckButton *) gTQSender;
	Int_t id = chb->WidgetId();
	printf("checkbox changed %i \n", id);
	
}

void ParamsMenu::DoComboBox()
{
	TGComboBox *cb = (TGComboBox *) gTQSender;
	Int_t id = cb->WidgetId();
		
	//printf("combobox changed %i  \n", id);
	
	switch (id){
		
		case 0:
			Dcfg.DParams.WFParams.vp1 = (CAENDPP_PHA_VirtualProbe1_t)fProbe[id]->GetSelected( );
			break;
		case 1:
			Dcfg.DParams.WFParams.vp2 = (CAENDPP_PHA_VirtualProbe2_t)fProbe[id]->GetSelected( );
			break;	
		case 2:
			Dcfg.DParams.WFParams.dp1 = (CAENDPP_PHA_DigitalProbe1_t)fProbe[id]->GetSelected( );
			break;
		case 3:
			Dcfg.DParams.WFParams.dp2 = (CAENDPP_PHA_DigitalProbe2_t)fProbe[id]->GetSelected( );
			break;	
		case 4:
			Dcfg.DParams.WFParams.probeTrigger = (CAENDPP_PHA_ProbeTrigger_t)fProbe[id]->GetSelected( );
			break;		
			
		case 5:
			Dcfg.DParams.PulsePolarity[0] = (CAENDPP_PulsePolarity_t)fPolarity->GetSelected( ); // == 0 ? CAENDPP_PulsePolarityPositive : CAENDPP_PulsePolarityNegative;
			break;			
		
		case 6:
			Dcfg.InputRange = InputRange_val[fInputRange->GetSelected( )]; // == 0 ? CAENDPP_PulsePolarityPositive : CAENDPP_PulsePolarityNegative;
			break;					
			
		case 7:
			Dcfg.DParams.ResetDetector[0].ResetDetectionMode = (CAENDPP_ResetDetectionMode_t)fRDMode->GetSelected( );
			break;			
			
		case 8:
			Dcfg.DParams.DPPParams.X770_extraparameters[0].InputImpedance = (CAENDPP_InputImpedance_t)fImpedance->GetSelected( ); // == 0 ? CAENDPP_InputImpedance_50O : CAENDPP_InputImpedance_1K;
			break;					
	}
	
	int32_t isAcq;	
	int32_t ret = CAENDPP_IsBoardAcquiring(handle, Dcfg.BID, &isAcq);	
	if (isAcq == 1){
		if (id!=6) //InputRange
			ret = CAENDPP_SetBoardConfiguration(handle, Dcfg.BID, Dcfg.acqMode, Dcfg.DParams);
		else
			ret = CAENDPP_SetInputRange(handle, 0, Dcfg.InputRange);
	}
	else{
		ret = CAENDPP_StopAcquisition(handle, 0);
		if (id!=6)
			ret = CAENDPP_SetBoardConfiguration(handle, Dcfg.BID, Dcfg.acqMode, Dcfg.DParams);
		else
			ret = CAENDPP_SetInputRange(handle, 0, Dcfg.InputRange);
		ret = CAENDPP_StartAcquisition(handle, 0);
	}	
		printf("(ret = %i ) vp1 %i  vp2 %i  dp1 %i  dp2 %i prTrigger %i Polarity %i RDMode %i Impedance %i InputRange %i \n", ret, Dcfg.DParams.WFParams.vp1, Dcfg.DParams.WFParams.vp2, Dcfg.DParams.WFParams.dp1, Dcfg.DParams.WFParams.dp2, Dcfg.DParams.WFParams.probeTrigger,
		Dcfg.DParams.PulsePolarity[0], Dcfg.DParams.ResetDetector[0].ResetDetectionMode, Dcfg.DParams.DPPParams.X770_extraparameters[0].InputImpedance, Dcfg.InputRange);
	
}

void ParamsMenu::TryToClose()
{
  
   printf("Can't close the window '%s' : a message box is still open\n", fMain->GetWindowName());
}
