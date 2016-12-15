#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main()
{
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
  TFile *f = new TFile("testBinary.root","recreate");
  TTree *tree = new TTree("Tree","Tree");

  int event;
  float time[2][1024];
  short raw[18][1024];
  short channel[18][1024];
  float channelCorrected[18][1024];

  tree->Branch("event", &event, "event/I");
  tree->Branch("raw", raw, "raw[18][1024]/S");   
  tree->Branch("channel", channel, "channel[18][1024]/S");
  tree->Branch("time", time, "time[2][1024]/F");

  int  event_size = 73752;
  int nevents = lSize/event_size;

  //*************************
  //Event Loop
  //*************************
  event = 0;
  for( int ievent = 0; ievent < nevents; ievent++)
    {
  	// copy the file into the buffer:
	  result = fread (buffer,1,event_size,pFile);
	  std::cout << result << " " << ievent  << std::endl;
	  if (result != event_size) {fputs ("Reading error\n",stderr); exit (3);}
	  double x;
	  x = *(buffer+6);
	  for(int i = 0; i < 1024; i++ )
	    { 
 		for(int j = 0; j < 18; j++)
		{
		  channel[j][i] = buffer[i+6+j*1024];
		  raw[j][i] = buffer[i+6+j*1024];
		}
	    }
	  tree->Fill();	
	  event++;
     }	 

  for (int i = 0; i < 1024; i++ ) std::cout << buffer[i+6] << " " << buffer[5*1024+i+6] << " " << buffer[2048+i+6] << std::endl;

  //event2
/*
  result = fread (buffer,1,lSize,pFile);
  std::cout << "EVENT 2  --> "<< result << " " << lSize << std::endl;
  if (result != lSize) {fputs ("Reading error\n",stderr); exit (3);}
*/
  for (int i = 0; i < 1024; i++ ) std::cout << buffer[i+6] << " " << buffer[5*1024+i+6] << " " << buffer[2048+i+6] << std::endl;
  /* the whole file is now loaded in the memory buffer. */

  //float myN =  *buffer << 4;
  //float myN2 =  *buffer >> 4;
  //float myN3 =  *buffer & 0xFFF0;
  //float myN4 =  *buffer & 0x0FFF;
  //for (int i = 0; i < 1024; i++ )printf("%f\n", *(buffer+i));
  //for(int i = 0; i < 1024*18; i++ )printf("%f\n", *(buffer+i));
  //printf("%f %f %f %f\n", myN, myN2, myN3, myN4);
  // terminate
/*
  TH1F *ch0 = new TH1F("ch0","ch0",1024,0,1024);
  TH1F *ch1 = new TH1F("ch1","ch1",1024,0,1024);
  TH1F *ch2 = new TH1F("ch2","ch2",1024,0,1024);
  TH1F *ch3 = new TH1F("ch3","ch3",1024,0,1024);
  TH1F *ch4 = new TH1F("ch4","ch4",1024,0,1024);
  TH1F *ch5 = new TH1F("ch5","ch5",1024,0,1024);
  TH1F *ch6 = new TH1F("ch6","ch6",1024,0,1024);
  TH1F *ch7 = new TH1F("ch7","ch7",1024,0,1024);

  TH1F *tr0 = new TH1F("tr0","tr0",1024,0,1024);

  TH1F *ch8 = new TH1F("ch8","ch8",1024,0,1024);
  TH1F *ch9 = new TH1F("ch9","ch9",1024,0,1024);
  TH1F *ch10 = new TH1F("ch10","ch10",1024,0,1024);
  TH1F *ch11 = new TH1F("ch11","ch11",1024,0,1024);
  TH1F *ch12 = new TH1F("ch12","ch12",1024,0,1024);
  TH1F *ch13 = new TH1F("ch13","ch13",1024,0,1024);
  TH1F *ch14 = new TH1F("ch14","ch14",1024,0,1024);
  TH1F *ch15 = new TH1F("ch15","ch15",1024,0,1024);

  TH1F *tr1 = new TH1F("tr1","tr1",1024,0,1024);	
*/
  double x;
  int ctr = 1;
  //x = *(buffer+ctr);
/*  
  while(ctr>0 && ctr<1024+1) { x = *(buffer+ctr); ch0->SetBinContent(ctr,x); ctr++;}
	    while(ctr>1024 && ctr<1024*2+1) { x = *(buffer+ctr); ch1->SetBinContent(ctr-1024,x); ctr++;}
	    while(ctr>1024*2 && ctr<1024*3+1) { x = *(buffer+ctr); ch2->SetBinContent(ctr-1024*2,x); ctr++;}
	    while(ctr>1024*3 && ctr<1024*4+1) { x = *(buffer+ctr); ch3->SetBinContent(ctr-1024*3,x); ctr++;}
	    while(ctr>1024*4 && ctr<1024*5+1) { x = *(buffer+ctr); ch4->SetBinContent(ctr-1024*4,x); ctr++;}
	    while(ctr>1024*5 && ctr<1024*6+1) { x = *(buffer+ctr); ch5->SetBinContent(ctr-1024*5,x); ctr++;}
	    while(ctr>1024*6 && ctr<1024*7+1) { x = *(buffer+ctr); ch6->SetBinContent(ctr-1024*6,x); ctr++;}
	    while(ctr>1024*7 && ctr<1024*8+1) { x = *(buffer+ctr); ch7->SetBinContent(ctr-1024*7,x); ctr++;}
		
	    while(ctr>1024*8 && ctr<1024*9+1) { x = *(buffer+ctr); tr0->SetBinContent(ctr-1024*8,x); ctr++;}

	    while(ctr>1024*9 && ctr<1024*10+1) { x = *(buffer+ctr); ch8->SetBinContent(ctr-1024*9,x); ctr++;}
	    while(ctr>1024*10 && ctr<1024*11+1) { x = *(buffer+ctr); ch9->SetBinContent(ctr-1024*10,x); ctr++;}
	    while(ctr>1024*11 && ctr<1024*12+1) { x = *(buffer+ctr); ch10->SetBinContent(ctr-1024*11,x); ctr++;}
	    while(ctr>1024*12 && ctr<1024*13+1) { x = *(buffer+ctr); ch11->SetBinContent(ctr-1024*12,x); ctr++;}
	    while(ctr>1024*13 && ctr<1024*14+1) { x = *(buffer+ctr); ch12->SetBinContent(ctr-1024*13,x); ctr++;}
	    while(ctr>1024*14 && ctr<1024*15+1) { x = *(buffer+ctr); ch13->SetBinContent(ctr-1024*14,x); ctr++;} 
	    while(ctr>1024*15 && ctr<1024*16+1) { x = *(buffer+ctr); ch14->SetBinContent(ctr-1024*15,x); ctr++;}
	    while(ctr>1024*16 && ctr<1024*17+1) { x = *(buffer+ctr); ch15->SetBinContent(ctr-1024*16,x); ctr++;}
	
	    while(ctr>1024*17 && ctr<1024*18+1) { tr1->SetBinContent(ctr-1024*17,x); ctr++;}
*/
  tree->Write();
  f->Write();
  f->Close();
  fclose (pFile);
  free (buffer);
  return 0;
}
