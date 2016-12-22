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
  float raw[18][1024];
  float channel[18][1024];
  float channelCorrected[18][1024];

  tree->Branch("event", &event, "event/I");
  tree->Branch("raw", raw, "raw[18][1024]/F");   
  tree->Branch("channel", channel, "channel[18][1024]/F");
  tree->Branch("time", time, "time[2][1024]/F");

  int  event_size = 73752;
  int nevents = lSize/event_size;

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
	  for(int i = 0; i < 1024; i++ )
	    { 
		time[0][i] = i*0.2;
		time[1][i] = i*0.2;
 		for(int j = 0; j < 18; j++)
		{
		   //channel[j][i] = buffer[i+6+j*1024];
		  raw[j][i] = buffer[i+6+j*1024];
		  channel[j][i] = ( raw[j][i] - 2047. )/4096. ;//converting to volts [V]
		  //std::cout << "i = " << i << " ; j = " << j << " ; raw[j][i] = " << raw[j][i] << " ; channel[j][i] = " << channel[j][i] << std::endl; 
		}
	    }
	  tree->Fill();	
	  event++;
     }	


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

  TH1F *channel0 = new TH1F("channel0","channel0",1024,-0.5,0.5);
  TH1F *channel1 = new TH1F("channel1","channel1",1024,-0.5,0.5);
  TH1F *channel2 = new TH1F("channel2","channel2",1024,-0.5,0.5);
  TH1F *channel3 = new TH1F("channel3","channel3",1024,-0.5,0.5);
  TH1F *channel4 = new TH1F("channel4","channel4",1024,-0.5,0.5);
  TH1F *channel5 = new TH1F("channel5","channel5",1024,-0.5,0.5);
  TH1F *channel6 = new TH1F("channel6","channel6",1024,-0.5,0.5);
  TH1F *channel7 = new TH1F("channel7","channel7",1024,-0.5,0.5);
  TH1F *channel8 = new TH1F("channel8","channel8",1024,-0.5,0.5);
  TH1F *channel9 = new TH1F("channel9","channel9",1024,-0.5,0.5);
  TH1F *channel10 = new TH1F("channel10","channel10",1024,-0.5,0.5);
  TH1F *channel11 = new TH1F("channel11","channel11",1024,-0.5,0.5);
  TH1F *channel12 = new TH1F("channel12","channel12",1024,-0.5,0.5);
  TH1F *channel13 = new TH1F("channel13","channel13",1024,-0.5,0.5);
  TH1F *channel14 = new TH1F("channel14","channel14",1024,-0.5,0.5);
  TH1F *channel15 = new TH1F("channel15","channel15",1024,-0.5,0.5);
  TH1F *channel16 = new TH1F("channel16","channel16",1024,-0.5,0.5);
  TH1F *channel17 = new TH1F("channel17","channel17",1024,-0.5,0.5);


  double x;
  double y ;
  int ctr = 1;
  float* buffer1;
  buffer1 = buffer + 6 ;  

  while(ctr>0 && ctr<1024+1) { x = *(buffer+6+ctr); ch0->SetBinContent(ctr,x);  y = (x-2047)/4096 ;  channel0->SetBinContent(ctr,y);  ctr++;}
  while(ctr>1024 && ctr<1024*2+1) { x = *(buffer+6+ctr); ch1->SetBinContent(ctr-1024,x);  y = (x-2047)/4096 ;  channel1->SetBinContent(ctr-1024,y);  ctr++;}
  while(ctr>1024*2 && ctr<1024*3+1) { x = *(buffer+6+ctr); ch2->SetBinContent(ctr-1024*2,x);  y = (x-2047)/4096 ;  channel2->SetBinContent(ctr-1024*2,y); ctr++;}
  while(ctr>1024*3 && ctr<1024*4+1) { x = *(buffer+6+ctr); ch3->SetBinContent(ctr-1024*3,x);  y = (x-2047)/4096 ;  channel3->SetBinContent(ctr-1024*3,y); ctr++;}
  while(ctr>1024*4 && ctr<1024*5+1) { x = *(buffer+6+ctr); ch4->SetBinContent(ctr-1024*4,x);  y = (x-2047)/4096 ;  channel4->SetBinContent(ctr-1024*4,y);  ctr++;}
  while(ctr>1024*5 && ctr<1024*6+1) { x = *(buffer+6+ctr); ch5->SetBinContent(ctr-1024*5,x);  y = (x-2047)/4096 ;  channel5->SetBinContent(ctr-1024*5,y);  ctr++;}
  while(ctr>1024*6 && ctr<1024*7+1) { x = *(buffer+6+ctr); ch6->SetBinContent(ctr-1024*6,x);  y = (x-2047)/4096 ;  channel6->SetBinContent(ctr-1024*6,y);  ctr++;}
  while(ctr>1024*7 && ctr<1024*8+1) { x = *(buffer+6+ctr); ch7->SetBinContent(ctr-1024*7,x);  y = (x-2047)/4096 ;  channel7->SetBinContent(ctr-1024*7,y);  ctr++;}

  while(ctr>1024*8 && ctr<1024*9+1) { x = *(buffer+6+ctr); tr0->SetBinContent(ctr-1024*8,x);  y = (x-2047)/4096 ;  channel8->SetBinContent(ctr-1024*8,y);  ctr++;}

  while(ctr>1024*9 && ctr<1024*10+1) { x = *(buffer+6+ctr); ch8->SetBinContent(ctr-1024*9,x);  y = (x-2047)/4096 ;  channel9->SetBinContent(ctr-1024*9,y);  ctr++;}
  while(ctr>1024*10 && ctr<1024*11+1) { x = *(buffer+6+ctr); ch9->SetBinContent(ctr-1024*10,x);  y = (x-2047)/4096 ;  channel10->SetBinContent(ctr-1024*10,y);  ctr++;}
  while(ctr>1024*11 && ctr<1024*12+1) { x = *(buffer+6+ctr); ch10->SetBinContent(ctr-1024*11,x);  y = (x-2047)/4096 ;  channel11->SetBinContent(ctr-1024*11,y);  ctr++;}
  while(ctr>1024*12 && ctr<1024*13+1) { x = *(buffer+6+ctr); ch11->SetBinContent(ctr-1024*12,x);  y = (x-2047)/4096 ;  channel12->SetBinContent(ctr-1024*12,y);  ctr++;}
  while(ctr>1024*13 && ctr<1024*14+1) { x = *(buffer+6+ctr); ch12->SetBinContent(ctr-1024*13,x);  y = (x-2047)/4096 ;  channel13->SetBinContent(ctr-1024*13,y);  ctr++;}
  while(ctr>1024*14 && ctr<1024*15+1) { x = *(buffer+6+ctr); ch13->SetBinContent(ctr-1024*14,x);  y = (x-2047)/4096 ;  channel14->SetBinContent(ctr-1024*14,y);  ctr++;}
  while(ctr>1024*15 && ctr<1024*16+1) { x = *(buffer+6+ctr); ch14->SetBinContent(ctr-1024*15,x);  y = (x-2047)/4096 ;  channel15->SetBinContent(ctr-1024*15,y);  ctr++;}
  while(ctr>1024*16 && ctr<1024*17+1) { x = *(buffer+6+ctr); ch15->SetBinContent(ctr-1024*16,x);  y = (x-2047)/4096 ;  channel16->SetBinContent(ctr-1024*16,y);  ctr++;}

  while(ctr>1024*17 && ctr<1024*18+1) { x = *(buffer+6+ctr); tr1->SetBinContent(ctr-1024*17,x);  y = (x-2047)/4096 ;  channel17->SetBinContent(ctr-1024*17,y);  ctr++;}
