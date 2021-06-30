#include "THKCerenkov2D.h"
#include <iostream>
using std::cout;
using std::endl;

ClassImp(THKCerenkov2D)
void THKCerenkov2D::SetValues(double Start[3],double Momentum,double Theta,double Phi)
{
	for(int i=0;i<3;i++)
	{
		start[i]=Start[i];
	}
	momentum=Momentum;
	theta=Theta;
	phi=Phi;
}
void THKCerenkov2D::Describe()
{
	cout<<GetTitle()<<": Momentum: "<<momentum<<" Theta: "<<theta<<" Phi: "<<phi<<" Vertex ("<<start[0]<<","<<start[1]<<","<<start[2]<<")"<<endl;
}

