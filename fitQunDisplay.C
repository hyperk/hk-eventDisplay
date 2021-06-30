#define fitQunDisplay_cxx
// The class definition in fitQunDisplay.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("fitQunDisplay.C")
// Root > T->Process("fitQunDisplay.C","some options")
// Root > T->Process("fitQunDisplay.C+")
//
#include <algorithm>
#include "fitQunDisplay.h"
#include <TH2.h>
#include <TStyle.h>
#include "THKGamma.h"
#include "THKLine.h"
#include <iostream>
#include "TEveVector.h"
#include <TGLViewer.h>
using std::cout;
using std::endl;
void fitQunDisplay::Begin(TTree * /*tree*/)
{
	// The Begin() function is called at the start of the query.
	// When running with PROOF Begin() is only called on the client.
	// The tree argument is deprecated (on PROOF 0 is passed).
	
	TString option = GetOption();
	
}

void fitQunDisplay::SlaveBegin(TTree * /*tree*/)
{
	// The SlaveBegin() function is called after the Begin() function.
	// When running with PROOF SlaveBegin() is called on each slave server.
	// The tree argument is deprecated (on PROOF 0 is passed).
	
	TString option = GetOption();
	
}
void fitQunDisplay::describe_event(int entry)
{
	
	cout<<endl<<endl<<" ================================================== "<<endl<<"     Entry "<<entry<<endl<<" ================================================== "<<endl;
	
	cout<<endl<<"Time-window information"<<endl
	<<"-------------------------"<<endl;
	cout<<fqntwnd<<" 	Number of time windows (good clusters) in this event"<<endl;
	for(int ifqntwnd =0;ifqntwnd<fqntwnd;ifqntwnd++){
		cout<<fqtwnd_iclstr[ifqntwnd]<<" 	Cluster index of the time window(corresponds to cluster_ncand)"<<endl;
		cout<<fqtwnd_npeak[ifqntwnd]<<" 	Number of peaks(sub-events) in the time window"<<endl;
		cout<<fqtwnd_prftt0[ifqntwnd]<<" 	Pre-fitter vertex time"<<endl;
		cout<<fqtwnd_prftpos[ifqntwnd][0]<<" "<<fqtwnd_prftpos[ifqntwnd][1]<<" "<<fqtwnd_prftpos[ifqntwnd][2]<<" 	Pre-fitter vertex position"<<endl;
		cout<<fqtwnd[ifqntwnd][0]<<fqtwnd[ifqntwnd][1]<<" 	Time window start/end time"<<endl;
		for(int i = 0;i<fqntwnd;i++){
			cout<<fqtwnd_peakt0[ifqntwnd][i]<<" 	Time of each sub-event in the time window"<<endl;
			cout<<fqtwnd_peakiness[ifqntwnd][i]<<" 	Vertex goodness parameter evaluated at the peak position"<<endl;
		}
	}
	
	
	cout<<endl<<"Sub-event information"<<endl
	<<"-------------------------"<<endl;
	cout<<fqnse<<" 	Total number of subevents in the event"<<endl;
	for(int ifqnse = 0;ifqnse<fqnse;ifqnse++){
		cout<<fqitwnd[ifqnse]<<" 	Index of the time window to which the subevent belongs"<<endl;
		cout<<fqipeak[ifqnse]<<" 	Peak index within the time window"<<endl;
		cout<<fqnhitpmt[ifqnse]<<" 	Number of hits in each subevent"<<endl;
		cout<<fqtotq[ifqnse]<<" 	Total charge in each subevent"<<endl;
		cout<<fq0rtotmu[ifqnse]<<" 	Total predicted charge for the 0-ring hypothesis in each subevent - these variables are the result of evaluating the likelihood function with a particle that is below Cherenkov threshold."<<endl;
		cout<<fq0rnll[ifqnse]<<" 	-log(L) value for the 0-ring hypothesis in each subevent"<<endl;
		cout<<fqn50[ifqnse]<<" 	n50 - In the TOF-corrected hit time distribution, number of hits within the 50ns time window centred at vertex time(1R electron fit vertex is used)"<<endl;
		cout<<fqq50[ifqnse]<<" 	q50 - Total charge of hits included in n50 above"<<endl;
	}
	cout<<endl<<" Number of candidate clusters (subevents) "<<cluster_ncand<<endl;
	for(int i=0;i<cluster_ncand;i++){
		cout<<" Cluster start time (ns) "<<cluster_tstart[cluster_ncand]<<endl;
		cout<<" Cluster end time (ns) "<<cluster_tend[cluster_ncand]<<endl;
		cout<<" Number of hits included in cluster"<<cluster_nhits[cluster_ncand]<<endl;
		cout<<" Total charge included in cluster "<<cluster_totq[cluster_ncand]<<endl;
	}
	cout<<endl<<" There are "<<fqnse<<" sub events "<<endl;
	
	
	cout<<endl<<"1-ring fits"<<endl
	<<"-------------"<<endl
	<<"- These variables are the result of the 1-ring fits. The first index "<<endl
	<<"is the subevent (1-ring fits are run on all subevents). The second"<<endl
	<<"index is the particle-hypothesis index (same as apfit):"<<endl
	<<"  0 = GAMMA, 1 = ELECTRON, 2 = MUON, 3 = PION, 4 = KAON, 5 = PROTON, "<<endl
	<<"6 = CONE GENERATOR"<<endl
        <<"  Currently, only the electron, muon, and pion (the upstream pion "<<endl
        <<"segement) hypotheses are implemented."<<endl;
	for(int ifqnse=0;ifqnse<fqnse;ifqnse++)
	{
		for(int PID=1;PID<4;PID++)
		{
			cout<<" subevent "<<ifqnse<<" hypothesis "<<HYPO[PID]<<endl;
			cout<<fq1rpcflg[ifqnse][PID]<<" 	Flag to indicate whether fiTQun believes the particle is exiting the ID(<0 if MINUIT did not converge)"<<endl;
			cout<<fq1rmom[ifqnse][PID]<<" 	Fit momentum"<<endl;
			cout<<fq1rt0[ifqnse][PID]<<" 	Fit particle creation time"<<endl;
			cout<<fq1rtotmu[ifqnse][PID]<<" 	Best-fit total predicted charge"<<endl;
			cout<<fq1rnll[ifqnse][PID]<<" 	Best-fit -lnL"<<endl;
			cout<<fq1rpos[ifqnse][PID][0]<< " ";
			cout<<fq1rpos[ifqnse][PID][1]<< " ";
			cout<<fq1rpos[ifqnse][PID][2]<< " ";
			cout<<" 	Fit vertex (0=X, 1=Y, 2=Z)"<<endl;
			cout<<fq1rdir[ifqnse][PID][0]<<" ";
			cout<<fq1rdir[ifqnse][PID][1]<<" ";
			cout<<fq1rdir[ifqnse][PID][2]<<" ";
			cout<<"  	Fit direction (0=X, 1=Y, 2=Z)"<<endl;
			cout<<fq1rdconv[ifqnse][PID]<<"Fit conversion length (always 0 for 1R fits)"<<endl;
			cout<<fq1reloss[ifqnse][PID]<<"	Energy lost in the upstream track segment before the hadronic interaction(for upstream tracks only)"<<endl;
			float x=fq1rdir[ifqnse][PID][0];
			float y=fq1rdir[ifqnse][PID][1];
			float z=fq1rdir[ifqnse][PID][2];
			float dl=sqrt(x*x+y*y+z*z);
			float theta =acos(z/dl);
			float phi =atan2(y,x);
			cout<<" x "<<x<<" y "<<y<<" z "<<z<<" dl "<<dl<<" theta "<<theta<<" phi "<<phi<<endl;
		}
	}
	
	cout<<endl<<" pi0 fit "<<endl
	<<"-----------"<<endl;
	cout<<"- Pi0 fits are only run on the first subevent. Index 0 gives the "<<endl
	<<"standard, unconstrained pi0 fit. (Index 1 is not filled currently)"<<endl;
	cout<<fqpi0pcflg[0]<<" 	 (PCflg for photon 1) + 2*(PCflg for photon 2)"<<endl;
	cout<<fqpi0mom1[0]<<"Fit momentum of first photon"<<endl;
	cout<<fqpi0mom2[0]<<"   	Fit momentum of second photon"<<endl;
	cout<<fqpi0momtot[0]<<"  	Fit momentum of the pi0"<<endl;
	cout<<fqpi0dconv1[0]<<" 	Fit conversion length for the first photon"<<endl;
	cout<<fqpi0dconv2[0]<<" 	Fit conversion length for the second photon"<<endl;
	cout<<fqpi0t0[0]<<" 	Fit pi0 creation time"<<endl;
	cout<<fqpi0totmu[0]<<" 	Best fit total predicted charge"<<endl;
	cout<<fqpi0nll[0]<<"  	Best fit -log(Likelihood)"<<endl;
	cout<<fqpi0mass[0]<<"  	Fit pi0 mass (always 134.9766 for constrained mass fit)"<<endl;
	cout<<fqpi0photangle[0]<<" 	Fit opening angle between the photons"<<endl;
	cout<<fqpi0pos[0][0]<<" ";
	cout<<fqpi0pos[0][1]<<" ";
	cout<<fqpi0pos[0][2]<<" ";
	cout<<" 	Fit vertex position"<<endl;
	cout<<fqpi0dir1[0][0]<<" ";
	cout<<fqpi0dir1[0][1]<<" ";
	cout<<fqpi0dir1[0][2]<<" ";
	cout<<" 	Fit direction of the first photon"<<endl;
	cout<<fqpi0dir2[0][0]<<" ";
	cout<<fqpi0dir2[0][1]<<" ";
	cout<<fqpi0dir2[0][2]<<" ";
	cout<<"  	Fit direction of the second photon"<<endl;
	cout<<fqpi0dirtot[0][0]<<" ";
	cout<<fqpi0dirtot[0][1]<<" ";
	cout<<fqpi0dirtot[0][2]<<" ";
	cout<<"   	Fit direction of the pi0"<<endl;
	
	/*
	Multi ring events
	*/
	cout<<endl<<"Multi-Ring Fits"<<endl
	<<"----------------"<<endl
	<<"- These are the results of the Multi-Ring(MR) fits. The number of "<<endl
	<<"executed multi-ring fits depends on the event topology,and the first "<<endl
	<<"index specifies different fit results.(Index 0 is the best-fit result.)"<<endl
	<<"Each fit result is assigned a unique fit ID which tells the type of the"<<endl
	<<"fit(see fiTQun.cc for more details):"<<endl;
	cout<<endl<<" There are "<<fqnmrfit<<" multi ring fit restults "<<endl;
	for(int ifqnmrfit=0;ifqnmrfit <fqnmrfit ; ifqnmrfit++)
	{
		cout<<fqmrifit[fqnmrfit]<<" Fit ID of MR Fit result "<<endl;
		cout<<fqmrnring[fqnmrfit]<<" 	Number of rings for this fit [1-6]"<<endl;
		cout<<fqmrpcflg[ifqnmrfit]<<" 	<0 if MINUIT did not converge during the fit"<<endl;
		cout<<fqmrnll[ifqnmrfit]<<" 	Best-fit -lnL"<<endl;
		cout<<fqmrtotmu[ifqnmrfit]<<" 	Best-fit total predicted charge"<<endl;
		for(int nRing=0 ;  nRing<fqmrnring[fqnmrfit]; nRing++){
			cout<<fqmrpid[ifqnmrfit][nRing]<<" 	Particle type index for each ring in this fit (Same convention as in 1R fit)"<<endl;
			cout<<fqmrmom[ifqnmrfit][nRing]<<" 	Fit momentum of each ring"<<endl;
			cout<<fqmrdconv[ifqnmrfit][nRing]<<" 	Fit conversion length of each ring(always \"0\" in default mode)"<<endl;
			cout<<fqmreloss[ifqnmrfit][nRing]<<" 	Energy lost in the upstream track segment(for upstream tracks only)"<<endl;
			cout<<fqmrt0[ifqnmrfit][nRing]<<" 	Fit creation time of each ring"<<endl;
			cout<<fqmrpos[ifqnmrfit][nRing][0]<<" "<<fqmrpos[ifqnmrfit][nRing][1]<<" "<<fqmrpos[ifqnmrfit][nRing][2]<<" 	Fit vertex position of each ring"<<endl;
			cout<<fqmrdir[ifqnmrfit][nRing][0]<<" "<<fqmrdir[ifqnmrfit][nRing][1]<<" "<<fqmrdir[ifqnmrfit][nRing][2]<<" 	Fit direction of each ring"<<endl;
		}
	}
	cout<<" Event Listing Ends "<<endl;
	
}
Bool_t fitQunDisplay::Process(Long64_t entry)
{
	
	fitQunDisplay::GetEntry(entry);
	// useful for debug...describe_event(entry);
	fitQunResults= new TEveElementList("fitQun Results");
	fitQunResults2D= new TEveElementList("fitQun Results (unrolled)");
	load_event();
	gEve->AddElement(fitQunResults);
	TEveScene* UnrolledScene=NULL;
	
	TEveSceneList*	scenes = gEve->GetScenes();
	
	for (TEveElement::List_i i=scenes->BeginChildren(); i!=scenes->EndChildren(); ++i)
	{
		TEveScene* scene=(TEveScene*)(*i);
		if(!strcmp(scene->GetName(),"Unrolled Event")){
			UnrolledScene = scene;
			break;
		}
	}
	UnrolledScene->AddElement(fitQunResults2D);
	
	return kTRUE;
}
void fitQunDisplay::load_event()
{
	/*
	Add fitqun results to the Eve event display
	*/
	//float electronMass=0.510998;
	//float muonMass=113.43;
	//float pionMass=139.57;
	//float kaonMass=493.677;
	//float protonMass=938.272;
	
	gEve->GetDefaultGLViewer()->UpdateScene();
	for(int se=0;se< fqnse;se++)
	{
		TEveElementList* EveSubEvent = new TEveElementList(Form("Sub event %i ",se));
		TEveElementList* EveSubEvent2D = new TEveElementList(Form("Sub event %i ",se));
		fitQunResults->AddElement(EveSubEvent);
		fitQunResults2D->AddElement(EveSubEvent2D);
		for(int PID=1;PID<4;PID++)
		{
			if(fq1rpcflg[se][PID]>-1)
			{			
				float x=fq1rdir[se][PID][0];
				float y=fq1rdir[se][PID][1];
				float z=fq1rdir[se][PID][2];
				float dl=sqrt(x*x+y*y+z*z);
				float theta =acos(z/dl);
				float phi =atan2(y,x);
				TString description=Form("Fitqun %s,  sub event %i",HYPO[PID].Data(),se);
				double pos[3];
				pos[0]=fq1rpos[se][PID][0];
				pos[1]=fq1rpos[se][PID][1];
				pos[2]=fq1rpos[se][PID][2];
				addTrack(pos,theta,phi,fq1rmom[se][PID],description,HYPERCOLOUR[PID],MASS[PID],EveSubEvent,EveSubEvent2D);
			}
		}
		
		if(se == 0)
		{
			float x=fqpi0dir1[0][0];
			float y=fqpi0dir1[0][1];
			float z=fqpi0dir1[0][2];
			float dl=sqrt(x*x+y*y+z*z);
			float theta1 =acos(z/dl);
			float phi1 =atan2(y,x);
			x=fqpi0dir2[0][0];
			y=fqpi0dir2[0][1];
			z=fqpi0dir2[0][2];
			dl=sqrt(x*x+y*y+z*z);
			
			float theta2=acos(z/dl);
			float phi2=atan2(y,x);
			double pos[3];
			pos[0]=fqpi0pos[0][0];
			pos[1]=fqpi0pos[0][1];
			pos[2]=fqpi0pos[0][2];
			addPi0(pos,theta1,phi1,fqpi0mom1[0],theta2,phi2,fqpi0mom2[0],EveSubEvent,EveSubEvent2D);
		}
	}
}

