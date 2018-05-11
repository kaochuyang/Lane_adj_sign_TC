#include "protocol_8F_LAS.h"
#include "SMEM.h"
#include "PTRAFFIC92COMM.h"
#include "CTIMER.h"
protocol_8F_LAS::protocol_8F_LAS()
{
    //ctor
}

protocol_8F_LAS::~protocol_8F_LAS()
{
    //dtor
}


bool protocol_8F_LAS::DoWorkViaPTraffic92(MESSAGEOK mes)
{

    try
    {


        switch(mes.packet[8])
        {
        case 0x10:
            _8f10_commonday_set(mes);

            break;

        case 0x40:
            _8f40_commanday_query(mes.packet[9]);
            break;
        case 0x11:
            _8f11_specialday_set(mes);
            break;
        case 0x41:
            _8f41_specialday_query(mes.packet[9]);
            break;
        case 0x12:
            _8f12_brightness_set(mes.packet[9]);
            break;
        case 0x42:
            _8f42_brightness_query();
            break;
        case 0x14:
            _8f14_module_report_period_set(mes.packet[9]);
            break;
        case 0x44:
            _8f44_module_report_period_query();
            break;
        case 0x45:
            _8f45_module_query();
            break;
        case 0x16:
            _8f16_default_light_set(mes);
            break;
        case 0x46:
            _8f46_default_light_query();
            break;
        case 0x47:
            _8f47_light_query();
            break;
        case 0x18:
            _8f18_light_report_period_set(mes.packet[9]);
            break;
        case 0x48:
            _8f48_light_report_period_query();
            break;

        default:

            vReturnToCenterNACK(0x8f,mes.packet[8],0,0);
            break;
        }


    }
    catch(...) {}


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
        printf("8f45 module query\n");
        smem.junbo_LASC_object.do_query_module();
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
            pack[2*(ID-1)+2]=ID;

            if(smem.lane_adj_run_state[ID]==1)
                pack[2*(ID-1)+3]=data[ID];
            else pack[2*(ID-1)+3]=0xff;
        }


        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 18,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);


//_intervalTimer.LAS_module_query_timer(0);

    }
    catch(...) {}


}

void protocol_8F_LAS::_8f05_module_act_report()
{
    try
    {
        bool send_flag=true;
        printf("8f05 module act report\n");
        unsigned char pack[18];
        unsigned char *data;
        data=smem.junbo_LASC_object.report_module_state();
        pack[0]=0x8f;
        pack[1]=0x05;
        for(int ID=1; ID<9; ID++)
        {


            pack[2*(ID-1)+2]=ID;

            if(smem.lane_adj_run_state[ID]==1)
            {

                pack[2*(ID-1)+3]=data[ID];
                smem.LAS_link_err_count[ID]=0;

            }
            else
            {
                pack[2*(ID-1)+3]=0xff;
                smem.LAS_link_err_count[ID]++;
                if(smem.LAS_link_err_count[ID]==128)smem.LAS_link_err_count[ID]=2;
            }
        }
        for(int i=1; i<9; i++)
        {
            if(smem.LAS_link_err_count[i]==1)send_flag=false;//because send() will clear the link_check's memory,that cause 8f05 let the pack[]=0xff,but the pack maybe just not yet receive the light report.
            // In the sentence,I let it check twice whitch pack receive 0xff 2 times,it will be disconnetion.
        }

        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack,18,true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        if(send_flag)writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);


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
            smem.Lane_adj_memo_object.defaul_light[ID]=mes.packet[ID+8];


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
        pack[0]=0x8f;
        pack[1]=0xc6;
        for(int ID=1; ID<9; ID++)
        {
            pack[ID+1]=smem.Lane_adj_memo_object.defaul_light[ID];
        }


        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack, 10,true);
        _MsgOK.InnerOrOutWard = cOutWard;

        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);


    }
    catch(...) {}
}

