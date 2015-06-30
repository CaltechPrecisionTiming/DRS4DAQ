/********************************************************************\

  Name:         drs_exam.cpp
  Created by:   Stefan Ritt

  Contents:     Simple example application to read out a DRS4
                evaluation board

  $Id: drs_exam.cpp 13482 2009-05-26 06:48:58Z ritt@PSI.CH $

\********************************************************************/


#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>

#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <TRandom.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TPostScript.h>
#include <TAxis.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TF1.h>
#include <TStyle.h>
#include <TProfile.h>
#include <TMapFile.h>




#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>



#include <math.h>

#ifdef _MSC_VER

#include <windows.h>

#elif defined(OS_LINUX)

#define O_BINARY 0

#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DIR_SEPARATOR '/'

#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "strlcpy.h"
#include "DRS.h"

/*------------------------------------------------------------------*/

int main( int argc, char** argv)
{

  char title[100];  sprintf( title, "%s_%s_%s.root", argv[1], argv[2], argv[3]);
  TFile* file = new TFile( title, "RECREATE", "MCP Test");

  TTree* tree = new TTree("pulse", "Wave Form");

  float b1_t[1024], b1_c[4][1024];
  float b2_t[1024], b2_c[4][1024];
  
  int event, tc1, tc2;


  time_t start, end;
  double tdiff;


  tree->Branch("event", &event, "event/I");
  tree->Branch("tc1", &tc1, "tc1/I");
  tree->Branch("b1_t",  b1_t, "b1_t[1024]/F");
  tree->Branch("b1_c",  b1_c, "b1_c1[4096]/F");


  tree->Branch("tc2", &tc2, "tc2/I");
  tree->Branch("b2_t",  b2_t,  "b2_t[1024]/F");
  tree->Branch("b2_c",  b2_c,  "b2_c1[4096]/F");


  int nBoards;
  DRS *drs;
  DRSBoard *b1;
  DRSBoard *b2;

  int serial_1, serial_2;
  serial_1 = serial_2 = 0;



  /* do initial scan */
  drs = new DRS();

  /* show any found board(s) */
 for (int i=0 ; i<drs->GetNumberOfBoards() ; i++) {
    b1 = drs->GetBoard(i);
    printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", 
           b1->GetBoardSerialNumber(), b1->GetFirmwareVersion());

    //-- if(  b1->GetBoardSerialNumber() == 2216) serial_1 = i;
    //-- if(  b1->GetBoardSerialNumber() == 2336) serial_2 = i;
    if(  b1->GetBoardSerialNumber() == 2336) serial_2 = i;
    if(  b1->GetBoardSerialNumber() == 2216) serial_1 = i;
  }



  if( 0) printf("Hello 0\n");

  /* exit if no board found */
  nBoards = drs->GetNumberOfBoards();
  if (nBoards == 0) {
    printf("No DRS4 evaluation board found\n");
    return 0;
  }

  if( 0) printf("Hello 1\n");

  /* continue working with first board only */
  b1 = drs->GetBoard(serial_1);
  b2 = drs->GetBoard(serial_2);

  

  printf("b1 :  DRS4 evaluation board, serial #%d\n", b1->GetBoardSerialNumber());
  printf("b2 :  DRS4 evaluation board, serial #%d\n", b2->GetBoardSerialNumber());


  if( 0) printf("Hello 2\n");

  /* initialize board */
  b1->Init();
  b2->Init();

  if ( 0) printf("Hello 3\n");

  /* set sampling frequency */
  b1->SetFrequency(5, true);
  b2->SetFrequency(5, true);

  if( 0)printf("Hello 4\n");

  /* enable transparent mode needed for analog trigger */
  b1->SetTranspMode(1);
  b2->SetTranspMode(1);

  /* set input range to -0.5V ... +0.5V */
  b1->SetInputRange(0.);
  b2->SetInputRange(0.);

  /* use following line to enable external hardware trigger (Lemo) */
  b1->EnableTrigger(1, 0);               // lemo off, analog trigger on
  b1->SetTriggerSource(1<<4);            
  b1->SetTriggerDelayNs(0);              

  b2->EnableTrigger(1, 0);               // lemo off, analog trigger on
  b2->SetTriggerSource(1<<4);            
  b2->SetTriggerDelayNs(0);              



  time( &start);

  /* repeat ten times */
  for( int i =0 ; i < atoi( argv[4]) ; i++) {

    /* start board (activate domino wave) */
    b1->StartDomino();
    b2->StartDomino();
     
    /* wait for trigger */
    while (b1->IsBusy() || b2->IsBusy());
          
    /* read all waveforms */
    b1->TransferWaves(0, 8);
    b2->TransferWaves(0, 8);
     
    tc1 = drs->GetBoard(0)->GetTriggerCell(0);
    tc2 = drs->GetBoard(1)->GetTriggerCell(0);
     
    /* read time (X) array in ns */
    b1->GetTime(0, tc1, b1_t);
    b2->GetTime(0, tc2, b2_t);
     
    /* decode waveform (Y) array first channel in mV */
    b1->GetWave(0, 0, b1_c[0]);
    b1->GetWave(0, 2, b1_c[1]);
    b1->GetWave(0, 4, b1_c[2]);
    b1->GetWave(0, 6, b1_c[3]);
    b2->GetWave(0, 0, b2_c[0]);
    b2->GetWave(0, 2, b2_c[1]);
    b2->GetWave(0, 4, b2_c[2]);
    b2->GetWave(0, 6, b2_c[3]);
     
    if( i%100 == 99){
      time( &end);
      tdiff = difftime( end, start);
      printf("Event#    %5d.  DAQ time = %6.2f(Second), Rate = %6.2f(Hz)\n", 
	     i+1, tdiff, (float)(i+1)/tdiff);
    }

    event = i;

    tree->Fill();
  }


  file->Write();
  file->Close();


  time( &end);

  tdiff = difftime( end, start);

  printf("DAQ time = %6.2f(Second),  Rate = %6.2f(Hz)\n", tdiff, atof(argv[4])/tdiff);
   
  /* delete DRS object -> close USB connection */
  delete drs;

  return 0;
}
