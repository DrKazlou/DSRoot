

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "DSFrame.h"
#include "DSFunc.h"
#include "DSReadout.h"

#include "TROOT.h"
#include "TApplication.h"
//#include "TCanvas.h"
//#include "TH1D.h"
//#include "TFile.h"
//#include "TTree.h"
//#include "TBranch.h"


//using namespace std;

	
	ReadoutConfig_t Rcfg;    
	DigitizerConfig_t Dcfg;    	
	Histograms_t Histo;    	
	
	int32_t handle;



//---- Main program ------------------------------------------------------------

int main(int argc, char **argv)
{
	char CName[100];
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }
   	
	memset(&Rcfg, 0, sizeof(Rcfg));
	memset(&Dcfg, 0, sizeof(Dcfg));
	memset(&Histo, 0, sizeof(Histo));
			
	InitReadoutConfig(&Rcfg);	
	
	handle = 0;
			
	new MainFrame(gClient->GetRoot(), 1700, 850);
		
	int32_t ret = DataAcquisition(handle);
	
	if(ret){
		sprintf(CName, "ERR_DAQ %i \n", ret);
		new TGMsgBox(gClient->GetRoot(), Rcfg.main, "Error", CName, kMBIconStop, kMBOk);
	}
	
	
	theApp.Run( );

   return 0;
}

