#include "protocol_8F_LAS.h"
#include "SMEM.h"
#include "PTRAFFIC92COMM.h"
protocol_8F_LAS::protocol_8F_LAS()
{
    //ctor
}

protocol_8F_LAS::~protocol_8F_LAS()
{
    //dtor
}
void protocol_8F_LAS::send_to_center_2(BYTE head,BYTE type)
{
    try
    {

        MESSAGEOK _MSG;
        unsigned char data3[2];
        data3[0]  = head;
        data3[1]  =type;

        _MSG = oDataToMessageOK.vPackageINFOTo92Protocol(data3, 2, true);
        _MSG.InnerOrOutWard = cOutWard;
        writeJob.WritePhysicalOut(_MSG.packet, _MSG.packetLength, DEVICECENTER92);



    }
    catch(...) {}
}
void protocol_8F_LAS::send_to_center_3(BYTE head,BYTE type,BYTE value)
{
    try
    {

        MESSAGEOK _MSG;
        unsigned char data3[3];
        data3[0]  = head;
        data3[1]  =type;
        data3[2]  =value;
        _MSG = oDataToMessageOK.vPackageINFOTo92Protocol(data3, 3, true);
        _MSG.InnerOrOutWard = cOutWard;
        writeJob.WritePhysicalOut(_MSG.packet, _MSG.packetLength, DEVICECENTER92);




    }
    catch(...) {}
}

//ACK------------------------------------------------------------------------
int protocol_8F_LAS::vReturnToCenterACK(unsigned char ucDevCode,
                                        unsigned char ucControlCode)
{
    try
    {
        unsigned char data[4];

        data[0] = 0x0F;
        data[1] = 0x80;
        data[2] = ucDevCode;
        data[3] = ucControlCode;

        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 4,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return 0;
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
int protocol_8F_LAS::vReturnToCenterNACK(unsigned char ucDevCode,
        unsigned char ucControlCode,
        unsigned char ErrorCode,
        unsigned char ParameterNumber)
{
    try
    {
        unsigned char data[6];

        data[0] = 0x0F;
        data[1] = 0x81;
        data[2] = ucDevCode;
        data[3] = ucControlCode;
        data[4] = ErrorCode;
        data[5] = ParameterNumber;

        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 6,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return 0;
    }
    catch (...) {}
}
//**************************brightness*******************************/
void protocol_8F_LAS::_8f12_brightness_set(BYTE brightness)
{

    try
    {



        if(0<brightness&&brightness<5)
        {

            printf("%x = brightness 8f 12 set\n",brightness);
            smem.junbo_LASC_object.brightness_control(brightness);
            vReturnToCenterACK(0x8f,0x12);


        }
        else vReturnToCenterNACK(0x8f,0x12,0x4,0x1);


    }
    catch(...) {}
}
void protocol_8F_LAS::_8f42_brightness_query()
{
    try
    {
        printf("8f42 query brightness \n");
        vReturnToCenterACK(0x8f,0x42);
        _8fc2_brightness_report();

    }
    catch(...) {}
}

void protocol_8F_LAS::_8fc2_brightness_report()
{
    try
    {

        printf("8fc2 brightness report\n");


        if(smem.Lane_adj_memo_object.brightness<1||smem.Lane_adj_memo_object.brightness>4)
        {
            send_to_center_3(0x8f,0xc2,0x1);
            smem.junbo_LASC_object.brightness_control(0x1);
        }
        else
            send_to_center_3(0x8f,0xc2,smem.Lane_adj_memo_object.brightness);
        vReturnToCenterACK(0x8f,0xc2);


    }
    catch(...) {}
}
//////////////////////////////////////////////////////////////////////////
void protocol_8F_LAS::_8f45_module_query()
{
    try
    {
        printf("8f45 module report\n");
        smem.junbo_LASC_object.query_module_state();
        vReturnToCenterACK(0x8f,0x45);

    }
    catch(...) {}
}
void protocol_8F_LAS::_8fc5_module_report()
{
    try
    {


        printf("8fc5 module report\n");
        vReturnToCenterACK(0x8f,0xc5);
        unsigned char pack[18];
        unsigned char *data;
        data=smem.junbo_LASC_object.report_module_state();

        pack[0]=0x8f;
        pack[1]=0xc5;
        for(int ID=1; ID<9; ID++)
        {
            pack[2*ID+1]=ID;
            pack[2*ID+2]=data[ID];
        }


        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 18,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);




    }
    catch(...) {}


}

void protocol_8F_LAS::_8f05_module_act_report()
{
    try
    {
        printf("8f05 module act report\n");

        unsigned char pack[18];
        unsigned char *data;
        data=smem.junbo_LASC_object.report_module_state();
        pack[0]=0x8f;
        pack[1]=0x05;
        for(int ID=1; ID<9; ID++)
        {
            pack[2*ID+1]=ID;
            pack[2*ID+2]=data[ID];
        }


        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 18,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);


    }
    catch(...) {}
}

///////////////////////////////////////////////////////////////////////////////////////

void protocol_8F_LAS::_8f16_default_light_set(MESSAGEOK mes)
{

    try
    {
        printf("8f16 default light set \n");


        for(int ID=1; ID<9; ID++)
            smem.Lane_adj_memo_object.defaul_light[ID]=mes.packet[ID+1];


        smem.junbo_LASC_object.store_lane_adj_setting(smem.Lane_adj_memo_object);
        vReturnToCenterACK(0x8f,0x16);



    }
    catch(...) {}



}