void fitQunDisplay::addPi0(double pos[3],double theta1,double phi1,double mom1,double theta2,double phi2,double mom2,TEveElementList* se3d,TEveElementList* se2d )
{
	THKLine* pi0Track=new THKLine("Fitqun pi0");
	pi0Track->SetElementTitle("Fitqun pi0");
	pi0Track->SetMainColor(kWhite);
	pi0Track->SetNextPoint(pos[0],pos[1],pos[2]);
	pi0Track->SetNextPoint(pos[0]+0.001,pos[1]+0.001,pos[2]+0.001);
	se3d->AddElement(pi0Track);
	
	TString Name("Fitqun Pi0->gamma #1");
	THKGamma* gammaTrack=new THKGamma(Name);
	gammaTrack->Create(Name,kWhite,pos,pScale*mom1,mom1,theta1,phi1);
	pi0Track->AddElement(gammaTrack);
	THKCerenkov* cone = new THKCerenkov(Name+" Cerenkov Cone");	
	THKCerenkov2D*	cone2D = new THKCerenkov2D(Name+"Cerenkov cone");
	createCerenkov(cone,cone2D,Name,kWhite,mom1,theta1,phi1,0.0,pos);
	gammaTrack->AddElement(cone);
	se2d->AddElement(cone2D);
	
	Name=TString("Fitqun Pi0->gamma #2");
	gammaTrack=new THKGamma(Name);
	gammaTrack->Create(Name,kWhite,pos,pScale*mom2,mom2,theta2,phi2);
	cone = new THKCerenkov(Name+"Cerenkov Cone");	
	cone2D = new THKCerenkov2D(Name+"Cerenkov cone");
	createCerenkov(cone,cone2D,Name,kWhite,mom2,theta2,phi2,0.0,pos);
	gammaTrack->AddElement(cone);
	se2d->AddElement(cone2D);
	pi0Track->AddElement(gammaTrack);
}
void fitQunDisplay::addTrack(double pos[3],double theta,double phi,double Momentum,TString Name,int colour,float Mass,TEveElementList* se3d, TEveElementList* se2d)
{
	if(Momentum<=0)return;
	THKLine* track=new THKLine(Name);
	track->Create(Name,colour,pos,pScale*Momentum,Momentum,theta,phi);
	THKCerenkov* cone = new THKCerenkov(Name+"Cerenkov Cone");	
	THKCerenkov2D*	cone2D = new THKCerenkov2D(Name+"Cerenkov cone");
	createCerenkov(cone,cone2D,Name,colour,Momentum,theta,phi,Mass,pos);
	track->AddElement(cone);
	se3d->AddElement(track);
	se2d->AddElement(cone2D);
	
}
void fitQunDisplay::createCerenkov(THKCerenkov* cone, THKCerenkov2D* cone2D,
	TString Name,Color_t colour,double Momentum,double theta,double phi,float Mass,double pos[3])
{
	cone2D->SetValues(pos,Momentum,theta,phi);
	
	//?	double length=pScale*Momentum;
	float length=1.0;
	double px = Momentum*cos(phi)*sin(theta);
	double py = Momentum*sin(phi)*sin(theta);
	double pz = Momentum*cos(theta);
	double E = sqrt(Momentum*Momentum+Mass*Mass);
	double Beta = Momentum/E;
	//cout<<" beta "<<beta;
	double n=1.34;
	double cosAlpha=1/(n*Beta);
	
	
	/*
	Construct a vector of points showing the cherenkov circle where it
	impacts with the detector. An alternative is to draw the cones themselves
	- great for publicity but not very practical
	*/
	
	cone->SetElementTitle(Name);
	cone->SetMainColor(colour);	
	cone->SetLineWidth(2.0);
	cone2D->SetElementTitle(Name);
	cone2D->SetMainColor(colour);	
	cone2D->SetLineWidth(2.0);
	//	double ct = cos(theta);
	//	double st = sin(theta);
	//	double sf = sin(phi);
	//	double cf = cos(phi);
	double azim=0;
	int nPoints=1000;
	double delta  = 6.283/nPoints;
	double alpha=acos(cosAlpha);
	//	double cScale=5.0;
	float firstX=0.0;
	float firstY=0.0;
	float firstZ=0.0;
	float firstX2D=0.0;
	float firstY2D=0.0;
	float firstZ2D=0.0;
	float oldX2D=0.0;
	float oldY2D=0.0;
	float oldZ2D=0.0;
	int oldlocation=-1;
	maxTube=-1;
	nextMaxTube=-1;
	nextNextMaxTube=-1;
	double endPoint[3];
	
	
	bool first=true;
	do
	{
		
		/*
		Point x1,y1,z1 is a point on the Cerenkov cone, in the frame which
		has Z along the track direction.
		The cone is at an angle alpha w.r.t. the direction of the track.
		*/
		double x1=length*sin(alpha)*cos(azim);
		double y1=length*sin(alpha)*sin(azim);
		double z1=length*cos(alpha);
		/*
		Rotate into the detector frame
		*/
		Double_t u1 = px/Momentum;
		Double_t u2 = py/Momentum;
		Double_t u3 = pz/Momentum;
		
		Double_t up = u1*u1 + u2*u2;
		double x2=0.0;
		double y2=0.0;
		double z2=0.0;
		if (up) {
			up = TMath::Sqrt(up);
			Double_t PX = x1,  PY = y1,  PZ = z1;
			x2= (u1*u3*PX - u2*PY + u1*up*PZ)/up;
			y2 = (u2*u3*PX + u1*PY + u2*up*PZ)/up;
			z2 = (u3*u3*PX -    PX + u3*up*PZ)/up;
		}
		else if (u3 < 0.) 
			{ x2 = -x1; z2 = -y1; }      // phi=0  teta=pi
		else {};
		
		/*
		Cerenkov photon  starts at pos[...], in direction cerenkov and ends at endPoint
		*/
		double cerenkov[3];
		cerenkov[0]=x2;
		cerenkov[1]=y2;
		cerenkov[2]=z2;
		
		float step=delta;
		int location;
		if(FindConeEnd(pos,cerenkov,endPoint,location))
		{
			float rTest=sqrt(endPoint[0]*endPoint[0]+endPoint[1]*endPoint[1]);
			if(rTest>maxR)
			{
				maxTube=-1;	
			}
			else{
				cone->SetNextPoint(endPoint[0],endPoint[1],endPoint[2]);
				if(first==true){
					firstX=endPoint[0];
					firstY=endPoint[1];
					firstZ=endPoint[2];
				}
				UnrollView(endPoint,location);
				//cone2D->SetNextPoint(endPoint[0],endPoint[1],endPoint[2]);
				if(!first){
					length=sqrt(
						(oldX2D-endPoint[0])*(oldX2D-endPoint[0])+
						(oldY2D-endPoint[1])*(oldY2D-endPoint[1])+
						(oldZ2D-endPoint[2])*(oldZ2D-endPoint[2]));
					if(oldlocation==location && length < 1000)
						cone2D->AddLine(oldX2D,oldY2D,oldZ2D,endPoint[0],endPoint[1],endPoint[2]);
				}
				oldX2D=endPoint[0];
				oldY2D=endPoint[1];
				oldZ2D=endPoint[2];
				oldlocation=location;
				if(first==true){
					first=false;
					firstX2D=endPoint[0];
					firstY2D=endPoint[1];
					firstZ2D=endPoint[2];
				}
			}
			float r=sqrt(endPoint[0]*endPoint[0]+endPoint[1]*endPoint[1]);
			float testz=endPoint[2];
			if((maxZ-abs(testz)<500)&&(maxR-r)<500)
			{
				step=delta/10.0;
			}
		}
		azim+=step;
		
	}
	while(azim<6.283);
	if(!first){
		cone->SetNextPoint(firstX,firstY,firstZ);
		cone2D->AddLine(endPoint[0],endPoint[1],endPoint[2],firstX2D,firstY2D,firstZ2D);
	}
}
void         fitQunDisplay::UnrollView(double position[3],int location)
{
	double pmtX=position[0];
	double pmtY=position[1];
	double pmtZ=position[2];
	if(location==0)
	{
		pmtY+=2.2*maxY;
	}
	if(location==2)
	{
		
		pmtY-=2.2*maxY;
	}	
	if(location==1)
	{
		float angle=atan2(pmtY,pmtX)+(3.1415927/2.0);
		float rho=maxY*angle;
		pmtX=rho;
		pmtY=pmtZ;		
	}
	pmtZ=0.0;
	float xshift=maxY*(3.1415927);
	float x=pmtX;
	if(x>xshift)x=x-(xshift*2);
	pmtX=x;
	position[0]=pmtX;
	position[1]=pmtY;
	position[2]=pmtZ;
	return;
}

