// Based on alice_esd.C from ROOT tutorials
// Modified for hyperk by Alex Finch
#include "fitqun_def.h"
#ifdef  FITQUNEXISTS
fitQunDisplay fiTQun;
#endif
WCSimRootGeom * wcsimrootgeom;
WCSimRootEvent * wcsimrootEvent;
WCSimRootTrigger * wcsimrootTrigger;
TFile* WCSimFile, *fiTQunFile;
TTree * wcsimGeoT;
TTree * wcsimT;
TTree*	fiTQunTree;
float maxX,maxY,maxZ,minZ;
TGeoVolume *WorldVolume,*SimpleVolume;
TEveElementList *FlatGeometry;
TEveScene*  UnrolledScene;
TEveScene*  flatGeometryScene;
TEveViewer* UnrolledView;

bool FITQUN,fAccumulateEvents,fDigitIsTime;

void       make_gui();
void       load_event();
void       createGeometry(bool flatTube=kFALSE);
void       wcsim_load_event(int iTrigger);
void       fitqun_load_event(int entry);
void       UnrollView(double* pmtX ,double* pmtY,double* pmtZ,int location,float maxY,float maxZ);
TEveViewer* New2dView(TString name,TGLViewer::ECameraType type, TEveScene* scene);
void update_html_summary(int iTrigger,WCSimRootTrigger * wcsimrootTrigger,bool firstTrackIsNeutrino,bool secondTrackIsTarget);


bool fSimpleGeometry;
TGTextButton *fGeometrySwitchButton ;
TGCheckButton *fAccumulateEventsBox;
TGCheckButton *fColourIsTimeBox;
Int_t event_id       = 0; // Current event id.

TEveTrackList *gTrackList = 0;

TEveGeoTopNode *geomRoot;

#include "hyperk_esd_html_summary.h"

HtmlSummary *fgHtmlSummary = 0;
TGHtml      *fgHtml        = 0;
#include "Picker.h"
Picker* fPicker;

/******************************************************************************/
// Initialization and steering functions
/******************************************************************************/

//______________________________________________________________________________
void hyperk_esd()
{
	// Main function, initializes the application.
	fAccumulateEvents=kTRUE;
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
	
	//	WCSimFile = new TFile("/home/aleph/ajf/hyperk/installationFromGit/hk-WCSim/HyperK_muminus_1000MeV_random_4pi_000.root");
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
	// for fast development, hardwire a file and comment out the block above
	//fiTQunFile = new TFile("/home/aleph/ajf/hyperk/installationFromGit/hk-fitqun/fitqun_HyperK_muminus_1000MeV_random_4pi_000.root");
	
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
	TEveManager::Create(kTRUE,"V");
	gEve->GetBrowser()->SetTabTitle("3D View",TRootBrowser::kRight,0);
	gEve->GetDefaultViewer()->SetName("3D View");
	gEve->GetDefaultViewer()->GetGLViewer()->SetResetCamerasOnUpdate(kFALSE);
	
	fPicker= new Picker();
	gEve->GetDefaultGLViewer()->Connect("Clicked(TObject*)", "Picker", fPicker, 
		"Picked(TObject*)");
	/*
	Initialise the html summary tab
	*/
	fgHtmlSummary = new HtmlSummary("HyperK Event Display Summary Table");
	slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
	fgHtml = new TGHtml(0, 100, 100);
	TEveWindowFrame *wf = slot->MakeFrame(fgHtml);
	fgHtml->MapSubwindows();
	wf->SetElementName("Summary");
	/*
	Initialise the geometry objects
	*/
	gSystem->Load("libGeom");
	new TGeoManager("HyperK", "HyperK Detector");
	createGeometry(kTRUE);
	gGeoManager->SetTopVolume(SimpleVolume);
	gGeoManager->SetTopVisible();
	TGeoNode* node = gGeoManager->GetTopNode();
	geomRoot = new TEveGeoTopNode(gGeoManager, node);
	geomRoot->SetVisLevel(4);
	geomRoot->GetNode()->GetVolume()->SetVisibility(kFALSE);
	gEve->AddGlobalElement(geomRoot);   	
	/*
	Set up the event control GUI
	*/
	gEve->GetBrowser()->GetTabRight()->SetTab(1);
	make_gui();
	/*
	Initialise the unrolled geometry view
	*/
	gEve->GetBrowser()->GetTabRight()->SetTab(0);
	UnrolledScene = gEve->SpawnNewScene("Unrolled Event");
	flatGeometryScene = gEve->SpawnNewScene("Unrolled Geometry");
	UnrolledView = New2dView("Unrolled View",TGLViewer::kCameraOrthoXnOY,UnrolledScene);   	
	flatGeometryScene->AddElement(FlatGeometry);
	UnrolledView->AddScene(flatGeometryScene);
	TEveSceneInfo* gSI= (TEveSceneInfo*) (UnrolledView->FindChild("SI - Geometry scene"));
	if(gSI!=NULL)gSI->Delete();
	UnrolledView->GetGLViewer()->Connect("Clicked(TObject*)", "Picker", fPicker,"Picked(TObject*)");
	
	
	/*
	Initialise FitQun
	*/
	if(FITQUN){
		fiTQun.setLimits(maxX,maxY,maxZ,minZ);
		fiTQun.SetWCSimGeom(wcsimrootgeom);
		fiTQun.maxY=maxY;
	}
	/*        
	get the WCSim information
	*/
	wcsimT = (TTree*)WCSimFile ->Get("wcsimT");
	wcsimrootEvent = new WCSimRootEvent ();
	wcsimT -> SetBranchAddress ("wcsimrootevent" ,&wcsimrootEvent);   	
	wcsimT->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);
	wcsimT->GetEvent(0);
	cout<<" there are "<<wcsimT->GetEntries()<<" events "<<endl;
	/*
	load the first event
	*/
	load_event();
	/*
	Get Eve started
	*/
	gEve->GetDefaultGLViewer()->UpdateScene();
	gEve->Redraw3D(kTRUE); // Reset camera after the first event has been shown.
	
}
void CartesianToPolar(double &mom,double &theta,double &phi,double p[3]){
	mom=sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
	theta=acos(p[2]/mom);
	phi=atan2(p[1],p[0]);
}
//______________________________________________________________________________

