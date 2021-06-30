// Based on alice_esd.C from ROOT tutorials
// Modified for hyperk by Alex Finch
#include "fitqun_def.h"
#ifdef  FITQUNEXISTS
fitQunDisplay fiTQun;
#endif
#include "TColor.h"
/*
  Function definitions
*/
void       make_gui();
void       load_event();
void       createGeometry(TGeoManager* geom,bool flatTube=kFALSE);
void       wcsim_load_event(int iTrigger,bool &firstTrackIsNeutrino,bool &secondTrackIsTarget );
void       wcsim_load_cherenkov(int iTrigger);
void       wcsim_load_truth_tracks(int iTrigger,bool &firstTrackIsNeutrino,bool &secondTrackIsTarget);
void       fitqun_load_event(int entry);
void       UnrollView(double* pmtX ,double* pmtY,double* pmtZ,int location,float maxY,float maxZ,bool debug=false);
TGeoVolume* createCylinder(TString name, float radius,float height);
TEveViewer* New2dView(TString name,TGLViewer::ECameraType type, TEveScene* scene);
void update_html_summary(int iTrigger,WCSimRootTrigger * wcsimrootTrigger,bool firstTrackIsNeutrino,bool secondTrackIsTarget);
void	   selectTruthTrack(WCSimRootTrack * wcsimroottrack,bool& keep,bool& draw);
TString    ParticleName(int pdgCode);
TString    paletteText();

/*
 Global variables (!)
 */
float             minTruthTrackEnergyToKeep=0.0;
float             minTruthTrackEnergyToDraw=100.0;
int	              fPredefinedPalette{kRainBow};
WCSimRootGeom     *wcsimrootgeom;
WCSimRootEvent    *wcsimrootEvent;
WCSimRootTrigger  *wcsimrootTrigger;
TFile* WCSimFile, *fiTQunFile;
TTree * wcsimGeoT;
TTree * wcsimT;
TTree*	fiTQunTree;
float maxX,maxY,maxZ,minZ;
TGeoVolume        *WorldVolume,*SimpleVolume;
TEveElementList *FlatGeometry;
TEveScene       *UnrolledScene;
TEveScene       *flatGeometryScene;
TEveViewer      *UnrolledView;
bool FITQUN,fAccumulateEvents{kTRUE},fDigitIsTime,fFirstTime{kTRUE};

bool fSimpleGeometry;
TGTextButton                *fGeometrySwitchButton;
TGCheckButton               *fAccumulateEventsBox;
TGCheckButton               *fColourIsTimeBox;
TGNumberEntry               *fTruthTrackMinimumEnergyWidget;
TEveRGBAPalette             *fHitPalette=0;
TEveRGBAPaletteOverlay		*fPalletteOverlay=0;
TGLAnnotation               *fPaletteTitle=0;    
TGLAnnotation               *fPaletteTitle2=0;    
Int_t event_id= 0;        
  // Current event id.
TEveTrackList               *gTrackList = 0;
TEveGeoTopNode              *geomRoot;
#include "hyperk_esd_html_summary.h"
HtmlSummary                 *fgHtmlSummary = 0;
TGHtml                      *fgHtml        = 0;
#include "Picker.h"
Picker* fPicker;
/******************************************************************************/
// Initialization and steering functions
/******************************************************************************/
/*
 Entry  point. Set everything up and then load the first event
*/
void hyperk_esd()
{
// Main function, initializes the application.
/*
 Open files
*/
	const char *filetypes[] = {
		"ROOT files",    "*.root",    	
		"All files",     "*",
		0,               0
	};
	TString CurrentDirectory=gSystem->pwd();
	TString originalDirectory=CurrentDirectory;
	TGFileInfo fi;
	fi.fFileTypes = filetypes;
	fi.fIniDir    = StrDup(CurrentDirectory);
	cout<<" Please choose your WCSim file "<<endl;
	new TGFileDialog(gClient->GetRoot(), 0, kFDOpen, &fi);
	if (!fi.fFilename) {
		cout<<" No WCSim file chosen "<<endl;
		return;
	}
	cout<<" opening file "<<fi.fFilename<<endl;
	WCSimFile = new TFile(fi.fFilename);
	gSystem->cd(originalDirectory);
	fi.fIniDir    = StrDup(CurrentDirectory);
#ifdef  FITQUNEXISTS
	FITQUN=kTRUE;
#else
	FITQUN=kFALSE;
#endif
	if(FITQUN)
	{
		cout<<" Please choose your fiTQun file, cancel if you don't have one "<<endl;
		new TGFileDialog(gClient->GetRoot(), 0, kFDOpen, &fi);
		if (!fi.fFilename) {
			cout<<" No fiTQun file chosen "<<endl;
			FITQUN=kFALSE;
		}
		else
		{
			cout<<" opening file "<<fi.fFilename<<endl;
			fiTQunFile = new TFile(fi.fFilename);
			fiTQunTree=(TTree *) fiTQunFile->Get("fiTQun");
			fiTQun.Init(fiTQunTree);
		}
	}
	if(FITQUN)
	{
		fiTQunTree=(TTree *) fiTQunFile->Get("fiTQun");
		fiTQun.Init(fiTQunTree);
	}
	gSystem->cd(originalDirectory);
/*
  Initialise WCSim
*/
	wcsimGeoT=(TTree *) WCSimFile->Get("wcsimGeoT");
	wcsimrootgeom = new WCSimRootGeom (); 
	wcsimGeoT -> SetBranchAddress ("wcsimrootgeom" ,&wcsimrootgeom );
	wcsimGeoT->GetEntry(0) ;
/*
  Initialise Eve
*/
	gStyle->SetPalette(fPredefinedPalette ,0);
	TColor::InvertPalette();
	TEveManager::Create(kTRUE,"V");
	gEve->GetBrowser()->SetTabTitle("3D View",TRootBrowser::kRight,0);
	gEve->GetDefaultViewer()->SetName("3D View");
	TGLViewer* GLViewer =gEve->GetDefaultViewer()->GetGLViewer();
	GLViewer->SetCurrentCamera(TGLViewer::kCameraPerspXOY);
	GLViewer->CurrentCamera().SetExternalCenter(kTRUE);
	GLViewer->CurrentCamera().SetCenterVec(0,0,0);
	GLViewer->UpdateScene();
// Create a clip box centree at 0, length 6000
	TGLClipSet* thisClipSet=GLViewer->GetClipSet();
	thisClipSet->SetClipType(TGLClip::kClipBox);
	TGLClipBox* thisClip = (TGLClipBox*) thisClipSet->GetCurrentClip();
	thisClip->SetMode(TGLClip::kOutside);
	TGLVector3 min_point,max_point;
	min_point.Set(-3000.,-3000.,-3000.);
	max_point.Set(3000.,3000.,3000.);
	thisClip->Setup( min_point,max_point);
	GLViewer->SetClipAutoUpdate(kFALSE);
	GLViewer->RefreshPadEditor();
	GLViewer->RequestDraw();
	GLViewer->SetResetCamerasOnUpdate(kFALSE);
// set up picking
	fPicker= new Picker();
	GLViewer->Connect("Clicked(TObject*)", "Picker", fPicker, "Picked(TObject*)");
/*
  Initialise the html summary tab
*/
	fgHtmlSummary = new HtmlSummary("HyperK Event Display Summary Table");
	auto slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
	fgHtml = new TGHtml(0, 100, 100);
	TEveWindowFrame *wf = slot->MakeFrame(fgHtml);
	fgHtml->MapSubwindows();
	wf->SetElementName("Summary");
/*
  Initialise the geometry objects
*/
	gSystem->Load("libGeom");
	auto geom = new TGeoManager("HyperK", "HyperK Detector");
	createGeometry(geom,kTRUE);
	TGeoNode* node = gGeoManager->GetTopNode();
	geomRoot = new TEveGeoTopNode(gGeoManager, node);
	geomRoot->SetVisLevel(4);
	geomRoot->GetNode()->GetVolume()->SetVisibility(kFALSE);
	gEve->AddGlobalElement(geomRoot); 
	gEve->Redraw3D(kTRUE);  	
/*
  Set up the event control GUI
*/
	gEve->GetBrowser()->GetTabRight()->SetTab(1);
	make_gui();
/*
  Initialise the unrolled geometry scene and view
*/
	gEve->GetBrowser()->GetTabRight()->SetTab(0);
	UnrolledScene = gEve->SpawnNewScene("Unrolled Event Scene");
	flatGeometryScene = gEve->SpawnNewScene("Unrolled Geometry");
	UnrolledView = New2dView("Unrolled View",TGLViewer::kCameraOrthoXnOY,UnrolledScene);   	
	flatGeometryScene->AddElement(FlatGeometry);
	UnrolledView->AddScene(flatGeometryScene);
	TEveSceneInfo* gSI= (TEveSceneInfo*) (UnrolledView->FindChild("SI - Geometry scene"));
	if(gSI!=NULL)gSI->Delete();
	auto UGLV=UnrolledView->GetGLViewer();
	UGLV->Connect("Clicked(TObject*)", "Picker", fPicker,"Picked(TObject*)");
	UGLV->AddOverlayElement(fPalletteOverlay);
	fPaletteTitle2 = new TGLAnnotation(UGLV,paletteText(), 0.7+(0.3/2.0)-0.05, 0.05+0.02+0.04);
	fHitPalette->SetInterpolate(kTRUE);
	fHitPalette->SetupColorArray();	
	fHitPalette->IncRefCount();
	fHitPalette->IncRefCount();
/*
  Initialise FitQun
*/
	if(FITQUN){
		fiTQun.setLimits(maxX,maxY,maxZ,minZ);
		fiTQun.SetWCSimGeom(wcsimrootgeom);
		fiTQun.maxY=maxY;
	}
/*        
  Get the WCSim information
*/
	wcsimT = (TTree*)WCSimFile ->Get("wcsimT");
	wcsimrootEvent = new WCSimRootEvent ();
	wcsimT -> SetBranchAddress ("wcsimrootevent" ,&wcsimrootEvent);   	
	wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);
	wcsimT->GetEvent(0);
	cout<<" there are "<<wcsimT->GetEntries()<<" events in this file."<<endl;