int  fitQunDisplay::xyIndex(float xOry)
{
	int nXYvalues=200;
	return nXYvalues*(maxR+xOry)/(2*maxR);
}
int fitQunDisplay::zIndex(float z)
{
	int indexMin= ((nZvalues)*(minZ/(maxZ-minZ)));
	
	int index =  ((nZvalues)*(z/(maxZ-minZ)))-indexMin;
	
	return index;
}
int fitQunDisplay::phiIndex(float x,float y)
{
	float phi = atan2(x,y);//?x,y reversed??!!
	float phiDegrees=phi*360.0/6.283185307;
	int  iPhiDegrees = (int) phiDegrees;
	return 180+iPhiDegrees;
	
}
void fitQunDisplay::PreProcessGeometry()
{
	
	nZvalues=(maxZ-minZ)/100;
	maxZIndex=0;
	maxPhiIndex=0;
	
	for(int tubeId=0;tubeId<wcsimrootgeom->GetWCNumPMT();tubeId++)
    	{
    		
    		WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
    		double pmtX = pmt.GetPosition (0);
    		double pmtY = pmt.GetPosition (1);
    		double pmtZ = pmt.GetPosition (2) ;
    		int location= pmt.GetCylLoc();
    		if(location ==1 )
    		{
    			
    			int zI = zIndex(pmtZ);
    			int phiI = phiIndex(pmtX,pmtY);
    			if(zI>maxZIndex)maxZIndex=zI;
    			if(phiI>maxPhiIndex)maxPhiIndex=phiI;
    		}
    		else
    		{
    			int xI = xyIndex(pmtX);
    			int yI = xyIndex(pmtY);
    			if(xI>maxXIndex)maxXIndex=xI;
    			if(yI>maxYIndex)maxYIndex=yI;
    		}
    		
    	}
    	CylinderTubeList = new int[(maxZIndex+1)*maxPhiIndex];	
    	for(int i = 0;i<(maxZIndex+1)*maxPhiIndex;i++)CylinderTubeList[i]=-1;
    	NegativeCapTubeList = new int[(maxXIndex+1)*maxYIndex];	
    	for(int i = 0;i<(maxXIndex+1)*maxYIndex;i++)NegativeCapTubeList[i]=-1;
    	
	PositiveCapTubeList = new int[(maxXIndex+1)*maxYIndex];	
    	for(int i = 0;i<(maxXIndex+1)*maxYIndex;i++)PositiveCapTubeList[i]=-1;
    	
    	int count=0;
    	for(int tubeId=0;tubeId<wcsimrootgeom->GetWCNumPMT();tubeId++)
    	{
   		
    		WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
    		double pmtX = pmt.GetPosition (0);
    		double pmtY = pmt.GetPosition (1);
    		double pmtZ = pmt.GetPosition (2) ;
    		int location= pmt.GetCylLoc();
    		if(location ==1 )
    		{
    			int zI = zIndex(pmtZ);
    			int phiI = phiIndex(pmtX,pmtY);
    			if(zI*maxPhiIndex+phiI>(maxZIndex+1)*maxPhiIndex)
    			{
    				cout<<" ERROR "<<endl;
    				cout<<pmtX<<" "<<pmtY<<" "<<pmtZ<<" "<<zI<<" "<<phiI<<endl;
    				cout<<" fill item "<<zI<<" "<<phiI<<" with "<<tubeId<<endl;
    				cout<<" index is "<<zI*maxPhiIndex+phiI<<endl;
    				cout<<" array dimensions are "<<(maxZIndex+1)*maxPhiIndex<<endl;
    			}
    			else
    				CylinderTubeList[zI*maxPhiIndex+phiI]=tubeId;
    		}
    		else 
    		{
    			int xI = xyIndex(pmtX);
    			int yI = xyIndex(pmtY);
    			if(xI*maxYIndex+yI>(maxXIndex+1)*maxYIndex)
    			{
    				cout<<" ERROR "<<endl;
    				cout<<pmtX<<" "<<pmtY<<" "<<pmtZ<<" "<<xI<<" "<<yI<<endl;
    				cout<<" fill item "<<xI<<" "<<yI<<" with "<<tubeId<<endl;
    				cout<<" index is "<<xI*maxYIndex+yI<<endl;
    				cout<<" array dimensions are "<<(maxXIndex+1)*maxYIndex<<endl;
    			}
    			else
    			{
    				if(location==0)
    				{
    					PositiveCapTubeList[xI*maxYIndex+yI]=tubeId;
    					count+=20;
    					//if(count>100){cout<<endl;count=0;}
    				}
    				if(location==2)
    					NegativeCapTubeList[xI*maxYIndex+yI]=tubeId;
    			}
    		}
    		
    		
    	}
    	
}
void fitQunDisplay::lineTubeIntersect(double cerenkov[3],double pos[3],float cylinderR,float &xc,float &yc)
{
	double lCxy=sqrt(cerenkov[0]*cerenkov[0]+cerenkov[1]*cerenkov[1]);
	double  cx = cerenkov[0]/lCxy;
	double  cy = cerenkov[1]/lCxy;
	double  px=pos[0];
	double  py=pos[1];
	
	float m=cy/cx;
	float Const=py-m*px;
	float a = m*m+1;
	float b = 2*m*Const;
	float c = Const*Const-cylinderR*cylinderR;
	
	
	
	xc = (-b + sqrt(b*b-4*a*c))/(2*a);
	
	yc=m*xc+Const;
}

