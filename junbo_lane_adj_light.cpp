#include "junbo_lane_adj_light.h"
#include "SMEM.h"
junbo_lane_adj_light::junbo_lane_adj_light()
{
    //ctor




}

junbo_lane_adj_light::~junbo_lane_adj_light()
{
    //dtor
}
void junbo_lane_adj_light::test_LAS_function(int command,int p)
{

    s_junbo_lane_adj *pf=NULL;

    switch(command)
    {
        case 0:
        if(p==0)pf=&light_off;
        if(p==1)pf=&light_on;
        break;

        case 1:
        if (p==0)pf=&light_black;
        if (p==1)pf=&straight;
        if (p==2)pf=&left;
        if (p==3)pf=&right;
        if (p==4)pf=&straight_left;
        if (p==5)pf=&straight_right;

        break;

        case 2:
        if(0<p&&p<100)
        light_timeout.parameter[0]=p;
        else light_timeout.parameter[0]=0;
        pf=&light_timeout;

        break;

        case 3:
        break;

        case 4:
            for(int ID=1;ID<9;ID++)
        {

          for(int i=0; i<4; i++)
                junbo_light_send_reference_select(ID,module_query[i]);}
        pf=&light_black;
        break;

        case 5:
             if(0<p&&p<5)
        brightness.parameter[0]=p;
        else p=0;
        pf=&brightness;

        break;




    }


    for(int ID=1;ID<9;ID++)
    smem.junbo_LASC_object.junbo_light_send_reference_select(ID,*pf);



}

