#include "hyperk_esd_html_summary.h"
#include "WCSimRootEvent.hh"
//==============================================================================

//==============================================================================

//______________________________________________________________________________
HtmlObjTable::HtmlObjTable(const char *name, Int_t nfields,  Bool_t exp) : 
fName(name),  fNFields(nfields), fExpand(exp)
{
	// Constructor.
	fNValues=0;
	fValues = new TArrayF[fNFields];
	fIsInt  = new bool[fNFields];
	fIsString  = new bool[fNFields];

	for (int ix=0;ix<fNFields;ix++)
	{
                fIsInt[ix]=kFALSE;
                fIsString[ix]=kFALSE;
	}
	fLabels = new TString[fNFields];
}
void     HtmlObjTable::SetValue(Int_t col, Int_t row, Float_t val) 
{
	if(row+1>fNValues)
	{
        	fValues[col].Set(row+1);
        }
	fValues[col].SetAt(val, row); 
}
void     HtmlObjTable::SetStringValue(Int_t col, Int_t row, TString val) 
{

	if((unsigned int) (row+1)>fStringValues.size())
	{
		fStringValues.resize(row+1);
		fStringValues[row].resize(fNFields);
    }
	fStringValues[row][col]=val; 
	fIsString[col]=kTRUE;
}
//______________________________________________________________________________
HtmlObjTable::~HtmlObjTable()
{
	// Destructor.
	delete [] fValues;
	delete [] fIsInt;
	delete [] fLabels;
}

//______________________________________________________________________________
void HtmlObjTable::Build()
{
	// Build HTML code.
	
	fHtml = "<table width=100% border=1 cellspacing=0 cellpadding=0 bgcolor=f0f0f0> ",
	
	BuildTitle();
	if (fExpand && (fNFields > 0) && (fNValues > 0)) {
		BuildLabels();
		BuildTable();
	}
	
	fHtml += "</table>";
}

//______________________________________________________________________________
void HtmlObjTable::BuildTitle()
{
	// Build table title.
	
	fHtml += "<tr><td colspan=";
	fHtml += Form("%d>", fNFields+1);
	fHtml += "<table width=100% border=0 cellspacing=2 cellpadding=0 bgcolor=6e6ea0>";
	fHtml += "<tr><td align=left>";
	fHtml += "<font face=Verdana size=3 color=ffffff><b><i>";
	fHtml += fName;
	fHtml += "</i></b></font></td>";
	fHtml += "<td>";
	fHtml += "<td align=right> ";
	fHtml += "<font face=Verdana size=3 color=ffffff><b><i>";
	//fHtml += Form("Size = %d", fNValues);
	fHtml += "</i></b></font></td></tr>";
	fHtml += "</table>";
	fHtml += "</td></tr>";
}

//______________________________________________________________________________
void HtmlObjTable::BuildLabels()
{
	// Build table labels.
	
	Int_t i;
	fHtml += "<tr bgcolor=c0c0ff>";
	//fHtml += "<th> </th>"; // for the check boxes
	for (i=0;i<fNFields;i++) {
		fHtml += "<th> ";
		fHtml += fLabels[i];
		fHtml += " </th>"; // for the check boxes
	}
	fHtml += "</tr>";
}

//______________________________________________________________________________
void HtmlObjTable::BuildTable()
{
	// Build part of table with values.
	
	for (int i = 0; i < (fNValues+1); i++) {

		if (i%2)
			fHtml += "<tr bgcolor=e0e0ff>";
		else
			fHtml += "<tr bgcolor=ffffff>";
		
		TString name = fName;
		name.ReplaceAll(" ", "_");
		// checkboxes
		//	fHtml += "<td bgcolor=d0d0ff align=\"center\">";
		//	fHtml += "<input type=\"checkbox\" name=\"";
		//	fHtml += name;
		//	fHtml += Form("[%d]\">",i);
		//	fHtml += "</td>";
		fHtml += Form("<td> %s", fRowNames[i].Data());
		fHtml += "</td>";
		for (int j = 1; j < fNFields; j++) {

			fHtml += "<td width=";
			fHtml += Form("%d%%", 100/fNFields);
			fHtml += " align=\"center\"";
			fHtml += ">";
			if(fIsInt[j]){
				fHtml += Form("%2i", (int)fValues[j][i]);

			}
			else if (fIsString[j])
			     fHtml += fStringValues[i][j]; // note reversed row, column order of indices
			else
			{


				if(fValues[j].GetSize()>i)
					fHtml += Form("%1.2f", fValues[j][i]);
				else
					fHtml += "-";
			}
			fHtml += "</td>";

		}
		fHtml += "</tr> ";
	}
}

//______________________________________________________________________________
HtmlSummary::HtmlSummary(const char *title) : fNTables(0), fTitle(title)
{
	// Constructor.
	
	fObjTables = new TOrdCollection();
}

//______________________________________________________________________________
HtmlSummary::~HtmlSummary()
{
	// Destructor.
	
	Reset();
}

//______________________________________________________________________________
HtmlObjTable *HtmlSummary::AddTable(const char *name, Int_t nfields, 
	Bool_t exp, Option_t *option)
{
	// Add a new table in our list of tables.
	
	TString opt = option;
	opt.ToLower();
	HtmlObjTable *table = new HtmlObjTable(name, nfields, exp);
	fNTables++;
	if (opt.Contains("first"))
		fObjTables->AddFirst(table);
	else
		fObjTables->Add(table);
	return table;
}

//______________________________________________________________________________
void HtmlSummary::Clear(Option_t *option)
{
	// Clear the table list.
	
	if (option && option[0] == 'D')
		fObjTables->Delete(option);
	else
		fObjTables->Clear(option);
	fNTables = 0;
}

//______________________________________________________________________________
void HtmlSummary::Reset(Option_t *)
{
	// Reset (delete) the table list;
	
	delete fObjTables; fObjTables = 0;
	fNTables = 0;
}

//______________________________________________________________________________
void HtmlSummary::Build()
{
	// Build the summary.
	
	MakeHeader();
	for (int i=0;i<fNTables;i++) {
		GetTable(i)->Build();
		fHtml += GetTable(i)->Html();
	}
	MakeFooter();
}

//______________________________________________________________________________
void HtmlSummary::MakeHeader()
{
	// Make HTML header.
	
	fHeader  = "<html><head><title>";
	fHeader += fTitle;
	fHeader += "</title></head><body>";
	fHeader += "<center><h2><font color=#2222ee><i>";
	fHeader += fTitle;
	fHeader += "</i></font></h2></center>";
	fHtml    = fHeader;
}

//______________________________________________________________________________
void HtmlSummary::MakeFooter()
{
	// Make HTML footer.
	
	fFooter  = "<br><p><br><center><strong><font size=2 color=#2222ee>";
	//	fFooter += "Example of using Html widget to display tabular data";
	//	fFooter += "<br>";
	//	fFooter += "(c) 2007-2010 Bertrand Bellenot";
	fFooter += "</font></strong></center></body></html>";  
	fHtml   += fFooter;
}

//==============================================================================