void protocol_8F_LAS::read_LSA_segment_data()
{
    try
    {
        /*printf("read_LSA_segment_data\n");
            printf("read_LSA_segment_data\n");
            printf("read_LSA_segment_data\n");
            printf("read_LSA_segment_data\n");
            printf("read_LSA_segment_data\n");*/
        LAS_excute_info* p=NULL;
        FILE *pf=NULL;
        char filename[256]= {0};
        for(int segmenttype=1; segmenttype<=3; segmenttype++)
        {
            sprintf(filename,"/cct/Data/SETTING/LAS_8f10_setting_%d.bin",segmenttype);
            pf=fopen(filename,"r");
            if(pf!=NULL)
            {
                fread(&smem.LAS_segmenttype[segmenttype],sizeof(LAS_excute_info),1,pf);
                fclose(pf);
                p=&smem.LAS_segmenttype[segmenttype];
                printf("read LAS_8f10_setting_%d success\n",segmenttype);
                printf("read segmenttype=%d segmentcount=%d  lightcount=%d  weekcount=%d\n",segmenttype,p->segmentcount,p->lightcount,p->weekcount);


            }
            else
            {
                smem.vWriteMsgToDOM("read LAS_8f10_setting_%d error\n");
                printf("read LAS_8f10_setting_%d object error\n",segmenttype);
                default_LAS_object(&smem.LAS_segmenttype[segmenttype]);

            }
        }
        pf=NULL;
        char filename2[256]= {0};
        for(int segmenttype=8; segmenttype<=20; segmenttype++)
        {
            sprintf(filename2,"/cct/Data/SETTING/LAS_8f11_setting_%d.bin",segmenttype);
            printf(filename2,"/cct/Data/SETTING/LAS_8f11_setting_%d.bin \n",segmenttype);
            printf("\n");
            pf=fopen(filename2,"r");
            if(pf!=NULL)
            {
                fread(&smem.LAS_segmenttype[segmenttype],sizeof(LAS_excute_info),1,pf);
                fclose(pf);

                printf("read LAS_8f11_setting_%d success\n",segmenttype);


            }
            else
            {
                smem.vWriteMsgToDOM("read LAS_8f10_setting_%d error\n");
                printf("read LAS_8f11_setting_%d object error\n",segmenttype);
                printf("\n");
                default_LAS_object(&smem.LAS_segmenttype[segmenttype]);

            }
        }
        pf=NULL;
        char filename3[256]= {0};

        sprintf(filename3,"/cct/Data/SETTING/segmenttype_setting.bin");
        printf(filename3,"/cct/Data/SETTING/segmenttype_setting.bin ");
        printf("\n");
        pf=fopen(filename3,"r");
        if(pf!=NULL)
        {
            fread(&smem.LAS_week_type_info,sizeof(int)*8,1,pf);
            fclose(pf);

            printf("read LAS_week_type_info success\n");


        }
        else
        {
            smem.vWriteMsgToDOM("read LAS_week_type_info error\n");
            printf("read LAS_week_type_info error\n");
            for(int i=0; i<7; i++)
                smem.LAS_week_type_info[i]=1;

        }


    }
    catch(...) {}
}

void protocol_8F_LAS::default_LAS_object(LAS_excute_info *pf)
{
    try
    {


        /*
          BYTE segmentcount;
            BYTE lightcount;
            BYTE light_select[9][24];//ID plan
            bool check_ID[9];
            int  hour[24];
            int  min[24];
            BYTE weekcount;
        */
        pf->segmentcount=1;
        pf->weekcount=7;
        pf->lightcount=2;
        pf->check_ID[1]=1;
        pf->check_ID[2]=1;
        for(int ID=3; ID<9; ID++)
            pf->check_ID[ID]=0;
        pf->light_select[1][1]=2;
        pf->light_select[2][1]=1;
        pf->hour[1]=0;
        pf->min[1]=0;


    }
    catch(...) {}
}

