#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <RooRealVar.h>
#include <RooPlot.h>
#include <RooDataSet.h>
#include <RooGaussian.h>
#include <TStyle.h>

//LOCAL INCLUDES
#include "Aux.hh"

int main(int argc, char **argv)
{


  bool drawDebugPulses = false;
  drawDebugPulses = true;
  /*  std::string _drawDebugPulses = ParseCommandLine( argv, "--debug" );
  bool drawDebugPulses = false;
  if ( _drawDebugPulses == "yes" ) {
    drawDebugPulses = true;
    std::cout << "draw: " << drawDebugPulses << std::endl;
  }
*/

  FILE* pFile;
  long lSize;
  float* buffer;
  size_t result;

  pFile = fopen ( "caltech_12_14_2016_v3.dat" , "rb" );
  if (pFile==NULL) {fputs ("File error\n",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  //printf("%d\n", lSize);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (float*) malloc (sizeof(float)*lSize);
  if (buffer == NULL) {fputs ("Memory error\n",stderr); exit (2);}
  
  //define the ouput root file and tree
  TFile *f = new TFile("output.root","recreate");
  TTree *tree = new TTree("data","data");

  int event;
  int bin[1024];
  float time[2][1024];
  float raw[18][1024];
  float channel[18][1024];
  float channelCorrected[18][1024];
  float base[18];
  int baseindex[18];
  float gauspeak[18];
  float linearTime0[18];
  float linearTime15[18];
  float linearTime30[18];
  float linearTime45[18];
  float linearTime60[18];
  float deltat;
  float time_resolution;

  tree->Branch("event", &event, "event/I");
  tree->Branch("bin", bin, "bin[1024]/I");   
  tree->Branch("raw", raw, "raw[18][1024]/F");   
  tree->Branch("channel", channel, "channel[18][1024]/F");
  tree->Branch("channelCorrected", channelCorrected, "channelCorrected[18][1024]/F");
  tree->Branch("time", time, "time[2][1024]/F");
  tree->Branch("base", base, "base[18]/F");
  tree->Branch("baseindex", baseindex, "baseindex[18]/I");
  tree->Branch("gauspeak", gauspeak, "gauspeak[18]/F");
  tree->Branch("linearTime0", linearTime0, "linearTime0[18]/F");
  tree->Branch("linearTime15", linearTime15, "linearTime15[18]/F");
  tree->Branch("linearTime30", linearTime30, "linearTime30[18]/F");
  tree->Branch("linearTime45", linearTime45, "linearTime45[18]/F");
  tree->Branch("linearTime60", linearTime60, "linearTime60[18]/F");
  tree->Branch("deltat", &deltat, "deltat/F");
  tree->Branch("time_resolution", &time_resolution, "time_resolution/F");

  TH1F *dt = new TH1F ("dt","deltat",100,0,0.1);
  int  event_size = 73752;
  int nevents = lSize/event_size;

  float tmp=0;

  std::cout << "nevents: " << nevents << std::endl;
  //*************************
  //Event Loop
  //*************************
  event = 0;
  for( int ievent = 0; ievent < nevents; ievent++)
    {
      if ( ievent % 1000 == 0 ) std::cout << "[INFO]: processing event #" << ievent << std::endl;
  	// copy the file into the buffer:
	  result = fread (buffer,1,event_size,pFile);
	  //std::cout << result << " " << ievent  << std::endl;
	  if (result != event_size) {fputs ("Reading error\n",stderr); exit (3);}
	  double x;
	  x = *(buffer+6);
 	for(int j = 0; j < 18; j++)
	    { 
	  	for(int i = 0; i < 1024; i++ )
			{
		  		raw[j][i] = buffer[i+6+j*1024];
		  		channel[j][i] = ( raw[j][i] - 2047. )/4096. ;//converting to volts [V]
			  	//std::cout << "i = " << i << " ; j = " << j << " ; raw[j][i] = " << raw[j][i] << " ; channel[j][i] = " << channel[j][i] << std::endl; 
				bin[i] = i;
				time[0][i] = i*0.2;
				time[1][i] = i*0.2;
			}
	    }
 	for(int j = 0; j < 18; j++)
	    { 
		//Find Peak Location
        	int index_min = FindMin (1024, channel[j]); // return index of the min     
	  	//if(j==0) std::cout << j <<  " index_min =  " << index_min  << std::endl;
		baseindex[j] = index_min;
        	//int index_minraw = FindMin (1024, raw[j]); // return index of the min     
	  	//std::cout << j <<  " index_minraw =  " << index_minraw  << std::endl;
		
		//Estimate baseline
        	float baseline = GetBaseline( index_min, channel[j]);
	  	//std::cout << j <<  " baseline =  " << baseline  << std::endl;
        	//float baselineraw = GetBaseline( index_minraw, raw[j]);
	  	//std::cout << j <<  " baselineraw =  " << baselineraw  << std::endl;
                base[j] = baseline;

		//Correct pulse shape for baseline offset
	  	for(int i = 0; i < 1024; i++ )
		{
			channelCorrected[j][i] = channel[j][i] - base[j];
		}
	
		//Make Pulse shape Graph
		TString pulseName = Form("pulse_event%d_ch%d", event, j);
		TGraphErrors* pulse = GetTGraph( channelCorrected[j], time[0] );	
		//if(j<9) TGraphErrors* pulse = GetTGraph( channelCorrected[j], time[0] );	
		//else TGraphErrors* pulse = GetTGraph( channelCorrected[j], time[1] );	

		//Gauss Time-Stamping 
		double min = 0.; double low_edge =0.; double high_edge =0.; double y = 0.; 
		pulse->GetPoint(index_min, min, y);	
		pulse->GetPoint(index_min-3, low_edge, y); // get the time of the low edge of the fit range
		pulse->GetPoint(index_min+3, high_edge, y);  // get the time of the upper edge of the fit range	

			
		float timepeak = 0.;
		float timecf0   = 0;
		float timecf15   = 0;
		float timecf30   = 0;
		float timecf45   = 0;
		float timecf60   = 0;
		if( drawDebugPulses) {
		  //std::cout << "draw -->" << pulseName << std::endl;
			timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge, pulseName); // get the time stamp
			float fs[5];
			RisingEdgeFitTime( pulse, index_min, fs, "linearFit_" + pulseName, false);
			timecf0  = fs[0];
			timecf15 = fs[1];
			timecf30 = fs[2];
			timecf45 = fs[3];
			timecf60 = fs[4];
	 	} else{
			timepeak =  GausFit_MeanTime(pulse, low_edge, high_edge); // get the time stamp
	  		float fs[5];
	  		RisingEdgeFitTime( pulse, index_min, fs, "");
	  		timecf0  = fs[0];
			timecf15 = fs[1];
			timecf30 = fs[2];
			timecf45 = fs[3];
			timecf60 = fs[4];
		}
		gauspeak[j]   = timepeak;
		linearTime0[j] = timecf0;
		linearTime15[j] = timecf15;
		linearTime30[j] = timecf30;
		linearTime45[j] = timecf45;
		linearTime60[j] = timecf60; 
	  	//if(j==0) std::cout <<  " timepeak  =  " << timepeak  << std::endl;

	    }

		deltat = gauspeak[0] - gauspeak[1];
		if(deltat>tmp) tmp = deltat;
	  	/*std::cout <<  "event =  " << event  << std::endl;
	  	std::cout <<  "ch0: index_min =  " << baseindex[0]  << std::endl;
	  	std::cout <<  " peak  =  " << channel[0][baseindex[0]]  << std::endl;
	  	std::cout <<  " base  =  " << base[0]  << std::endl;
	  	std::cout <<  "ch8: index_min =  " << baseindex[8]  << std::endl;
	  	std::cout <<  " peak  =  " << channel[8][baseindex[8]]  << std::endl;
	  	std::cout <<  " base  =  " << base[8]  << std::endl;
*/

	  if(deltat!=0) dt->Fill(deltat);
	  if(deltat!=0) tree->Fill();	
	  event++;
     }
    
  gStyle->SetOptFit();
  TF1* fpeak = new TF1("fpeak","gaus", 0.02 , 0.035);
  //fpeak->SetParameter(1,0.025); 
  TCanvas* c = new TCanvas("canvas","canvas",800,400) ;
  //dt->GetXaxis()->SetLimits(0.01,0.06);
  //dt->GetYaxis()->SetRangeUser(0,100);
  //dt->SetMarkerSize(1);
  //dt->SetMarkerStyle(20);
  dt->Fit( fpeak,"","R");
  time_resolution = fpeak->GetParameter(2);
  dt->Draw();
  //fpeak->Draw("SAME");
  //fpeak->SetLineColor(2);
  c->SaveAs("time_resolution.pdf");
  delete fpeak;

  std::cout <<  " time resolution  =  " << time_resolution  << std::endl;
	
  //event2
/*
  result = fread (buffer,1,lSize,pFile);
  std::cout << "EVENT 2  --> "<< result << " " << lSize << std::endl;
  if (result != lSize) {fputs ("Reading error\n",stderr); exit (3);}
*/
//  for (int i = 0; i < 1024; i++ ) std::cout << buffer[i+6] << " " << buffer[5*1024+i+6] << " " << buffer[2048+i+6] << std::endl;
//  for (int i = 0; i < 1024; i++ ) std::cout << buffer[i+6] << " " << buffer[5*1024+i+6] << " " << buffer[2048+i+6] << std::endl;
  /* the whole file is now loaded in the memory buffer. */

  //float myN =  *buffer << 4;
  //float myN2 =  *buffer >> 4;
  //float myN3 =  *buffer & 0xFFF0;
  //float myN4 =  *buffer & 0x0FFF;
  //for (int i = 0; i < 1024; i++ )printf("%f\n", *(buffer+i));
  //for(int i = 0; i < 1024*18; i++ )printf("%f\n", *(buffer+i));
  //printf("%f %f %f %f\n", myN, myN2, myN3, myN4);
  // terminate

  tree->Write();
  f->Write();
  f->Close();
  fclose (pFile);
  free (buffer);
  return 0;
}
