#include "THKMCTrack.h"
#include <iostream>
using std::cout;
using std::endl;


ClassImp(THKMCTrack)
void THKMCTrack::SetValues(float Start[3],float Stop[3],float Mass,float Momentum,
    	float Energy,float Theta,float Phi,bool Drawn)
{
	for(int i=0;i<3;i++)
	{
		start[i]=Start[i];
		stop[i]=Stop[i];
	}
	mass=Mass;
	momentum=Momentum;
	energy=Energy;
	theta=Theta;
	phi=Phi;
	drawn=Drawn;
}
void THKMCTrack::Describe()
{
	cout<<" MC truth track: "<<GetTitle()<<endl;
	cout<<" Start: (";
	for(int i=0;i<3;i++){cout<<start[i];if(i<2)cout<<",";}
	cout<<")";
	cout<<" Stop: (";
	for(int i=0;i<3;i++){cout<<stop[i];if(i<2)cout<<",";}
	cout<<")";
	cout<<", Mass: "<<mass<<", Momentum: "<<momentum<<", Energy: "<<energy<<", Theta: "<<theta<<", Phi: "<<phi;
	if(drawn)cout<<" Drawn ";
	else
	cout<<" Not Drawn ";
	cout<<endl;	
}

