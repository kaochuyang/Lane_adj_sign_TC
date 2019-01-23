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
            break;
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
    case 0:
        printf("case 1\n");
        result=value_record.ID1_value;
        printf("result=%d\n",result);
        break;
    case 1:
        printf("case 2\n");
        result=value_record.ID2_value;
        printf("result=%d\n",result);
        break;
    case 2:
        printf("case 3\n");
        result=value_record.ID3_value;
        printf("result=%d\n",result);
        break;
    default:
        printf("result=%d\n",result);
        break;
    }
    printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\__\n");
    printf("IDvalue_%d =%d\n",1,value_record.ID1_value);
    printf("IDvalue_%d =%d\n",2,value_record.ID2_value);
    printf("IDvalue_%d =%d\n",3,value_record.ID3_value);
    printf("result=%d\n",result);
    printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\__\n");

    return result;
}
/*set the traveltime_CMS display object's value */
void protocol_AD_cms_travel_time::setValueRecord(int num,int value)
{

    if(value>99||value<1)value=0;
    switch(num)
    {
    case 0:
        value_record.ID1_value=value;
        break;
    case 1:
        value_record.ID2_value=value;
        break;
    case 2:
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

        int lightQuantity=(mes.packetLength-12)/2;
        int ID_value[3]= {0};
        int error_flag=0;
        if(mes.packetLength>18)
        {
            error_flag=1;
            vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);
        }
        else
            for(int i=0; i<lightQuantity; i++)
            {
                ID_value[i]=mes.packet[10+2*i];
                if (((ID_value[i]<254)&&(ID_value[i]>99))||(ID_value[i]<1))
                {
                    error_flag=ID_value[i];
                    vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);
                    break;
                }
                else
                {
                    if(ID_value[i]==255) ID_value[i]=0;
                    setValueRecord(i,ID_value[i]);
                }
            };

        if(error_flag==0)
        {
            for(int i=value_record.lightQuantity; i<3; i++)setValueRecord(i,0);
            value_record.lightQuantity=lightQuantity;
            printf("ADID1=%d minutes ID2=%d minutes ID3=%d minutes quantity=%d\n"
                   ,value_record.ID1_value,
                   value_record.ID2_value,
                   value_record.ID3_value
                   ,value_record.lightQuantity);

            store_value(value_record);
            vReturnToCenterACK(0xAD,0x10);
        }
        else
        {
            sprintf(cTMP, "AD10 enter ERROR\n");
            smem.vWriteMsgToDOM(cTMP);
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
        if(ID1_value!=255)
        {
            if (ID1_value>99||ID1_value<0)
            {
                error_flag=ID1_value;
                vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);
            }
        }
        else value_record.ID1_value=ID1_value;
        if(ID2_value!=255)
        {
            if (ID2_value>99||ID2_value<0)
            {
                error_flag=ID2_value;
                vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);
            }
        }
        else value_record.ID2_value=ID2_value;
        if(ID3_value!=255)
        {
            if (ID3_value>99||ID3_value<0)
            {
                error_flag=ID3_value;
                vReturnToCenterNACK(0xAD,0x10,0x4,error_flag);
            }
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
void protocol_AD_cms_travel_time::revAPP_AD10_test_time_display_set(int x,int y,int z)
{
    try
    {
    char cTMP[256];
    sprintf(cTMP, "revAPP_AD10_test_time_display_set ID1=%d ID2=%d ID3=%d\n", x,y,z);
    smem.vWriteMsgToDOM(cTMP);

    smem.vWriteMsgToDOM("revAPP_AD10_test_time_display_set");
        BYTE data[2];
        data[0]=0x0f;
        data[1]=0x80;
        _AD10_test_time_display_set( x, y,z);
        writeJob.WritePhysicalOut(data,2,revAPP);

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
        int ID_value=0;
        printf("IDvalue_%d =%d\n",1,value_record.ID1_value);
        printf("IDvalue_%d =%d\n",2,value_record.ID2_value);
        printf("IDvalue_%d =%d\n",3,value_record.ID3_value);
        for(int i=0; i<value_record.lightQuantity; i++)
        {
            ID_value=getValueRecord(i);
            printf("IDvalue_%d =%d\n",i+1,getValueRecord(i));

            pack[2+2*i]=i+1;
            if (ID_value>99||ID_value<1)
                pack[3+2*i]=0xff;
            else pack[3+2*i]=ID_value;
        }
        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 2+2*(value_record.lightQuantity),true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);
//_intervalTimer.LAS_module_query_timer(0);
    }
    catch(...) {}
}
void protocol_AD_cms_travel_time::revAPP_AD40_time_display_query()
{
    try
    {
    smem.vWriteMsgToDOM("revAPP_AD40_time_display_query");
        printf("display value report\n");
        unsigned char pack[8];

        pack[0]=0x1f;
        pack[1]=0xAD;
        pack[2]=0xC0;
        int ID_value=0;
        printf("IDvalue_%d =%d\n",1,value_record.ID1_value);
        printf("IDvalue_%d =%d\n",2,value_record.ID2_value);
        printf("IDvalue_%d =%d\n",3,value_record.ID3_value);
        for(int i=0; i<3; i++)
        {
            ID_value=getValueRecord(i);
            printf("IDvalue_%d =%d\n",i+1,getValueRecord(i));
            if (ID_value>99||ID_value<1)
                pack[2+i+1]=0xff;
            else pack[2+i+1]=ID_value;
        }

        writeJob.WritePhysicalOut(pack, 6, revAPP);
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
        int ID_value=0;

        for(int i=0; i<value_record.lightQuantity; i++)
        {
            ID_value=getValueRecord(i);
            pack[2+2*i]=i+1;
            if (ID_value>99||ID_value<1)
                pack[3+2*i]=0xff;
            else pack[3+2*i]=ID_value;
        }
        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 2+2*value_record.lightQuantity,true);
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
        if(meg.packet[9]>4||meg.packet[9]<1)vReturnToCenterNACK(0xAD,0x11,0x4,0x1);
        else
        {

            if(meg.packetLength<14)
                value_record.interrrupt_time=30;
            else value_record.interrrupt_time=meg.packet[10];
            value_record.display_mode=meg.packet[9];
//save
            sprintf(cTMP, "interrupt time = %d minutes,switch button=%d", value_record.interrrupt_time, value_record.display_mode);
            smem.vWriteMsgToDOM(cTMP);
            store_value(value_record);
            vReturnToCenterACK(0xAD,0x11);
        }
    }
    catch(...) {}
}
void protocol_AD_cms_travel_time::revAPP_AD11_CMS_controler_interrupt_set(int interruptTime,int mode)
{
    try
    {
    char cTMP[256];
    sprintf(cTMP, "revAPP_AD11_CMS_controler_interrupt_set mode=%d interrupt_time=%d\n", mode,interruptTime);
    smem.vWriteMsgToDOM(cTMP);

        if(interruptTime>99||interruptTime<0)
            value_record.interrrupt_time=30;
        else value_record.interrrupt_time=interruptTime;
        value_record.display_mode=mode;
//save
        sprintf(cTMP, "interrupt time = %d minutes,switch button=%d", value_record.interrrupt_time, value_record.display_mode);
        smem.vWriteMsgToDOM(cTMP);
        store_value(value_record);
        BYTE data[2];
        data[0]=0x0f;
        data[1]=0x80;

        writeJob.WritePhysicalOut(data,2,revAPP);

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
        //  pack[2]=value_record.interrrupt_time;

        pack[2]=value_record.display_mode;
        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 3,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

    }
    catch(...) {}

}