/*
  Get Eve started
*/
	gEve->GetDefaultGLViewer()->UpdateScene();
	// Reset camera after the first event has been shown.
	gEve->Redraw3D(kTRUE); 
/*
  Load the first event
*/
	load_event();

}
/*
 Cartesian to Polar conversion
*/
void CartesianToPolar(double &mom,double &theta,double &phi,double p[3]){
	mom=sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
	theta=acos(p[2]/mom);
	phi=atan2(p[1],p[0]);
}
/*
  Load an event.
*/
void load_event()
{
// Load event specified in global event_id.
// The contents of previous event are removed.
	if(fFirstTime)
	{
		gStyle->SetPalette(fPredefinedPalette ,0);
		TColor::InvertPalette();
		fFirstTime=kFALSE;
	}	
	TEveEventManager* CurrentEvent =gEve->GetCurrentEvent();
	if(! fAccumulateEvents ){
		if( CurrentEvent  != 0)CurrentEvent->DestroyElements();
		if( UnrolledScene !=0 )UnrolledScene->DestroyElements();	
	}
	/*
	  Get the next event from WCSim, loop over its triggers and put it into Eve
	*/
	wcsimT->GetEvent(event_id);	
	fgHtmlSummary->Clear("D");
	for(int iTrigger=0;iTrigger<wcsimrootEvent->GetNumberOfEvents();iTrigger++)
	{
		wcsimrootTrigger = wcsimrootEvent->GetTrigger(iTrigger); 
		bool firstTrackIsNeutrino,secondTrackIsTarget;
		wcsim_load_event(iTrigger,firstTrackIsNeutrino,secondTrackIsTarget);
		update_html_summary(iTrigger,wcsimrootTrigger,firstTrackIsNeutrino,secondTrackIsTarget);
	}
	if(FITQUN){
		fitqun_load_event(event_id);
		fiTQun.CreateTable( fgHtmlSummary);
	}
	fgHtmlSummary->SetTitle(Form("HyperK Event Display Summary, for entry %i",event_id));
	fgHtmlSummary->Build();
	fgHtml->Clear();
	fgHtml->ParseText((char*)fgHtmlSummary->Html().Data());
	fgHtml->Layout();
	TEveElement* top = gEve->GetCurrentEvent();
	gEve->Redraw3D(kFALSE, kTRUE);
}
/******************************************************************************/
// GUI
/******************************************************************************/
/* 
 EvNavHandler class is needed to connect GUI signals to matching actions.
*/
class EvNavHandler
{
public:
	void Fwd()
	{
		if (event_id < wcsimT->GetEntries()) {
			if(UnrolledScene != 0){
				TEveElement*  CherenkovHits=UnrolledScene->FindChild(Form("CherenkovHits (Unrolled version) %i",event_id));
				if(CherenkovHits!=NULL)UnrolledScene->RemoveElement(CherenkovHits);
			}
			++event_id;
			load_event();
		} else {
			printf("Already at last event.\n");
		}
	}
	void Bck()
	{
		if (event_id > 0) {
			--event_id;
			load_event();
		} else {
			printf("Already at first event.\n");
		}
	}
	void AccumulateEventsBoxChanged(Bool_t s)
	{
		fAccumulateEvents=s;
	}
	void TruthTrackMinimumEnergyChanged()
	{
		minTruthTrackEnergyToDraw = fTruthTrackMinimumEnergyWidget->GetNumber();
		cout<<" New minimum energy to draw truth tracks is "<<minTruthTrackEnergyToDraw<<endl;
		load_event();
	}
	void ColourIsTimeBoxChanged(Bool_t s)
	{
		fDigitIsTime=s;
		for(int iTrigger=0;iTrigger<wcsimrootEvent->GetNumberOfEvents();iTrigger++)
		{
			wcsimrootTrigger = wcsimrootEvent->GetTrigger(iTrigger); 
			reload_wcsim_hits(iTrigger);
		}
		
		fPaletteTitle ->SetText(paletteText());
		fPaletteTitle2->SetText(paletteText());
		gEve->GetDefaultGLViewer()->UpdateScene();
		UnrolledView->GetGLViewer()->UpdateScene();
	}
	void reload_wcsim_hits(int iTrigger)
	{
		TEveEventManager* CurrentEvent =gEve->GetCurrentEvent();
		if(CurrentEvent != 0){
			TEveElement*  CherenkovHits=CurrentEvent->FindChild(Form("Cherenkov Hits subevent %i",iTrigger));
			if(CherenkovHits !=NULL)CurrentEvent->RemoveElement(CherenkovHits);
		}
		if(UnrolledScene != 0){
			TEveElement*  CherenkovHits=UnrolledScene->FindChild(Form("CherenkovHits (Unrolled version) %i",iTrigger));
			if(CherenkovHits!=NULL)UnrolledScene->RemoveElement(CherenkovHits);
		}
		wcsim_load_cherenkov(iTrigger);
	}
	void SwitchGeometry()
	{
		if(fSimpleGeometry)
		{
// simple to full
			fSimpleGeometry=kFALSE;
			gGeoManager->SetTopVolume(WorldVolume);
			TGeoNode* CurrentNode =  gGeoManager->GetCurrentNode();
			if(geomRoot!=NULL)gEve->GetGlobalScene()->RemoveElement(geomRoot);
			geomRoot = new TEveGeoTopNode(gGeoManager, CurrentNode);
			gEve->AddGlobalElement(geomRoot);
			fGeometrySwitchButton->SetText("Simple Geometry");
			fGeometrySwitchButton->SetToolTipText("Switch to Simple Geometry");
		}
		else
		{
// full to simple
			fSimpleGeometry=kTRUE;
			gGeoManager->SetTopVolume(SimpleVolume);
			TGeoNode* CurrentNode =  gGeoManager->GetCurrentNode();
			if(geomRoot!=NULL)gEve->GetGlobalScene()->RemoveElement(geomRoot);
			geomRoot = new TEveGeoTopNode(gGeoManager, CurrentNode);
			gEve->AddGlobalElement(geomRoot);
			fGeometrySwitchButton->SetText("Full Geometry");
			fGeometrySwitchButton->SetToolTipText("Switch to Full Geometry");
		}
		gEve->GetDefaultGLViewer()->UpdateScene();
	}
};
//______________________________________________________________________________
void make_gui()
{
// Create minimal GUI for event navigation, etc.
	TEveBrowser* browser = gEve->GetBrowser();
	browser->StartEmbedding(TRootBrowser::kLeft);
	TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
	frmMain->SetWindowName("HyperK Event Display");
	frmMain->SetCleanup(kDeepCleanup);
	TGCanvas*  fCanvasWindow = new TGCanvas(frmMain, 400, 240);
	TGCompositeFrame* fFrame = new TGCompositeFrame(fCanvasWindow->GetViewPort(), 10, 10, kVerticalFrame);
	fFrame->SetLayoutManager(new TGVerticalLayout(fFrame));    
	fCanvasWindow->SetContainer(fFrame);   
// use hierarchical cleaning for container
	fFrame->SetCleanup(kDeepCleanup);  
	EvNavHandler    *fh = new EvNavHandler;
	TGGroupFrame* Group;
	{
		Group = new TGGroupFrame(fCanvasWindow->GetContainer(),"Event Navigation");
		TGHorizontalFrame* hf = new TGHorizontalFrame(Group);
		{
			TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
			TGPictureButton* b = 0;
			b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoBack.gif"));
			hf->AddFrame(b);
			b->Connect("Clicked()", "EvNavHandler", fh, "Bck()");
			b = new TGPictureButton(hf, gClient->GetPicture(icondir+"GoForward.gif"));
			hf->AddFrame(b);
			b->Connect("Clicked()", "EvNavHandler", fh, "Fwd()");
		}
		Group->AddFrame(hf);
		fCanvasWindow->AddFrame(Group);
/*
Control to toggle from full to simple geometry
*/
		Group = new TGGroupFrame(fCanvasWindow->GetContainer(),"Geometry Choice");
		fSimpleGeometry=kTRUE; 
		hf = new TGHorizontalFrame(Group);
		{
			if(!fSimpleGeometry)
			{
				fGeometrySwitchButton = new TGTextButton(Group, " Simple Geometry ");
				fGeometrySwitchButton->SetToolTipText("Switch to Simple Geometry");
			}
			else
			{
				fGeometrySwitchButton = new TGTextButton(Group, "    Full Geometry   ");
				fGeometrySwitchButton->SetToolTipText("Switch to Full Geometry");
			}
			fGeometrySwitchButton->Connect("Clicked()","EvNavHandler", fh,"SwitchGeometry()");
		}
		Group->AddFrame( fGeometrySwitchButton);
		fCanvasWindow->AddFrame(Group);
/*
Control to toggle event accumulation on or off
*/
		fAccumulateEvents=kFALSE; 
		Group = new TGGroupFrame(fCanvasWindow->GetContainer(),"Event accumulation");
		hf = new TGHorizontalFrame(Group);
		{
			fAccumulateEventsBox = new TGCheckButton(hf, "Accumulate  events.",1);
			fAccumulateEventsBox->SetState(kButtonUp);
			fAccumulateEventsBox->SetToolTipText("If this is checked, don't unload events from Eve.");
			fAccumulateEventsBox->Connect("Toggled(Bool_t)", "EvNavHandler", fh, "AccumulateEventsBoxChanged(Bool_t)");
			hf->AddFrame(fAccumulateEventsBox);
		}
		Group->AddFrame(hf);
		fCanvasWindow->AddFrame(Group);
/*
Control to toggle event colour is time
*/
		fDigitIsTime=kTRUE; 
		Group = new TGGroupFrame(fCanvasWindow->GetContainer(),"Colour for Digits");
		hf = new TGHorizontalFrame(Group);
		{
			fColourIsTimeBox = new TGCheckButton(hf, "Digit Colour is Time.",1);
			fColourIsTimeBox->SetState(kButtonDown);
			fColourIsTimeBox->SetToolTipText("If this is checked, digit colour represents time, otherwise it is charge.");
			fColourIsTimeBox->Connect("Toggled(Bool_t)", "EvNavHandler", fh, "ColourIsTimeBoxChanged(Bool_t)");
			hf->AddFrame(fColourIsTimeBox);
		}
		Group->AddFrame(hf);
		fCanvasWindow->AddFrame(Group);
		fCanvasWindow->AddFrame(Group);
		frmMain->AddFrame(fCanvasWindow,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,
			0, 0, 2, 2));
/*
Control to set energy range for display of truth tracks
*/
		Group = new TGGroupFrame(fCanvasWindow->GetContainer(),"Truth Track Display Minimum Energy (MeV)");
		hf = new TGHorizontalFrame(Group);
		{
			fTruthTrackMinimumEnergyWidget = new TGNumberEntry(hf,minTruthTrackEnergyToDraw, 9, 1,
				TGNumberFormat::kNESReal,  
 //style
				TGNumberFormat::kNEAPositive, 
  //input value filter
				TGNumberFormat::kNELNoLimits, 
//specify limits
				0.,10000.);   
			fTruthTrackMinimumEnergyWidget->Connect("ValueSet(Long_t)", "EvNavHandler", fh, "TruthTrackMinimumEnergyChanged()");
			hf->AddFrame(fTruthTrackMinimumEnergyWidget);
		}
		Group->AddFrame(hf);
	}
	fCanvasWindow->AddFrame(Group);
	frmMain->AddFrame(fCanvasWindow,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,
		0, 0, 2, 2));
	frmMain->MapSubwindows();
	frmMain->Resize();
	frmMain->MapWindow();
	browser->StopEmbedding();
	browser->SetTabTitle("Event Control", 0);
