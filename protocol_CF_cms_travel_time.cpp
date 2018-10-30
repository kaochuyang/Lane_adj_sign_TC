#include "protocol_CF_cms_travel_time.h"
#include "SMEM.h"
#include "CDataToMessageOK.h"
#include "WRITEJOB.h"
protocol_CF_cms_travel_time::protocol_CF_cms_travel_time()
{
    //ctor
}

protocol_CF_cms_travel_time::~protocol_CF_cms_travel_time()
{
    //dtor
}

bool protocol_CF_cms_travel_time::DoWorkViaPTraffic92(MESSAGEOK mes)
{

    try
    {


        switch(mes.packet[8])
        {
        case 0x10:
            _CF10_time_display_set(mes);
            break;
        case 0x40:
            _CF40_time_display_query();
            break;
        case 0x11:
            _CF11_CMS_controler_interrupt_set(mes);
            break;
        case 0x41:
            _CF41_CMS_controler_interrupt_query();

        default:

            vReturnToCenterNACK(0xCF,mes.packet[8],0,0);
            break;
        }


    }
    catch(...) {}


}

void protocol_CF_cms_travel_time::_CF10_time_display_set(MESSAGEOK mes)
{

    try
    {
        char cTMP[256];
        smem.count_vd_alive=0;
        int ID1_value=mes.packet[10];
        int ID2_value=mes.packet[12];
        int ID3_value=mes.packet[14];
        sprintf(cTMP, "ID0_value=%d ID1_value=%d ID2_value=%d", ID1_value,ID2_value, ID3_value);
        smem.vWriteMsgToDOM(cTMP);
        int error_flag=0;
        if (ID1_value>99&&ID1_value<0)
        {
            error_flag=ID1_value;
            vReturnToCenterNACK(0xCF,0x10,0x4,error_flag);

        }
        else value_record.ID1_value=ID1_value;
        if (ID2_value>99&&ID2_value<0)
        {
            error_flag=ID2_value;
            vReturnToCenterNACK(0xCF,0x10,0x4,error_flag);

        }
        else value_record.ID2_value=ID2_value;
        if (ID3_value>99&&ID3_value<0)
        {
            error_flag=ID3_value;
            vReturnToCenterNACK(0xCF,0x10,0x4,error_flag);

        }
        else value_record.ID3_value=ID3_value;


        if(error_flag==0)
        {
            printf("ID1=%d minutes ID2=%d minutes ID3=%d minutes",ID1_value,ID2_value,ID3_value);
            store_value(value_record);
            vReturnToCenterACK(0xCF,0x10);
        }

    }
    catch(...) {}
}
void protocol_CF_cms_travel_time::_CF10_test_time_display_set(int x,int y,int z)
{

     try
    {

        smem.vWriteMsgToDOM("manual setting CF10");
        char cTMP[256];
        smem.count_vd_alive=0;
        int ID1_value=x;
        int ID2_value=y;
        int ID3_value=z;
        sprintf(cTMP, "ID0_value=%d ID1_value=%d ID2_value=%d", ID1_value,ID2_value, ID3_value);
        smem.vWriteMsgToDOM(cTMP);
        int error_flag=0;
        if (ID1_value>99&&ID1_value<0)
        {
            error_flag=ID1_value;
            vReturnToCenterNACK(0xCF,0x10,0x4,error_flag);

        }
        else value_record.ID1_value=ID1_value;
        if (ID2_value>99&&ID2_value<0)
        {
            error_flag=ID2_value;
            vReturnToCenterNACK(0xCF,0x10,0x4,error_flag);

        }
        else value_record.ID2_value=ID2_value;
        if (ID3_value>99&&ID3_value<0)
        {
            error_flag=ID3_value;
            vReturnToCenterNACK(0xCF,0x10,0x4,error_flag);

        }
        else value_record.ID3_value=ID3_value;


        if(error_flag==0)
        {
            printf("ID1=%d minutes ID2=%d minutes ID3=%d minutes",ID1_value,ID2_value,ID3_value);
            store_value(value_record);
            vReturnToCenterACK(0xCF,0x10);
        }

    }
    catch(...) {}


}


void protocol_CF_cms_travel_time::_CF40_time_display_query()
{

    try
    {
        smem.vWriteMsgToDOM("_CF40_time_display_query");
        _CFC0_time_display_rport_report();

    }
    catch(...) {}


}



void protocol_CF_cms_travel_time::_CFC0_time_display_rport_report()
{

    try
    {

        printf("display value report\n");
        unsigned char pack[8];


        pack[0]=0xCF;
        pack[1]=0xC0;
        int ID1_value=value_record.ID1_value;
        int ID2_value=value_record.ID2_value;
        int ID3_value=value_record.ID3_value;
        int error_flag=0;
        if (ID1_value>99&&ID1_value<0)
        {
            pack[3]=0xff;
        }
        else pack[3]=ID1_value;
        if (ID2_value>99&&ID2_value<0)
        {
            pack[5]=0xff;
        }
        else pack[5]=ID2_value;
        if (ID3_value>99&&ID3_value<0)
        {
            pack[7]=0xff;

        }
        else pack[7]=ID3_value;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 8,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);


//_intervalTimer.LAS_module_query_timer(0);


    }
    catch(...) {}
}

