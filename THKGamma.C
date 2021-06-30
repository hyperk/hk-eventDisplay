#include "THKGamma.h"
#include <iostream>
using std::cout;
using std::endl;


ClassImp(THKGamma)
/// Override selection, select a gamma and you select my whole family!
void THKGamma::FillImpliedSelectedSet(Set_t& impSelSet)
{
	TEveElement::FillImpliedSelectedSet(impSelSet);
	
	for (List_ci i=fParents.begin(); i!=fParents.end(); ++i)
	{
		impSelSet.insert(*i);
	}
	
	for (List_ci i=fChildren.begin(); i!=fChildren.end(); ++i)
	{
		impSelSet.insert(*i);
	}
}
void THKGamma::Create(TString title,Color_t color,double pos[3],double length,double Momentum,double Theta,double Phi){
	theta=Theta;
	phi=Phi;
	x=pos[0];
	y=pos[1];
	z=pos[2];
	momentum=Momentum;
	SetElementTitle(title);
	SetMainColor(color);
	SetNextPoint(pos[0],pos[1],pos[2]);
	double xend = pos[0]  + length*cos(phi)*sin(theta);
	double yend = pos[1]  + length*sin(phi)*sin(theta);
	double zend = pos[2]  + length*cos(theta);
	SetNextPoint(xend,yend,zend);
}
void THKGamma::Describe()
{
	cout<<GetTitle()<<": Momentum: "<<momentum<<" Theta: "<<theta<<" Phi: "<<phi<<" Vertex ("<<x<<","<<y<<","<<z<<")"<<endl;
}
