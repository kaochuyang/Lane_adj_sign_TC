#include "protocol_AD_cms_travel_time.h"
#include "SMEM.h"
#include "CDataToMessageOK.h"
#include "WRITEJOB.h"
protocol_AD_cms_travel_time::protocol_AD_cms_travel_time()
{
    //ctor
}

protocol_AD_cms_travel_time::~protocol_AD_cms_travel_time()
{
    //dtor
}

bool protocol_AD_cms_travel_time::DoWorkViaPTraffic92(MESSAGEOK mes)
{

    try
    {


        switch(mes.packet[8])
        {
        case 0x10:
            _AD10_time_display_set(mes);
            break;
        case 0x40:
            _AD40_time_display_query();
            break;
        case 0x11:
            _AD11_CMS_controler_interrupt_set(mes);
            break;
        case 0x41:
            _AD41_CMS_controler_interrupt_query();

        default:

            vReturnToCenterNACK(0xAD,mes.packet[8],0,0);
            break;
        }


    }
    catch(...) {}


}
int protocol_AD_cms_travel_time::getValueRecord(int num)
{
    int result=0;
    switch(num)
    {
    case 1:
        result=value_record.ID1_value;
        break;
    case 2:
        result=value_record.ID2_value;
        break;
    case 3:
        result=value_record.ID3_value;
        break;
    default:
        break;
        return result;
    }

}

void protocol_AD_cms_travel_time::setValueRecord(int num,int value)
{

    if(value>99||value<1)value=0;
    switch(num)
    {
    case 1:
        value_record.ID1_value=value;
        break;
    case 2:
        value_record.ID2_value=value;
        break;
    case 3:
        value_record.ID3_value=value;
        break;
    default:
        break;

    }

}
void protocol_AD_cms_travel_time::_AD10_time_display_set(MESSAGEOK mes)
{

    try
    {
        char cTMP[256];
        smem.count_vd_alive=0;
        int ID_value[3]= {0};
        int error_flag=0;
        for(int i=0; i<2; i++)
        {
            ID_value[i]=mes.packet[10+2*i];
            if (((ID_value[i]<254)&&(ID_value[i]>99))||(ID_value[i]<1))
            {
                error_flag=ID_value[i];
                vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);
            }
            else
            {
                if(ID_value[i]==255) {} ID_value[i]=0;
                setValueRecord(i,ID_value[i]);
            }
        }
        sprintf(cTMP, "AD__ID0_value=%d ID1_value=%d ID2_value=%d", ID_value[0],ID_value[1], ID_value[2]);
        smem.vWriteMsgToDOM(cTMP);


        if(error_flag==0)
        {
            printf("AD__ID1=%d minutes ID2=%d minutes ID3=%d minutes",ID_value[0],ID_value[1], ID_value[2]);
            store_value(value_record);
            vReturnToCenterACK(0xAD,0x10);
        }

    }
    catch(...) {}
}
void protocol_AD_cms_travel_time::_AD10_test_time_display_set(int x,int y,int z)
{

    try
    {

        smem.vWriteMsgToDOM("manual setting AD10");
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
            vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);

        }
        else value_record.ID1_value=ID1_value;
        if (ID2_value>99&&ID2_value<0)
        {
            error_flag=ID2_value;
            vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);

        }
        else value_record.ID2_value=ID2_value;
        if (ID3_value>99&&ID3_value<0)
        {
            error_flag=ID3_value;
            vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);

        }
        else value_record.ID3_value=ID3_value;


        if(error_flag==0)
        {
            printf("ID1=%d minutes ID2=%d minutes ID3=%d minutes",ID1_value,ID2_value,ID3_value);
            store_value(value_record);
            vReturnToCenterACK(0xAD,0x10);
        }

    }
    catch(...) {}


}


void protocol_AD_cms_travel_time::_AD40_time_display_query()
{

    try
    {
        smem.vWriteMsgToDOM("_AD40_time_display_query");
        _ADC0_time_display_rport_report();

    }
    catch(...) {}


}



void protocol_AD_cms_travel_time::_ADC0_time_display_rport_report()
{

    try
    {

        printf("display value report\n");
        unsigned char pack[8];


        pack[0]=0xAD;
        pack[1]=0xC0;
        int ID1_value=value_record.ID1_value;
        int ID2_value=value_record.ID2_value;
        int ID3_value=value_record.ID3_value;
        int error_flag=0;
        if (ID1_value>99&&ID1_value<1)
        {
            pack[3]=0xff;
        }
        else pack[3]=ID1_value;
        if (ID2_value>99&&ID2_value<1)
        {
            pack[5]=0xff;
        }
        else pack[5]=ID2_value;
        if (ID3_value>99&&ID3_value<1)
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

void protocol_AD_cms_travel_time::_AD00_time_display_auto_report()
{


    try
    {
        printf("display value report\n");
        unsigned char pack[8];


        pack[0]=0xAD;
        pack[1]=0x00;
        int ID1_value=value_record.ID1_value;
        int ID2_value=value_record.ID2_value;
        int ID3_value=value_record.ID3_value;
        int error_flag=0;
        pack[2]=1;
        if (ID1_value>99&&ID1_value<1)
        {
            pack[3]=0xff;
        }
        else pack[3]=ID1_value;
        pack[4]=2;
        if (ID2_value>99&&ID2_value<1)
        {
            pack[5]=0xff;
        }
        else pack[5]=ID2_value;
        pack[6]=3;
        if (ID3_value>99&&ID3_value<1)
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

void protocol_AD_cms_travel_time::_AD11_CMS_controler_interrupt_set(MESSAGEOK meg)
{

    try
    {

        char cTMP[256];
        if(meg.packet[9]>99&&meg.packet[9]<0)vReturnToCenterNACK(0xAD,0x11,0x4,0x1);
        else
        {
            value_record.interrrupt_time=meg.packet[9];

            value_record.switch_button=meg.packet[10];
//save
            sprintf(cTMP, "interrupt time = %d minutes,switch button=%d", value_record.interrrupt_time, value_record.switch_button);
            smem.vWriteMsgToDOM(cTMP);
            store_value(value_record);
            vReturnToCenterACK(0xAD,0x11);
        }

    }
    catch(...) {}

}

void protocol_AD_cms_travel_time::_AD41_CMS_controler_interrupt_query()
{
    try
    {
        smem.vWriteMsgToDOM("_AD41_CMS_controler_interrupt_query");
        _ADC1_CMS_controler_interrupt_report();


    }
    catch(...) {}

}
void protocol_AD_cms_travel_time::_ADC1_CMS_controler_interrupt_report()
{

    try
    {


        printf("display value report\n");
        unsigned char pack[4];


        pack[0]=0xAD;
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









