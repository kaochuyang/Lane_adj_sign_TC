#include "CMS_hw_protocol.h"
#include "SMEM.h"
#include "var.h"
CMS_hw_protocol::CMS_hw_protocol()
{
    //ctor
}

CMS_hw_protocol::~CMS_hw_protocol()
{
    //dtor
}
void   CMS_hw_protocol::AVI_protocol(int ID1_value,int ID2_value,int ID3_value)
{


    try
    {

        BYTE packet[21];
        packet[0]=0xaa;
        packet[1]=0xbb;
        packet[2]=0x36;
        packet[3]=0xff;
        packet[4]=0xff;
        packet[5]=0x0;
        packet[6]=0x15;
        packet[7]=0xea;
        packet[8]=0x15;//cms travel time protocol only
        packet[9]=0x08;
///////context///////
        packet[10]=ID1_value;
        packet[11]=ID2_value;
        packet[12]=ID3_value;
//////////context////////
        packet[13]=0x0;
        packet[14]=0x0;
        packet[15]=0x0;
        packet[16]=0x0;
        packet[17]=0x0;
        packet[18]=0xaa;
        packet[19]=0xcc;
        packet[20] = 0x0;//cks
        for (int a=0; a<20; a++)
            packet[20]^=packet[a];

smem.redCountPort.Rs232Write(packet,21,"dev-RedCount");



    }
    catch(...)
    {}





}