void protocol_AD_cms_travel_time::revAPP_AD41_CMS_controler_interrupt_query()
{
    try
    {smem.vWriteMsgToDOM("revAPP_AD40_time_display_query");
        printf("display value report by revAPP\n");
        unsigned char pack[5];
        pack[0]=0x1f;
        pack[1]=0xAD;
        pack[2]=0xC1;
        pack[3]=value_record.interrrupt_time;
        pack[4]=value_record.display_mode;
        writeJob.WritePhysicalOut(pack,5,revAPP);
    }
    catch(...) {}

}


void protocol_AD_cms_travel_time::revAPP_CMS_Traveltime_switch(MESSAGEOK mes)
{
    try
    {

        if(mes.packet[1]==0xAD)
        {
            switch(mes.packet[2])
            {
            case 0x10:
                if(mes.packetLength==6)
                    revAPP_AD10_test_time_display_set(mes.packet[3],mes.packet[4],mes.packet[5]);
                break;

            case 0x40:
                if(mes.packetLength==3)
                    revAPP_AD40_time_display_query();
                break;
            case 0x11:
                if(mes.packetLength==5)
                    revAPP_AD11_CMS_controler_interrupt_set(mes.packet[3],mes.packet[4]);
                break;
            case 0x41:
                if(mes.packetLength==3)
                    revAPP_AD41_CMS_controler_interrupt_query();
                break;
            default:
                break;
            }

        }


    }
    catch(...) {}
}