/*
Set up color palette object
*/
	gStyle->SetPalette(fPredefinedPalette ,0);
	fHitPalette = new TEveRGBAPalette(0, 10000);
	fHitPalette->SetFixColorRange(kFALSE);
	fHitPalette->SetUnderflowAction( TEveRGBAPalette::kLA_Clip);
	fHitPalette->SetOverflowAction( TEveRGBAPalette::kLA_Clip);
	fHitPalette->SetupColorArray();
/* 
Create palette overlay once
*/
	fPalletteOverlay =new  TEveRGBAPaletteOverlay(fHitPalette, 0.7, 0.05, 0.3, 0.02);
	gEve->GetDefaultGLViewer()->AddOverlayElement(fPalletteOverlay);
	fPaletteTitle = new TGLAnnotation(gEve->GetDefaultGLViewer(),paletteText(), 0.7+(0.3/2.0)-0.05, 0.05+0.02+0.04);
	fHitPalette->IncRefCount();
	fHitPalette->IncRefCount();
}
TString paletteText()
{
	if(fDigitIsTime)
		return "PMT Hit Times";
	else
		return "PMT Charge";
}

/******************************************************************************/
// Event Loading
/******************************************************************************/
//______________________________________________________________________________
void fitqun_load_event(int entry)
{
	fiTQun.Process(entry);	
}
void wcsim_load_event(int iTrigger,bool &firstTrackIsNeutrino,bool &secondTrackIsTarget )
{
	wcsim_load_cherenkov(iTrigger);
	wcsim_load_truth_tracks(iTrigger,firstTrackIsNeutrino,secondTrackIsTarget);
}
/*
	Call back function for displaying information about Cherenkov hits
*/
void PrintCherenkovHitInformation(TEveDigitSet* ds, Int_t idx, TObject* obj)
{
	if(obj)
	{
		WCSimRootCherenkovDigiHit *cDigiHit=(WCSimRootCherenkovDigiHit*) obj;
		if(cDigiHit)
		{
			int tubeId=cDigiHit->GetTubeId();
			float Q=cDigiHit->GetQ(); 
			float Time=cDigiHit->GetT(); 
			WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
			int location=pmt.GetCylLoc();					
			cout<<"WCSIM Cherenkov Hit. Id:"<<tubeId<<" Ccharge: "<<cDigiHit->GetQ()
			<<" Time:"<<cDigiHit->GetT()
			<<" Position : ("<< pmt.GetPosition (0)<<","<< pmt.GetPosition (1)<<","<< pmt.GetPosition (2)<<")"
			<<endl;
		}
		else
			cout<<" PrintCherenkovHitInformation called without a WCSimRootCherenkovDigiHit object."<<endl;
	}
	else
		cout<<" PrintCherenkovHitInformation called without an object."<<endl;
}
/*
	Loop over cherenkov digits and add them to Eve
*/
void wcsim_load_cherenkov(int iTrigger){
/* 
	Set up box set for 3D view
*/
	TEveBoxSet*  CherenkovHits= new TEveBoxSet(Form("Cherenkov Hits subevent %i",iTrigger));
	CherenkovHits->SetPalette(fHitPalette);
	CherenkovHits->SetEmitSignals(kTRUE);
	CherenkovHits->SetCallbackFoo(&PrintCherenkovHitInformation);
	CherenkovHits->Reset(TEveBoxSet::kBT_Cone, kFALSE, 64);
/*
	set up box set for unrolled view
*/
	TEveBoxSet*  CherenkovHits2= new TEveBoxSet(Form("CherenkovHits (Unrolled version) %i",iTrigger));
	CherenkovHits2->SetPalette(fHitPalette);
	CherenkovHits2->SetEmitSignals(kTRUE);
	CherenkovHits2->SetCallbackFoo(&PrintCherenkovHitInformation);
	CherenkovHits2->Reset(TEveBoxSet::kBT_Cone, kFALSE, 64);
	float PMTRadius = wcsimrootgeom->GetWCPMTRadius() ;
	int max=wcsimrootTrigger->GetNcherenkovdigihits();
	float maxCharge=0;
	float minT=1e10;
	float maxT=-1e10;
/*
	Loop over hits and add them to the digit set objects
	We need the mean and standard deviation of the data to set a sensible range
	for the colours used in the display/
*/
	float count=0;
	float mean=0;
	float M2=0;
	for (int i = 0; i<max; i++){
		WCSimRootCherenkovDigiHit *cDigiHit = (WCSimRootCherenkovDigiHit*) wcsimrootTrigger->GetCherenkovDigiHits()->At(i);
		int tubeId=cDigiHit->GetTubeId();
		float Q=cDigiHit->GetQ(); 
		float Time=cDigiHit->GetT();
		maxCharge=TMath::Max(Q,maxCharge);
		minT = TMath::Min(minT,Time);
		maxT = TMath::Max(maxT,Time); 
		WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
		double pmtX = pmt.GetPosition (0);
		double pmtY = pmt.GetPosition (1);
		double pmtZ = pmt.GetPosition (2);
		float R=sqrt(pmtX*pmtX+pmtY*pmtY+pmtZ*pmtZ);
		int location=pmt.GetCylLoc();
		double pmtX2=pmtX;
		double pmtY2=pmtY;
		double pmtZ2=pmtZ;
		if(R<100 || location <0 || location >2)
			cout<<"INVALID photo tube location . Tube id:" <<tubeId<<" location "<<location<<" distance from centre = "<<R<<endl;
		else
		{
			UnrollView(&pmtX2,&pmtY2,&pmtZ2,location,maxY,maxZ);
			if(location==1)
			{ 
// barrel
				float lengthXY=sqrt(pmtX*pmtX+pmtY*pmtY);
				float xd=pmtX/lengthXY;
				float yd=pmtY/lengthXY;
				CherenkovHits2->AddCone(TEveVector(pmtX2,pmtY2,pmtZ2),TEveVector(0.0,0.0,1.0) ,PMTRadius ); 
				CherenkovHits ->AddCone(TEveVector(pmtX ,pmtY ,pmtZ) ,TEveVector(xd,yd,0.0)   ,PMTRadius );
			}
			else
			{
				CherenkovHits2->AddCone(TEveVector(pmtX2,pmtY2,pmtZ2),TEveVector(0.0,0.0,1.0) ,PMTRadius ); 
				CherenkovHits ->AddCone(TEveVector(pmtX ,pmtY ,pmtZ) ,TEveVector(0.0,0.0,1.0) ,PMTRadius ); 
			}
			float newValue;
			if(fDigitIsTime)
				newValue=Time;
			else
				newValue=Q;
			CherenkovHits2->DigitValue(newValue);
			CherenkovHits ->DigitValue(newValue);
			CherenkovHits2->DigitId(cDigiHit);	
			CherenkovHits ->DigitId(cDigiHit);
			/*
				Calculate mean and variance of values
			*/
			count++;
			if(count==1)
				mean=newValue;
			float delta=newValue-mean;
			mean+=delta/count;
			float delta2=newValue-mean;
			M2+=delta*delta2;
		}
	}
	/*
		Set ultimate limits for display to the extreme values seen.
		Use mean and standard deviation to set range of values displayed.
	*/
	if(fDigitIsTime)
		fHitPalette->SetLimits(minT,maxT);
	else
		fHitPalette->SetLimits(0.0,maxCharge);
	
	float SD=sqrt(M2/count);
	float pMin=TMath::Max(0.0,mean-1.0*SD);
	float pMax=mean+2.0*SD;
	
	fHitPalette->SetMinMax(pMin,pMax);

	CherenkovHits ->RefitPlex();
	CherenkovHits2->RefitPlex();
	TEveTrans& t = CherenkovHits->RefMainTrans();
	t.SetPos(0.0,0.0,0.0);
	gEve->AddElement(CherenkovHits);
	if(UnrolledScene && CherenkovHits2)
		UnrolledScene->AddElement(CherenkovHits2);
}
/*
   Provide a TString name for a given PDG code
*/
TString ParticleName(int pdgCode)
{
	TString Name;
	if(pdgCode==11)Name+="electron";
	if(pdgCode==-11)Name+="positron";
	if(pdgCode==12)Name+="electron neutrino";
	if(pdgCode==-12)Name+="electron anti-neutrino";
	if(pdgCode==13)Name+="muon";
	if(pdgCode==-13)Name+="anti-muon";
	if(pdgCode==14)Name+="muon neutrino";
	if(pdgCode==-14)Name+="muon anti-neutrino";
	if(pdgCode==2212)Name+="proton";
	if(pdgCode==-2212)Name+="anti-proton";
	if(pdgCode==22)Name+="photon";
	return Name;
}
/*
	Loop over truth tracks and add them to Eve
*/
void wcsim_load_truth_tracks(int iTrigger,bool &firstTrackIsNeutrino,bool &secondTrackIsTarget)
{
// Get the number of tracks
	int ntrack = wcsimrootTrigger->GetNtrack();
	if(ntrack>0)
	{
		int npar = wcsimrootTrigger->GetNpar();
		int i;
		if(iTrigger==0)
		{
			firstTrackIsNeutrino=kFALSE;
			secondTrackIsTarget=kFALSE;
	// Take a first look and decide if we have a neutrino interaction.
	// If so the neutrino and target will need their start position
	// adjusting.
			TObject *element = (wcsimrootTrigger->GetTracks())->At(0);
			WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
			int pdgCode=wcsimroottrack->GetIpnu();
			if(abs(pdgCode)==12 || abs(pdgCode)==14)
				firstTrackIsNeutrino=kTRUE;
			if(ntrack>1)
			{
				TObject *element = (wcsimrootTrigger->GetTracks())->At(1);
				WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
				int pdgCode=wcsimroottrack->GetIpnu();
				if(abs(pdgCode)==2212 
					&& wcsimroottrack->GetStart(0)==0.0
					&& wcsimroottrack->GetStart(1)==0.0
					&& wcsimroottrack->GetStart(2)==0.0
					)
					secondTrackIsTarget=kTRUE;
			}
		}
// Loop through elements in the TClonesArray of WCSimTracks
		TEveElementList* TrueTracks = new TEveElementList(Form("Truth Tracks subevent %i",iTrigger));
		for (i=0; i<ntrack; i++)
		{
			TObject *element = (wcsimrootTrigger->GetTracks())->At(i);
			WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
			bool keep;
			bool draw;
			selectTruthTrack(wcsimroottrack,keep,draw);
			if(!keep)continue;
			int pdgCode=wcsimroottrack->GetIpnu();
			if(pdgCode==0)continue;
			TString Name("Truth "+ParticleName(pdgCode));

			if(iTrigger==0)
			{
				if(i==1 && secondTrackIsTarget) Name+=" (target) ";
				if(i==0 && firstTrackIsNeutrino) Name+=" (beam) ";
			}
			Name=Name+Form(" (%d) ",i);
			THKMCTrack* track=new THKMCTrack(Name);
			float Start[3];
			float Stop[3];
			float Mass=0;
			float Momentum=0;
			double Theta=0;
			double Phi=0;  
			float Energy=0;
			track->SetElementTitle(Name);
			track->SetMainColor(kWhite);
			if(abs(pdgCode)==11)track->SetMainColor(kYellow);
			if(abs(pdgCode)==12)track->SetMainColor(kBlue);
			if(abs(pdgCode)==13)track->SetMainColor(kMagenta);
			if(abs(pdgCode)==14)track->SetMainColor(kGreen);
			if(abs(pdgCode)==2212)track->SetMainColor(kRed);
			Stop[0]=wcsimroottrack->GetStop(0);
			Stop[1]=wcsimroottrack->GetStop(1);
			Stop[2]=wcsimroottrack->GetStop(2);
			Start[0]=wcsimroottrack->GetStart(0);
			Start[1]=wcsimroottrack->GetStart(1);
			Start[2]=wcsimroottrack->GetStart(2);
/*
Implement fix for neutrino and target start positions
*/
			if(i==0 && firstTrackIsNeutrino && iTrigger == 0)
			{
				track->SetNextPoint(wcsimroottrack->GetStop(0),wcsimroottrack->GetStop(1),maxZ*-1.1);
				Start[0]=wcsimroottrack->GetStop(0);
				Start[1]=wcsimroottrack->GetStop(1);
				Start[2]=maxZ*-1.1;
			}
			else
			{
				if(i==1 && secondTrackIsTarget && iTrigger == 0 )
				{
					track->SetNextPoint(wcsimroottrack->GetStop(0),wcsimroottrack->GetStop(1),wcsimroottrack->GetStop(2));
					Start[0]=wcsimroottrack->GetStop(0);
					Start[1]=wcsimroottrack->GetStop(1);
					Start[2]=wcsimroottrack->GetStop(2);
				}
				else
				{
					track->SetNextPoint(wcsimroottrack->GetStart(0),wcsimroottrack->GetStart(1),wcsimroottrack->GetStart(2));
				}
			}
			if(abs(pdgCode)==12 || abs(pdgCode)==14 || abs(pdgCode)==16
				||abs(pdgCode)==22  ||abs(pdgCode)==111  ||abs(pdgCode)==310)
				track->SetLineStyle(2);
			track->SetNextPoint(wcsimroottrack->GetStop(0),wcsimroottrack->GetStop(1),wcsimroottrack->GetStop(2));
			Mass=wcsimroottrack->GetM();
			Momentum=wcsimroottrack->GetP();
			Energy=wcsimroottrack->GetE();
			double PT[3];
			double PTtot=0;
			if(wcsimroottrack->GetP()>0){
				PT[0]=wcsimroottrack->GetP()*wcsimroottrack->GetPdir(0);
				PT[1]=wcsimroottrack->GetP()*wcsimroottrack->GetPdir(1);
				PT[2]=wcsimroottrack->GetP()*wcsimroottrack->GetPdir(2);
				CartesianToPolar(PTtot,Theta,Phi,PT);
			}
			track->SetValues(Start,Stop,Mass,Momentum,Energy,Theta,Phi,draw);
			if(!draw)
			{
				track->SetRnrSelf(false);
			}
			TrueTracks->AddElement(track);
		} 
	 // End of loop over tracks
		if(ntrack>0)gEve->AddElement(TrueTracks);
//Hists->Update();
	}
}	
/******************************************************************************/
// Geometry
/******************************************************************************/
/// Create the geometry obects.
/// There are Full , and Simple (3D)
/// and unrolled (2D)
void createGeometry(TGeoManager* geom, bool UseSimpleGeometry )
{
/* 
Find the maximum values of X,Y,Z 
*/
	maxX=0;
	maxY=0;
	maxZ=0;
	minZ=0;
// I assume z goes negative!
	float maxR=0;
	for(int tubeId=0;tubeId<wcsimrootgeom->GetWCNumPMT();tubeId++)
	{
		WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
		int location=pmt.GetCylLoc();
		if(location !=0 && location !=1 && location !=2)continue;
		double pmtX = pmt.GetPosition (0);
		double pmtY = pmt.GetPosition (1);
		double pmtZ = pmt.GetPosition (2);
		if(pmtX>maxX)maxX=pmtX;
		if(pmtY>maxY)maxY=pmtY;
		if(pmtZ>maxZ)maxZ=pmtZ;   		
		if(pmtZ<minZ)minZ=pmtZ;
		float R=sqrt( pmtX*pmtX+pmtY*pmtY);
		if(R>maxR)maxR=R;
	}
/// Now we know the maximum extent
// MATERIALS, MIXTURES AND TRACKING MEDIA
// Material: world
	TGeoMaterial* worldMaterial = new TGeoMaterial("world", 0.0,0.0,0.0);
	worldMaterial->SetIndex(0);
	worldMaterial->SetTransparency(60);
// Medium: medium0
	Int_t numed   = 0;
  // medium number
/
	auto worldMedium = new TGeoMedium("worldMedium", numed,worldMaterial);
//, par);
	float dx = 2*maxX;
	float dy = 2*maxY;
	float dz = 2*maxZ;
	TGeoShape *pworldbox_1 = new TGeoBBox("worldbox", dx,dy,dz);
	WorldVolume = new TGeoVolume("FullGeometry",pworldbox_1, worldMedium);
	WorldVolume->SetVisLeaves(kTRUE);
	SimpleVolume = new TGeoVolume("SimpleGeometry",pworldbox_1, worldMedium);
	SimpleVolume->SetVisLeaves(kTRUE);
//UnrolledVolume = new TGeoVolume("UnrolledGeometry",pworldbox_1, pMed1);
//UnrolledVolume->SetVisLeaves(kTRUE);
//UnrolledVolume = new TGeoVolume("UnrolledGeometry",pworldbox_1, pMed1);
//UnrolledVolume->SetVisLeaves(kTRUE);
	FlatGeometry = new TEveElementList("Flat Geometry");
	TEveGeoShape *Shape;
// SET TOP VOLUME OF GEOMETRY
	geom->SetMaxVisNodes(wcsimrootgeom->GetWCNumPMT()+1000);
	if(UseSimpleGeometry	)
		geom->SetTopVolume(SimpleVolume);
	else	
		geom->SetTopVolume(WorldVolume);
//FlatGeometry->SetTopVolume(UnrolledVolume);
// SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
// Shape: phototube type: TGeoSphere
	float PMTRadius = wcsimrootgeom->GetWCPMTRadius() ;
	Double_t rmin   = 0.900000*PMTRadius;
	Double_t rmax   = PMTRadius;
	Double_t theta1 = 0.000000;
	Double_t theta2 = 90.000000;
	Double_t phi1   = 0.000000;
	Double_t phi2   = 360.000000;
/*
Full geometry uses hemisphere for tube, UseSimpleGeometry=>just a disk
*/
	Double_t zSize=0.1*PMTRadius;
	cout<<" create a phototube shape of dimenstion 0.0,"<<rmax<<" "<<zSize<<endl;
	TGeoShape *PhotoTubeShape   = new TGeoTube("phototube",0.0,rmax,zSize);  
	TGeoShape *PhotoSphereShape = new TGeoSphere("photosphere",rmin,rmax,theta1, theta2,phi1,phi2);
// Volume: phototube
	auto PhotoTubeVolume = new TGeoVolume("phototube",PhotoTubeShape, worldMedium);
	PhotoTubeVolume->SetVisLeaves(kTRUE);
	PhotoTubeVolume->SetLineColor(kYellow-5);
	auto PhotoSphereVolume = new TGeoVolume("photosphere",PhotoSphereShape, worldMedium);
	PhotoSphereVolume->SetVisLeaves(kTRUE);
	PhotoSphereVolume->SetLineColor(kYellow-5);
	TGeoRotation rota("rot",10,20,30);
	TGeoTranslation trans;
	TGeoCombiTrans *c1 = new TGeoCombiTrans(trans,rota);
/*
Read in location of ALL phototubes and add to the different scenes
*/
	Double_t theta, phi;
	double pmtX2=0;
	double pmtY2=0;
	double pmtZ2=0;
	for(int tubeId=0;tubeId<wcsimrootgeom->GetWCNumPMT();tubeId++)
	{
		WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
		double pmtX = pmt.GetPosition (0);
		double pmtY = pmt.GetPosition (1);
		double pmtZ = pmt.GetPosition (2);
		int location= pmt.GetCylLoc();
		pmtX2=pmtX;
		pmtY2=pmtY;
		pmtZ2=pmtZ;
// Work out x,y in unrolled view
		UnrollView(&pmtX2,&pmtY2,&pmtZ2,location,maxY,maxZ);
		theta=0.0;
		phi=0.0;
		float rad2deg=57.3;
		if(location==1)
		{
			float lengthXY=sqrt(pmtX*pmtX+pmtY*pmtY);
			float xd=pmtX/lengthXY;
			float yd=pmtY/lengthXY;
			TVector3 D(xd,yd,0.0);
			theta=D.Theta()*rad2deg;
			phi=D.Phi()*rad2deg;
		}
		else
		{
			theta=0.0;
			phi=0.0;
			if(location==2)theta=180.0; 
		}
		if(location==0 || location ==1 || location ==2)
		{
/* create a fake geometry object out of tevegeoshapes for the rolled out view*/
			/*
			TGeoTranslation PhototubeUnrolledPositionMatrix("ExlodedShift",pmtX2,pmtY2,pmtZ2);
			auto shape = new TEveGeoShape(Form("Phototube %i",tubeId));
			shape->SetShape(PhotoTubeShape);
			shape->SetTransMatrix(PhototubeUnrolledPositionMatrix);
			shape->SetMainColor(kYellow-5);
			shape->SetMainTransparency(70);
			if(tubeId<100)
			{
				cout<<" adding object to FlatGeometry at "<<pmtX2<<" "<<pmtY2<<" "<<pmtZ2<<endl;
				FlatGeometry->AddElement(shape);
			}
			*/
/* now the 'normal' root geometry objects */
			TGeoRotation TubeRotation("rotation",phi-90.0,theta,0.0);
			//D.Phi(),D.Theta,0.0);
			TGeoTranslation PhototubePositionMatrix("shift",pmtX,pmtY,pmtZ);
			TGeoCombiTrans *ShiftAndTwist = new TGeoCombiTrans(PhototubePositionMatrix,TubeRotation);
//SimpleVolume->AddNode(PhotoTubeVolume, tubeId, ShiftAndTwist);
			WorldVolume-> AddNode(PhotoSphereVolume, tubeId, ShiftAndTwist);
		}
	}
	TGeoVolume* OutlineVolume = createCylinder("Inner Detector",maxR,maxZ);
	OutlineVolume->SetVisLeaves(kTRUE);
	OutlineVolume->SetLineColor(kGray);
	WorldVolume->AddNode(OutlineVolume,0);
	SimpleVolume->AddNode(OutlineVolume,0);
	geom->CloseGeometry();
	/*
	 Create a simple background box object to represent the unrolled cylinder in the unrolled view
	*/

	TEveBox* cylinder=new TEveBox("UnrolledBarell");
	pmtX2=0.0;
	pmtY2=maxY;
	pmtZ2=maxZ;
// Work out x,y in unrolled view
	UnrollView(&pmtX2,&pmtY2,&pmtZ2,1,maxY,maxZ);
	pmtZ2=10000;
	cylinder->SetVertex(0,-pmtX2,-pmtY2,pmtZ2-100);
	cylinder->SetVertex(1,-pmtX2,+pmtY2,pmtZ2-100);
	cylinder->SetVertex(2, pmtX2,+pmtY2,pmtZ2-100);
	cylinder->SetVertex(3, pmtX2,-pmtY2,pmtZ2-100);
	cylinder->SetVertex(4,-pmtX2,-pmtY2,pmtZ2+100);
	cylinder->SetVertex(5,-pmtX2,+pmtY2,pmtZ2+100);
	cylinder->SetVertex(6, pmtX2,+pmtY2,pmtZ2+100);
	cylinder->SetVertex(7, pmtX2,-pmtY2,pmtZ2+100);
	cylinder->SetMainColor(kGray);
	cylinder->SetMainTransparency(80);
	//cylinder->SetLineColor(kGray);
	FlatGeometry->AddElement(cylinder);

	for(int location=0;location<3;location+=2)
	{
		auto arr=new TEveArrow();
		arr->SetTubeR(maxR);
		arr->SetConeR(0.1);
		arr->SetConeL(0.0);
		arr->SetDrawQuality(100);
		arr->SetMainColor(kGray);
		//arr->SetLineColor(kGray);
		arr->SetMainTransparency(80);
		pmtX2=0.0;
		pmtY2=0.0;
		pmtZ2=maxZ;
		UnrollView(&pmtX2,&pmtY2,&pmtZ2,location,maxY,maxZ);
		arr->SetOrigin(pmtX2,pmtY2,10000);
		FlatGeometry->AddElement(arr);
	}
	
}
// Convert x,y,z of phototubes to position in Unrolled view
void       UnrollView(double* pmtX ,double* pmtY,double* pmtZ,int location,float maxY,float maxZ,bool debug)
{
	if(location==0)
	{
//	cout<<" add 2* "<<maxY<<" to "<<*pmtY<<endl;
		*pmtY+=2.2*maxY;
//cout<<" result is "<<*pmtY<<endl;
	}
	if(location==2)
	{
//		cout<<" subtract  2* "<<maxY<<" from "<<*pmtY<<endl;
		*pmtY-=2.2*maxY;
	}	
	if(location==1)
	{
		float angle=atan2(*pmtY,*pmtX)+(3.1415927/2.0);
		if(debug)cout<<" angle is "<<angle<<endl;
		float rho=maxY*angle;
		if(debug)cout<<" x = rho = "<<rho<<endl;
		*pmtX=rho;
		*pmtY=*pmtZ;		
	}
	*pmtZ=0.0;
//cout<<" x,y,z of Unrolled phototube "<<*pmtX<<" "<<*pmtY<<" "<<*pmtZ<<endl;
	float xshift=maxY*(3.1415927);
	float x=*pmtX;
	if(x>xshift)x=x-(xshift*2);
	*pmtX=x;
//cout<<" x,y,z of Unrolled phototube "<<*pmtX<<" "<<*pmtY<<" "<<*pmtZ<<endl;
	return;
}
TGeoVolume* createCylinder(TString name,float radius,float height)
{
	TGeoMaterial* pMaterial = new TGeoMaterial(name, 0,0,0);
	pMaterial->SetIndex(0);
	pMaterial->SetTransparency(86);
	TGeoMedium* pMedium = new TGeoMedium(name, 1,pMaterial);
	TGeoXtru *xtru = new TGeoXtru(2);
	int nvertices=1000;
	double x[nvertices];
	double y[nvertices];
#define PI 3.14159
	float stepSize=2*PI/(float)nvertices;
	float angle=0.0;
	for(int step=0;step<nvertices;step++)
	{
		x[step]=radius*cos(angle);
		y[step]=radius*sin(angle);
		angle+=stepSize;
	}
	xtru->DefinePolygon(nvertices,x,y);
	xtru->DefineSection(0,-height);
	xtru->DefineSection(1,height);
	return new TGeoVolume(name,xtru,pMedium);
}
TEveViewer* New2dView(TString name,TGLViewer::ECameraType type, TEveScene* scene)
{ 
	TEveViewer* View =gEve->SpawnNewViewer(name,name);
	View->AddScene(scene); 
 // add the special scene that only applies to this view 
	View->AddScene(gEve->GetGlobalScene()); 
// add the geometry information
	View->GetGLViewer()->SetCurrentCamera(type);
	View->GetGLViewer()->GetLightSet()->SetLight( TGLLightSet::kLightFront,kFALSE);
	View->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
	return View;
}
//______________________________________________________________________________
void update_html_summary(int iTrigger,WCSimRootTrigger * wcsimrootTrigger,bool firstTrackIsNeutrino,bool secondTrackIsTarget)
{
// Update summary of current event.
	int ntrack = wcsimrootTrigger->GetNtrack();
	if(ntrack==0)return;
	int nused=0;
	HtmlObjTable *table;
	if(iTrigger==0)
		table = fgHtmlSummary->AddTable(Form("Truth Tracks subevent: %i",iTrigger), 13,kTRUE,"first"); 
	else
		table = fgHtmlSummary->AddTable(Form("Truth Tracks subevent: %i",iTrigger), 13,kTRUE,"other"); 
	table->SetLabel(0, "Type");
	table->SetLabel(1, "Start X");
	table->SetLabel(2, "Start Y");
	table->SetLabel(3, "Start Z");
	table->SetLabel(4, "Stop X");	
	table->SetLabel(5, "Stop Y");	
	table->SetLabel(6, "Stop Z");	
	table->SetLabel(7, "Mass ");	
	table->SetLabel(8, "Momentum ");	
	table->SetLabel(9, "Energy ");	
	table->SetLabel(10, "Theta");	
	table->SetLabel(11, "Phi");	
	table->SetLabel(12, "Drawn");	
	int i;
	int used=0;
// Loop through elements in the TClonesArray of WCSimTrack
	for (i=0; i<ntrack; i++)
	{
		TObject *element = (wcsimrootTrigger->GetTracks())->At(i);
		WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
		int pdgCode=wcsimroottrack->GetIpnu();
		if(pdgCode==0)continue;
		TString Name(Form(" #%d (%d) ",i,pdgCode));
		if(pdgCode==11)Name+="electron";
		if(pdgCode==-11)Name+="positron";
		if(pdgCode==12)Name+="electron neutrino";
		if(pdgCode==-12)Name+="electron anti-neutrino";
		if(pdgCode==13)Name+="muon";
		if(pdgCode==-13)Name+="anti-muon";
		if(pdgCode==14)Name+="muon neutrino";
		if(pdgCode==-14)Name+="muon anti-neutrino";
		if(pdgCode==2212)Name+="proton";
		if(pdgCode==-2212)Name+="anti-proton";
		if(pdgCode==22)Name+="photon";
//Name=Name+Form(" (%d) ",i);
		if(i==0 && firstTrackIsNeutrino) Name+=" (beam) ";
		if(i==1 && secondTrackIsTarget) Name+=" (target) ";
		table->SetRowName(used, Name);
		for(int l=0;l<3;l++){
			table->SetValue(l+1,used,wcsimroottrack->GetStart(l));
			table->SetValue(l+4,used,wcsimroottrack->GetStop(l));
		}
		if(i==0 && firstTrackIsNeutrino && iTrigger==0)
		{
			table->SetValue(1,used,wcsimroottrack->GetStop(0));
			table->SetValue(2,used,wcsimroottrack->GetStop(1));
			table->SetValue(3,used,-1.1*maxZ);
		}
		if(i==1 && secondTrackIsTarget && iTrigger==0)
		{
			table->SetValue(1,used,wcsimroottrack->GetStop(0));
			table->SetValue(2,used,wcsimroottrack->GetStop(1));
			table->SetValue(3,used,wcsimroottrack->GetStop(2));
		}
//	Int_t     GetIpnu() const { return fIpnu;}
		table->SetValue(7,used,wcsimroottrack->GetM());
		table->SetValue(8,used,wcsimroottrack->GetP());
		table->SetValue(9,used,wcsimroottrack->GetE());
		double PT[3];
		double PTtot=0;
		double Theta=0;
		double Phi=0;
		if(wcsimroottrack->GetP()>0){
			PT[0]=wcsimroottrack->GetP()*wcsimroottrack->GetPdir(0);
			PT[1]=wcsimroottrack->GetP()*wcsimroottrack->GetPdir(1);
			PT[2]=wcsimroottrack->GetP()*wcsimroottrack->GetPdir(2);
			CartesianToPolar(PTtot,Theta,Phi,PT);
		}
		table->SetValue(10,used,Theta);
		table->SetValue(11,used,Phi);
		bool keep;
		bool draw;
		selectTruthTrack(wcsimroottrack,keep,draw);
		if(draw)
			table->SetStringValue(12,used,"drawn");
		else
			table->SetStringValue(12,used,"not drawn");
//for(int l=0;l<3;l++)
//	table->SetValue(10+l,used,wcsimroottrack->GetPdir(l));
		used++;
	} 
// End of loop over tracks
//	
}
void	selectTruthTrack(WCSimRootTrack * wcsimroottrack,bool& keep,bool& draw)
{
	float Energy=wcsimroottrack->GetE();
	keep=true;
	draw=true;
	if(Energy<minTruthTrackEnergyToDraw)draw=false;			
	if(Energy<minTruthTrackEnergyToKeep)keep=false;			
}
