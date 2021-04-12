#ifndef HYPERK_ESD_HTML_SUMMARY
#define HYPERK_ESD_HTML_SUMMARY
// @(#)root/eve:$Id$

// Html table and event summary for hyperk_esd.C

//==============================================================================
#include <TOrdCollection.h>
#include <TArrayF.h>
#include <TArrayI.h>
class HtmlObjTable : public TObject
{
public:                     // make them public for shorter code
	
	TString   fName;
	Int_t     fNValues;      // number of values
	Int_t     fNFields;      // number of fields
	TArrayF  *fValues;
	bool     *fIsInt;
	TString  fRowNames[1000];
	TString  *fLabels;
	Bool_t    fExpand;
	
	TString   fHtml;         // HTML output code
	
	void Build();
	void BuildTitle();
	void BuildLabels();
	void BuildTable();
	
public:
	HtmlObjTable(const char *name, Int_t nfields, Bool_t exp=kTRUE);
	virtual ~HtmlObjTable();
	
	void     IsInteger(Int_t col){fIsInt[col]=kTRUE;}
	void     SetLabel(Int_t col, const char *label) { fLabels[col] = label; }
	void     SetValue(Int_t col, Int_t row, Float_t val) ;
	void     SetRowName(Int_t row, TString val) { fRowNames[row]=val ;
	    fNValues=std::max(fNValues,row);
	}
	TString  Html() const { return fHtml; }
	
	ClassDef(HtmlObjTable, 0);
};

//==============================================================================

class HtmlSummary
{
public:                           // make them public for shorter code
	Int_t           fNTables;
	TOrdCollection *fObjTables;    // ->array of object tables
	TString         fHtml;         // output HTML string
	TString         fTitle;        // page title
	TString         fHeader;       // HTML header
	TString         fFooter;       // HTML footer
	
	void     MakeHeader();
	void     MakeFooter();
	void     SetTitle(TString t){fTitle=t;}
	
public:
	HtmlSummary(const char *title);
	virtual ~HtmlSummary();
	
	HtmlObjTable  *AddTable(const char *name, Int_t nfields, 
		Bool_t exp=kTRUE, Option_t *opt="");
	HtmlObjTable  *GetTable(Int_t at) const { return (HtmlObjTable *)fObjTables->At(at); }
	void           Build();
	void           Clear(Option_t *option="");
	void           Reset(Option_t *option="");
	TString        Html() const { return fHtml; }
	
	ClassDef(HtmlSummary, 0);
};
#endif