void junbo_lane_adj_light::test_step()
{
    try
    {

//lcn=21951 zhuchon/wensin  lcn=21952  bright_6_rd/shen2   lcn21593=huko
        printf("test test test \n");
        static time_t now;
        static struct tm* currenttime;
        now = time(NULL);
        currenttime = localtime(&now);

        printf("Date: %4d/%2d/%2d  Time: %2d:%2d:%2d  week=%d\n "
               , currenttime->tm_year+1900,currenttime->tm_mon+1,currenttime->tm_mday
               , currenttime->tm_hour, currenttime->tm_min, currenttime->tm_sec,currenttime->tm_wday);


        int iCurrentSec = currenttime->tm_hour * 3600 + currenttime->tm_min * 60 + currenttime->tm_sec;
        int _7_before=7*3600-6;
        int _7_=7*3600;
        int _9_before=9*3600-6;
        int _9_=9*3600;
        int _858_before=8*3600*58*60-6;
        int _858_=8*3600*58*60;
        int _17_=17*3600;
        int _17_before=17*3600-6;
        int _1958_=19*3600+58*60;
        int _1958_before=19*3600+58*60-6;
        int _2028_before=20*3600+28*60-6;
        int _2028_=20*3600+28*60;

        int select=0;




        if(currenttime->tm_wday==6||currenttime->tm_wday==0)//weekend
        {
            printf("state 1\n");
            /*  junbo_light_send_reference_select(0x1,left);
              junbo_light_send_reference_select(0x2,straight);*/
            select=4;

        }
        else
        {



            if(smem.GetAddress()==21951)//wensin/zhuchon
            {



                if(iCurrentSec>=_7_before&&iCurrentSec<_7_)
                {
                    /*  junbo_light_send_reference_select(0x1,light_black);
                      junbo_light_send_reference_select(0x2,light_black);
                      */

                    select=1;
                }
                else if(iCurrentSec>=_7_&&iCurrentSec<_858_before)
                {

                    printf("state 2\n");

                    select=3;
                }
                else if(iCurrentSec>=_858_before&&iCurrentSec<_858_)
                {
                    /* junbo_light_send_reference_select(0x1,light_black);
                     junbo_light_send_reference_select(0x2,light_black);*/
                    select=1;

                }
                else if(iCurrentSec>=_17_before&&iCurrentSec<_17_)
                {
                    /* junbo_light_send_reference_select(0x1,light_black);
                     junbo_light_send_reference_select(0x2,light_black);*/
                    select=1;

                }
                else if(iCurrentSec>=_17_&&iCurrentSec<_1958_before)
                {

                    printf("state 2\n");

                    select=3;
                }
                else if(iCurrentSec>=_1958_before&&iCurrentSec<_1958_)
                {
                    /* junbo_light_send_reference_select(0x1,light_black);
                     junbo_light_send_reference_select(0x2,light_black);*/
                    select=1;

                }
                else
                {
                    printf("state 3\n");
                    /* junbo_light_send_reference_select(0x1,left);
                     junbo_light_send_reference_select(0x2,straight);*/
                    select=4;

                }


            }

            if(smem.GetAddress()==21952)//bright_6  shen_2
            {
                printf("bright_6  shen_2\n");

                if(iCurrentSec>=_7_before&&iCurrentSec<_7_)
                {
                    /*  junbo_light_send_reference_select(0x1,light_black);
                      junbo_light_send_reference_select(0x2,light_black);
                      */

                    select=1;
                }
                else if(iCurrentSec>=_7_&&iCurrentSec<_858_before)
                {

                    printf("state 2\n");
                    /* junbo_light_send_reference_select(0x1,left);
                     junbo_light_send_reference_select(0x2,left);*/
                    select=2;
                }
                else if(iCurrentSec>=_858_before&&iCurrentSec<_858_)
                {
                    /* junbo_light_send_reference_select(0x1,light_black);
                     junbo_light_send_reference_select(0x2,light_black);*/
                    select=1;

                }
                else if(iCurrentSec>=_17_before&&iCurrentSec<_17_)
                {
                    /* junbo_light_send_reference_select(0x1,light_black);
                     junbo_light_send_reference_select(0x2,light_black);*/
                    select=1;

                }
                else if(iCurrentSec>=_17_&&iCurrentSec<_2028_before)
                {

                    printf("state 2\n");
                    /* junbo_light_send_reference_select(0x1,left);
                     junbo_light_send_reference_select(0x2,left);*/
                    select=2;
                }
                else if(iCurrentSec>=_2028_before&&iCurrentSec<_2028_)
                {
                    /* junbo_light_send_reference_select(0x1,light_black);
                     junbo_light_send_reference_select(0x2,light_black);*/
                    select=1;

                }
                else
                {
                    printf("state 3\n");
                    /* junbo_light_send_reference_select(0x1,left);
                     junbo_light_send_reference_select(0x2,straight);*/
                    select=4;

                }
            }
            if(smem.GetAddress()==21953) //huko
            {
                printf("huko\n");

                if(iCurrentSec>=_7_before&&iCurrentSec<_7_)
                {


                    select=1;
                }
                else if(iCurrentSec>=_7_&&iCurrentSec<_9_before)
                {

                    printf("state 2\n");
                    /* junbo_light_send_reference_select(0x1,left);
                     junbo_light_send_reference_select(0x2,left);*/
                    select=2;
                }
                else if(iCurrentSec>=_9_before&&iCurrentSec<_9_)
                {
                    /* junbo_light_send_reference_select(0x1,light_black);
                     junbo_light_send_reference_select(0x2,light_black);*/
                    select=1;

                }

                else
                {
                    printf("state 3\n");
                    /* junbo_light_send_reference_select(0x1,left);
                     junbo_light_send_reference_select(0x2,straight);*/
                    select=4;

                }



            }//weekday

            else select=4;



        }

        switch (select)
        {
        case 1:
            junbo_light_send_reference_select(0x1,light_black);
            junbo_light_send_reference_select(0x2,light_black);

            break;
        case 2:
            junbo_light_send_reference_select(0x1,left);
            junbo_light_send_reference_select(0x2,left);

            break;
        case 3:
            junbo_light_send_reference_select(0x1,straight);
            junbo_light_send_reference_select(0x2,straight);

            break;
        case 4:
            junbo_light_send_reference_select(0x1,left);
            junbo_light_send_reference_select(0x2,straight);
            break;
        default:
            printf("error\n");
            break;
        };
    }
    catch(...) {}
}

