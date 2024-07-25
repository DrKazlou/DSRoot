#include "DSCalib.h"
#include "DSFunc.h"

//extern int32_t handle;
//extern DigitizerConfig_t Dcfg;
extern Histograms_t Histo;

CalibMenu::CalibMenu(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h, UInt_t options)
{
		
	char str[10];
	int j;

   fMain = new TGTransientFrame(p, main, w, h, options);
   fMain->Connect("CloseWindow()", "CalibMenu", this, "CloseWindow()");
   fMain->DontCallClose(); // to avoid double deletions.

   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);

   fMain->ChangeOptions((fMain->GetOptions() & ~kVerticalFrame) | kHorizontalFrame);

   f1 = new TGCompositeFrame(fMain, 400, 200, kHorizontalFrame); //| kFixedWidth
	

	TGHorizontalFrame *hframe1 = new TGHorizontalFrame(fMain, 200, 40);
	TGVerticalFrame *vframe1 = new TGVerticalFrame(hframe1, 200, 40);		
    hframe1->AddFrame(vframe1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//
		
	TGHorizontalFrame *header_hf = new TGHorizontalFrame(vframe1, 200, 40);		
	vframe1->AddFrame(header_hf, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//
	
	for (int i =0; i<3; i++){
		fPointLabel[i] = new TGLabel(header_hf, titlelabel[i]);
		header_hf->AddFrame(fPointLabel[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	}
	
	header_hf->Resize( );
	
	for (int i =0; i<10; i++){
		fSub_HF[i] = new TGHorizontalFrame(vframe1, 200, 40);	
		vframe1->AddFrame(fSub_HF[i], new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//
	
		sprintf(str, "[%i]",i);
		fPointLabel[i] = new TGLabel(fSub_HF[i], str);
		fSub_HF[i]->AddFrame(fPointLabel[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
		fNumericEntries[0][i] = new TGNumberEntry(fSub_HF[i], Histo.arr_calib[0][i], 8, i, (TGNumberFormat::EStyle) 0); //numinit[i], 7, i + 20, (TGNumberFormat::EStyle) iStyle[i]
		fNumericEntries[0][i]->Connect("ValueSet(Long_t)", "CalibMenu", this, "DoSetVal()");
		fSub_HF[i]->AddFrame(fNumericEntries[0][i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
		fNumericEntries[1][i] = new TGNumberEntry(fSub_HF[i], Histo.arr_calib[1][i], 8, i+20, (TGNumberFormat::EStyle) 1); //numinit[i], 7, i + 20, (TGNumberFormat::EStyle) iStyle[i]
		fNumericEntries[1][i]->Connect("ValueSet(Long_t)", "CalibMenu", this, "DoSetVal()");
		fSub_HF[i]->AddFrame(fNumericEntries[1][i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
		fSub_HF[i]->Resize();
	}
	
	fFitLabel = new TGLabel(vframe1, Histo.fFormula);
	//fFitLabel = new TGLabel(vframe1, "            Hr wll b smthng         ");
	fFitLabel->SetTextFont(gfFont);
	fFitLabel->Resize(200, 30);
	
	vframe1->AddFrame(fFitLabel, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
	vframe1->Resize();
	
	fEcanvas = new TRootEmbeddedCanvas("Ecanvas", hframe1, 600, 420); // 1600 780
	hframe1->AddFrame(fEcanvas, new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5));//kLHintsExpandX |   kLHintsExpandY
	hframe1->Resize();
	
	can = fEcanvas->GetCanvas( );
	
    fMain->AddFrame(hframe1, new TGLayoutHints(kLHintsCenterX, 2, 2 , 2, 2) );
			
	fMain->MapSubwindows();
	fMain->Resize();

	fMain->CenterOnParent();

	fMain->SetWindowName("Calibration");

	fMain->MapWindow();
}



CalibMenu::~CalibMenu()
{

   fMain->DeleteWindow();  // deletes fMain
}

void CalibMenu::CloseWindow()
{
   

   delete this;
}

void CalibMenu::DoClose()
{
    //Handle Close button.
	
   CloseWindow();
}

void CalibMenu::DoSetVal()
{
	TGNumberEntry *ne = (TGNumberEntry *) gTQSender;
	Int_t id = ne->WidgetId();
	char CName[30];
	
	int NPoint = id >19 ? (id-20) : id;
	Double_t param[2] = {0.0, 0.0};
	
	gr->SetPoint(NPoint, fNumericEntries[0][NPoint]->GetNumber( ), fNumericEntries[1][NPoint]->GetNumber( ) );
	Histo.arr_calib[0][NPoint]  = fNumericEntries[0][NPoint]->GetNumber( );
	Histo.arr_calib[1][NPoint]  = fNumericEntries[1][NPoint]->GetNumber( );
	
	can->cd(1);
	gr->SetMarkerStyle(21);
	gr->SetMarkerSize(1.0);
	gr->SetMarkerColor(kBlue);
	gr->Draw("AP");
		
	if (gr->GetN( )>1){
		
		TF1* ffun = new TF1("linfit", "[0]*x+[1]", 0, 16364);
		gr->Fit(ffun,"MQ");
		ffun->GetParameters(param);
		
		sprintf(CName,"y = %0.4f X %s %0.4f",  param[0], param[1] > 0 ? (char*)("+") : (char*)"-", param[1] > 0 ? param[1] : -param[1] );
		Histo.k = param[0];
		Histo.b = param[1];
		
		fFitLabel->SetText(CName);
		
		sprintf(Histo.fFormula, "%s", CName);
		
		//That should help with bining troubles
		if (Histo.detHE){
			delete Histo.detHE;
			Histo.detHE = new TH1D("h_detHE", "h_detHE", 16384, 0, 16384 * Histo.k + Histo.b); 
		}
	}
	
	gr->GetYaxis()->SetTitleOffset(1.3);
	gr->GetYaxis()->SetTitle("Energy, keV");
	gr->GetXaxis()->SetTitle(" Channels, N "); // Time, ns
	
	can->Modified( );
	can->Update( );
}



void CalibMenu::TryToClose()
{
  
   printf("Can't close the window '%s' : a message box is still open\n", fMain->GetWindowName());
}
