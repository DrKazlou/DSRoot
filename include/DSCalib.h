#include <stdlib.h>

#include <TROOT.h>
#include <TClass.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include <TVirtualPadEditor.h>
#include <TGResourcePool.h>
#include <TGListBox.h>
#include <TGListTree.h>
#include <TGFSContainer.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGStatusBar.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TGTextEdit.h>
#include <TGShutter.h>
#include <TGProgressBar.h>
#include <TGColorSelect.h>
#include <RQ_OBJECT.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TMath.h>
#include <TLatex.h>

#include <TF1.h>

#include <TRandom.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include "TGraph.h"

#include <TGFontDialog.h>

class CalibMenu {
	
RQ_OBJECT("CalibMenu")

private:
   TGTransientFrame     *fMain;
   TGCompositeFrame     *f1;
   
   //TGTextButton      *fSetButton;
      	
		
	TGHorizontalFrame    *fSub_HF[10];
	
	TRootEmbeddedCanvas *fEcanvas;
	
	TGCheckButton *fC[16], *fCselft[16];	
		
   TGLabel *fHeaderLabel[8], *fPointLabel[10], *fFitLabel;
   
   TGNumberEntry        *fNumericEntries[2][10];
   
	TGTextEntry        *fTEntries[16][16];
	TGTextBuffer *tbuf[16][16];
	
	
	
public:
   CalibMenu(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
              UInt_t options = kHorizontalFrame);

   virtual ~CalibMenu();
	
	const char *sFont = "-Ubuntu-bold-r-*-*-16-*-*-*-*-*-iso8859-1";
	const char *gfFont = "-Ubuntu-bold-r-*-*-14-*-*-*-*-*-iso8859-1";
	const char *labelFont = "-Ubuntu-bold-r-*-*-13-*-*-*-*-*-iso8859-1";
	const char *paramFont = "-adobe-courier-bold-r-*-*-15-*-*-*-*-*-iso8859-1";
	
	char str[5];
	const char titlelabel[3][20] = {" X ", "Channel, N", "     Energy, keV"};
	
	TCanvas *can;
	
	TGraph *gr = new TGraph( );
	
   // slots
   void TryToClose();
   void CloseWindow();
   void DoClose();
   void DoSetVal();
   void DoCheckBox();
   void DoComboBox();
   
};