void junbo_lane_adj_light::step_control(int segment_type)
{
    try
    {
        int count=0;
        printf("step control\n");
        time_t now;
        struct tm* currenttime;
        now = time(NULL);
        currenttime = localtime(&now);
        int current_sec=currenttime->tm_hour*3600+currenttime->tm_min*60;
        s_junbo_lane_adj* p_act=NULL;

        LAS_excute_info* pf_l=NULL;
        pf_l=&smem.LAS_segmenttype[segment_type];
        printf("segment count=%d\n",pf_l->segmentcount);
        for(int i=1; i<pf_l->segmentcount+1; i++)
        {
//printf("mark 1\n");


            if(current_sec>=(pf_l->hour[i]*3600+pf_l->min[i]*60-6&&current_sec<(pf_l->hour[i]*3600+pf_l->min[i]*60)))//transfer step
            {
                //printf("mark 2\n");
                for(int ID=1; ID<3; ID++)
                {
                    printf("check[%d]=%d\n",ID,pf_l->check_ID[ID]);
                    if(pf_l->check_ID[ID]==1)
                        junbo_light_send_reference_select(ID,straight_left);
                    count++;
                }
            }

            else if(current_sec>=(pf_l->hour[i]*3600+pf_l->min[i]*60&&current_sec<(pf_l->hour[i+1]*3600+pf_l->min[i+1]*60)))

            {
                //printf("mark 3\n");
                for(int ID=1; ID<9; ID++)
                {
                    switch(pf_l->light_select[ID][pf_l->segmentcount])
                    {
                    case 0:
                     if(pf_l->check_ID[ID]==1)
                   junbo_light_send_reference_select(ID,light_black);
                        p_act=&smem.junbo_LASC_object.light_black;
                        break;

                    case 1:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight);

                        break;

                    case 2:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,left);

                        break;

                    case 3:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,right);

                        break;

                    case 4:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight_left);

                        break;

                    case 5:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight_right);

                        break;


                    default:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight_left);

                        break;
                    }


                }
            }
            else
            {
                //printf("mark 4\n");
                for(int ID=1; ID<9; ID++)
                {  switch(pf_l->light_select[ID][pf_l->segmentcount])
                    {
                    case 0:
                     if(pf_l->check_ID[ID]==1)
                   junbo_light_send_reference_select(ID,light_black);
                        p_act=&smem.junbo_LASC_object.light_black;
                        break;

                    case 1:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight);

                        break;

                    case 2:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,left);

                        break;

                    case 3:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,right);

                        break;

                    case 4:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight_left);

                        break;

                    case 5:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight_right);

                        break;


                    default:
                     if(pf_l->check_ID[ID]==1)
                     junbo_light_send_reference_select(ID,straight_left);

                        break;
                    }
                    }

               } }





        // else printf("segment type error by step control\n");




        printf("count=%d\n",count);
    }
    catch(...)
    {

    }
}

