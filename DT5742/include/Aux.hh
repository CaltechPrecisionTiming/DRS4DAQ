#ifndef Aux_HH
#define Aux_HH

#include <TGraphErrors.h>
#include <TH1F.h>
#include <TF1.h>
#include <TCanvas.h>

static const int nPoints = 24;
static float outputAmplitude[nPoints] = { 11.7, 13.5, 16.7, 20.7, 24.6, 33.6, 47.4, 59, 73, 87, 
					  101, 116, 131, 144, 163, 197, 235, 306, 391, 466, 
					  544, 669, 831, 937 };		       

//correcting the extrapolated points by measured / extrapolated ratio
static float amplificationFactor[nPoints] = { 27.9/1.37*10 , 26.8/1.37*10, 31.6/1.37*10, 33.9/1.37*10, 37.3/1.37*10, 40.2/1.37*10, 43.5/1.37*10, 43.1/1.37*10, 44.1/1.37*10, 45.5/1.37*10, 
					      34*10, 35.6*10, 37*10, 40*10, 42*10, 45*10, 48*10, 53*10, 58*10, 61*10, 
					      63*10, 66*10, 68*10, 66*10 };  

double GetAmplificationFactor ( double measuredAmplitude );
TGraphErrors* GetTGraphFilter( short* channel, float* time, TString pulseName, bool makePlot = false );
TGraphErrors* GetTGraph( float* channel, float* time );
TGraphErrors* GetTGraph( short* channel, float* time );
double GetGaussTime( TGraphErrors* pulse );
int FindMin( int n, short *a);
int FindMin( int n, float *a);
float GausFit_MeanTime(TGraphErrors * pulse, const float index_first, const float index_last);
float RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, const float constantFraction, TString fname, bool makePlot = false );
void RisingEdgeFitTime(TGraphErrors * pulse, const float index_min, float* tstamp, TString fname, bool makePlot = false );
float GausFit_MeanTime(TGraphErrors* pulse, const float index_first, const float index_last, TString fname);
float GetBaseline( int peak, short *a );
float GetBaseline( int peak, float *b );
float GetPulseIntegral(int peak, short *a);
float GetTimeResolution( float deltat );

#endif
