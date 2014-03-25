#include <TFile.h>
#include <TTree.h>

#include <iostream>

using std::cout;		using std::endl;

void pulseConvert(const char* ifname)
{
   TFile* ifile = TFile::Open(ifname);
   if (!ifile) {
      cout<< "File not found: " << ifname <<endl;
      return;
   }
   TTree* itree = (TTree*) ifile->Get("pulse");
   if (!itree) {
      cout<< "Error: could not find tree \"pulse\"" <<endl;
      return;
   }

   Int_t event, tc1, tc2;
   Float_t b1_t[1024], b1_c[4096], b2_t[1024], b2_c[4096];

   Float_t* b1_c1 = b1_c;
   Float_t* b1_c2 = b1_c + 1024;
   Float_t* b1_c3 = b1_c + 2048;
   Float_t* b1_c4 = b1_c + 3072;
   Float_t* b2_c1 = b2_c;
   Float_t* b2_c2 = b2_c + 1024;
   Float_t* b2_c3 = b2_c + 2048;
   Float_t* b2_c4 = b2_c + 3072;

   itree->SetBranchAddress("event", &event);
   itree->SetBranchAddress("tc1", &tc1);
   itree->SetBranchAddress("tc2", &tc2);
   itree->SetBranchAddress("b1_t", &b1_t);
   itree->SetBranchAddress("b2_t", &b2_t);
   itree->SetBranchAddress("b1_c", &b1_c);
   itree->SetBranchAddress("b2_c", &b2_c);

   TFile* ofile = TFile::Open(Form("%s.pulse.root",ifname), "recreate");

   // TTree* otree = new TTree("pulse", "old pulse tree");
   TTree* otree = new TTree("p", "new pulse tree");
   otree->SetMarkerStyle(6);
   otree->SetMarkerColor(2);
   otree->SetLineColor(2);

   // otree->Branch("event", &event, "event/I");
   // otree->Branch("tc1", &tc1, "tc1/I");
   // otree->Branch("b1_t",  b1_t, "b1_t[1024]/F");
   // otree->Branch("b1_c1", b1_c1, "b1_c1[1024]/F");
   // otree->Branch("b1_c2", b1_c2, "b1_c2[1024]/F");
   // otree->Branch("b1_c3", b1_c3, "b1_c3[1024]/F");
   // otree->Branch("b1_c4", b1_c4, "b1_c4[1024]/F");
   // otree->Branch("tc2", &tc2, "tc2/I");
   // otree->Branch("b2_t",  b2_t, "b2_t[1024]/F");
   // otree->Branch("b2_c1", b2_c1, "b2_c1[1024]/F");
   // otree->Branch("b2_c2", b2_c2, "b2_c2[1024]/F");
   // otree->Branch("b2_c3", b2_c3, "b2_c3[1024]/F");
   // otree->Branch("b2_c4", b2_c4, "b2_c4[1024]/F");

   otree->Branch("event", &event, "event/I");
   otree->Branch("tc1", &tc1, "tc1/I");
   otree->Branch("t1",  b1_t, "t1[1024]/F");
   otree->Branch("c1", b1_c1, "c1[1024]/F");
   otree->Branch("c2", b1_c2, "c2[1024]/F");
   otree->Branch("c3", b1_c3, "c3[1024]/F");
   otree->Branch("c4", b1_c4, "c4[1024]/F");
   otree->Branch("tc2", &tc2, "tc2/I");
   otree->Branch("t2",  b2_t, "t2[1024]/F");
   otree->Branch("c5", b2_c1, "c5[1024]/F");
   otree->Branch("c6", b2_c2, "c6[1024]/F");
   otree->Branch("c7", b2_c3, "c7[1024]/F");
   otree->Branch("c8", b2_c4, "c8[1024]/F");

   for (Long64_t jentry=0; jentry<itree->GetEntries(); jentry++)
   {
      if (jentry % 1000 == 0) cout<< "jentry = " << jentry <<endl;
      itree->LoadTree(jentry);
      itree->GetEntry(jentry);

      otree->Fill();
   }

   cout<< "Write " << otree->GetEntries() << " events into file " << ofile->GetName() <<endl;
   ofile->Write();
}
