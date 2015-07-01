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
  float b3_t[1024], b3_c[4][1024];
  float b4_t[1024], b4_c[4][1024];

//   float t[1024];
//   float c1[1024];
//   float c2[1024];
//   float c3[1024];
//   float c4[1024];
//   float c5[1024];
//   float c6[1024];
//   float c7[1024];
//   float c8[1024];

  int event, tc1, tc2, tc3, tc4;


  time_t start, end;
  double tdiff;


  tree->Branch("event", &event, "event/I");
  tree->Branch("tc1", &tc1, "tc1/I");
  tree->Branch("b1_t",  b1_t, "b1_t[1024]/F");
  tree->Branch("b1_c",  b1_c, "b1_c1[4096]/F");
  tree->Branch("tc2", &tc2, "tc2/I");
  tree->Branch("b2_t",  b2_t,  "b2_t[1024]/F");
  tree->Branch("b2_c",  b2_c,  "b2_c1[4096]/F");
  tree->Branch("tc3", &tc3, "tc3/I");
  tree->Branch("b3_t",  b3_t,  "b3_t[1024]/F");
  tree->Branch("b3_c",  b3_c,  "b3_c1[4096]/F");
  tree->Branch("tc4", &tc4, "tc4/I");
  tree->Branch("b4_t",  b4_t,  "b4_t[1024]/F");
  tree->Branch("b4_c",  b4_c,  "b4_c1[4096]/F");