void protocol_8F_LAS::_8f10_commonday_set(MESSAGEOK mes)
{



    try
    {
        bool error_mark=false;
        int ID=0;
        printf("8f10 commonday set\n");

        int typeID=mes.packet[1+8];
        int error_count=13,error_count_record=0;
        LAS_excute_info* pf;

        if(typeID<1||typeID>7)
        {
            (0x8f,0x10,0x4,0x1);
            error_mark=true;
        }
        else smem.LAS_segmenttype[typeID].segmentcount=mes.packet[2+8];

        pf=&smem.LAS_segmenttype[typeID];


        int lightcount;
        int segmentcount=mes.packet[2+8];

        pf->lightcount=lightcount=mes.packet[3+8];
        pf->hour[0]=0;
        pf->min[0]=0;
        int light_series=2+2*lightcount;
        for(int i=1; i<segmentcount+1; i++)
        {

            pf->hour[i]=mes.packet[3+8+(i-1)*light_series+1];
            pf->min[i]=mes.packet[3+8+(i-1)*light_series+2];
            error_count=error_count+2;
            if((pf->hour[i]*3600+pf->min[i]*60)<(pf->hour[i-1]*3600+pf->min[i-1]*60))
            {
                error_mark=true;
                error_count_record=error_count;
            }

            for(int j=0; j<lightcount; j++)
            {
                ID=mes.packet[3+8+(i-1)*light_series+4+2*j];
                pf->check_ID[ID]=true;
                pf->light_select[ID][i]=mes.packet[3+8+(i-1)*light_series+3+2*j];//ID , plan
            }
        }
        int numweek;
        pf->weekcount=numweek=mes.packet[3+8+light_series*segmentcount+1];//numweek locate=4+4*lightcount+1
        printf("error_mark=%d\n",error_mark);
        for(int i=0; i<numweek; i++)
        {

            switch (mes.packet[3+8+light_series*segmentcount+2+i])
            {
                printf("weekday=%d  segment=%d\n",mes.packet[3+8+light_series*segmentcount+2+i],typeID);
            case 1:
                smem.LAS_week_type_info[1]=typeID;

                break;

            case 2:
                smem.LAS_week_type_info[2]=typeID;

                break;

            case 3:
                smem.LAS_week_type_info[3]=typeID;
                break;

            case 4:
                smem.LAS_week_type_info[4]=typeID;
                break;

            case 5:
                smem.LAS_week_type_info[5]=typeID;
                break;

            case 6:
                smem.LAS_week_type_info[6]=typeID;
                break;


            case 7:
                smem.LAS_week_type_info[0]=typeID;//sunday
                break;


            default:
                vReturnToCenterNACK(0x8f,0x10,0x4,3+light_series*segmentcount+i);
                error_mark=true;
                printf("error happen 8f 10 \n");
                break;
            }
        }


        if(error_mark==false)
        {


            FILE *pf1=NULL;
            FILE *pf2=NULL;
            char filename[256]= {0};
            char filename2[256]= {0};
            sprintf(filename,"/cct/Data/SETTING/LAS_8f10_setting_%d.bin",typeID);

            sprintf(filename2,"/cct/Data/SETTING/segmenttype_setting.bin");

            pf2=fopen(filename2,"w+");
            if(pf2!=NULL)
            {

                fwrite(&smem.LAS_week_type_info,sizeof(int)*7,1,pf2);
                fclose(pf2);
                printf("write segmenttype 8f10 success!.\n");
                smem.vWriteMsgToDOM("write segmenttype 8f10 success\n");


            }
            else
            {
                smem.vWriteMsgToDOM("write segmenttype 8f10 error\n");
                printf("write segmenttype 8f10 no file\n");

            };

            pf1=fopen(filename,"w+");
            if(pf1!=NULL)
            {
                //
                fwrite(pf,sizeof(LAS_excute_info),1,pf1);
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
        else if(error_mark==true)
        {

            smem.junbo_LASC_object.read_lane_adj_setting(&smem.Lane_adj_memo_object);
            printf("init read_lane_adj_setting\n");
            smem.protocol_8f_object.read_LAS_report_object();
            printf("init read_LAS_report_object\n");

            smem.protocol_8f_object.read_LSA_segment_data();
            printf("init read_LSA_segment_data\n");
            vReturnToCenterNACK(0x8f,0x10,0x4,error_count_record);



        }
        smem.junbo_LASC_object.link_ID_check();
        smem.junbo_LASC_object.determind_weekday_specialday();
        for(int ID=1; ID<9; ID++)
            printf("checkID %d=%d\n",ID,smem.LAS_segmenttype[typeID].check_ID[ID]);
        printf("lightcount=%d\n",smem.LAS_segmenttype[typeID].lightcount);
        printf("segmentcount=%d\n",smem.LAS_segmenttype[typeID].segmentcount);
        printf("weekcount=%d\n",smem.LAS_segmenttype[typeID].weekcount);

        printf("SEGMENTTYPE =%d\n",typeID);
        for(int i=0; i<7; i++)
            printf("weekday%d=%d_segmenttype\n",i,smem.LAS_week_type_info[i]);

    }
    catch(...) {}




}



void protocol_8F_LAS::_8f40_commanday_query(int segmenttype)
{
    try
    {

        printf("8f40 commanday query\n");

        if(segmenttype>3||segmenttype<1)vReturnToCenterNACK(0x8f,0x40,0x4,0x1);


        vReturnToCenterACK(0x8f,0x40);

        _8fc0_commonday_report(segmenttype);




    }
    catch(...) {}
}

void protocol_8F_LAS::_8fc0_commonday_report(int segmenttype)
{

    try
    {



        LAS_excute_info* pf_o=NULL;

        pf_o=&smem.LAS_segmenttype[segmenttype];
        printf("segmntcount=%d,\n",pf_o->segmentcount);
        printf("lightcount=%d,\n",pf_o->lightcount);


        FILE *pf=NULL;
        char filename[256]= {0};
        sprintf(filename,"/cct/Data/SETTING/LAS_8f10_setting_%d.bin",segmenttype);
        pf=fopen(filename,"r");
        if(pf!=NULL)
        {
            fread(pf_o,sizeof(LAS_excute_info),1,pf);
            fclose(pf);
            smem.vWriteMsgToDOM("read LAS_8f10_setting success\n");
            printf("read LAS_8f10_setting success\n");


        }
        else
        {
            smem.vWriteMsgToDOM("read LAS_8f10_setting error\n");
            printf("read LAS_8f10_setting object error\n");


        }

        BYTE pack[374];//pack max=5+20*(2+2*8)+1+7=373

        pack[0]=0x8f;
        pack[1]=0xc0;
        pack[2]=segmenttype;

        pack[3]=smem.LAS_segmenttype[segmenttype].segmentcount;//pf_o->segmentcount;//plan count max

        pack[4]=pf_o->lightcount;


        int light_series=2+2*pack[4];// hour+min+(light+ID)*lightcount


        for(int i=1; i<pack[3]+1; i++)
        {
            pack[4+(i-1)*light_series+1]=pf_o->hour[i];
            pack[4+(i-1)*light_series+2]=pf_o->min[i];

            for(int j=0; j<pf_o->lightcount+1; j++)
            {


                pack[4+(i-1)*light_series+3+2*j]=pf_o->light_select[j+1][i];//ID plan
                pack[4+(i-1)*light_series+4+2*j]=j+1;//ID




            }





        }


        int z=0;
        pack[4+light_series*pf_o->segmentcount+1]=pf_o->weekcount;//numweek;
        for(int i=1; i<7; i++)
        {
            if(smem.LAS_week_type_info[i]==segmenttype)
            {
                z++;// the last z shuld be equal to weekcount=numweek

                pack[4+light_series*pf_o->segmentcount+1+z]=i;
            }

        }
        if(smem.LAS_week_type_info[0]==segmenttype)
        {


            z++;// the last z shuld be equal to weekcount=numweek

            pack[4+light_series*pf_o->segmentcount+1+z]=7;
        }



        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack,4+light_series*pf_o->segmentcount+1+z+1 ,true);
        _MsgOK.InnerOrOutWard = cOutWard;

        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);




    }
    catch(...) {}

}





