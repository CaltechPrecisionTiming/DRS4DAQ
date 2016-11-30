#include <iostream>
#include <stdio.h>
#include <stdlib.h>

int main()
{
  FILE * pFile;
  long lSize;
  float * buffer;
  size_t result;

  pFile = fopen ( "wave_0.dat" , "rb" );
  if (pFile==NULL) {fputs ("File error\n",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  printf("%d\n", lSize);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (float*) malloc (sizeof(float)*lSize);
  if (buffer == NULL) {fputs ("Memory error\n",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error\n",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */

  //float myN =  *buffer << 4;
  //float myN2 =  *buffer >> 4;
  //float myN3 =  *buffer & 0xFFF0;
  //float myN4 =  *buffer & 0x0FFF;
  for (int i = 0; i < 1024; i++ )printf("%f\n", *(buffer+i));
  //printf("%f %f %f %f\n", myN, myN2, myN3, myN4);
  // terminate
  fclose (pFile);
  free (buffer);
  return 0;
}