void fitQunDisplay::searchForTube(int *List,int xCentre,int yCentre,
	int maxYSearchIndex, double pos[3],double cerenkov[3],
	bool debug,int tubes[3])
{
	//bool foundIt=kFALSE;
	tubes[0]=-1;
	tubes[1]=-1;
	tubes[2]=-1;
	if(debug)cout<<"fitQunDisplay::searchForTube"<<endl;
	
	/* 
	Start searching at centre and look at close neighbours
	- keep looking untill we don't find a different tube to the one at 
	the centre of the search.
	The criterion is that the cosine of the angle between the cerenkov light and
	the tube must be maximized.
	*/
	int xI,yI;
	int nX,nY;
	int newXCentre=xCentre;
	int newYCentre=yCentre;
	int nMax=4;
	double cosMax=-1;
	do
	{
		xCentre=newXCentre;
		yCentre=newYCentre;
		if(debug)cout<<" searching around "<<xCentre<<" "<<yCentre<<endl;
		for(int n=0;n<nMax;n++) // n is distance from centre
		{
			nX=n;
			for(nY=-n;nY<=n;nY++)
			{
				xI=xCentre+nX;
				yI=yCentre+nY;
				if(xI>=0&&yI>=0&&xI<=maxXIndex&&yI<=maxYSearchIndex){
					int iTube=List[xI*maxYSearchIndex+yI];
					if(iTube >0  ){
						double cosAng=cosAngleToTube(pos,cerenkov,iTube);
						if(cosAng>cosMax)
						{
							cosMax=cosAng;
							newXCentre=xI;
							newYCentre=yI;
						}
						//if(debug)cout<<xI<<" "<<yI<<" "<<iTube<<endl;
					}
				}
			}
			nY=n;
			for(nX=1-n;nX<=(n-1);nX++)
			{
				xI=xCentre+nX;
				yI=yCentre+nY;
				if(xI>=0&&yI>=0&&xI<=maxXIndex&&yI<=maxYSearchIndex){
					int iTube=List[xI*maxYSearchIndex+yI];
					if(iTube >0){
						double cosAng=cosAngleToTube(pos,cerenkov,iTube);
						if(cosAng>cosMax)
						{
							cosMax=cosAng;
							newXCentre=xI;
							newYCentre=yI;
						}
						//if(debug)cout<<xI<<" "<<yI<<" "<<iTube<<endl;
					}
					
				}
			}
			nX=-n;
			for(nY=-n;nY<=n;nY++)
			{
				xI=xCentre+nX;
				yI=yCentre+nY;
				if(xI>=0&&yI>=0&&xI<=maxXIndex&&yI<=maxYSearchIndex){
					int iTube=List[xI*maxYSearchIndex+yI];
					if(iTube >0){
						double cosAng=cosAngleToTube(pos,cerenkov,iTube);
						if(cosAng>cosMax)
						{
							cosMax=cosAng;
							newXCentre=xI;
							newYCentre=yI;
						}
					}
					
				}
			}
			nY=-n;
			for(nX=1-n;nX<=(n-1);nX++)
			{
				xI=xCentre+nX;
				yI=yCentre+nY;
				if(xI>=0&&yI>=0&&xI<=maxXIndex&&yI<=maxYSearchIndex){
					int iTube=List[xI*maxYSearchIndex+yI];
					if(iTube >0){
						double cosAng=cosAngleToTube(pos,cerenkov,iTube);
						if(cosAng>cosMax)
						{
							cosMax=cosAng;
							newXCentre=xI;
							newYCentre=yI;
						}
					}
					
				}
			}
			
			
		}
	}while(newXCentre!=xCentre||newYCentre!=yCentre);
	/*
	If the cos(angle) is too small return an invalid result
	*/
	if(debug)cout<<" cosMax = "<<cosMax<<endl;
	if(cosMax<0.999)
	{
		if(debug)cout<<" which is not close enough "<<endl;
		return;
	}
	/*
	found centre, now find two neighbours
	*/
	if(newXCentre==xCentre&&newYCentre==yCentre)
	{
		tubes[0]=List[xCentre*maxYSearchIndex+yCentre];
		cosMax=-1;
		int iXneighour=-1;
		int iYneighour=-1;
		int n=nMax;
		nY=0;
		for(nX=1-n;nX<=(n-1);nX++) //scan x directin
		{
			xI=xCentre+nX;
			yI=yCentre+nY;
			if(xI>=0&&yI>=0&&xI<=maxXIndex&&yI<=maxYSearchIndex&&xI!=xCentre){
				int iTube=List[xI*maxYSearchIndex+yI];
				if(iTube >0){
					double cosAng=cosAngleToTube(pos,cerenkov,iTube);
					if(cosAng>cosMax){
						iXneighour=iTube;
						cosMax=cosAng;
					}
				}
			}
		}
		tubes[1]=iXneighour;
		nX=0;
		cosMax=-1;
		for(nY=1-n;nY<=(n-1);nY++)// now scan in Y
		{
			xI=xCentre+nX;
			yI=yCentre+nY;
			if(xI>=0&&yI>=0&&xI<=maxXIndex&&yI<=maxYSearchIndex&&yI!=yCentre){
				int iTube=List[xI*maxYSearchIndex+yI];
				if(iTube >0){
					double cosAng=cosAngleToTube(pos,cerenkov,iTube);
					if(cosAng>cosMax){
						iYneighour=iTube;
						cosMax=cosAng;
					}
				}
			}
		}
		tubes[2]=iYneighour;
	}
	if(debug)cout<<" normal return "<<tubes[0]<<" "<<tubes[1]<<" "<<tubes[2]<<endl;
}
double fitQunDisplay::cosAngleToTube(double pos[3],double cerenkov[3],int tubeId)
{
	WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( tubeId );
	int location= pmt.GetCylLoc();
	if(location !=0 && location !=1 && location !=2)return -999;
	double pmtX = pmt.GetPosition (0) - pos[0];
	double pmtY = pmt.GetPosition (1) - pos[1];
	double pmtZ = pmt.GetPosition (2) - pos[2];
	double dot=pmtX*cerenkov[0]+pmtY*cerenkov[1]+pmtZ*cerenkov[2];
	double lTube=sqrt(pmtX*pmtX+pmtY*pmtY+pmtZ*pmtZ);
	double lCerenkov=sqrt(cerenkov[0]*cerenkov[0]+cerenkov[1]*cerenkov[1]+cerenkov[2]*cerenkov[2]);
	
	double cosAng=dot/lTube/lCerenkov;
	return cosAng;
}
bool fitQunDisplay::FindConeEnd(double pos[3],double cerenkov[3],double endPoint[3],int &location)
{
	if(wcsimrootgeom==NULL)return false;
	/*
	Loop over all tubes and find 3 nearest ones from which
	we can form a plane
	*/
	bool debug=kFALSE;
	if(debug)cout<<endl<<" fitQunDisplay::FindConeEnd starts"<<endl;
	float maxCosine=-1.0;
	float nextMaxCosine=-1.0;
	float nextNextMaxCosine=-1.0;
	int  tubes[3];
	
	double lCerenkov=sqrt(cerenkov[0]*cerenkov[0]+cerenkov[1]*cerenkov[1]+cerenkov[2]*cerenkov[2]);
	double  l1 = cerenkov[0]/lCerenkov;
	double  l2 = cerenkov[1]/lCerenkov;
	double  l3 = cerenkov[2]/lCerenkov; 
	double  px=pos[0];
	double  py=pos[1];
	double  pz=pos[2];
	
	/*
	First look close to the last tube we found (maxTube), if we have one.
	*/
	
	if(maxTube>0)
	{
		if(debug)cout<<" start searching close to previous candidate "<<maxTube;
		WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( maxTube );
		float pmtx=pmt.GetPosition (0);
		float pmty=pmt.GetPosition (1);
		float pmtz=pmt.GetPosition (2);
		if(debug)cout<<" located at "<<pmtx<<" , "<<pmty<<" , "<<pmtz<<endl;
		if(debug)cout<<" min/max z "<<minZ<<" "<<maxZ<<endl;
		if(debug)cout<<" maxR "<<maxR<<endl;
		location=pmt.GetCylLoc();
		/*
		Check for situations where we are about to jump from cylinder to end wall or vice versa.
		In these cases just looking around the previous tube does not work, so don't try
		*/
		if(location==1){
			if(pmtz<(minZ+50) || pmtz>(maxZ-50)){
				//cout<<" cylinder tube is close to min or max Z"<<endl;
				maxTube=-1;
				goto Global;
			}
		}
		if(location==0||location==2){
			float r=sqrt(pmtx*pmtx+pmty*pmty);
			//cout<<" end cap tube r"<<r<<endl;
			
			if(r>(maxR-500)){
				//cout<<" is close to maxR"<<maxR<<endl;
				maxTube=-1;
				goto Extrapolate;
			}
			//else
			//	cout<<" is ok "<<endl;
		}
		int xI,yI,zI;
		if(location==0)
		{
			pmtx=pmt.GetPosition (0);
			xI=xyIndex(pmtx);
			pmty=pmt.GetPosition (1);
			yI=xyIndex(pmty);
			searchForTube(PositiveCapTubeList,xI,yI,maxYIndex,pos,cerenkov,debug,tubes);
		}
		if(location==1)
			
		{
			zI=zIndex(pmtz);
			int phiI=phiIndex(pmtx,pmty);
			//int zStep=20;
			//int phiStep=20;
			searchForTube(CylinderTubeList,zI,phiI,360,pos,cerenkov,debug,tubes);
		}
		if(location==2)
		{
			//	cout<<" search in -ve cap "<<endl;
			
			xI=xyIndex(pmtx);
			yI=xyIndex(pmty);
			searchForTube(NegativeCapTubeList,xI,yI,maxYIndex,pos,cerenkov,debug,tubes);
		}
		maxTube=tubes[0];
	}
	/*
	Calculate phi, z index to start looking in the array of tube ids.
	
	Assume detector is a cylinder of radius maxR, end plates as minZ, maxZ
	
	*/
Extrapolate:
	if(maxTube<0)
	{
		if(debug)cout<<" start searching close to extrapolated cerenkov light "<<endl;
		
		float xc,yc;
		lineTubeIntersect(cerenkov,pos,maxR,xc,yc);
		//cout<<" xc,yc from lineTubeIntersect"<<xc<<" "<<yc<<endl;	
		//cout<<" point of intersect is "<<xc<<" "<<yc<<endl;
		float rp=sqrt(px*px+py*py);
		float rc = maxR-rp;
		
		float theta = acos(l3);
		float czIntersect = pz+ rc/tan(theta);
		//cout<<"  czIntersect= "<<czIntersect<<" compare to "<<minZ<<" "<<maxZ<<endl;
		if(czIntersect<minZ||czIntersect>maxZ)
		{
			//cout<<" probably in end plates. czIntersect= "<<czIntersect<<" compare to "<<minZ<<" "<<maxZ<<endl;
			/* calculate x,y if we have hit an end plate */
			if(l3>0)rc=(maxZ-pz)*tan(theta);
			else rc=(minZ-pz)*tan(theta);
			float rCap=rc+rp;
			//	cout<<" r at end cap is "<<rCap<<endl;
			lineTubeIntersect(cerenkov,pos,rCap,xc,yc);
			//	cout<<" xc,yc from lineTubeIntersect: "<<xc<<" "<<yc;
			int xI = xyIndex(xc);
			int yI = xyIndex(yc);
			//	cout<<" x/y indices: "<<xI<<" "<<yI<<endl;
			//	int xStep=50;
			//	int yStep=50;
			//temp!
			if(czIntersect>maxZ)
			{
				//	cout<<" search in positive cap "<<endl;
				searchForTube(PositiveCapTubeList,xI,yI,maxYIndex,
					pos,cerenkov,kFALSE,tubes);
			}
			else
			{
				//	cout<<" search in negative cap "<<endl;
				searchForTube(NegativeCapTubeList,xI,yI,maxYIndex,
					pos,cerenkov,kFALSE,tubes);
			}
			
		}
		else
		{
			if(debug)cout<<" intersect with cylinder = "<<xc<<" "<<yc<<" "<<czIntersect<<endl;
			int zI = zIndex(czIntersect);
			int phiI = phiIndex(xc,yc);
			//	cout<<" phi and z index "<<phiI<<" "<<zI<<endl;
			//	cout<<" tube at this location is "<<CylinderTubeList[zI*maxPhiIndex+phiI]<<endl;
			/*
			search for tubes near this index
			*/
			//	int zStep=20;
			//	int phiStep=20;
			if(debug)cout<<"search in cylinder "<<endl;
			searchForTube(CylinderTubeList,zI,phiI,360,pos,cerenkov,debug,tubes);
		}
	}
	maxTube=tubes[0];
	nextMaxTube=tubes[1];
	nextNextMaxTube=tubes[2];
Global:
	if(maxTube<0 ||nextMaxTube<0 || nextNextMaxTube<0 )
	{
		/* that also failed so do a global search */
		if(debug)cout<<" trying a brute force search "<<endl;
		for(int tubeId=0;tubeId<wcsimrootgeom->GetWCNumPMT();tubeId++)
		{
			
			double cosAng=cosAngleToTube(pos,cerenkov,tubeId);
			if(cosAng<-100)continue;
			
			if(cosAng>maxCosine){
				nextNextMaxCosine=nextMaxCosine;
				nextMaxCosine=maxCosine;
				maxCosine=cosAng;
				nextNextMaxTube=nextMaxTube;
				nextMaxTube=maxTube;
				maxTube=tubeId;
			}
			else if(cosAng>nextMaxCosine)
			{
				nextNextMaxCosine=nextMaxCosine;
				nextMaxCosine=cosAng;
				nextNextMaxTube=nextMaxTube;
				nextMaxTube=tubeId;
			}
			else if(cosAng>nextNextMaxCosine)
			{
				nextNextMaxCosine=cosAng;
				nextNextMaxTube=tubeId;
			}
			
		}
	}
	/*
	We need all three close tubes to be part of the same part of the detector
	to avoid corner effects
	*/
	if(debug)cout<<" tubes found:"<<maxTube<<" "<<nextMaxTube<<" "<<nextNextMaxTube<<endl;
	if(maxTube<0)return kFALSE;
	if(nextMaxTube<0)return kFALSE;
	if(nextNextMaxTube<0)return kFALSE;
	if(debug)cout<<" plane of tubes found ok"<<maxTube<<" "<<nextMaxTube<<" "<<nextNextMaxTube<<endl;
	WCSimRootPMT pmt = wcsimrootgeom -> GetPMT ( maxTube );
	if(debug)cout<<" position of nearest "<<pmt.GetPosition (0)<<" , "<<pmt.GetPosition (1)<<" , "<<pmt.GetPosition (2)<<endl;
	pmt = wcsimrootgeom -> GetPMT (maxTube); 
	location= pmt.GetCylLoc();
	if(debug)cout<<" location type of nearest tube is "<<location<<endl;
	if(wcsimrootgeom -> GetPMT (nextMaxTube).GetCylLoc() != location){
		//	cout<<" next tube not in same location "<<endl;
		return kFALSE; 
	}
	if(wcsimrootgeom -> GetPMT (nextNextMaxTube).GetCylLoc() != location){
		//	cout<<" next to next  tube not in same location "<<endl;
		return kFALSE;
	}
	/*
	Now calculate the point that cerenkov vector
	crosses the plane formed by three nearest tubes
	*/
	pmt = wcsimrootgeom -> GetPMT (maxTube); 
	double Xt1= pmt.GetPosition (0);  	
	double Yt1= pmt.GetPosition (1);  	
	double Zt1= pmt.GetPosition (2);  	
	pmt = wcsimrootgeom -> GetPMT (nextMaxTube); 
	double Xt2= pmt.GetPosition (0);  	
	double Yt2= pmt.GetPosition (1);  	
	double Zt2= pmt.GetPosition (2);  
	pmt = wcsimrootgeom -> GetPMT (nextNextMaxTube); 
	double Xt3= pmt.GetPosition (0);  	
	double Yt3= pmt.GetPosition (1);  	
	double Zt3= pmt.GetPosition (2); 
	
	if(debug)cout<<" location of next neighbour "<<Xt2<<" "<<Yt2<<" "<<Zt2<<endl;
	if(debug)cout<<" location of next next neighbour "<<Xt3<<" "<<Yt3<<" "<<Zt3<<endl;
	/*
	construct vectors along two sides of triangle 
	formed by 3 nearest tubes
	*/
	double u1=Xt2-Xt1;
	double u2=Yt2-Yt1;
	double u3=Zt2-Zt1;
	double v1=Xt3-Xt1;
	double v2=Yt3-Yt1;
	double v3=Zt3-Zt1;
	/*
	Normal to the plane:
	*/
	double n1 = u2*v3-u3*v2;
	double n2 = u3*v1-u1*v3;
	double n3 = u1*v2-u2*v1;
	
	/*
	If there is no normal to the plane, return an error 
	*/
	if((n1*n1+n2*n2+n3*n3)==0)
		return kFALSE;
	/*
	Now solve for the distance along the cerenkov vector to this plane
	
	d = (p0 - l0) . n /l.n
	where p0 is a point on the plane, l0 is a point on the line and
	l is a vector along the line
	*/
	l1 = cerenkov[0]/lCerenkov;
	l2 = cerenkov[1]/lCerenkov;
	l3 = cerenkov[2]/lCerenkov; 
	
	double lDotn = l1*n1+l2*n2+l3*n3;
	
	double p0minusl0X =  Xt1 - pos[0];
	double p0minusl0Y =  Yt1 - pos[1];
	double p0minusl0Z =  Zt1 - pos[2];
	
	
	double p0minusl0ZDotN = p0minusl0X*n1+p0minusl0Y*n2+p0minusl0Z*n3;
	
	double distance = p0minusl0ZDotN / lDotn;
	
	endPoint[0]=pos[0]+distance*l1;
	endPoint[1]=pos[1]+distance*l2;
	endPoint[2]=pos[2]+distance*l3;
	/*
	Calculate distance between end point and nearest tube
	
	*/
	double TEx=endPoint[0]-Xt1;
	double TEy=endPoint[1]-Yt1;
	double TEz=endPoint[2]-Zt1;
	float Tube2EndPoint=sqrt(TEx*TEx+TEy*TEy+TEz*TEz);
	/*
	Calculate distance between tube and nearest neighbour(s)
	*/
	double TNx=Xt2-Xt1;
	double TNy=Yt2-Yt1;
	double TNz=Zt2-Zt1;
	float Tube2Neighbour=sqrt(TNx*TNx+TNy*TNy+TNz*TNz);
	double TNNx=Xt3-Xt1;
	double TNNy=Yt3-Yt1;
	double TNNz=Zt3-Zt1;
	float Tube2NextNeighbour=sqrt(TNNx*TNNx+TNNy*TNNy+TNNz*TNNz);
	if(debug)cout<<" distance from end point to tube is "<<Tube2EndPoint<<endl;
	if(debug)cout<<" distance from neighbour to tube is "<<Tube2Neighbour<<endl;
	if(debug)cout<<" distance from next neighbour to tube is "<<Tube2NextNeighbour<<endl;
	if(Tube2EndPoint>Tube2Neighbour)
	{
	 	if(debug) cout<<" return false "<<endl;
	 	return kFALSE;
	}
	if(Tube2NextNeighbour>(1.5*Tube2Neighbour))
	{
	 	if(debug) cout<<" return false "<<endl;
	 	return kFALSE;
	}
	
	
	
	if(endPoint[2]<minZ)endPoint[2]=minZ;
	if(endPoint[2]>maxZ)endPoint[2]=maxZ;
	if(debug)cout<<" succesful return "<<endl;
	if(debug)cout<<" location of end point  is "<<endPoint[0]<<" "<<endPoint[1]<<" "<<endPoint[2]<<endl;
	
	//if(location !=1)
	//{
	//	float r=sqrt(endPoint[0]*endPoint[0]+endPoint[1]*endPoint[1]);
	//	cout<<" end point R = "<<r<<endl;
	//}
	
	return kTRUE;
}

