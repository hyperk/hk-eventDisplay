#include "THKCerenkov.h"
#include <iostream>
using std::cout;
using std::endl;

ClassImp(THKCerenkov)
void THKCerenkov::FillImpliedSelectedSet(Set_t& impSelSet)
{
	TEveElement::FillImpliedSelectedSet(impSelSet);
	
	for (List_ci i=fParents.begin(); i!=fParents.end(); ++i)
	{
		impSelSet.insert(*i);
	}
}
void THKCerenkov::Describe()
{
	cout<<"Cerenkov ring from "<<GetTitle()<<endl;
	List_ci i=fParents.begin();
	TEveElement* obj=(*i);
	THKGamma *g = dynamic_cast<THKGamma*> (obj);
	if(g){
		g->Describe();
	}
	else
	{
		cout<<Form("User clicked on: \"%s\"", obj->GetElementName())<<endl;
		//cout<<" of class "<<obj->ClassElementName()<<endl;
	}
}
