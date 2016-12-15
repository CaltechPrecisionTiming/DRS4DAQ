#include <iostream>
#include <fstream>
//ROOT
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TF1.h>

int main() 
{
  //test
  float ch[18];
  TH1F* h[18];
  TF1* f[18];
  TF1* c[18];
  TH1F* deltaT = new TH1F("deltaT", "deltaT", 1000, -1, 1);
  for ( int i = 0; i < 18; i++ )
    {
      h[i] = new TH1F( Form("ch_%d",i), Form("ch_%d",i), 1024, 0, 1023 );
      f[i] = new TF1 ( Form("f_%d",i), "[0]*x+[1]", 125, 140);
      c[i] = new TF1 ( Form("c_%d",i), "[0]", 0, 1023);
    }
  
  std::ifstream ifs( "caltech_format_singleEventSiPM.txt", std::ifstream::in );
  int j = 0;
  int event = 0;
  char buffer[1000];
   
  if ( ifs.is_open() )
    {
      while ( ifs.good() )
	{
	  if (event == 50 ) break;
	  if ( j == 0  && event == 0 )
	    {
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	    }
	  else if ( j == 0  && event != 0 )
	    {
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	      ifs.getline( buffer, 1000);
	    }
	  ifs >> ch[0] >> ch[1] >> ch[2] >> ch[3] >> ch[4] >> ch[5] >> ch[6] >> ch[7] >> ch[8] >> ch[9] >> ch[10] >> ch[11] >> ch[12] >> ch[13] >> ch[14] >> ch[15] >> ch[16] >> ch[17];

	  if ( ifs.eof() ) break;
	  //std::cout << "sample: " << j  << " " << ch[0] << std::endl;
	  for ( int i = 0; i < 18; i++ )
	    {
	      h[i]->SetBinContent( j+1, ch[i]/4096.0 -0.5 );
	    }
	  
	  j++;
	  if ( j == 1024 )
	    {
	      h[0]->Fit(c[0], "LQ", "", 1,10);
	      h[1]->Fit(c[1], "LQ", "", 1,10);
	      for( int k = 0; k < 1023; k++ )
		{
		  h[0]->SetBinContent( k+1, h[0]->GetBinContent( k+1 ) - c[0]->GetParameter(0) );
		  h[1]->SetBinContent( k+1, h[1]->GetBinContent( k+1 ) - c[1]->GetParameter(0) );
		}
	      
	      h[0]->Fit(c[0], "LQ", "", h[0]->FindFirstBinAbove(0.98*h[0]->GetMaximum()) , h[0]->FindFirstBinAbove(0.99*h[0]->GetMaximum())+3);
	      h[1]->Fit(c[1], "LQ", "", h[1]->FindFirstBinAbove(0.98*h[1]->GetMaximum()) , h[1]->FindFirstBinAbove(0.99*h[1]->GetMaximum())+3);
	      float max_0 = c[0]->GetParameter(0);
	      float max_1 = c[1]->GetParameter(0);
	      //std::cout << max_0 << " " << max_1 << std::endl;
	      h[0]->Fit(f[0], "LQ", "", h[0]->FindFirstBinAbove(0.2*h[0]->GetMaximum()) , h[0]->FindFirstBinAbove(0.80*h[0]->GetMaximum()));
	      h[1]->Fit(f[1], "LQ", "", h[1]->FindFirstBinAbove(0.2*h[1]->GetMaximum()) , h[1]->FindFirstBinAbove(0.80*h[1]->GetMaximum()));

	      float t0 = 0.20*(0.2*max_0 - f[0]->GetParameter(1))/f[0]->GetParameter(0);
	      float t1 = 0.20*(0.2*max_1 - f[1]->GetParameter(1))/f[1]->GetParameter(0);
	      //std::cout << "t0: " << t0 << " t1: " << t1 << std::endl;
	      deltaT->Fill(t0-t1);
	      j = 0;
	      event++;
	    }
	}
    }
  else
    {
      std::cout << "Unable to open TXT file, please check" << std::endl;
    }
  

  TFile* fout = new TFile("fout.root", "recreate");

  for ( int i = 0; i < 18; i++ ) h[i]->Write();
  deltaT->Write();
  fout->Close();
  
  return 0;
}
