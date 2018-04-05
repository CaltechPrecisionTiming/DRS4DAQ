#include <fstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <sstream> //BBT, 04-04-18

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
#include <sys/types.h>
#include <sys/stat.h>


#include <vme/vme.h>
#include <vme/vme_api.h>




//LORE
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <ctype.h>

#define THIS_IP            "192.168.133.50"   // the IP users will be connecting to
#define COMMUNICATION_PORT "5000"             // the port on ZedBoard for communicating with XDAQ
#define STREAMING_PORT     "5001"             // the port on ZedBoard for streaming to XDAQ
//#define DESTINATION_IP     "192.168.133.1"    // the IP for the destination of the datastream (ftbftracker02)
#define DESTINATION_IP     "192.168.133.46"    // the IP for the destination of the datastream (ftbf-daq-08)
#define DESTINATION_PORT   47004              // the port for the destination of the datastream
#define MAXBUFLEN 1492


//========================================================================================================================
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//========================================================================================================================
int makeSocket(const char* ip, const char* port, struct addrinfo*& addressInfo)
{
  std::cout << __PRETTY_FUNCTION__ << "Opening socket. IP: " << ip << " port: " << port << std::endl;
  int socketId = 0;
  struct addrinfo hints, *servinfo, *p;
  //int sendSockfd=0;
  int rv;
  //int numbytes;
  //struct sockaddr_storage their_addr;
  //char buff[MAXBUFLEN];
  //socklen_t addr_len;
  //char s[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  //    hints.ai_family   = AF_UNSPEC; // set to AF_INET to force IPv4
  hints.ai_family   = AF_INET; // set to AF_INET to force IPv4
  hints.ai_socktype = SOCK_DGRAM;
  if(ip == NULL)
    hints.ai_flags    = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(ip, port, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((socketId = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("listener: socket");
      continue;
    }

    if (bind(socketId, p->ai_addr, p->ai_addrlen) == -1) {
      close(socketId);
      perror("listener: bind");
      continue;
    }

    break;
  }

  if (p == NULL) {
    std::cout << "Can't open socket" << std::endl;
    fprintf(stderr, "listener: failed to bind socket\n");
    return 2;
  }
  freeaddrinfo(servinfo);
  return socketId;
}

//========================================================================================================================
struct sockaddr_in setupSocketAddress(const char* ip, unsigned int port)
{
  //std::cout << __PRETTY_FUNCTION__ << std::endl;
  //network stuff
  struct sockaddr_in socketAddress;
  socketAddress.sin_family = AF_INET;// use IPv4 host byte order
  socketAddress.sin_port   = htons(port);// short, network byte order

  if(inet_aton(ip, &socketAddress.sin_addr) == 0)
    {
      std::cout << "FATAL: Invalid IP address " <<  ip << std::endl;
      exit(0);
    }

  memset(&(socketAddress.sin_zero), '\0', 8);// zero the rest of the struct
  return socketAddress;
}

//========================================================================================================================
int send(int toSocket, struct sockaddr_in& toAddress, const std::string& buffer)
{
//   std::cout << "Socket Descriptor #: " << toSocket
// 	    << " ip: " << std::hex << toAddress.sin_addr.s_addr << std::dec
// 	    << " port: " << ntohs(toAddress.sin_port)
// 	    << std::endl;
  if (sendto(toSocket, buffer.c_str(), buffer.size(), 0, (struct sockaddr *)&(toAddress), sizeof(sockaddr_in)) < (int)(buffer.size()))
    {
      std::cout << "Error writing buffer" << std::endl;
      return -1;
    }
  return 0;
}

//========================================================================================================================
int receiveAndAcknowledge(int fromSocket, struct sockaddr_in& fromAddress, std::string& buffer)
{
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 10; //set timeout period for select()
  fd_set fileDescriptor;  //setup set for select()
  FD_ZERO(&fileDescriptor);
  FD_SET(fromSocket,&fileDescriptor);
  select(fromSocket+1, &fileDescriptor, 0, 0, &tv);

  if(FD_ISSET(fromSocket,&fileDescriptor))
    {
      std::string bufferS;
      //struct sockaddr_in fromAddress;
      socklen_t addressLength = sizeof(fromAddress);
      int nOfBytes;
      buffer.resize(MAXBUFLEN);
      if ((nOfBytes=recvfrom(fromSocket, &buffer[0], MAXBUFLEN, 0, (struct sockaddr *)&fromAddress, &addressLength)) == -1)
	return -1;

      // Confirm you've received the message by returning message to sender
      send(fromSocket, fromAddress, buffer);
      buffer.resize(nOfBytes);
      //char address[INET_ADDRSTRLEN];
      //inet_ntop(AF_INET, &(fromAddress.sin_addr), address, INET_ADDRSTRLEN);
      //unsigned long  fromIPAddress = fromAddress.sin_addr.s_addr;
      //unsigned short fromPort      = fromAddress.sin_port;

    }
  else
    return -1;

  return 0;
}

//END LORE






struct commandLineParameters {
  int      status;
  int      digitizeTrigger;    // 1|0
  int      sampleRate;         // 5|2.5|1 => 50/25/10
  int      numSamples;         // 1024|520|256|136
  int      softwareTrigger;    // 1|0
  int      run;                // 1|0
  int      testMode;           // 1|0
  uint     maxTriggers;        // n
  char*    eventDataFileName;  // when data are split
  char*    datasetName;        // datafile_name (6 characters limit)
  int      runNumber;          // n
  int      triggerRisingEdge;  // 1|0
  int      postTriggerDelay;   // n
  int      bufferLimit;        // 1|0
  int      siftData;           // 1|0   
  char*    inputConfig;        // BBT, 04-04-18: name of config file to set indiv. channel offsets

};

struct vmeBusHandles {
  vme_bus_handle_t     busHandle;
  vme_dma_handle_t     dmaHandle;
  vme_master_handle_t  masterHandle;
  unsigned int*        mappedMemory;
  unsigned int*        dmaBufferMemory;
};

/* We're doing D32 cycles on the VME bus and treat the memory in the digitizer 
   as an array of integers.  The addresses here are in terms of 32 bit words.
*/
enum v1742Regs {
  V1742_Group_Status           = 0x0422, // 0x1088,
  V1742_Group_Buffer_Occupancy = 0x0425, // 0x1094,
  V1742_TR0_Threshold          = 0x0435, // 0x10D4,
  V1742_TR0_DC_Offset          = 0x0437, // 0x10DC,
  V1742_TR1_Threshold          = 0x04B5, // 0x12D4,
  V1742_TR1_DC_Offset          = 0x04B7, // 0x12DC,
  //BBT: 04-04-18
  V1742_GRP0_CH_DC_Offset = 0x0426, //0x1098,
  V1742_GRP1_CH_DC_Offset = 0x0466, //0x1198,
  V1742_GRP2_CH_DC_Offset = 0x04A6, //0x1298,
  V1742_GRP3_CH_DC_Offset = 0x04E6, //0x1398,
  // ===============
  V1742_Group_Config           = 0x2000, // 0x8000,
  V1742_Group_Config_Set       = 0x2001, // 0x8004,
  V1742_Group_Config_Clear     = 0x2002, // 0x8008,
  V1742_Custom_Size            = 0x2008, // 0x8020,
  V1742_Sampling_Frequency     = 0x2036, // 0x80D8,
  V1742_Acquisition_Mode       = 0x2040, // 0x8100,
  V1742_Software_Trigger       = 0x2042, // 0x8108,
  V1742_Trigger_Enable         = 0x2043, // 0x810C,
  V1742_Trigger_Out_Enable     = 0x2044, // 0x8110,
  V1742_Post_Trigger_Delay     = 0x2045, // 0x8114
  V1742_Group_Enable           = 0x2048, // 0x8120,
  V1742_Event_Stored           = 0x204B, // 0x812C,
  V1742_Event_Size             = 0x2053, // 0x814C,
  V1742_Clear_Data             = 0x3BCA, // 0xEF28,
  V1742_Config_Reload          = 0x3BCD, // 0xEF34,
};

enum PARSER_STATE {
  PARSE_EVENT_HEADER,
  PARSE_GROUP,
};

static unsigned int eventHeader[4];
static unsigned int groupData[4][3470];
static unsigned int groupTimeTag[4];
static unsigned int groupCellIndex[4];

static unsigned int parserState = PARSE_EVENT_HEADER;
static unsigned int wordsInEventHeader = 0;
static unsigned int wordsInGroupData = 0;
static unsigned int numGroups = 0;
static unsigned int groupNumber = 0;
static unsigned int sampleSize = 0;
static unsigned int eventCount = 0;
static unsigned int fixedEvent = 0;

FILE *siftedHandle;

void writeOutEvent() {
  unsigned int i;
  unsigned int groupSize;

  fwrite(eventHeader, sizeof(unsigned int), 4, siftedHandle);
  for (i = 0; i < 4; i++) {
    groupSize = (groupData[i][0] & 0xfff) + 2;
    if ((groupData[i][0] >> 12) & 0x1) {
      groupSize += 384;
    }

    fwrite(groupData[i], sizeof(unsigned int), groupSize, siftedHandle);
  }

  return;
}

void parseEventHeader() {
  unsigned int groupMask;
  
  printf("Event: iCnt %d, Size %d, BrdId %d, Pattern 0x%x, GrpMsk 0x%x, Counter 0x%x, Time 0x%x\n",
	 eventCount, (eventHeader[0] & 0x0fffffff) * sizeof(unsigned int), eventHeader[1] >> 27,
	 (eventHeader[1] >> 8) & 0x3fff, eventHeader[1] & 0xf, eventHeader[2],
	 eventHeader[3]);
  numGroups = 0;
  groupMask = eventHeader[1] & 0xf;
  while(groupMask) {
    if (groupMask & 0x1) {
      numGroups++;
    }
    groupMask = groupMask >> 1;
  }
  return;
}

void parseGroupHeader(unsigned int groupNumber, unsigned int groupHeader) {
  printf("  Group: 0x%x, %d, CellIdx %d, Freq ", groupHeader, groupNumber, groupHeader >> 20);
  groupCellIndex[groupNumber] = groupHeader >> 20;
  switch ((groupHeader >> 16) & 3) {
  case 0x0:
    printf("5GS/s, ");
    break;
  case 0x1:
    printf("2.5GS/s, ");
    break;
  case 0x2:
    printf("1GS/s, ");
    break;
  case 0x3:
    printf("Unknown, ");
  }
  printf("TrEn %d, ", (groupHeader >> 12) & 0x1);
  sampleSize = (groupHeader & 0xfff) + 2;
  if ((groupHeader >> 12) & 0x1) {
    sampleSize += 384;
  }
  printf("Size 0x%x ", sampleSize);
  return;
}

void parseGroupFooter(unsigned int groupNumber, unsigned int groupFooter) {
  printf("TT: 0x%x\n", groupFooter);
  groupTimeTag[groupNumber] = groupFooter;
  return;
}

void evaluateEvent() {
  unsigned int i;
  unsigned int misMatchFound = 0;

  for (i = 0; i < numGroups; i++) {
    if (groupTimeTag[i] != groupTimeTag[0]) {
      misMatchFound = 1;
      break;
    }
  }

  if (groupTimeTag[0] == groupTimeTag[1] && groupTimeTag[2] == groupTimeTag[3]) {
    if (groupTimeTag[0] - 1 == groupTimeTag[2] || groupTimeTag[0] + 1 == groupTimeTag[2]) {
      printf("Event %d is off by 1.\n", eventCount);
      return;
    }
  }

  if (groupNumber == numGroups && !misMatchFound) {
    writeOutEvent();
    printf("Event %d is fine.\n", eventCount);
    return;
  }

  if (groupNumber == 2 && !misMatchFound) {
    writeOutEvent();
    printf("Event %d fixed\n", eventCount);
    fixedEvent = 1;
  } else if (groupNumber == 2) {
    fixedEvent = 0;
  }

  if (groupNumber == numGroups && misMatchFound && fixedEvent == 0) {
    printf("Event %d is BAD.\n", eventCount);
  }

  return;
}


int siftData(unsigned int wordsRead, unsigned int *data) {
  char *bufferPointer;
  // unsigned int bytesToCopy;

  while (wordsRead > 0) {
    switch (parserState) {
    case PARSE_EVENT_HEADER:
      bufferPointer = (char *)eventHeader + wordsInEventHeader * sizeof(unsigned int);
      if (wordsRead + wordsInEventHeader >= 4) {
	memcpy(bufferPointer, data, 
	       sizeof(unsigned int) * (4 - wordsInEventHeader));

	parserState = PARSE_GROUP;
	groupNumber = 0;
	wordsRead -= 4 - wordsInEventHeader;
	data += 4 - wordsInEventHeader;

	wordsInEventHeader = 4;
	parseEventHeader();
      } else {
	memcpy(bufferPointer, data,
	       wordsRead * sizeof (unsigned int));
	wordsInEventHeader += wordsRead;
	wordsRead = 0;
      }

      break;
    case PARSE_GROUP:
      if (wordsInGroupData == 0) {
	parseGroupHeader(groupNumber, data[0]);
      }

      bufferPointer = ((char *)groupData[groupNumber]) + wordsInGroupData * sizeof(unsigned int);
      if (wordsRead + wordsInGroupData >= sampleSize) {
	memcpy(bufferPointer, data, 
	       sizeof(unsigned int) * (sampleSize - wordsInGroupData));

	parseGroupFooter(groupNumber, *((data + (sampleSize - wordsInGroupData)) - 1));

	wordsRead -= sampleSize - wordsInGroupData;
	data += sampleSize - wordsInGroupData;

	wordsInGroupData = 0;
	groupNumber++;

	if (groupNumber == 2) {
	  evaluateEvent();
	}
	if (groupNumber >= numGroups) {
	  evaluateEvent();
	  parserState = PARSE_EVENT_HEADER;
	  wordsInEventHeader = 0;
	  eventCount++;
  	}
      } else {
	memcpy(bufferPointer, data,
	       wordsRead * sizeof (unsigned int));
	wordsInGroupData += wordsRead;
	wordsRead = 0;
      }
    }
  }
    
  return 0;
}

void splitGroupData(char *baseFileName, FILE *eventDataHandle, 
		    unsigned int groupNum, unsigned int numSamples, 
		    unsigned int readoutTR)
{
  unsigned int samples[8][1024];
  unsigned int trData[1024];
  unsigned int eventData[3];
  unsigned int i, j;
  FILE *channelDataHandle;
  FILE *trDataHandle;
  char channelFileName[50];

  for(i = 0; i < numSamples; i++) {
    fread(eventData, sizeof(unsigned int), 3, eventDataHandle);
    samples[0][i] = eventData[0] & 0xfff;
    samples[1][i] = (eventData[0] >> 12) & 0xfff;
    samples[2][i] = (eventData[0] >> 24) | ((eventData[1] & 0xf) << 8);
    samples[3][i] = (eventData[1] >> 4) & 0xfff;
    samples[4][i] = (eventData[1] >> 16) & 0xfff;
    samples[5][i] = (eventData[1] >> 28) | ((eventData[2] & 0xff) << 4);
    samples[6][i] = (eventData[2] >> 8) & 0xfff;
    samples[7][i] = eventData[2] >> 20;
  }

  if (readoutTR != 0) {
    for(i = 0; i < (numSamples / 8); i++) {
      fread(eventData, sizeof(unsigned int), 3, eventDataHandle);
      trData[i * 8 + 0] = eventData[0] & 0xfff;
      trData[i * 8 + 1] = (eventData[0] >> 12) & 0xfff;
      trData[i * 8 + 2] = (eventData[0] >> 24) | ((eventData[1] & 0xf) << 8);
      trData[i * 8 + 3] = (eventData[1] >> 4) & 0xfff;
      trData[i * 8 + 4] = (eventData[1] >> 16) & 0xfff;
      trData[i * 8 + 5] = (eventData[1] >> 28) | ((eventData[2] & 0xff) << 4);
      trData[i * 8 + 6] = (eventData[2] >> 8) & 0xfff;
      trData[i * 8 + 7] = eventData[2] >> 20;
    }
  }

  for(i = 0; i < 8; i++) {
    snprintf(channelFileName, 50, "%s-group%d-channel%d", baseFileName, 
	     groupNum, i);
    channelDataHandle = fopen(channelFileName, "w");

    for(j = 0; j < numSamples; j++) {
      fprintf(channelDataHandle, "%d\t%d\n", j, samples[i][j]);
    }

    fclose(channelDataHandle);
  }

  if (readoutTR != 0) {
    snprintf(channelFileName, 50, "%s-group%d-tr", baseFileName, 
	     groupNum);
    trDataHandle = fopen(channelFileName, "w");

    for(i = 0; i < numSamples; i++) {
      fprintf(trDataHandle, "%d\t%d\n", i, trData[i]);
    }

    fclose(trDataHandle);
  }

  // Move the file handle past the trigger time tag
  fseek(eventDataHandle, sizeof(unsigned int), SEEK_CUR);
  return;
}


int splitData(char *eventDataFileName)
{
  FILE *eventDataHandle;
  unsigned int eventHeader[4];
  unsigned int groupEventDesc;
  unsigned int numSamples;
  unsigned int readoutTR;
  unsigned int groupNum;
  unsigned int groupMask;

  eventDataHandle = fopen(eventDataFileName, "r");
  fread(eventHeader, sizeof(unsigned int), 4, eventDataHandle);
  groupMask = eventHeader[1] & 0xf;

  for(groupNum = 0; groupNum < 4; groupNum++) {
    if(!(groupMask & (1 << groupNum))) {
      continue;
    }

    fread(&groupEventDesc, sizeof(unsigned int), 1, eventDataHandle);
    numSamples = (groupEventDesc & 0xfff) / 3;
    readoutTR = (groupEventDesc >> 12) & 0x1;
    splitGroupData(eventDataFileName, eventDataHandle, groupNum, numSamples, 
		   readoutTR);
  }

  fclose(eventDataHandle);
  return 0;
}


int displayStatus(unsigned int *mappedMemory)
{
  unsigned int groupStatus; // 0x1088, 0x1188, 0x1288, 0x1388
  unsigned int temperature; // 0x10A0, 0x11A0, 0x12A0, 0x13A0
  unsigned int acqStatus;   // 0x8104
  int i;

  for (i = 0; i < 4; i++) {
    groupStatus = mappedMemory[(0x1088 | (i << 8)) / 4];
    temperature = mappedMemory[(0x10A0 | (i << 8)) / 4];

    printf("Group %d (%dC):\n", i, temperature);

    if(groupStatus & 0x100)
      printf("  DRS Chips Busy\n");
    else 
      printf("  DRS Chips Idle\n");

    if(groupStatus & 0x80)
      printf("  Odd PLL Lock\n");

    if(groupStatus & 0x40)
      printf("  Even PLL Lock\n");

    if(groupStatus & 0x10)
      printf("  Odd Enable\n");
    
    if(groupStatus & 0x8)
      printf("  Even Enable\n");

    if(groupStatus & 0x4)
      printf("  SPI Busy\n");
    else
      printf("  SPI Idle\n");
    
    if(groupStatus & 0x2)
      printf("  Memory Empty\n");
    
    if(groupStatus & 0x1)
      printf("  Memory Full\n");
    
    printf("\n");
  }

  printf("Acquisition Status:\n");
  acqStatus = mappedMemory[0x8104 / 4];

  if (acqStatus & 0x100)
    printf("  Board Ready\n");
  else
    printf("  Board not ready\n");
  
  if (acqStatus & 0x80)
    printf("  PLL loss of lock\n");
  else
    printf("  No PLL loss of lock\n");
  
  if(acqStatus & 0x40)
    printf("  PLL Byspass Mode\n");
  
  if(acqStatus & 0x20)
    printf("  External clock source\n");
  else
    printf("  Internal clock source\n");
  
  if (acqStatus & 0x10)
    printf("  Event full\n");
  else 
    printf("  Event empty\n");
  
  if(acqStatus & 0x04)
    printf("  Run ON\n");
  else 
    printf("  Run off\n");
  
  return 0;
}


int setReg(unsigned int *mappedMemory, unsigned int addr, unsigned int value)
{
  mappedMemory[addr] = value;

  /* Don't read back write only registers. */
  if(addr == V1742_Software_Trigger   || addr == V1742_Config_Reload ||
     addr == V1742_Clear_Data         || addr == V1742_Group_Config_Set ||
     addr == V1742_Group_Config_Clear || addr == V1742_Post_Trigger_Delay ||
     addr == V1742_Sampling_Frequency) {
    return 0;
  }

  if((mappedMemory[addr]) != value) {
    printf("Error setting register at 0x%x (0x%x)\n", addr * 4, mappedMemory[addr]);
    return -1;
  }

  return 0;
}

// BBT: 04-04-18
void setChannelOffsetsFromConfig(unsigned int *mappedMemory, const char* inputConfig){
  
  std::ifstream in(inputConfig);
  std::string line;
  unsigned int offset;
  int channel, group, ch, c;

  // check to make sure file exists
  if(!in){
    printf("Cannot open config file: %s! Using VME defaults... probably bad\n", inputConfig);
    return;
  }

  // file exists, let's get cracking
  while( std::getline(in, line) ) {
    if ( line.find("|") == std::string::npos ) {// ignore header
      std::string readline[7];
      c = 0;
      ch = group = channel = -1;
      offset = 99999;
      // parse string into array, split by ' '
      std::stringstream ssin(line);
      while( ssin.good() && c < 7 ) {
	ssin >> readline[c];
	c++;
      }
      // get channel, group, and offset values as ints
      if ( readline[0]!=""){
	std::istringstream( readline[0] ) >> ch;
	channel = ch % 9;
	group = ch / 9;
      }
      if ( readline[6]!="")
	std::istringstream( readline[6] ) >> offset;
      
      // skip trigger channels and channels with no offset given --> these channels take on VME default of 33086
      if ( offset == 99999 || channel == 8) continue;

      printf("Configuring: Ch = %i, Group = %i, Channel = %i, Offset = %i\n", ch, group, channel, offset);

      // this next part involves turning offset into hex, adding channel, and turning back into int. it's ugly and should definitely be improved
      char c_hex[10];
      std::string s_hex;
      unsigned int u_hex = 0; 
      std::stringstream ss;

      std::sprintf( c_hex, "%x", offset );
      s_hex = c_hex; // store as string so inserting channel # easier
      // add leading zeroes if hex code less than four characters
      while (s_hex.length() < 4)
	s_hex.insert(0, "0");
      // add channel
      ss << channel;
      s_hex.insert(0, ss.str());
      sscanf( s_hex.c_str(), "%x", &u_hex );
      //std::cout << "Hex value: 0x" << s_hex << " , uint = " << u_hex << std::endl;
      
      unsigned int addr = 0;
      if (group == 0) addr = V1742_GRP0_CH_DC_Offset; // GRP0, Channels [0,7]
      if (group == 1) addr = V1742_GRP1_CH_DC_Offset; // GRP1, Channels [9,16]
      if (group == 2) addr = V1742_GRP2_CH_DC_Offset; // GRP2, Channels [17,25]
      if (group == 3) addr = V1742_GRP3_CH_DC_Offset; // GRP3, Channels [27,34]
      
      // FINALLY SET GRn_CH_DC_OFFSET
      setReg(mappedMemory, addr, u_hex);
      sleep(1); // sleep so enough time between writing registers

    } // if statement skipping config header
  } // close loop over lines in config file
  
}

void dumpRegisters(struct vmeBusHandles *busHandles, unsigned int *mappedMemory, const char* label="")
{
  // BBT: 04-04-18
  printf("============ %s =============\n", label);
  
  unsigned int groupStatus;   // 0x1088, 0x1188, 0x1288, 0x1388
  unsigned int temperature;   // 0x10A0, 0x11A0, 0x12A0, 0x13A0
  unsigned int grpChDcOffset; // 0x1n98 for n=0,1,2,3
  unsigned int TR_dcOffset;   // 0x10DC (i=0,1), 0x12D4 (i=2,3)
  unsigned int TR_threshold;  // 0x10D4 (i=0,1), 0x12DC (i=2,3)
  unsigned int groupStatus_w; // second bit of 0x1n88 --> clue about writing
  for (int i = 0; i < 4; i++) {
    groupStatus   = mappedMemory[(0x1088 | (i << 8)) / 4];
    groupStatus_w = ( (groupStatus) & (1 << 2) );
    temperature   = mappedMemory[(0x10A0 | (i << 8)) / 4];
    TR_dcOffset  = mappedMemory[(0x10DC | (i << 8)) / 4];
    TR_threshold  = mappedMemory[(0x10D4 | (i << 8)) / 4];
    // first set channel 0 before reading channel DC offset --> unnecessary i think
    //setReg(busHandles->mappedMemory, ((0x10A4 | (i << 8)) / 4), 0x0);
    grpChDcOffset = mappedMemory[(0x1098 | (i << 8)) / 4];
    printf("[grpChDcOffset] Group %d , CH0, (%d):\n", i, grpChDcOffset);
    //if(i<3){
    //printf("[TEMP] Group %d (%dC):\n", i, temperature);
    //printf("[G-Status] Group %d (%d):\n", i, groupStatus);
    //printf("[G-Status Write] Group %d (%d):\n", i, groupStatus_w );
    //printf("[TR dcOffset] Group %d (%d):\n", i, TR_dcOffset);
    //printf("[TR threshold] Group %d (%d):\n", i, TR_threshold);
    //}  
  }
}
// ===============


void readOutData(struct vmeBusHandles *busHandles, struct commandLineParameters params)
{
  FILE *fileHandle;
  char fileName[100];

  unsigned int eventSize;
  unsigned int eventData;
  unsigned int eventCount;
  unsigned int eventsStored;

  int readResult;
  int acqStopped = 0;

  printf("Polling digitizer for triggers:\n");
  eventCount = 0;

  //snprintf(fileName, 100, "%6s.%06d", params.datasetName, params.runNumber);
  sprintf(fileName, "%s-%d.dat", params.datasetName, params.runNumber);
  printf("Writing data to: %s\n", fileName);
  fileHandle = fopen(fileName, "w");

  if (params.siftData) {
    // snprintf(fileName, 100, "%6s.%06d-sifted", params.datasetName, params.runNumber);
    sprintf(fileName, "%s-%d-sifted.dat", params.datasetName, params.runNumber);
    siftedHandle = fopen(fileName, "w");
  }

  while (eventCount < params.maxTriggers) {


    eventsStored = busHandles->mappedMemory[V1742_Event_Stored];
    eventSize    = busHandles->mappedMemory[V1742_Event_Size];

    //Here polling the register to see if there was a trigger!!!
    while (eventSize == 0) {
      if (acqStopped) {
	printf("Buffers empty, starting acquisition...\n");
	acqStopped = 0;
	setReg(busHandles->mappedMemory, V1742_Acquisition_Mode, 0x000C);
      }

      eventSize = busHandles->mappedMemory[V1742_Event_Stored];
      if(eventSize == 0) usleep(100);
      //else std::cout << "ev size: " << eventSize << " acq stopped? " << acqStopped << std::endl;
    }

    if (params.bufferLimit == 1) {
      std::cout << "I NEVER GET INSIDE HERE BECAUSE bufferLimits is not set : )->Buffer limits: " << params.bufferLimit << std::endl;
      if (busHandles->mappedMemory[V1742_Group_Status] & 0x1 && acqStopped == 0) {
	printf("Buffers full, stopping acquisition...\n");
	acqStopped = 1;
	setReg(busHandles->mappedMemory, V1742_Acquisition_Mode, 0x0000);
	usleep(100000);
	printf("Event stored: %d\n", busHandles->mappedMemory[V1742_Event_Stored]);
      } else if (acqStopped == 0) {
	usleep(100000);
	continue;
      }
    }

    eventSize = busHandles->mappedMemory[V1742_Event_Size];
    //std::cout << "Ev size: " << eventSize << std::endl;
    if (eventCount % 50 == 0)
      printf("Read out triggered: event %d, %d bytes.\n", eventCount, eventSize * 4);

    eventCount++;    

    while (eventSize > 0) {

      //      std::cout << "Ev size: " << eventSize << std::endl;
      if(eventSize > 1023){
	readResult = vme_dma_read(busHandles->busHandle, busHandles->dmaHandle, 0, 0x10000000, VME_A32UD, 0xFFC, 0);
	fwrite(busHandles->dmaBufferMemory, sizeof(unsigned int), 1023, fileHandle);
	if (params.siftData) {
	  siftData(1023, busHandles->dmaBufferMemory);
	}
	eventSize -= 1023;
      } 
      else {
	eventData = busHandles->mappedMemory[0];
	fwrite(&eventData, sizeof(unsigned int), 1, fileHandle);
	if (params.siftData) {
	  siftData(1, &eventData);
	}
	eventSize--;
      }
    }
  }

  fclose(fileHandle);
}


void setupVME(struct vmeBusHandles *handles)
{
  if (vme_init(&handles->busHandle)) {
    printf("Error initializing the bus handle.\n");
    exit(-1);
  }

  if (vme_master_window_create(handles->busHandle, &handles->masterHandle, 
			       0x10000000, VME_A32UD, 0xF400, 
			       VME_CTL_PWEN, NULL)) {
    printf("Error creating VME master window.\n");
    vme_term(handles->busHandle);
    exit(-1);
  }

  handles->mappedMemory = (unsigned int*)vme_master_window_map(handles->busHandle, handles->masterHandle, 0);

  if (!handles->mappedMemory) {
    printf("Error mapping VME master window.\n");
    vme_master_window_release( handles->busHandle, handles->masterHandle);
    vme_term(handles->busHandle);
    exit(-1);
  }


  if (vme_dma_buffer_create(handles->busHandle, &handles->dmaHandle, 0xFFC, 0, NULL)) {
    printf("Error creating DMA buffer.\n");
    vme_term(handles->busHandle);
    exit(-1);
  }

  handles->dmaBufferMemory = (unsigned int*)vme_dma_buffer_map(handles->busHandle, handles->dmaHandle, 0);
  if (!handles->dmaBufferMemory) {
    printf("Error mapping DMA buffer.\n");

    vme_dma_buffer_release(handles->busHandle, handles->dmaHandle);
    vme_term(handles->busHandle);
    exit(-1);
  }

  return;
}


void tearDownVME(struct vmeBusHandles *handles)
{
  if (vme_master_window_unmap(handles->busHandle, handles->masterHandle)) {
    printf("Error unmapping VME master window.\n");
    exit(-1);
  }

  if (vme_master_window_release(handles->busHandle, handles->masterHandle)) {
    printf("Error releasing VME master window.\n");
    exit(-1);
  }

  if (vme_dma_buffer_unmap(handles->busHandle, handles->dmaHandle)) {
    printf("Error unmapping DMA buffer.\n");
    exit(-1);
  }

  if (vme_dma_buffer_release(handles->busHandle, handles->dmaHandle)) {
    printf("Error releasing DMA buffer.\n");
    exit(-1);
  }

  if (vme_term(handles->busHandle)) {
    printf("Error releasing VME bus.\n");
    exit(-1);
  }

  return;
}


void printUsage()
{
  printf("Command line parameters:\n");
  printf("  --status                        Display the status of the digitizer board.\n");
  printf("  --run                           Configure the board to acquire data and read out\n");
  printf("                                  data once it is acquired.\n");
  printf("  --test-mode                     Replace ADC samples with a sawtooth generated by\n");
  printf("                                  the FPGA.\n");
  printf("  --digitize-trigger              Digitize the TR0 and TR1 signals and include\n");
  printf("                                  them in the output data.\n");
  printf("  --sample-rate                   Set the sample rate.  Can be 5, 2.5 or 1.\n");
  printf("  --num-samples                   Set the number of samples to take per trigger.\n");
  printf("                                  Can be 1024, 520, 256 or 136.\n");
  printf("  --software-trigger              Trigger an acquistion with the software\n");
  printf("                                  trigger.\n");
  printf("  --max-triggers                  Maximum number of triggers to take before exiting.\n");
  printf("  --split-data [file]             Read in the raw data from the digitizer and\n");
  printf("                                  split it into individual files for each channel.\n");
  printf("                                  The metadata is removed and each file only contains\n");
  printf("                                  the samples for that channel.\n");
  printf("  --dataset-name                  Specificy the name of the dataset which will be used\n");
  printf("                                  to construct output file names.  This is required.\n");
  printf("  --run-number [run]              Specify the run number.  This will be used to\n");
  printf("                                  construct output file names and will override the\n");
  printf("                                  --runNumberFileName parameter.\n");
  printf("  --trigger-edge [rising|falling] Configure whether readout is triggered on the risiing\n");
  printf("                                  or falling edge.\n");
  printf("  --trigger-delay [delay]         The amount of time to wait after the trigger before the\n");
  printf("                                  the start of digitization.  Specified in steps on 8.5ns.\n");
  printf("  --buffer-limit                  Prevent all the board's buffer from being used.  If data\n");
  printf("                                  readout falls behind acquisition turn off acquisition.\n");
  printf("  --sift-data                     Attempt to sift through the data read out of the board\n");
  printf("                                  throwing out bad triggers and grouping data based on the\n");
  printf("                                  group time tag.\n");
  return;
}



struct commandLineParameters parseCommandLine(int argc, char **argv)
{
  struct commandLineParameters params;
  // struct stat st;
  int i;
  FILE *nameHandle;
  char runNumberFile[50];

  params.status = 0;
  params.digitizeTrigger = 0;
  params.sampleRate = 0;
  params.numSamples = 0;
  params.softwareTrigger = 0;
  params.run = 0;
  params.testMode = 0;
  params.eventDataFileName = NULL;
  params.maxTriggers = 1000;
  params.datasetName = NULL;
  params.runNumber = -1;
  params.triggerRisingEdge = 0;
  params.postTriggerDelay = -1;
  params.siftData = 0;
  params.inputConfig = "config/December2017_UVA.config"; // BBT: 03-28-18

  if(argc == 1) {
    printUsage();
    exit(0);
  }    

  for(i = 1; i < argc; i++){
    if(strcmp("--status", argv[i]) == 0)
      params.status = 1;
    else if(strcmp("--buffer-limit", argv[i]) == 0)
      params.bufferLimit = 1;
    else if(strcmp("--sift-data", argv[i]) == 0)
      params.siftData = 1;
    else if(strcmp("--digitize-trigger", argv[i]) == 0)
      params.digitizeTrigger = 1;
    else if(strcmp("--run", argv[i]) == 0)
      params.run = 1;
    else if(strcmp("--test-mode", argv[i]) == 0)
      params.testMode = 1;
    else if(strcmp("--max-triggers", argv[i]) == 0){
      if(i + 1 >= argc){
	printf("The maximum number of trigger must be specified:\n");
	printf("  %s --max-triggers [N]\n", argv[0]);
	exit(-1);
      }
      sscanf(argv[i + 1], "%d", &params.maxTriggers);
      i++;
    } 
    else if(strcmp("--sample-rate", argv[i]) == 0){
      if(i + 1 >= argc){
	printf("A sample rate must be included with the sample rate parameter:\n");
	printf("  %s --sample-rate [5|2.5|1]\n", argv[0]);
	exit(-1);
      } 
      else if(strcmp("5", argv[i + 1]) == 0){
	params.sampleRate = 50;
	i++;
      } 
      else if(strcmp("2.5", argv[i + 1]) == 0){
	params.sampleRate = 25;
	i++;
      } 
      else if(strcmp("1", argv[i + 1]) == 0){
	params.sampleRate = 10;
	i++;
      } 
      else{
	printf("Invalid sample rate.\n");
	printf("  %s --sample-rate [5|2.5|1]\n", argv[0]);
	exit(-1);
      }
    } 
    else if(strcmp("--num-samples", argv[i]) == 0){
      if(i + 1 >= argc){
	printf("The number of samples must be included with the num samples parameter:\n");
	printf("  %s --num-samples [1024|520|256|136]\n", argv[0]);
	exit(-1);
      } 
      else if(strcmp("1024", argv[i + 1]) == 0)
	params.numSamples = 1024;
      else if(strcmp("520", argv[i + 1]) == 0)
	params.numSamples = 520;
      else if(strcmp("256", argv[i + 1]) == 0)
	params.numSamples = 256;
      else if(strcmp("136", argv[i + 1]) == 0)
	params.numSamples = 136;
      else{
	printf("Invalid number of samples:\n");
	printf("  %s --num-samples [1024|520|256|136]\n", argv[0]);
	exit(-1);
      }
      i += 1;
    } 
    else if(strcmp("--trigger-edge", argv[i]) == 0){
      if(i + 1 >= argc){
	printf("You must specify the edge with the trigger-edge parameter:\n");
	printf("  %s --trigger-edge [rising|falling]\n", argv[0]);
	exit(-1);
      } 
      else if(strcmp("rising", argv[i + 1]) == 0)
	params.triggerRisingEdge = 1;
      else if(strcmp("falling", argv[i + 1]) == 0)
	params.triggerRisingEdge = 0;
      else{
	printf("Invalid edge:\n");
	printf("  %s --trigger-edge [rising|falling]\n", argv[0]);
	exit(-1);
      }
      i += 1;
    } 
    else if(strcmp("--software-trigger", argv[i]) == 0)
      params.softwareTrigger = 1;
    else if(strcmp("--split-data", argv[i]) == 0){
      if(i + 1 >= argc){
	printf("You must specify a file name with the --split-data parameter.\n");
	exit(-1);
      }
      params.eventDataFileName = argv[i + 1];
      i += 1;
    } 
    else if(strcmp("--trigger-delay", argv[i]) == 0){
      if(i + 1 >= argc) {
	printf("You must specify a delay with the --split-data parameter.\n");
	exit(-1);
      }
      sscanf(argv[i + 1], "%d", &params.postTriggerDelay);
      i += 1;
    } 
    else if(strcmp("--dataset-name", argv[i]) == 0){
      if(i + 1 >= argc){
	printf("You must specify a dataset name with the --datasetName parameter.\n");
	exit(-1);
      }

      // else if(strlen(argv[i + 1]) > 6){
      //	printf("The dataset name must be at most six characters.\n");
      // 	exit(-1);
      // }

      params.datasetName = argv[i + 1];
      i += 1;
    } 
    else if(strcmp("--run-number", argv[i]) == 0){
      if(i + 1 >= argc) {
	printf("You must specify a run number with the --runNumber parameter.\n");
      }
      sscanf(argv[i + 1], "%d", &params.runNumber);
      i += 1;
    } 
    else if(strcmp("--inputConfig", argv[i]) == 0){
      if(i + 1 >= argc) { // BBT- this does not print
	printf("Using default config file. Specify config with the --inputConfig parameter.\n");
      }
      else
	params.inputConfig = argv[i + 1];
     
      printf("input config: %s\n", params.inputConfig);
      i += 1;
    } // end input config
    else{
      printf("Unknown parameter: %s\n", argv[i]);
      printUsage();
      exit(-1);
    }
  }


  //LORE FINAL CHECKS THAT I DON'T CARE ABOUT!
//   if(params.run == 0)
//     return params;
  

//   if(params.datasetName == NULL){
//     printf("You must specify a dataset name on the command line.\n");
//     exit(-1);
//   }

  
  // if(stat("/mnt/data/vmedigi", &st) != 0)
  // mkdir("/mnt/data/vmedigi", 0755);
 
  if(0){
    snprintf(runNumberFile, 50, "%6s.last_run", params.datasetName);

    if(params.runNumber == -1) {
      nameHandle = fopen(runNumberFile, "r");
      if (nameHandle) {
	fscanf(nameHandle, "%d", &params.runNumber);
	fclose(nameHandle);
      } 
      else
	params.runNumber = 0;
    }

    params.runNumber++;
    nameHandle = fopen(runNumberFile, "w");

    if(!nameHandle) {
      printf("Error writing to run number file.\n");
      exit(-1);
    }

    fprintf(nameHandle, "%d", params.runNumber);
    fclose(nameHandle);
  }
  
  return params;
}




void configureDigitizer(struct commandLineParameters params,
			struct vmeBusHandles *busHandles)
{
  /* Reset the board so it's in a known configuration and clear out any 
     data that may have been collected and not read out of the buffers.
     There doesn't seem to be a good way to tell when the board has come
     up so we'll sleep and hope that's enough.
  */
  printf("Resetting Digitizer...\n");
  setReg(busHandles->mappedMemory, V1742_Config_Reload, 0x0001);
  sleep(1);
  setReg(busHandles->mappedMemory, V1742_Clear_Data, 0x0001);
  sleep(1);

  printf("Configuring Digitizer...\n");

  /* Explicitly stop data acquisition. */
  setReg(busHandles->mappedMemory, V1742_Acquisition_Mode, 0x0008);

  /* Enable all four groups and the external and software triggers. */
  setReg(busHandles->mappedMemory, V1742_Trigger_Enable, 0xC0000000);
  setReg(busHandles->mappedMemory, V1742_Trigger_Out_Enable, 0x000F);
  setReg(busHandles->mappedMemory, V1742_Group_Enable, 0x000F);

  /* Configure the DACs that bias the TR0 and TR1 signals.  This is currently
     setup for NIM logic levels.
  */
  //Hex: 32768 = 8000, 20934 = 51C6, 15000 == 3A98, 10000 == 2710, 5000 == 1388, 1000 == 03E8

  // BBT: 04-04-18
  //dumpRegisters(busHandles, busHandles->mappedMemory, "before config");
  printf("\n=== Parsing configuration file %s ===\n", params.inputConfig);
  setChannelOffsetsFromConfig(busHandles->mappedMemory, params.inputConfig);
  //dumpRegisters(busHandles, busHandles->mappedMemory, "after config");

  setReg(busHandles->mappedMemory, V1742_TR0_DC_Offset, 0x8000);
  setReg(busHandles->mappedMemory, V1742_TR0_Threshold, 0x51C6);
  setReg(busHandles->mappedMemory, V1742_TR1_DC_Offset, 0x8000);
  setReg(busHandles->mappedMemory, V1742_TR1_Threshold, 0x51C6);
  
  //Si's changed values
  //setReg(busHandles->mappedMemory, V1742_TR0_DC_Offset, 0x8000);
  //setReg(busHandles->mappedMemory, V1742_TR0_Threshold, 0xFFFF);
  //setReg(busHandles->mappedMemory, V1742_TR1_DC_Offset, 0x9088);
  //setReg(busHandles->mappedMemory, V1742_TR1_Threshold, 0x51C6);
  
  /* Parameters which can be altered on the command line. */
  setReg(busHandles->mappedMemory, V1742_Group_Config, 0x30001110);
  if(params.digitizeTrigger) {
    printf("  Will digitize TR0/TR1 signals.\n");
    setReg(busHandles->mappedMemory, V1742_Group_Config_Set, 0x1 << 11);
  } else {
    printf("  Will NOT digitize TR0/TR1 signals.\n");
  }

  if(params.triggerRisingEdge) {
    printf("  Will trigger on the rising edge of the trigger signal.\n");
    setReg(busHandles->mappedMemory, V1742_Group_Config_Clear, 0x1 << 6);
  } else {
    printf("  Will trigger on the falling edge of the trigger signal.\n");
    setReg(busHandles->mappedMemory, V1742_Group_Config_Set, 0x1 << 6);
  }

  if(params.postTriggerDelay != -1) {
    printf("  Setting post trigger delay to 0x%x.\n", params.postTriggerDelay);
    setReg(busHandles->mappedMemory, V1742_Post_Trigger_Delay, params.postTriggerDelay);
  }
  
  if(params.testMode) {
    printf("  Enabling test mode.\n");
    setReg(busHandles->mappedMemory, V1742_Group_Config_Set, 0x1 << 3);
  }

  if(params.numSamples == 0 || params.numSamples == 1024) {
    printf("  1024 samples at ");
    setReg(busHandles->mappedMemory, V1742_Custom_Size, 0x0000);
  } else if(params.numSamples == 520) {
    printf("  520 samples at ");
    setReg(busHandles->mappedMemory, V1742_Custom_Size, 0x0001);
  } else if(params.numSamples == 256) {
    printf("  256 samples at ");
    setReg(busHandles->mappedMemory, V1742_Custom_Size, 0x0002);
  } else {
    printf("  136 samples at ");
    setReg(busHandles->mappedMemory, V1742_Custom_Size, 0x0003);
  }

  if(params.sampleRate == 0 || params.sampleRate == 50) {
    printf("5GS/s.\n");
    setReg(busHandles->mappedMemory, V1742_Sampling_Frequency, 0x0000);
  } else if(params.sampleRate == 25) {
    printf("2.5GS/s.\n");
    setReg(busHandles->mappedMemory, V1742_Sampling_Frequency, 0x0001);
  } else {
    printf("1GS/s.\n");
    setReg(busHandles->mappedMemory, V1742_Sampling_Frequency, 0x0002);
  }

  /* Enable acquisition. */
  setReg(busHandles->mappedMemory, V1742_Acquisition_Mode, 0x000C);
  printf("\n");
  return;
}

void triggerDigitizer(struct vmeBusHandles *busHandles)
{
  busHandles->mappedMemory[V1742_Software_Trigger] = 1;
  return;
}


int 
main(int argc, char **argv){

  std::cout << "Running digitizer!" << std::endl;
  //LORE
  /////////////////////
  // Bind UDP socket //
  /////////////////////
      
  //sendSockfd = makeSocket(string("localhost").c_str(),myport,p);

  //struct addrinfo hints, *servinfo;
  struct addrinfo* p;

  int communicationSocket              = makeSocket(THIS_IP,COMMUNICATION_PORT,p);
  int streamingSocket                  = makeSocket(THIS_IP,STREAMING_PORT,p);
  struct sockaddr_in streamingReceiver = setupSocketAddress(DESTINATION_IP, DESTINATION_PORT);
  struct sockaddr_in messageSender;

  std::string communicationBuffer;
  //unsigned int data_buffer[32];
  //END LORE

  struct commandLineParameters params;
  struct vmeBusHandles busHandles;

  params = parseCommandLine(argc, argv);


  setupVME(&busHandles);

  bool writeFile = false;

  //LORE
  unsigned int memory[1024];
  FILE* fileHandle = 0;
  std::string fileName;
  unsigned int eventSize     = 0;
  unsigned int eventCount    = 0;
  unsigned int eventsStored  = 0;
  unsigned int dataCounter   = 0;
  std::string currentRun;
  int readResult = 0;
  bool running = false;

  while(1){
    communicationBuffer = "";
    //std::cout << "looping" << std::endl;
    if (receiveAndAcknowledge(communicationSocket, messageSender, communicationBuffer) >= 0){
      std::cout << "Received: " << communicationBuffer << std::endl;

      if (communicationBuffer.substr(0,5) == "START") {
	currentRun = communicationBuffer.substr(6,communicationBuffer.length()-6);
	running     = true;
	eventCount  = 0;
	dataCounter = 0;
	if(writeFile){
	  fileName = "Run_" + currentRun + ".dat";
	  std::cout << "-" << fileName << "-" << std::endl;
	  fileHandle = fopen(fileName.c_str(), "w");
	}
	configureDigitizer(params, &busHandles);
// 	displayStatus(busHandles.mappedMemory);
	std::cout << "Run " << currentRun << " started!" << std::endl;
      } else if (communicationBuffer == "STOP") {
	running = false;
	if(writeFile) fclose(fileHandle);
	dataCounter = 0;
// 	std::cout << "Final count: " << dataCounter << std::endl;
	std::cout << "Run " << currentRun << " stopped!" << std::endl;
	std::cout << "Processed: " << eventCount << " triggers for Run " << currentRun << "!" << std::endl;
      } else if (communicationBuffer == "PAUSE") {
	running = false;
      } else if (communicationBuffer == "RESUME") {
	configureDigitizer(params, &busHandles);
	running = true;
      } else if (communicationBuffer == "CONFIGURE") {
	configureDigitizer(params, &busHandles);
	displayStatus(busHandles.mappedMemory);
      } 
    }
      
    if(running){
      eventsStored = busHandles.mappedMemory[V1742_Event_Stored];
      eventSize    = busHandles.mappedMemory[V1742_Event_Size];

      //Here polling the register to see if there was a trigger!!!
      eventSize = busHandles.mappedMemory[V1742_Event_Stored];//WEIRD SHOULD BE eventSIZE!!!!
      if(eventSize == 0) continue;
      eventSize = busHandles.mappedMemory[V1742_Event_Size];
      //std::cout << "Ev size: " << eventSize << std::endl;

      ++eventCount;
//       if (eventCount == 500)
// 	running = false;
//       if (eventCount % 50 == 0){
      std::cout << "Event #: " << std::hex << "0x" << eventCount << " -> dec: " << std::dec << eventCount << std::endl;
//       }

     
      while (eventSize > 0) {
	// 	std::cout << "Ev size: " << eventSize << std::endl;
	
	if(eventSize > 1023){
// 	  std::cout << "Ev size 1023: " << eventSize << std::endl;
	  readResult = vme_dma_read(busHandles.busHandle, busHandles.dmaHandle, 0, 0x10000000, VME_A32UD, 0xFFC, 0);
	  if(writeFile) fwrite(busHandles.dmaBufferMemory, sizeof(unsigned int), 1023, fileHandle);
	  sendto(streamingSocket, busHandles.dmaBufferMemory, sizeof(unsigned int)*1023, 0, (struct sockaddr *)&streamingReceiver, sizeof(streamingReceiver));
// 	  for(unsigned int i=0; i<1023; i++)
// 	    std::cout << dataCounter++ << ") " << 1023*4 << "->" << std::hex << busHandles.dmaBufferMemory[1] << std::dec << std::endl;
// 	  std::cout << 4092 << " -> " << std::hex << busHandles.dmaBufferMemory[0] << std::dec << std::endl;
 	  eventSize -= 1023;
 	} 
 	else {
	  for(unsigned int i=0; i<eventSize; i++){
	    memory[i] = busHandles.mappedMemory[0];
	    //	    eventSize--;
	  }
  	  if(writeFile) fwrite(memory, sizeof(unsigned int), eventSize, fileHandle);
  	  sendto(streamingSocket, memory, sizeof(unsigned int)*eventSize, 0, (struct sockaddr *)&streamingReceiver, sizeof(streamingReceiver));
//   	  std::cout << dataCounter++ << ") " << 4 << "->" << eventSize << std::endl;
	  eventSize = 0;
 	}
      }
    }
    //usleep(1);
  }

  // Clean up and exit
  close(communicationSocket);
  close(streamingSocket);
  //LORE


  std::cout << "SW TRIG: " << params.softwareTrigger << std::endl;
  if(params.softwareTrigger)
    triggerDigitizer(&busHandles);
    
  tearDownVME(&busHandles);

  return 0;
}