//______________________________________________________________________________
void load_event()
{
	// Load event specified in global event_id.
	// The contents of previous event are removed.
	
	//printf("Loading event %d.\n", event_id);
	
	gEve->GetViewers()->DeleteAnnotations();
	TEveEventManager* CurrentEvent =gEve->GetCurrentEvent();
	if(! fAccumulateEvents ){
		if(CurrentEvent != 0)CurrentEvent->DestroyElements();
		if( UnrolledScene !=0)UnrolledScene->DestroyElements();	
	}
	
	wcsimT->GetEvent(event_id);	
	fgHtmlSummary->Clear("D");
	for(int iTrigger=0;iTrigger<wcsimrootEvent->GetNumberOfEvents();iTrigger++)
	{
		wcsimrootTrigger = wcsimrootEvent->GetTrigger(iTrigger); 
		bool firstTrackIsNeutrino,secondTrackIsTarget;
		wcsim_load_event(iTrigger,firstTrackIsNeutrino,secondTrackIsTarget);
		update_html_summary(iTrigger,wcsimrootTrigger,firstTrackIsNeutrino,secondTrackIsTarget);
	}
	if(FITQUN)fitqun_load_event(event_id);
	if(FITQUN)fiTQun.CreateTable( fgHtmlSummary);
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
//______________________________________________________________________________
// 
// EvNavHandler class is needed to connect GUI signals.

class EvNavHandler
{
public:
	void Fwd()
	{
		if (event_id < wcsimT->GetEntries()) {
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
	void ColourIsTimeBoxChanged(Bool_t s)
	{
		
		fDigitIsTime=s;
		for(int iTrigger=0;iTrigger<wcsimrootEvent->GetNumberOfEvents();iTrigger++)
		{
			//if(iTrigger==0)cout<<" main event "<<endl;
			//if(iTrigger>0)cout<<" delayed sub Event "<<iTrigger<<endl;
			wcsimrootTrigger = wcsimrootEvent->GetTrigger(iTrigger); 
			reload_wcsim_hits(iTrigger);
		}
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
		{// simple to full
			//	f3Dchoice->SetEnabled(kTRUE);
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
		{// full to simple
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
	TGGroupFrame* Group;
	{
		Group = new TGGroupFrame(fCanvasWindow->GetContainer(),"Event Navigation");
		TGHorizontalFrame* hf = new TGHorizontalFrame(Group);
		{
			
			TString icondir( Form("%s/icons/", gSystem->Getenv("ROOTSYS")) );
			TGPictureButton* b = 0;
			EvNavHandler    *fh = new EvNavHandler;
			
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
		fDigitIsTime=kFALSE; 
		Group = new TGGroupFrame(fCanvasWindow->GetContainer(),"Colour for Digits");
		hf = new TGHorizontalFrame(Group);
		{
			
			fColourIsTimeBox = new TGCheckButton(hf, "Digit Colour is Time.",1);
			fColourIsTimeBox->SetState(kButtonUp);
			fColourIsTimeBox->SetToolTipText("If this is checked, digit colour represents time, otherwise it is charge.");
			fColourIsTimeBox->Connect("Toggled(Bool_t)", "EvNavHandler", fh, "ColourIsTimeBoxChanged(Bool_t)");
			
			
			hf->AddFrame(fColourIsTimeBox);
		}
		Group->AddFrame(hf);
		fCanvasWindow->AddFrame(Group);
	}
	
	
	fCanvasWindow->AddFrame(Group);
	frmMain->AddFrame(fCanvasWindow,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,
		0, 0, 2, 2));
	
	frmMain->MapSubwindows();
	frmMain->Resize();
	frmMain->MapWindow();
	
	browser->StopEmbedding();
	browser->SetTabTitle("Event Control", 0);
}
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
Loop over cherenkov digits and add them to Eve
*/
wcsim_load_cherenkov(int iTrigger){
	/*
	Loop over all cherenkov hits and add them to Eve event
	*/
	gStyle->SetPalette(1, 0);
	
	TEveRGBAPalette* pal = new TEveRGBAPalette(0, 30);
	
	
	TEveBoxSet*  CherenkovHits= new TEveBoxSet(Form("Cherenkov Hits subevent %i",iTrigger));
	CherenkovHits->SetPalette(pal);
	pal->SetFixColorRange(kTRUE);
	pal->SetOverflowAction( TEveRGBAPalette::kLA_Clip);
	CherenkovHits->Reset(TEveBoxSet::kBT_Cone, kFALSE, 64);
	TEveBoxSet*  CherenkovHits2= new TEveBoxSet(Form("CherenkovHits (Unrolled version) %i",iTrigger));
	CherenkovHits2->SetPalette(pal);
	CherenkovHits2->Reset(TEveBoxSet::kBT_Cone, kFALSE, 64);
	
	float PMTRadius = wcsimrootgeom->GetWCPMTRadius() ;
	int max=wcsimrootTrigger->GetNcherenkovdigihits();
	float maxCharge=0;
	float minT=1e10;
	float maxT=-1e10;
	for (int i = 0; i<max; i++){
		WCSimRootCherenkovDigiHit *cDigiHit = (WCSimRootCherenkovDigiHit*) wcsimrootTrigger->GetCherenkovDigiHits()->At(i);
		float Q=cDigiHit->GetQ(); 
		float Time=cDigiHit->GetT(); 
		maxCharge=TMath::Max(Q,maxCharge);
		minT = TMath::Min(minT,Time);
		maxT = TMath::Max(maxT,Time);
	}
	if(fDigitIsTime)
	{
		pal->SetLimits(minT,maxT);
		pal->SetMin(minT);
		pal->SetMax(maxT);
	}
	else
	{
		pal->SetLimits(0,maxCharge);
		pal->SetMin(0);
		pal->SetMax(maxCharge);
	}	
	//cout<<" Cherenkov digits, larges  charge seen is "<<maxCharge;
	//cout<<". Times run from "<<minT<<" to "<<maxT<<endl;
	for (int i = 0; i<max; i++){
		WCSimRootCherenkovDigiHit *cDigiHit = (WCSimRootCherenkovDigiHit*) wcsimrootTrigger->GetCherenkovDigiHits()->At(i);
		int tubeId=cDigiHit->GetTubeId();
		float Q=cDigiHit->GetQ(); 
		float Time=cDigiHit->GetT(); 
		WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
		double pmtX = pmt.GetPosition (0);
		double pmtY = pmt.GetPosition (1);
		double pmtZ = pmt.GetPosition (2);
		int location=pmt.GetCylLoc();
		double pmtX2=pmtX;
		double pmtY2=pmtY;
		double pmtZ2=pmtZ;
		UnrollView(&pmtX2,&pmtY2,&pmtZ2,location,maxY,maxZ);
		
		if(location==1)
		{
			float lengthXY=sqrt(pmtX*pmtX+pmtY*pmtY);
			float xd=pmtX/lengthXY;
			float yd=pmtY/lengthXY;
			CherenkovHits2->AddCone(TEveVector(pmtX2,pmtY2,pmtZ2),TEveVector(0.0,0.0,1.0) ,PMTRadius ); 
			CherenkovHits->AddCone(TEveVector(pmtX,pmtY,pmtZ),TEveVector(xd,yd,0.0) ,PMTRadius);
		}
		else
		{
			CherenkovHits2->AddCone(TEveVector(pmtX2,pmtY2,pmtZ2),TEveVector(0.0,0.0,1.0) ,PMTRadius ); 
			CherenkovHits->AddCone(TEveVector(pmtX,pmtY,pmtZ),TEveVector(0.0,0.0,1.0) ,PMTRadius ); 
		}
		if(fDigitIsTime)
		{
			CherenkovHits2->DigitValue(Time);
			CherenkovHits->DigitValue(Time);
		}
		else
		{
			CherenkovHits2->DigitValue(Q);
			CherenkovHits->DigitValue(Q);
		}
	}
	
	CherenkovHits->RefitPlex();
	CherenkovHits2->RefitPlex();
	
	TEveTrans& t = CherenkovHits->RefMainTrans();
	t.SetPos(0.0,0.0,0.0);
	gEve->AddElement(CherenkovHits);
	UnrolledScene->AddElement(CherenkovHits2);
}
/*
Loop over truth tracks and add them to Eve
*/
wcsim_load_truth_tracks(int iTrigger,bool &firstTrackIsNeutrino,bool &secondTrackIsTarget)
{
	// Get the number of tracks
	int ntrack = wcsimrootTrigger->GetNtrack();
	//printf("ntracks=%d\n",ntrack);
	if(ntrack>0)
	{
		
		int npar = wcsimrootTrigger->GetNpar();
		int i;
		if(iTrigger==0){
			firstTrackIsNeutrino=kFALSE;
			secondTrackIsTarget=kFALSE;
			// Take a first look and decide if we have a neutrino interaction.
			// If so the neutrino and target will need their start position
			// adjusting.
			
			TObject *element = (wcsimrootTrigger->GetTracks())->At(0);
			WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
			int pdgCode=wcsimroottrack->GetIpnu();
			if(abs(pdgCode)==12 || abs(pdgCode==14))
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
				)secondTrackIsTarget=kTRUE;
			}
		}
		// Loop through elements in the TClonesArray of WCSimTracks
		TEveElementList* TrueTracks = new TEveElementList(Form("Truth Tracks subevent %i",iTrigger));
		for (i=0; i<ntrack; i++)
		{
			TObject *element = (wcsimrootTrigger->GetTracks())->At(i);
			
			WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
			int pdgCode=wcsimroottrack->GetIpnu();
			if(pdgCode==0)continue;
			TString Name("Truth ");
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
			if(iTrigger==0)
			{
				if(i==1 && secondTrackIsTarget) Name+=" (target) ";
				if(i==0 && firstTrackIsNeutrino) Name+=" (beam) ";
			}
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
			track->SetValues(Start,Stop,Mass,Momentum,Energy,Theta,Phi);
			
			TrueTracks->AddElement(track);
			
		}  // End of loop over tracks
		if(ntrack>0)gEve->AddElement(TrueTracks);
		//Hists->Update();
	}
	
}	
void createGeometry(bool flatTube)
{
	/* 
	Find the maximum values of X,Y,Z 
	*/
	maxX=0;
	maxY=0;
	maxZ=0;
	minZ=0; // I assume z goes negative!
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
	}
	cout<<" Maximum values x,y,x "<<maxX<<" "<<maxY<<" "<<maxZ<<endl;
	// MATERIALS, MIXTURES AND TRACKING MEDIA
	// Material: world
	Double_t a       = 0.000000;
	Double_t z       = 0.000000;
	Double_t density = 0.000000;
	Double_t radl    = 1000000000000000019884624838656.000000;
	Double_t absl    = 1000000000000000019884624838656.000000;
	TGeoMaterial* pMat1 = new TGeoMaterial("world", a,z,density,radl,absl);
	pMat1->SetIndex(0);
	pMat1->SetTransparency(60);
	// Medium: medium0
	Int_t numed   = 0;  // medium number
	Double_t par[20];
	par[0]  = 0.000000; // isvol
	par[1]  = 0.000000; // ifield
	par[2]  = 0.000000; // fieldm
	par[3]  = 0.000000; // tmaxfd
	par[4]  = 0.000000; // stemax
	par[5]  = 0.000000; // deemax
	par[6]  = 0.000000; // epsil
	par[7]  = 0.000000; // stmin
	pMed1 = new TGeoMedium("medium0", numed,pMat1, par);
	
	
	float dx = 2*maxX;
	float dy = 2*maxY;
	float dz = 2*maxZ;
	TGeoShape *pworldbox_1 = new TGeoBBox("worldbox", dx,dy,dz);
	// Volume: volume0
	WorldVolume = new TGeoVolume("FullGeometry",pworldbox_1, pMed1);
	WorldVolume->SetVisLeaves(kTRUE);
	// Volume: volume0
	SimpleVolume = new TGeoVolume("SimpleGeometry",pworldbox_1, pMed1);
	SimpleVolume->SetVisLeaves(kTRUE);
	
	//UnrolledVolume = new TGeoVolume("UnrolledGeometry",pworldbox_1, pMed1);
	//UnrolledVolume->SetVisLeaves(kTRUE);
	
	//UnrolledVolume = new TGeoVolume("UnrolledGeometry",pworldbox_1, pMed1);
	//UnrolledVolume->SetVisLeaves(kTRUE);
	FlatGeometry = new TEveElementList("Flat Geometry");
	
	TEveGeoShape *Shape;
	
	// SET TOP VOLUME OF GEOMETRY
	gGeoManager->SetMaxVisNodes(wcsimrootgeom->GetWCNumPMT()+1000);
	if(flatTube)	
		gGeoManager->SetTopVolume(SimpleVolume);
	else	
		gGeoManager->SetTopVolume(WorldVolume);
	
	//FlatGeometry->SetTopVolume(UnrolledVolume);
	//FlatGeometry->GetTopNode()->ls();
	
	gGeoManager->GetTopNode()->ls();
	
	// SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
	// Shape: phototube type: TGeoSphere
	float PMTRadius = wcsimrootgeom->GetWCPMTRadius() ;
	cout<<" Phototube radius is "<<PMTRadius<<endl;
	Double_t rmin   = 0.900000*PMTRadius;
	Double_t rmax   = PMTRadius;
	Double_t theta1 = 0.000000;
	Double_t theta2 = 90.000000;
	Double_t phi1   = 0.000000;
	Double_t phi2   = 360.000000;
	/*
	Full geometry uses hemisphere for tube,  flatTube means just a disk
	*/
	Double_t zSize=0.1*PMTRadius;
	TGeoShape *PhotoTubeShape   = new TGeoTube("phototube",0.0,rmax,zSize);  
	TGeoShape *PhotoSphereShape = new TGeoSphere("photosphere",rmin,rmax,theta1, theta2,phi1,phi2);
	// Volume: phototube
	PhotoTubeVolume = new TGeoVolume("phototube",PhotoTubeShape, pMed1);
	PhotoTubeVolume->SetVisLeaves(kTRUE);
	PhotoTubeVolume->SetLineColor(kYellow-5);
	
	PhotoSphereVolume = new TGeoVolume("photosphere",PhotoSphereShape, pMed1);
	PhotoSphereVolume->SetVisLeaves(kTRUE);
	PhotoSphereVolume->SetLineColor(kYellow-5);
	TGeoRotation rota("rot",10,20,30);
	TGeoTranslation trans;
	TGeoCombiTrans *c1 = new TGeoCombiTrans(trans,rota);
	
	/*
	Read in location of ALL phototubes
	*/
	//bool first=true;
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
		
		//cout<<" location "<<location<<endl;
		//cout<<" x,y,z of phototube          "<<pmtX<<" "<<pmtY<<" "<<pmtZ<<endl;
		pmtX2=pmtX;
		pmtY2=pmtY;
		pmtZ2=pmtZ;
		
		UnrollView(&pmtX2,&pmtY2,&pmtZ2,location,maxY,maxZ);
		//cout<<" x,y,z of Unrolled phototube "<<pmtX<<" "<<pmtY<<" "<<pmtZ<<endl;
		
		
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
		//cout<<" tube> "<<tubeId<<endl;
		if(location==0 || location ==1 || location ==2){
			/* create a fake geometry object out of tevegeoshapes for the rolled out view*/
			//cout<<" create translation using "<<pmtX2<<" "<<pmtY2<<" "<<pmtZ2<<endl;
			TGeoTranslation PhototubeUnrolledPositionMatrix("ExlodedShift",pmtX2,pmtY2,pmtZ2);
			shape = new TEveGeoShape(Form("Phototube %i",tubeId));
			shape->SetShape(PhotoTubeShape);
			shape->SetTransMatrix(PhototubeUnrolledPositionMatrix);
			shape->SetMainColor(kYellow-5);
			shape->SetMainTransparency(70);
			FlatGeometry->AddElement(shape);
			/* now the 'normal' root geometry objects */
			TGeoRotation TubeRotation("rotation",phi-90.0,theta,0.0);//D.Phi(),D.Theta,0.0);
			TGeoTranslation PhototubePositionMatrix("shift",pmtX,pmtY,pmtZ);
			TGeoCombiTrans *ShiftAndTwist = new TGeoCombiTrans(PhototubePositionMatrix,TubeRotation);
			SimpleVolume->AddNode(PhotoTubeVolume, tubeId, ShiftAndTwist);
			WorldVolume-> AddNode(PhotoSphereVolume, tubeId, ShiftAndTwist);
			//		cout<<" added to geometry "<<endl;
		}
		//	else
		//		cout<<" NOT added to geometry, location = "<<location<<endl;
		
	}
	// CLOSE GEOMETRY
	gGeoManager->CloseGeometry();
	//pmtX2=500.0;
	//pmtY2=0.0;
	//pmtZ2=0.0;
	
}
// Convert x,y,z of phototubes to position in Unrolled view
void       UnrollView(double* pmtX ,double* pmtY,double* pmtZ,int location,float maxY,float maxZ)
{
	if(location==0)
	{
		//	cout<<" add 2* "<<maxY<<" to "<<*pmtY<<endl;
		*pmtY+=2.2*maxY;
		//cout<<" result is "<<*pmtY<<endl;
	}
	if(location==2)
	{//		cout<<" subtract  2* "<<maxY<<" from "<<*pmtY<<endl;
		
		
		*pmtY-=2.2*maxY;
	}	
	if(location==1)
	{
		float angle=atan2(*pmtY,*pmtX)+(3.1415927/2.0);
		float rho=maxY*angle;
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

TEveViewer* New2dView(TString name,TGLViewer::ECameraType type, TEveScene* scene)
{ 
	TEveViewer* View =gEve->SpawnNewViewer(name,name);
	View->AddScene(scene);  // add the special scene that only applies to this view 
	View->AddScene(gEve->GetGlobalScene()); // add the geometry information
	View->GetGLViewer()->SetCurrentCamera(type);
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
	/*
	for (int i=0; i<ntrack; i++)
	{
	TObject *element = (wcsimrootTrigger->GetTracks())->At(i);
	WCSimRootTrack *wcsimroottrack = dynamic_cast<WCSimRootTrack*>(element);
	int pdgCode=wcsimroottrack->GetIpnu();
	if(pdgCode==0)continue;
	nused++;
	}
	
	*/
	
	HtmlObjTable *table;
	//fgHtmlSummary->Clear("D");
	if(iTrigger==0)
		table = fgHtmlSummary->AddTable(Form("Truth Tracks subevent: %i",iTrigger), 12,kTRUE,"first"); 
	else
		table = fgHtmlSummary->AddTable(Form("Truth Tracks subevent: %i",iTrigger), 12,kTRUE,"other"); 
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
	int i;
	int used=0;
	// Loop through elements in the TClonesArray of WCSimTracks
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
		
		//for(int l=0;l<3;l++)
		//	table->SetValue(10+l,used,wcsimroottrack->GetPdir(l));
		used++;
	}  // End of loop over tracks
	//	
	
}