void protocol_8F_LAS::_8f46_default_light_query()
{
    try
    {

        printf("8f46 default light set \n");


        vReturnToCenterACK(0x8f,0x46);
        _8fc6_default_light_report();


    }
    catch(...) {}
}


void protocol_8F_LAS::_8fc6_default_light_report()
{
    try
    {
        printf("8fc6 default light report\n");

        unsigned char pack[10];
        unsigned char *data;
        data=smem.Lane_adj_memo_object.defaul_light;
        pack[0]=0x8f;
        pack[1]=0xc6;
        for(int ID=1; ID<9; ID++)
        {
            pack[ID+2]=data[ID];
        }


        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 10,true);
        _MsgOK.InnerOrOutWard = cOutWard;

        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);


    }
    catch(...) {}
}

void protocol_8F_LAS::_8f10_commonday_set(MESSAGEOK mes)
{



    try
    {
        bool error_mark=false;

        printf("8f10 commonday set\n");

        int typeID=mes.packet[2];
        LAS_excute_info* pf;


        if(typeID<1||typeID>7)
        {
            vReturnToCenterNACK(0x8f,0x10,0x4,0x1);
            error_mark=true;
        }
        else smem.LAS_segmenttype[typeID].segmentcount=mes.packet[3];

        pf=&smem.LAS_segmenttype[typeID];


        int lightcount;
        int segmentcount=mes.packet[3];

        pf->lightcount=lightcount=mes.packet[4];
 int light_series=2+2*lightcount;
        for(int i=1; i<segmentcount+1; i++)
        {

            pf->hour[i]=mes.packet[4+(i-1)*light_series+1];
            pf->min[i]=mes.packet[4+(i-1)*light_series+2];

            for(int j=0; j<lightcount; j++)
            {
                pf->light_select[4+(i-1)*light_series+3+2*j][i];//ID , plan
                pf->lightID[j+1]=mes.packet[4+(i-1)*light_series+4+2*j];
            }
        }
        int numweek;
        pf->weekcount=numweek=mes.packet[4+light_series*segmentcount+1];//numweek locate=4+4*lightcount+1
        for(int i=0; i<numweek; i++)
        {

            switch (mes.packet[4+light_series*segmentcount+2+i])
            {

            case 1:
                pf->week[1]=typeID;

                break;

            case 2:
                pf->week[1]=typeID;

                break;

            case 3:
                pf->week[1]=typeID;
                break;

            case 4:
                pf->week[1]=typeID;
                break;

            case 5:
                pf->week[1]=typeID;
                break;

            case 6:
                pf->week[1]=typeID;
                break;


            case 7:
                pf->week[1]=typeID;
                break;


            default:
                vReturnToCenterNACK(0x8f,0x10,0x4,4+light_series*segmentcount+i);
                error_mark=true;
                break;
            }
        }


        if(error_mark==false)
        {


            FILE *pf1=NULL;
            char filename[256]= {0};
            sprintf(filename,"/cct/Data/SETTING/LAS_8f10_setting.bin");
            pf1=fopen(filename,"w+");
            if(pf1!=NULL)
            {

                fwrite(pf,sizeof(LAS_excute_info),typeID,pf1);
                fclose(pf1);
                printf("write LAS_excute_info success!.");
                smem.vWriteMsgToDOM("write LAS_excute_info success\n");


            }
            else
            {
                smem.vWriteMsgToDOM("write LAS_excute_info error\n");
                printf("write LAS_excute_info no file\n");

            };
            vReturnToCenterACK(0x8f,0x10);

        }





    }
    catch(...) {}




}



void protocol_8F_LAS::_8f40_commanday_query(BYTE segmenttype)
{
    try
    {

        printf("8f40 commanday query\n");

        if(segmenttype>7||segmenttype<1)vReturnToCenterNACK(0x8f,0x40,0x4,0x1);


        vReturnToCenterACK(0x8f,0x40);

        _8fc0_commonday_report();




    }
    catch(...) {}
}

void protocol_8F_LAS::_8fc0_commonday_report(BYTE segmenttype)
{

    try
    {



        LAS_excute_info *pf_o;



        FILE *pf=NULL;
        char filename[256]="/cct/Data/SETTING/LAS_8f10_setting.bin";
        pf=fopen(filename,"r");
        if(pf!=NULL)
        {
            fread(&pf_o,sizeof(LAS_excute_info),1*segmenttype,pf);
            fclose(pf);
            smem.vWriteMsgToDOM("read LAS_8f10_setting success\n");
            printf("read LAS_8f10_setting success\n");


        }
        else
        {
            smem.vWriteMsgToDOM("read LAS_8f10_setting error\n");
            printf("read LAS_8f10_setting object error\n");


        }

        BYTE pack[45];//pack max=5+8*4+1+7=45

        pack[0]=0x8f;
        pack[1]=0xc0;
        pack[2]=segmenttype;
        pack[3]=pf_o->segmentcount;
        pack[4]=pf_o->lightcount;

        for(int i=1; i<pack[4]+1; i++)
        {






        }



        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 10,true);
        _MsgOK.InnerOrOutWard = cOutWard;

        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);




    }
    catch(...) {}

}


