void protocol_8F_LAS::_8f11_specialday_set(MESSAGEOK mes)
{

    try
    {
        bool error_mark=false;
        int ID=0;
        printf("8f11 specialday set\n");
        int error_count=13,error_count_record=0;
        int typeID=mes.packet[3+6];
        printf("mark %d,",0);
        LAS_excute_info* pf=NULL;
        pf=&smem.LAS_segmenttype[typeID];
        pf->hour[0]=0;
        pf->min[0]=0;
        if(typeID<7||typeID>20)
        {
            vReturnToCenterNACK(0x8f,0x11,0x4,0x1);
            error_mark=true;
        }

        printf("mark %d,",1);

        printf("segmenttype=%d\n",typeID);

        int lightcount=0;
        int segmentcount=mes.packet[4+6];
        printf("segmentcount=%d\n",segmentcount);
        pf->lightcount=lightcount=mes.packet[5+6];
        int light_series=2+2*lightcount;
        for(int i=1; i<segmentcount+1; i++)
        {

            pf->hour[i]=mes.packet[3+8+(i-1)*light_series+1];
            pf->min[i]=mes.packet[3+8+(i-1)*light_series+2];
            error_count=error_count+2;
            if((pf->hour[i]*3600+pf->min[i]*60)<(pf->hour[i-1]*3600+pf->min[i-1]*60))
            {
                error_mark=true;
                error_count_record=error_count;
            }
            for(int j=0; j<lightcount; j++)
            {
                ID=mes.packet[3+8+(i-1)*light_series+4+2*j];
                pf->check_ID[ID]=true;
                pf->light_select[ID][i]=mes.packet[3+8+(i-1)*light_series+3+2*j];//ID , plan
            }
        }
        printf("mark %d,",2);
        smem.specialtype[typeID].start_year=mes.packet[5+6+light_series*segmentcount+1];
        smem.specialtype[typeID].start_month=mes.packet[5+6+light_series*segmentcount+2];
        smem.specialtype[typeID].start_day=mes.packet[5+6+light_series*segmentcount+3];
        smem.specialtype[typeID].end_year=mes.packet[5+6+light_series*segmentcount+4];
        smem.specialtype[typeID].end_month=mes.packet[5+6+light_series*segmentcount+5];
        smem.specialtype[typeID].end_day=mes.packet[5+6+light_series*segmentcount+6];
        printf("\n\n");
        printf("%d %d %d %d %d %d\n",smem.specialtype[typeID].start_year,smem.specialtype[typeID].start_month,smem.specialtype[typeID].start_day,smem.specialtype[typeID].end_year,
               smem.specialtype[typeID].end_month,smem.specialtype[typeID].end_day);
        for(int i=1; i<7; i++)
            printf("%d",mes.packet[4+6+light_series*segmentcount+i]);
        printf("\n\n");
        printf("mark %d,",3);

        if(error_mark==false)
        {
            printf("mark %d,",4);

            FILE *pf1=NULL;
            FILE *pf2=NULL;
            char filename[256]= {0};
            char filename2[256]= {0};
            sprintf(filename,"/cct/Data/SETTING/LAS_8f11_setting_%d.bin",typeID);

            sprintf(filename2,"/cct/Data/SETTING/special_type_setting.bin");
//////////////////////////////////////////special segmenttype/////////////////////////////////////////////////////////
            pf2=fopen(filename2,"w+");
            if(pf2!=NULL)
            {

                fwrite(&smem.specialtype,sizeof(LAS_YMD),1,pf2);
                fclose(pf2);
                printf("write special_type_setting 8f11 success!.");
                smem.vWriteMsgToDOM("write special_type_setting 8f11 success\n");


            }
            else
            {
                smem.vWriteMsgToDOM("write special_type_setting 8f11 error\n");
                printf("write special_type_setting 8f11 no file\n");

            };
///////////////////////////////////////////////////////////////////////////////
            pf1=fopen(filename,"w+");
            if(pf1!=NULL)
            {

                fwrite(pf,sizeof(LAS_excute_info),1,pf1);
                fclose(pf1);
                printf("write LAS_excute_info success!.");
                smem.vWriteMsgToDOM("write LAS_excute_info success\n");


            }
            else
            {
                smem.vWriteMsgToDOM("write LAS_excute_info error\n");
                printf("write LAS_excute_info no file\n");

            };
            vReturnToCenterACK(0x8f,0x11);

        }
        else if(error_mark==true)
        {

            smem.junbo_LASC_object.read_lane_adj_setting(&smem.Lane_adj_memo_object);
            printf("init read_lane_adj_setting\n");
            smem.protocol_8f_object.read_LAS_report_object();
            printf("init read_LAS_report_object\n");

            smem.protocol_8f_object.read_LSA_segment_data();
            printf("init read_LSA_segment_data\n");

            vReturnToCenterNACK(0x8f,0x10,0x4,error_count_record);

        }
        smem.junbo_LASC_object.link_ID_check();

        smem.junbo_LASC_object.determind_weekday_specialday();



    }
    catch(...) {}




}