void junbo_lane_adj_light::determind_weekday_specialday()
{
    try
    {

        time_t now;
        struct tm* currenttime;
        now = time(NULL);
        currenttime = localtime(&now);

        printf("Date: %4d/%2d/%2d  Time: %2d:%2d:%2d  week=%d\n"
               , currenttime->tm_year+1900,currenttime->tm_mon+1,currenttime->tm_mday
               , currenttime->tm_hour, currenttime->tm_min, currenttime->tm_sec,currenttime->tm_wday);


        bool special_flag=false;
        int segment_type=0;
        for(int i=4; i<21; i++)
        {

            /*  printf("smem.specialtype[%d].start_year: %d\n", i, smem.specialtype[i].start_year);
              printf("smem.specialtype[%d]._start_month: %d\n", i, smem.specialtype[i].start_month);
              printf("smem.specialtype[%d]._start_day: %d\n", i, smem.specialtype[i].start_day);
              printf("smem.specialtype[%d]._end_year: %d\n", i, smem.specialtype[i].end_year);
              printf("smem.specialtype[%d]._end_month: %d\n", i, smem.specialtype[i].end_month);
              printf("smem.specialtype[%d]._end_day: %d\n", i, smem.specialtype[i].end_day);
            */

            if(    (    ( (currenttime->tm_year-11) > smem.specialtype[i].start_year )
                        || ( (currenttime->tm_year-11)== smem.specialtype[i].start_year && (currenttime->tm_mon+1) > smem.specialtype[i].start_month )
                        || ( (currenttime->tm_year-11)== smem.specialtype[i].start_year && (currenttime->tm_mon+1)== smem.specialtype[i].start_month && (currenttime->tm_mday)>= smem.specialtype[i].start_day ) )
                    && (    ( (currenttime->tm_year-11) < smem.specialtype[i].end_year )
                            || ( (currenttime->tm_year-11)== smem.specialtype[i].end_year   && (currenttime->tm_mon+1) < smem.specialtype[i].end_month )
                            || ( (currenttime->tm_year-11)== smem.specialtype[i].end_year   && (currenttime->tm_mon+1)== smem.specialtype[i].end_month   && (currenttime->tm_mday)<= smem.specialtype[i].end_day ) ) )
            {
                printf(" go specialday!\n");
                special_flag=true;
                segment_type=i;
                break;
            }
        }
        if(special_flag==false)
        {

            segment_type=smem.LAS_week_type_info[currenttime->tm_wday];


        }
        printf("segment_type=%d\n",segment_type);
        if(segment_type==0)segment_type=1;
        smem.segmenttype_8f=segment_type;
       // step_control(smem.segmenttype_8f);

    }
    catch(...) {}
}

bool junbo_lane_adj_light::ParseBlock(int receiveBlockLength,BYTE *block,MESSAGEOK *messageIn,int *lastPacketIndex)
{
    /*printf("ParseBlock  junbo_lane_adj_light\n ");

     for(int i =0; i<messageIn->packetLength; i++)
                                     printf("%x ",messageIn->packet[i]);*/

    int i,j,k;

    i=0;                         //block[i]
    j=0;
    k=*lastPacketIndex;          //messageIn.packet[k]

    for (i=0; i<receiveBlockLength; i++)
    {
//      printf("_%C", messageIn[j].packet[k]=block[i]);

        if ( block[i] == 0xaa)
        {
            if((i>0)&&(messageIn[j].success==true))
            {
                junbo_light_receive(messageIn[j]);    //save log which is wrong message
                vClearMsg(messageIn,j);
            }

            messageIn[j].success=true;
            messageIn[j].packetLength = 8;

            k=0;
            messageIn[j].packet[k] = block[i];
            k++;

        }
        else
        {
            if(k<messageIn[j].packetLength-1)
            {
                messageIn[j].packet[k]=block[i];
                k++;
            }
            else if(k==messageIn[j].packetLength-1)
            {
                for(int a=0; a<messageIn[j].packetLength-1; a++)
                    messageIn[j].packet[k]^=messageIn[j].packet[a];

                if(messageIn[j].packet[k]==block[i])
                {
                    messageIn[j].cksStatus=true;
                    junbo_light_receive(messageIn[j]);    //save log
                    vClearMsg(messageIn,j);
                    k=0;
                }
                else
                {
                    junbo_light_receive(messageIn[j]);    //save log which is wrong message
                    vClearMsg(messageIn,j);
                    k=0;
                };

            }
        }
    }

    *lastPacketIndex=k;
    return true;
}
bool junbo_lane_adj_light::vClearMsg(MESSAGEOK *messageIn, unsigned short int msgID)
{
    messageIn[msgID].cksStatus = false;
    messageIn[msgID].success=false;
    messageIn[msgID].UDPorRS232=0;
    messageIn[msgID].ReadorWrite=0;
    messageIn[msgID].bGetDLE = false;
    messageIn[msgID].bGetSOH = false; //the same with STX
    messageIn[msgID].bGetACK = false;
    messageIn[msgID].bGetNAK = false;
    messageIn[msgID].uiPacketTEXTLEN = 65535;

    // messageIn[msgID].usiExternCounter = 0;;

    /*
    for (int b = 0; b < BUFFERSIZE; b++) {
      messageIn[msgID].packet[b]=0;
    }
    */
    bzero(messageIn[msgID].packet, BUFFERSIZE);
}