void fitQunDisplay::SlaveTerminate()
{
	// The SlaveTerminate() function is called after all entries or objects
	// have been processed. When running with PROOF SlaveTerminate() is called
	// on each slave server.
	
}

void fitQunDisplay::Terminate()
{
	// The Terminate() function is the last function to be called during
	// a query. It always runs on the client, it can be used to present
	// the results graphically or save the results to file.
	
}
void             fitQunDisplay::CreateTable(HtmlSummary* fgHtmlSummary){
	/*
	Add an html table describing fitqun results
	*/
	HtmlObjTable *table = fgHtmlSummary->AddTable("FitQun results", 8, kTRUE,""); 
	
	table->SetLabel(0, "Type");
	table->SetLabel(1, "Subevent");
	table->IsInteger(1);
	table->SetLabel(2, "Vertex X");
	table->SetLabel(3, "Vertex Y");
	table->SetLabel(4, "Vertex Z");
	table->SetLabel(5, "Momentum ");
	table->SetLabel(6, "Theta ");
	table->SetLabel(7, "Phi ");
	int nused=0;
	int se;
	for(se=0;se< fqnse;se++)
	{
		for(int PID=1;PID<4;PID++)
		{
			if(fq1rpcflg[se][PID]>-1)
			{
				float x=fq1rdir[se][PID][0];
				float y=fq1rdir[se][PID][1];
				float z=fq1rdir[se][PID][2];
				float dl=sqrt(x*x+y*y+z*z);
				float theta =acos(z/dl);
				float phi =atan2(y,x);
				TString description=Form("Fitqun %s,  sub event %i",HYPO[PID].Data(),se);
				double pos[3];
				pos[0]=fq1rpos[se][PID][0];
				pos[1]=fq1rpos[se][PID][1];
				pos[2]=fq1rpos[se][PID][2];
				addTableRow(table,nused++,se,HYPO[PID],pos,theta,phi,fq1rmom[se][PID]);
				//	if(f1mu_PCflg ==0)addTableRow(table,nused++,se,"muon",f1mu_pos_sub[se],f1mu_theta_sub[se],f1mu_phi_sub[se],f1mu_mom_sub[se]);
				//	if(f1pip_PCflg==0)addTableRow(table,nused++,se,"pi+",f1pip_pos_sub[se],f1pip_theta_sub[se],f1pip_phi_sub[se],f1pip_mom_sub[se]);
				//	if(f1kp_PCflg ==0)addTableRow(table,nused++,se,"Kaon",f1kp_pos_sub[se],f1kp_theta_sub[se],f1kp_phi_sub[se],f1kp_mom_sub[se]);
				//	if(f1p_PCflg  ==0)addTableRow(table,nused++,se,"proton",f1p_pos_sub[se],f1p_theta_sub[se],f1p_phi_sub[se],f1p_mom_sub[se]);		
			}
			
		}
		float x=fqpi0dir1[0][0];
		float y=fqpi0dir1[0][1];
		float z=fqpi0dir1[0][2];
		float dl=sqrt(x*x+y*y+z*z);
		float theta1 =acos(z/dl);
		float phi1 =atan2(y,x);
		x=fqpi0dir2[0][0];
		y=fqpi0dir2[0][1];
		z=fqpi0dir2[0][2];
		dl=sqrt(x*x+y*y+z*z);
		
		float theta2=acos(z/dl);
		float phi2=atan2(y,x);
		double pos[3];
		pos[0]=fqpi0pos[0][0];
		pos[1]=fqpi0pos[0][1];
		pos[2]=fqpi0pos[0][2];
		addPi0Rows(table,nused,se,pos,theta1,phi1,fqpi0mom1[0],theta2,phi2,fqpi0mom2[0]);
		nused+=3;
	}
	
	//	int used=0;
	
	
	return;
}
void fitQunDisplay::addTableRow(HtmlObjTable *table,int used,int subevent,const char* name,Double_t pos[3],Double_t theta,Double_t phi,Double_t mom)
{
	table->SetRowName(used, name);
	table->SetValue(1,used,subevent);
	table->SetValue(2,used,pos[0]);
	table->SetValue(3,used,pos[1]);
	table->SetValue(4,used,pos[2]);
	table->SetValue(5,used,mom);	
	table->SetValue(6,used,theta);	
	table->SetValue(7,used,phi);	
}
void fitQunDisplay::CartesianToPolar(double &mom,double &theta,double &phi,double p[3]){
	mom=sqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
	theta=acos(p[2]/mom);
	phi=atan2(p[1],p[0]);
}
void fitQunDisplay::polarToCartesian(double mom,double theta,double phi,double p[3]){
	p[0]=mom*sin(theta)*cos(phi);
	p[1]=mom*sin(theta)*sin(phi);
	p[2]=mom*cos(theta);
}

