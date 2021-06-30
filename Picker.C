#include "Picker.h"
#include <iostream>
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TGStatusBar.h>
#include "THKGamma.h"
#include "THKMCTrack.h"
#include "THKCerenkov.h"
#include "THKCerenkov2D.h"
#include "TEveBoxSet.h"
using std::cout;
void Picker::Picked(TObject* obj)
{
	if(obj==NULL)return;
	//cout<<obj->ClassName()<<endl;
	//cout<<obj->GetName()<<endl;
	//cout<<obj->GetTitle()<<endl;
	TEveElement *el = dynamic_cast<TEveElement*> (obj);
	if(el){
		//cout<<" converted toTEveElement* "<<endl; 
		if(el->GetUserData() !=NULL){
			//cout<<" user data found "<<el->GetUserData()<<endl;
			cout<<" user data is <"<<*((TString*)el->GetUserData())<<">"<<endl;
		}
		//else 
		//	cout<<" no user data found "<<endl;
	}
	THKCerenkov *C = dynamic_cast<THKCerenkov*> (obj);
	if(C){
		C->Describe();
		cout<<" For information on individual hits do CTRL+ALT (Left mouse click)"<<endl;
		return;
	}
	THKGamma *g = dynamic_cast<THKGamma*> (obj);
	if(g){
		g->Describe();
		return;
	}
	THKMCTrack *mc =  dynamic_cast<THKMCTrack*> (obj);
	if(mc)
	{
		mc->Describe();
		return;
	}
	THKCerenkov2D *c2 =  dynamic_cast< THKCerenkov2D*> (obj);
	if(c2)
	{
		c2->Describe();
		cout<<" For information on individual hits do CTRL+ALT (Left mouse click)"<<endl;
		return;
	}
	TEveBoxSet *bs = dynamic_cast<TEveBoxSet*> (obj);
	if(bs)
	{
		cout<<"You clicked on "<<bs->GetName()<<" "<<bs->GetTitle()<<endl;
		cout<<" For information on individual hits do CTRL+ALT+(Left mouse click)"<<endl;
		return;
	}
	obj->ls();
	cout<<Form("User clicked on: \"%s\"", obj->GetName())<<endl;
	cout<<" of class "<<obj->ClassName()<<endl;
//if (obj)
//	gEve->GetBrowser()->GetStatusBar()->SetText(Form("User clicked on: \"%s\"", obj->GetName()), 1);
//else
//	gEve->GetBrowser()->GetStatusBar()->SetText("", 1);
}	