void junbo_lane_adj_light::initial_junbo_control(char *output_tty_name)
{
    tty_name=output_tty_name;


    if (junbo_lane_adj_port.OpenRs232Port(output_tty_name, 9600, false)>0)
    {
        printf("open lane_adj_sign_port %s  Success!!\n",output_tty_name);
    }
    else printf("open lane_adj_sign_port Fail!!\n");
    light_off.command=light_on.command=0xc0;
    light_on.parameter[0]=0x01;
    light_off.parameter[0]=0x0;
///////////////////////////////////////////////////////////////////////
    straight.command=left.command=right.command=straight_left.command=straight_right.command=0xc1;
    straight.parameter[0]=0x5;
    left.parameter[0]=0x1;
    right.parameter[0]=0x2;
    straight_left.parameter[0]=0x3;
    straight_right.parameter[0]=0x4;
///////////////////////////////////////////////////////
    for(int i=0; i<4; i++)
    {
        module_query[i].command=0xc4;
        module_query[i].parameter[0]=i;
    };
/////////////////////////////////////////////
    light_timeout.command=0xc2;
    light_timeout.parameter[0]=0x3c;
//////////////////////////////////////
    brightness.command=0xc5;
////////////////////////////////////
    query_state.command=0xc6;
    query_state.parameter[0]=0x0;

    left.command=right.command=straight.command=straight_left.command=straight_right.command=light_black.command=0xc1;
    light_black.parameter[0]=0x0;
    left.parameter[0]=0x2;
    straight.parameter[0]=0x1;
    right.parameter[0]=0x3;
    straight_left.parameter[0]=0x4;
    straight_right.parameter[0]=0x5;


}

junbo_packet junbo_lane_adj_light::junbo_Packeted(s_junbo_lane_adj Action)
{
    try
    {

        junbo_packet send_packet;

        send_packet.packet[0]=0xaa;
        send_packet.packet[1]=0xbb;
        send_packet.packet[2]=smem.GetSequence();
        send_packet.packet[3]=Action.ID;
        send_packet.packet[4]=Action.command;
        send_packet.packet[5]=Action.parameter[0];
        send_packet.packet[6]=0x0;
        send_packet.packet[7]=0x0;

        for(int i=0; i<7; i++)
        {
            send_packet.packet[7]^=send_packet.packet[i];
        }

        return send_packet;
    }
    catch(...) {}



}
void junbo_lane_adj_light::junbo_light_send(junbo_packet send)
{



    BYTE command_type;
    int ID=0;
    struct tm* currenttime;
    time_t now = time(NULL);
    currenttime = localtime(&now);
    int length=0;

    char cFileTmp[64]= {0};
    sprintf(cFileTmp, "/cct/Data/junborecord/");

    char dateTemp[32]= {0};
    sprintf(dateTemp, "%#04d%#02d%#02d", currenttime->tm_year+1900, currenttime->tm_mon+1, currenttime->tm_mday);

    strcat(cFileTmp,dateTemp);
    strcat(cFileTmp,"_send_junborecord.txt");
    char cTimeHeader[64]= {0};
    sprintf(cTimeHeader, " %#04d/%#02d/%#02d %#02d:%#02d:%#02d  \0", currenttime->tm_year+1900, currenttime->tm_mon+1, currenttime->tm_mday, currenttime->tm_hour, currenttime->tm_min, currenttime->tm_sec);

    for (int i=0; i<300; i++)
        if (cTimeHeader[i]=='\0')
        {
            length=i;
            break;
        }


    FILE *pf=NULL;

    printf("\n junbo_command=%2x parameter_1=%2x parameter_2=%2x\n",send.packet[4],send.packet[5],send.packet[6]);
    send.packet[7]=0x0;
    for(int i=0; i<7; i++)
    {
        send.packet[7]^=send.packet[i];
    }


    ID=send.packet[3];
    switch(send.packet[4])
    {
    case 0xc0:
    case 0xc1:
        clear_s_junbo_lane_adj(&smem.lane_adj_light_record[ID]);
        break;
    case 0xc2:
        clear_s_junbo_lane_adj(&smem.lane_adj_timeout_record[ID]);
        break;

    case 0xc4:
        clear_s_junbo_lane_adj(&smem.lane_adj_module_state[ID]);
        break;

    case 0xc5:
        clear_s_junbo_lane_adj(&smem.lane_adj_brightness_record[ID]);
        break;

    default:
        printf("wrong command to junbo lane_adj_light\n");
        break;
    }


    junbo_lane_adj_port.Rs232Write(send.packet,8,tty_name);//write out
    memset(input_string,'0',sizeof(input_string));
    pf=fopen(cFileTmp,"a+");
    if(pf!=NULL)
    {

        sprintf(input_string,"\n    Send junbo_protocol=");
        fwrite(input_string,sizeof(char),sizeof(input_string),pf);
        for(int i=0; i<8; i++)
        {
            sprintf(input_string," %x ",send.packet[i]);
           // fwrite(input_string,3*sizeof(char),sizeof(char),pf);
        }
        fwrite( cTimeHeader, length, 1, pf );
        fclose(pf);
    }
    else printf("pf=NULL!!\n");

    usleep(30000);
}