void protocol_CF_cms_travel_time::_CF00_time_display_auto_report()
{


    try
    {
        printf("display value report\n");
        unsigned char pack[8];


        pack[0]=0xCF;
        pack[1]=0x00;
        int ID1_value=value_record.ID1_value;
        int ID2_value=value_record.ID2_value;
        int ID3_value=value_record.ID3_value;
        int error_flag=0;
        pack[2]=1;
        if (ID1_value>99&&ID1_value<0)
        {
            pack[3]=0xff;
        }
        else pack[3]=ID1_value;
        pack[4]=2;
        if (ID2_value>99&&ID2_value<0)
        {
            pack[5]=0xff;
        }
        else pack[5]=ID2_value;
        pack[6]=3;
        if (ID3_value>99&&ID3_value<0)
        {
            pack[7]=0xff;

        }
        else pack[7]=ID3_value;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 8,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);
    }
    catch(...) {}

}

void protocol_CF_cms_travel_time::_CF11_CMS_controler_interrupt_set(MESSAGEOK meg)
{

    try
    {

        char cTMP[256];
        if(meg.packet[9]>99&&meg.packet[9]<0)vReturnToCenterNACK(0xCF,0x11,0x4,0x1);
        else
        {
            value_record.interrrupt_time=meg.packet[9];

            value_record.switch_button=meg.packet[10];
//save
            sprintf(cTMP, "interrupt time = %d minutes,switch button=%d", value_record.interrrupt_time, value_record.switch_button);
            smem.vWriteMsgToDOM(cTMP);
            store_value(value_record);
            vReturnToCenterACK(0xCF,0x11);
        }

    }
    catch(...) {}

}

void protocol_CF_cms_travel_time::_CF41_CMS_controler_interrupt_query()
{
    try
    {
        smem.vWriteMsgToDOM("_CF41_CMS_controler_interrupt_query");
        _CFC1_CMS_controler_interrupt_report();
    }
    catch(...) {}

}
void protocol_CF_cms_travel_time::_CFC1_CMS_controler_interrupt_report()
{

    try
    {


        printf("display value report\n");
        unsigned char pack[4];


        pack[0]=0xCF;
        pack[1]=0xC1;

        pack[2]=value_record.interrrupt_time;
        pack[3]=value_record.switch_button;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 4,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);
  }
    catch(...) {}

}


void protocol_CF_cms_travel_time::store_value(Value_Record object)
{

    try
    {
        printf("Record_Value_object_store\n");
        FILE *pf=NULL;
        char filename[256]= {0};
        sprintf(filename,"/cct/Data/SETTING/CMS_TRAVEL_TIME_ini.bin");
        pf=fopen(filename,"w+");
        if(pf!=NULL)
        {

            fwrite(&object,sizeof(Value_Record),1,pf);
            fclose(pf);
            printf("write Value_Record_object success!.");
            smem.vWriteMsgToDOM("write CMS_TRAVEL_TIME_ini success\n");

        }
        else
        {
            smem.vWriteMsgToDOM("write CMS_TRAVEL_TIME_ini error\n");
            printf("write CMS_TRAVEL_TIME_ini  no file\n");

        };

    }
    catch(...) {}
}

void protocol_CF_cms_travel_time::read_value(Value_Record *object)
{

    try
    {
        FILE *pf=NULL;
        char filename[256]="/cct/Data/SETTING/CMS_TRAVEL_TIME_ini.bin";
        pf=fopen(filename,"r");
        if(pf!=NULL)
        {
            fread(object,sizeof(Value_Record),1,pf);
            fclose(pf);
            smem.vWriteMsgToDOM("read CMS_TRAVEL_TIME_ini success\n");
            printf("read CMS_TRAVEL_TIME_ini success\n");
        }
        else
        {
             pf=fopen(filename,"w+");
             fwrite(&object,sizeof(Value_Record),1,pf);
            smem.vWriteMsgToDOM("read CMS_TRAVEL_TIME_ini error\n");
            printf("read CMS_TRAVEL_TIME_ini object error\n");
        }
    }
    catch(...) {}
}
void protocol_CF_cms_travel_time::clear_value_record(Value_Record *object)
{
    try
    {
        object->ID1_value=0;
        object->ID2_value=0;
        object->ID3_value=0;
        object->interrrupt_time=30;
        object->switch_button=0;
        object->display_mode=1;
        object->lightQuantity=3;

    }
    catch(...) {}

}

void protocol_CF_cms_travel_time::_CF02_hw_state_auto_report()
{

    try
    {
        printf("CF02_hw_state_auto_report\n");
        unsigned char pack[3];


        pack[0]=0xCF;
        pack[1]=0x02;
        pack[2]=0x1;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 3,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);



    }
    catch(...) {}


}