/*
  while(ctr>0 && ctr<1024+1) { x = *(buffer+6+ctr); ch0->SetBinContent(ctr,x);  ctr++;}
  while(ctr>1024 && ctr<1024*2+1) { x = *(buffer+6+ctr); ch1->SetBinContent(ctr-1024,x); ctr++;}
  while(ctr>1024*2 && ctr<1024*3+1) { x = *(buffer+6+ctr); ch2->SetBinContent(ctr-1024*2,x); ctr++;}
  while(ctr>1024*3 && ctr<1024*4+1) { x = *(buffer+6+ctr); ch3->SetBinContent(ctr-1024*3,x); ctr++;}
  while(ctr>1024*4 && ctr<1024*5+1) { x = *(buffer+6+ctr); ch4->SetBinContent(ctr-1024*4,x); ctr++;}
  while(ctr>1024*5 && ctr<1024*6+1) { x = *(buffer+6+ctr); ch5->SetBinContent(ctr-1024*5,x); ctr++;}
  while(ctr>1024*6 && ctr<1024*7+1) { x = *(buffer+6+ctr); ch6->SetBinContent(ctr-1024*6,x); ctr++;}
  while(ctr>1024*7 && ctr<1024*8+1) { x = *(buffer+6+ctr); ch7->SetBinContent(ctr-1024*7,x); ctr++;}
		
  while(ctr>1024*8 && ctr<1024*9+1) { x = *(buffer+6+ctr); tr0->SetBinContent(ctr-1024*8,x); ctr++;}

  while(ctr>1024*9 && ctr<1024*10+1) { x = *(buffer+6+ctr); ch8->SetBinContent(ctr-1024*9,x); ctr++;}
  while(ctr>1024*10 && ctr<1024*11+1) { x = *(buffer+6+ctr); ch9->SetBinContent(ctr-1024*10,x); ctr++;}
  while(ctr>1024*11 && ctr<1024*12+1) { x = *(buffer+6+ctr); ch10->SetBinContent(ctr-1024*11,x); ctr++;}
  while(ctr>1024*12 && ctr<1024*13+1) { x = *(buffer+6+ctr); ch11->SetBinContent(ctr-1024*12,x); ctr++;}
  while(ctr>1024*13 && ctr<1024*14+1) { x = *(buffer+6+ctr); ch12->SetBinContent(ctr-1024*13,x); ctr++;}
  while(ctr>1024*14 && ctr<1024*15+1) { x = *(buffer+6+ctr); ch13->SetBinContent(ctr-1024*14,x); ctr++;} 
  while(ctr>1024*15 && ctr<1024*16+1) { x = *(buffer+6+ctr); ch14->SetBinContent(ctr-1024*15,x); ctr++;}
  while(ctr>1024*16 && ctr<1024*17+1) { x = *(buffer+6+ctr); ch15->SetBinContent(ctr-1024*16,x); ctr++;}
	
  while(ctr>1024*17 && ctr<1024*18+1) { x = *(buffer+6+ctr); tr1->SetBinContent(ctr-1024*17,x); ctr++;}
*/
  tree->Write();
  f->Write();
  f->Close();
  fclose (pFile);
  free (buffer);
  return 0;
}