void protocol_8F_LAS::_8f41_specialday_query(BYTE segmenttype)
{
    try
    {

        printf("8f41 commanday query\n");

        if(segmenttype>20||segmenttype<7)vReturnToCenterNACK(0x8f,0x41,0x4,0x1);
        else
        {

            vReturnToCenterACK(0x8f,0x41);

            _8fc1_specialday_report(segmenttype);
        }



    }
    catch(...) {}
}

void protocol_8F_LAS::_8fc1_specialday_report(BYTE segmenttype)
{
    try
    {





        LAS_excute_info *pf_o=NULL;
        pf_o=&smem.LAS_segmenttype[segmenttype];


        FILE *pf=NULL;
        char filename[256]= {0};
        sprintf(filename,"/cct/Data/SETTING/LAS_8f11_setting_%d.bin",segmenttype);
        pf=fopen(filename,"r");
        if(pf!=NULL)
        {
            fread(pf_o,sizeof(LAS_excute_info),1,pf);
            fclose(pf);
            smem.vWriteMsgToDOM("read LAS_8f10_setting success\n");
            printf("read LAS_8f10_setting success\n");


        }
        else
        {
            smem.vWriteMsgToDOM("read LAS_8f10_setting error\n");
            printf("read LAS_8f10_setting object error\n");


        }

        BYTE pack[374];//pack max=5+20*(2+2*8)+1+7=373
        int segmentcount=0;
        pack[0]=0x8f;
        pack[1]=0xc1;
        pack[2]=segmenttype;
        pack[3]=segmentcount=pf_o->segmentcount;//plan count max
        pack[4]=pf_o->lightcount;


        int light_series=2+2*pack[4];


        for(int i=1; i<pack[3]+1; i++)
        {
            pack[4+(i-1)*light_series+1]=pf_o->hour[i];
            pack[4+(i-1)*light_series+2]=pf_o->min[i];

            for(int j=0; j<pf_o->lightcount+1; j++)
            {




                pack[4+(i-1)*light_series+3+2*j]=pf_o->light_select[j+1][i];//ID plan
                pack[4+(i-1)*light_series+4+2*j]=j+1;//ID




            }





        }


        pack[4+light_series*segmentcount+1]=smem.specialtype[segmenttype].start_year;
        pack[4+light_series*segmentcount+2]=smem.specialtype[segmenttype].start_month;
        pack[4+light_series*segmentcount+3]=smem.specialtype[segmenttype].start_day;
        pack[4+light_series*segmentcount+4]=smem.specialtype[segmenttype].end_year;
        pack[4+light_series*segmentcount+5]=smem.specialtype[segmenttype].end_month;
        pack[4+light_series*segmentcount+6]=smem.specialtype[segmenttype].end_day;


        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack,4+light_series*segmentcount+6+1,true);
        _MsgOK.InnerOrOutWard = cOutWard;

        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);






    }
    catch(...) {}



}