//   tree->Branch("t", &t, "t[1024]/F");
//   tree->Branch("c1", &c1, "c1[1024]/F");
//   tree->Branch("c2", &c2, "c2[1024]/F");
//   tree->Branch("c3", &c3, "c3[1024]/F");
//   tree->Branch("c4", &c4, "c4[1024]/F");
//   tree->Branch("c5", &c5, "c5[1024]/F");
//   tree->Branch("c6", &c6, "c6[1024]/F");
//   tree->Branch("c7", &c7, "c7[1024]/F");
//   tree->Branch("c8", &c8, "c8[1024]/F");


  int nBoards;
  DRS *drs;
  DRSBoard *b1;
  DRSBoard *b2;
  DRSBoard *b3;
  DRSBoard *b4;

  int serial_1, serial_2, serial_3, serial_4;
  serial_1 = serial_2 = serial_3 = serial_4 = 0;



  /* do initial scan */
  drs = new DRS();

  /* show any found board(s) */
 for (int i=0 ; i<drs->GetNumberOfBoards() ; i++) {
    b1 = drs->GetBoard(i);
    printf("Found DRS4 evaluation board, serial #%d, firmware revision %d\n", 
           b1->GetBoardSerialNumber(), b1->GetFirmwareVersion());

    if(  b1->GetBoardSerialNumber() == 2422) serial_1 = i;
    if(  b1->GetBoardSerialNumber() == 2442) serial_2 = i;
    if(  b1->GetBoardSerialNumber() == 2216) serial_3 = i;
    if(  b1->GetBoardSerialNumber() == 2449) serial_4 = i;
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
  b3 = drs->GetBoard(serial_3);
  b4 = drs->GetBoard(serial_4);

  

  printf("b1 :  DRS4 evaluation board, serial #%d\n", b1->GetBoardSerialNumber());
  printf("b2 :  DRS4 evaluation board, serial #%d\n", b2->GetBoardSerialNumber());
  printf("b3 :  DRS4 evaluation board, serial #%d\n", b3->GetBoardSerialNumber());
  printf("b4 :  DRS4 evaluation board, serial #%d\n", b4->GetBoardSerialNumber());


  if( 0) printf("Hello 2\n");

  /* initialize board */
  b1->Init();
  b2->Init();
  b3->Init();
  b4->Init();

  if ( 0) printf("Hello 3\n");

  /* set sampling frequency */
  b1->SetFrequency(5, true);
  b2->SetFrequency(5, true);
  b3->SetFrequency(5, true);
  b4->SetFrequency(5, true);

  if( 0)printf("Hello 4\n");

  /* enable transparent mode needed for analog trigger */
  b1->SetTranspMode(1);
  b2->SetTranspMode(1);
  b3->SetTranspMode(1);
  b4->SetTranspMode(1);

  /* set input range to -0.5V ... +0.5V */
  b1->SetInputRange(0.);
  b2->SetInputRange(0.);
  b3->SetInputRange(0.);
  b4->SetInputRange(0.);

  /* use following line to enable external hardware trigger (Lemo) */
  b1->EnableTrigger(1, 0);               // lemo off, analog trigger on
  b1->SetTriggerSource(1<<4);            
  b1->SetTriggerDelayNs(55);              
  b1->SetIndividualTriggerLevel(1, 0.1);
  b1->SetTriggerPolarity(false);        // positive edge
 
  b2->EnableTrigger(1, 0);               // lemo off, analog trigger on
  b2->SetTriggerSource(1<<4);            
  b2->SetTriggerDelayNs(55);              
  b2->SetIndividualTriggerLevel(1, 0.1);
  b2->SetTriggerPolarity(false);        // positive edge

  b3->EnableTrigger(1, 0);               // lemo off, analog trigger on
  b3->SetTriggerSource(1<<4);            
  b3->SetTriggerDelayNs(55);              
  b3->SetIndividualTriggerLevel(1, 0.1);
  b3->SetTriggerPolarity(false);        // positive edge

  b4->EnableTrigger(1, 0);               // lemo off, analog trigger on
  b4->SetTriggerSource(1<<4);            
  b4->SetTriggerDelayNs(55);              
  b4->SetIndividualTriggerLevel(1, 0.1);
  b4->SetTriggerPolarity(false);        // positive edge

  time( &start);

  /* repeat ten times */
  for( int i =0 ; i < atoi( argv[4]) ; i++) {
    /* start board (activate domino wave) */
    b1->StartDomino();
    b2->StartDomino();
    b3->StartDomino();
    b4->StartDomino();
     
    /* wait for trigger */
    //    while (b1->IsBusy() ); //trigger on board1 AND board2 AND board3
    while (b1->IsBusy() || b2->IsBusy() || b3->IsBusy() || b4->IsBusy()); //trigger on board1 AND board2 AND board3
          
    /* read all waveforms */
    b1->TransferWaves(0, 8);
    b2->TransferWaves(0, 8);
    b3->TransferWaves(0, 8);
    b4->TransferWaves(0, 8);
     
    tc1 = drs->GetBoard(0)->GetTriggerCell(0);
    tc2 = drs->GetBoard(1)->GetTriggerCell(0);
    tc3 = drs->GetBoard(2)->GetTriggerCell(0);
    tc4 = drs->GetBoard(3)->GetTriggerCell(0);
     
    /* read time (X) array in ns */
    b1->GetTime(0, 0, tc1, b1_t);
    b2->GetTime(0, 0, tc2, b2_t);
    b3->GetTime(0, 0, tc3, b3_t);
    b4->GetTime(0, 0, tc4, b4_t);
     
    /* decode waveform (Y) array first channel in mV */
    b1->GetWave(0, 0, b1_c[0]);
    b1->GetWave(0, 2, b1_c[1]);
    b1->GetWave(0, 4, b1_c[2]);
    b1->GetWave(0, 6, b1_c[3]);
    b2->GetWave(0, 0, b2_c[0]);
    b2->GetWave(0, 2, b2_c[1]);
    b2->GetWave(0, 4, b2_c[2]);
    b2->GetWave(0, 6, b2_c[3]);
    b3->GetWave(0, 0, b3_c[0]);
    b3->GetWave(0, 2, b3_c[1]);
    b3->GetWave(0, 4, b3_c[2]);
    b3->GetWave(0, 6, b3_c[3]);
    b4->GetWave(0, 0, b4_c[0]);
    b4->GetWave(0, 2, b4_c[1]);
    b4->GetWave(0, 4, b4_c[2]);
    b4->GetWave(0, 6, b4_c[3]);
     
    if( i%10 == 9){
      time( &end);
      tdiff = difftime( end, start);
      printf("Event#    %5d.  DAQ time = %6.2f(Second), Rate = %6.2f(Hz)\n", 
	     i+1, tdiff, (float)(i+1)/tdiff);
    }


    //**************************
    //Fill the output tree
    //**************************
//     for( int i=0; i<1024; i++) {
//       t[i] = b1_t[i];
//       c1[i] = -1 * b1_c[0][i];
//       c2[i] = -1 * b1_c[1][i];
//       c3[i] = -1 * b1_c[2][i];
//       c4[i] = -1 * b1_c[3][i];
//       c5[i] = -1 * b2_c[0][i];
//       c6[i] = -1 * b2_c[1][i];
//       c7[i] = -1 * b2_c[2][i];
//       c8[i] = -1 * b2_c[3][i];
//     }

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
