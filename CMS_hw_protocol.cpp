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
void   CMS_hw_protocol::AVI_protocol(int value,int ID)
{
    try
    {if(ID>3||ID<1)ID=1;
        BYTE packet[8];
        packet[0]=0xaa;
        packet[1]=0xbb;
        packet[2]=(BYTE)smem.GetSequence();
        packet[3]=ID;
        packet[4]=0x10;//hard code
        packet[5]=(BYTE)value;
        packet[6]=0x1e;//timeout 30 sec hard code
        packet[7]=0x0;//cks
     /*   packet[8]=0x15;//cms travel time protocol only
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
        packet[20] = 0x0;//cks*/
        for (int a=0; a<7; a++)
            packet[7]^=packet[a];
smem.redCountPort.Rs232Write(packet,8,"dev-CMSTravelTime");
    }
    catch(...)
    {}
}