void junbo_lane_adj_light::junbo_light_send_reference_select(BYTE ID,s_junbo_lane_adj Action)
{

    try
    {
//printf("junbo_light_send_reference_select ");
//printf("ID=%d action=%x %d\n",ID,Action.command,Action.parameter[0]);


        Action.ID=ID;

        //  if(smem.lane_adj_run_state[ID]==1)
        junbo_light_send(junbo_Packeted(Action));

    }
    catch(...) {}

}

void junbo_lane_adj_light::junbo_light_receive(MESSAGEOK messageIn)//just for receive the junbo_cms return message
{
    struct tm* currenttime;
    time_t now = time(NULL);
    currenttime = localtime(&now);
    FILE *pf=NULL;
    int readSelectLength=0;
    char receive_string[25]= {};
    char cReadString[255];
    unsigned char wrongstring[300];
    char cFileTmp[64]= {0};
    sprintf(cFileTmp, "/cct/Data/junborecord/");
    char dateTemp[32]= {0};
    sprintf(dateTemp, "%#04d%#02d%#02d", currenttime->tm_year+1900, currenttime->tm_mon+1, currenttime->tm_mday);
    strcat(cFileTmp,dateTemp);
    strcat(cFileTmp,"_receive_junborecord.txt");
    char cTimeHeader[64]= {0};
    sprintf(cTimeHeader, "%#04d/%#02d/%#02d %#02d:%#02d:%#02d  \0", currenttime->tm_year+1900, currenttime->tm_mon+1, currenttime->tm_mday, currenttime->tm_hour, currenttime->tm_min, currenttime->tm_sec);
    int length=0;
    for (int i=0; i<300; i++)
        if (cTimeHeader[i]=='\0')
        {
            length=i;
            break;
        }
    int ID=0;
    try
    {
        pf=fopen(cFileTmp,"a+");
        for(int i=0; i<8; i++)junbo_receive_packet[i]=messageIn.packet[i];
        if(messageIn.cksStatus==true)
        {


            if((junbo_receive_packet[4]==0xb1)
                    ||(junbo_receive_packet[4]==0xb0)
              )
            {
                printf("receive lightID message\n");
                ID=junbo_receive_packet[3];
                smem.lane_adj_light_record[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_light_record[ID].command=junbo_receive_packet[4];
                smem.lane_adj_light_record[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_light_record[ID].parameter[1]=junbo_receive_packet[6];


            }
            else if(junbo_receive_packet[4]==0xb2)
            {
                printf("printf timeout messgae\n");
                ID=junbo_receive_packet[3];
                smem.lane_adj_timeout_record[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_timeout_record[ID].command=junbo_receive_packet[4];
                smem.lane_adj_timeout_record[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_timeout_record[ID].parameter[1]=junbo_receive_packet[6];
            }
            else if(junbo_receive_packet[4]==0xb5)
            {
                printf("receive brightness message\n");
                ID=junbo_receive_packet[3];
                smem.lane_adj_brightness_record[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_brightness_record[ID].command=junbo_receive_packet[4];
                smem.lane_adj_brightness_record[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_brightness_record[ID].parameter[1]=junbo_receive_packet[6];
            }
            else if(junbo_receive_packet[4]==0xb4)
            {
                printf("receive module message\n");
                ID=junbo_receive_packet[3];
                smem.lane_adj_module_state[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_module_state[ID].command=junbo_receive_packet[4];
                smem.lane_adj_module_state[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_module_state[ID].parameter[1]=junbo_receive_packet[6];
            }

            else if(junbo_receive_packet[4]==0xb6)
            {
                printf("receive check link message\n");

                ID=junbo_receive_packet[3];
                smem.lane_adj_run_state[ID]=1;
            }





            printf("Receive junbo_protocol=");
            for(int i=0; i<8; i++)printf("%x ",junbo_receive_packet[i]);
            if(pf!=NULL)
            {
                sprintf(cReadString,"%x %x %x %x %x %x %x %x\n",junbo_receive_packet[0],junbo_receive_packet[1],junbo_receive_packet[2],
                        junbo_receive_packet[3],junbo_receive_packet[4],junbo_receive_packet[5],junbo_receive_packet[6],junbo_receive_packet[7]);
                sprintf(receive_string,"\n Receive junbo_protocol=");
                fwrite(receive_string,sizeof(char),sizeof(receive_string),pf);
                fwrite(cReadString,3*sizeof(char),8,pf);
            }
            printf("\n");
        }
        else if(messageIn.cksStatus==false)//check the packet always right
        {
            if(pf!=NULL)
            {

                sprintf(receive_string,"wrong junbo_protocol=\n");
                fwrite(receive_string,sizeof(char),sizeof(receive_string),pf);
                sprintf(cReadString,"%x %x %x %x %x %x\n",junbo_receive_packet[0],junbo_receive_packet[1],junbo_receive_packet[2],
                        junbo_receive_packet[3],junbo_receive_packet[4],junbo_receive_packet[5],junbo_receive_packet[6],junbo_receive_packet[7]);
                fwrite(cReadString,3*sizeof(char),8,pf);
            }
        }

        fwrite( cTimeHeader, length, 1, pf );
        fclose(pf);
        memset(junbo_receive_packet,0,8);
        memset(cReadString,'\0',sizeof(cReadString));
        memset(wrongstring,'\0',sizeof(wrongstring));
    }
    catch(...) {}
}



void junbo_lane_adj_light::link_ID_check()
{
    try
    {
        memset(smem.lane_adj_run_state,0,sizeof(smem.lane_adj_run_state));

        for(int ID=1; ID<9; ID++)junbo_light_send_reference_select(ID,query_state);


    }
    catch(...) {}
}

void junbo_lane_adj_light::light_timeout_control(int control_parameter)
{
    try
    {

        light_timeout.parameter[0]=control_parameter;
        if(control_parameter>99||control_parameter<0)control_parameter=90;

        for(int i=1; i<9; i++)
            junbo_light_send_reference_select(i,light_timeout);


        smem.Lane_adj_memo_object.default_timeout=control_parameter;
        store_lane_adj_setting(smem.Lane_adj_memo_object);

    }
    catch(...) {}
}
void junbo_lane_adj_light::brightness_control(int bright_parameter)
{
    try
    {
        if(bright_parameter>4||bright_parameter<1)bright_parameter=1;
        brightness.parameter[0]=bright_parameter;
        for(int i=1; i<9; i++)
            junbo_light_send_reference_select(i,brightness);
        smem.Lane_adj_memo_object.brightness=bright_parameter;
        store_lane_adj_setting(smem.Lane_adj_memo_object);

    }
    catch(...) {}
}


void junbo_lane_adj_light::query_module_state()

{
    try
    {

        for(int ID=1; ID<9; ID++)
        {

            for(int i=0; i<4; i++)
                junbo_light_send_reference_select(ID,module_query[i]);

        }

    }
    catch(...) {}
}
BYTE *junbo_lane_adj_light::report_module_state()
{
    try
    {

        for(int ID=1; ID<9; ID++)
        {
            module_err[ID]=0;
            if((smem.lane_adj_module_state[ID].parameter[0]&0x8)!=0)module_err[ID]++;
            if((smem.lane_adj_module_state[ID].parameter[0]&0x4)!=0)module_err[ID]++;
            if((smem.lane_adj_module_state[ID].parameter[0]&0x2)!=0)module_err[ID]++;
            if((smem.lane_adj_module_state[ID].parameter[0]&0x1)!=0)module_err[ID]++;


        }

        return module_err;
    }
    catch(...) {}


}

void junbo_lane_adj_light::store_lane_adj_setting(junbo_lane_adj_memory_object object)
{
    try
    {
        printf("junbo_lane_adj_memory_object\n");
        FILE *pf=NULL;
        char filename[256]= {0};
        sprintf(filename,"/cct/Data/SETTING/junbo_lane_adj_setting.bin");
        pf=fopen(filename,"w+");
        if(pf!=NULL)
        {

            fwrite(&object,sizeof(junbo_lane_adj_memory_object),1,pf);
            fclose(pf);
            printf("write junbo_lane_adj_memory_object success!.");
            smem.vWriteMsgToDOM("write junbo_lane_adj_memory_object success\n");
            smem.Lane_adj_memo_object=object;

        }
        else
        {
            smem.vWriteMsgToDOM("write junbo_lane_adj_memory_object error\n");
            printf("write junbo_lane_adj_memory_object no file\n");

        };

    }
    catch(...) {}
}
void junbo_lane_adj_light::read_lane_adj_setting(junbo_lane_adj_memory_object *object)
{

    try
    {

        FILE *pf=NULL;
        char filename[256]="/cct/Data/SETTING/junbo_lane_adj_setting.bin";
        pf=fopen(filename,"r");
        if(pf!=NULL)
        {
            fread(&object,sizeof(junbo_lane_adj_memory_object),1,pf);
            fclose(pf);
            smem.vWriteMsgToDOM("read junbo_lane_adj_memory_object success\n");
            printf("read junbo_lane_adj_memory_object success\n");


        }
        else
        {
            smem.vWriteMsgToDOM("read junbo_lane_adj_memory_object error\n");
            printf("read junbo_lane_adj_memory_object object error\n");


        }
    }
    catch(...) {}

}

void junbo_lane_adj_light::clear_s_junbo_lane_adj(s_junbo_lane_adj *object)
{
    try
    {

        object->ID=0;
        object->command=0x0;
        object->parameter[0]=0x0;
        object->parameter[1]=0x0;

    }
    catch(...) {}
}
void junbo_lane_adj_light::delete_record_before_15day()
{
    try
    {


        struct tm* currenttime;
        time_t now = time(NULL);
        localtime(&now);
        now=now-1296000;
        currenttime = localtime(&now);
        char buf[256];
        buf[255] = '\0';
        char dateTemp[32]= {0};
        sprintf(dateTemp, "%#04d%#02d%#02d*", currenttime->tm_year+1900, currenttime->tm_mon+1, currenttime->tm_mday);
        if (snprintf(buf, 255, "rm /cct/Data/junborecord/%s",dateTemp ) != -1)
        {

            printf("%s",buf);
            system(buf);
        }
    }
    catch(...) {}



}


void junbo_lane_adj_light::auto_minus_bright()
{


    try
    {

        int i=1;
         struct tm* currenttime;
        time_t now = time(NULL);
        localtime(&now);


        switch(currenttime->tm_hour)
        {   case 6:
        case 23:
        case 22:
        case 21:
        case 20:
        case 19:


            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            i=4;
            break;

            case 18:
            case 17:
            case 16:
            i=3;
            break ;


            case 15:
            case 14:
            case 13:
            case 12:
            case 11:
            case 10:
            case 9:
            case 8:
            case 7:
            i=2;
            break;


            default:
            i=4;
            break;



        }


         brightness.parameter[0]=i;


         for(int i=1; i<9; i++)
            junbo_light_send_reference_select(i,brightness);
        smem.Lane_adj_memo_object.brightness=i;



    }
    catch(...)
    {

    }



}