void protocol_8F_LAS::_8f47_light_query()
{
    try
    {
        printf("8f47 light query\n");

        vReturnToCenterACK(0x8f,0x47);
        _8fc7_light_report();


    }
    catch(...) {}
}

void protocol_8F_LAS::_8fc7_light_report()
{
    try
    {
        BYTE pack[19];
        int light_count=0;
        pack[0]=0x8f;
        pack[1]=0xc7;



        printf("8fc7 light report\n");

        for(int ID=1; ID<9; ID++)
        {
            if(smem.lane_adj_run_state[ID]==1)
            {
                light_count++;
                pack[2+2*(light_count-1)+1]=smem.lane_adj_light_record[ID].parameter[0] ;
                pack[2+2*(light_count-1)+2]=ID;

            }
        }
        pack[2]=light_count;




        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack,3+2*light_count,true);
        _MsgOK.InnerOrOutWard = cOutWard;

        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);







    }
    catch(...) {}
}



void protocol_8F_LAS::_8f07_light_act_report()
{

    try
    {


        BYTE pack[19];
        int light_count=0;
        pack[0]=0x8f;
        pack[1]=0x07;



        printf("8f07 light report\n");

        for(int ID=1; ID<9; ID++)
        {
            if(smem.lane_adj_run_state[ID]==1)
            {
                light_count++;
                pack[2+2*(light_count-1)+1]=smem.lane_adj_light_record[ID].parameter[0];
                pack[2+2*(light_count-1)+2]=ID;

            }
        }
        pack[2]=light_count;




        MESSAGEOK _MsgOK;

        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(pack,3+2*light_count,true);
        _MsgOK.InnerOrOutWard = cOutWard;

        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);






    }
    catch(...) {}


}