void fitQunDisplay::addPi0Rows(HtmlObjTable *table,int used,int subevent,double pos[3],double theta1,double phi1,double mom1,double theta2,double phi2,double mom2)
{
	double p1[3];
	polarToCartesian(mom1,theta1,phi1,p1);
	double p2[3];
	polarToCartesian(mom2,theta2,phi2,p2);
	double ppi0[3];
	ppi0[0]=p1[0]+p1[0];
	ppi0[1]=p1[1]+p1[1];
	ppi0[2]=p1[2]+p1[2];
	double mompi0=-1;
	double thetapi0=-1;
	double phipi0=-1;
	CartesianToPolar(mompi0,thetapi0,phipi0,ppi0);
	table->SetRowName(used, "pi0");
	table->SetValue(1,used,subevent);
	table->SetValue(2,used,pos[0]);
	table->SetValue(3,used,pos[1]);
	table->SetValue(4,used,pos[2]);
	table->SetValue(5,used,mompi0);	
	table->SetValue(6,used,thetapi0);	
	table->SetValue(7,used,phipi0);
	used++;	
	table->SetRowName(used, " -> gamma 1");
	table->SetValue(1,used,subevent);
	table->SetValue(2,used,pos[0]);
	table->SetValue(3,used,pos[1]);
	table->SetValue(4,used,pos[2]);
	table->SetValue(5,used,mom1);	
	table->SetValue(6,used,theta1);	
	table->SetValue(7,used,phi1);	
	used++;	
	table->SetRowName(used, " -> gamma 2");
	table->SetValue(1,used,subevent);
	table->SetValue(2,used,pos[0]);
	table->SetValue(3,used,pos[1]);
	table->SetValue(4,used,pos[2]);
	table->SetValue(5,used,mom2);	
	table->SetValue(6,used,theta2);	
	table->SetValue(7,used,phi2);	
	return;
}

