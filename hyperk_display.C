{
	gROOT->ProcessLine(".L $WCSIMDIR/libWCSimRoot.so");
	gSystem->AddIncludePath(" -I$FITQUN_ROOT");
	gSystem->AddIncludePath(" -I$WCSIMDIR/include/");
	gROOT->ProcessLine(".L hyperk_esd_html_summary.C+");
	gROOT->ProcessLine(".L THKGamma.C+");
	gROOT->ProcessLine(".L THKMCTrack.C+");
	gROOT->ProcessLine(".L THKCerenkov.C+");
	gROOT->ProcessLine(".L THKCerenkov2D.C+");
	gROOT->ProcessLine(".L THKLine.C+");

	Long_t* id;
	Long_t* size;
	Long_t* flags;
	Long_t* modtime;
	TString fitqun_dir = gSystem->Getenv("FITQUN_ROOT");
	ofstream fitqundef_file("fitqun_def.h");
	if(!gSystem->GetPathInfo(fitqun_dir.Data(),id,size,flags,modtime))
		{
			fitqundef_file << "#define FITQUNEXISTS" << endl;
			gROOT->ProcessLine(".L fitQunDisplay.C+");
		}
	fitqundef_file.close();

	gROOT->ProcessLine(".L Picker.C+");
	gROOT->ProcessLine(".x hyperk_esd.C");

}