void  protocol_8F_LAS::read_LAS_report_object()
{
    try
    {

        FILE *pf=NULL;
        char filename[256]="/cct/Data/SETTING/LAS_report_object.bin";
        pf=fopen(filename,"r");
        if(pf!=NULL)
        {
            fread(&LAS_report_object,sizeof(report_memory),1,pf);
            fclose(pf);
            smem.vWriteMsgToDOM("read LAS_report_object success\n");
            printf("read LAS_report_object success\n");





        }
        else
        {
            smem.vWriteMsgToDOM("read LAS_report_object error\n");
            printf("read LAS_report_object error\n");
            LAS_report_object.light_report_second=60;
            LAS_report_object.module_report_hour=1;
            sprintf(filename,"/cct/Data/SETTING/LAS_report_object.bin");
            pf=fopen(filename,"w+");
            if(pf!=NULL)
            {

                fwrite(&LAS_report_object,sizeof(report_memory),1,pf);
                fclose(pf);
                printf("write LAS_report_object success!.");
                smem.vWriteMsgToDOM("write LAS_report_object success\n");


            }
            else
            {
                smem.vWriteMsgToDOM("write LAS_report_object error\n");
                printf("write LAS_report_object error\n");

            };

        };



    }
    catch(...) {}
}

void protocol_8F_LAS::store_LAS_report_object()
{

    try
    {

        printf("store_LAS_report_object\n");
        FILE *pf=NULL;
        char filename[256]= {0};
        sprintf(filename,"/cct/Data/SETTING/LAS_report_object.bin");
        pf=fopen(filename,"w+");
        if(pf!=NULL)
        {

            fwrite(&LAS_report_object,sizeof(report_memory),1,pf);
            fclose(pf);
            printf("write LAS_report_object success!.");
            smem.vWriteMsgToDOM("write LAS_report_object success\n");


        }
        else
        {
            smem.vWriteMsgToDOM("write LAS_report_object error\n");
            printf("write LAS_report_object error\n");

        };


    }
    catch(...) {}
}




void protocol_8F_LAS::_8f18_light_report_period_set(int report_cycle)
{
    try
    {


        if(report_cycle<1||report_cycle>60)vReturnToCenterNACK(0x8f,0x18,0x4,0x1);
        else
        {
            printf("_8f18_light_report_period_set second=%d\n",report_cycle);

            _intervalTimer.set_light_report_timer(report_cycle);
            LAS_report_object.light_report_second=report_cycle;

            vReturnToCenterACK(0x8f,0x18);
        }



    }
    catch(...)
    {
    }


}

void protocol_8F_LAS::_8f48_light_report_period_query()
{

    try
    {
        printf("_8f48_light_report_period_query\n");
        vReturnToCenterACK(0x8f,0x48);

        _8fc8_light_report_preriod_report();
    }
    catch(...) {}
}
void protocol_8F_LAS::_8fc8_light_report_preriod_report()
{
    try
    {

        printf("_8fc8_light_report_preriod_report second=%d\n",LAS_report_object.light_report_second);
        send_to_center_3(0x8f,0xc8,LAS_report_object.light_report_second);


    }
    catch(...) {}
}



void protocol_8F_LAS::_8f14_module_report_period_set(int cycle)
{
    try
    {

        if(cycle<0||cycle>24)vReturnToCenterNACK(0x8f,0x14,0x4,1);
        else
        {


            printf("_8f14_module_report_period_set hour=%d\n",cycle);

            _intervalTimer.set_module_report_timer(cycle);
            LAS_report_object.module_report_hour=cycle;

            vReturnToCenterACK(0x8f,0x14);
        };



    }
    catch(...) {    }


}

void protocol_8F_LAS::_8f44_module_report_period_query()
{

    try
    {
        printf("_8f44_module_report_period_query\n");
        vReturnToCenterACK(0x8f,0x44);

        _8fc4_module_report_period_report();
    }
    catch(...) {}
}
void protocol_8F_LAS::_8fc4_module_report_period_report()
{
    try
    {

        printf("_8fc4_module_report_period_report hour=%d\n",LAS_report_object.module_report_hour);
        send_to_center_3(0x8f,0xc4,LAS_report_object.module_report_hour);


    }
    catch(...) {}
}















