#include "junbo_lane_adj_light.h"

junbo_lane_adj_light::junbo_lane_adj_light()
{
    //ctor




}

junbo_lane_adj_light::~junbo_lane_adj_light()
{
    //dtor
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
    query_textID.command=0xc6;
    query_textID.parameter[0]=0x0;



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
            fwrite(input_string,3*sizeof(char),sizeof(char),pf);
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
        Action.ID=ID;
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

            /*
                        if((junbo_receive_packet[3]==0xb1)
                                ||(junbo_receive_packet[3]==0xb0)
                                ||(junbo_receive_packet[3]==0xb3))
                        {
                            ID=junbo_receive_packet[2];
                            smem.record_light[ID].ID=junbo_receive_packet[2];
                            smem.record_light[ID].command=junbo_receive_packet[3];//junbo_receive_packet[2]   I am ID
                            smem.record_light[ID].parameter=junbo_receive_packet[4];

                            if(smem.record_light[1].parameter==light_off.parameter&&smem.record_light[2].parameter==light_off.parameter)smem.protocol_9F_object.o_CMS_mannager._9f08_cms_off_report();
                        }
                        else if(junbo_receive_packet[3]==0xb2)
                        {
                            ID=junbo_receive_packet[2];
                            smem.record_timeout[ID].ID=junbo_receive_packet[2];
                            smem.record_timeout[ID].command=junbo_receive_packet[3];//junbo_receive_packet[2]   I am ID
                            smem.record_timeout[ID].parameter=junbo_receive_packet[4];
                        }
                        else if(junbo_receive_packet[3]==0xb5)
                        {
                            ID=junbo_receive_packet[2];
                            smem.record_brightness[ID].ID=junbo_receive_packet[2];
                            smem.record_brightness[ID].command=junbo_receive_packet[3];//junbo_receive_packet[2]   I am ID
                            smem.record_brightness[ID].parameter=junbo_receive_packet[4];
                        }
                        else if(junbo_receive_packet[3]==0xb4)
                        {
                            module_state_object.bit_block_ID=((junbo_receive_packet[4]>>4)&0xff);
                            smem.record_state[ID][module_state_object.bit_block_ID].ID=junbo_receive_packet[2];
                            smem.record_state[ID][module_state_object.bit_block_ID].command=junbo_receive_packet[3];//junbo_receive_packet[2]   I am ID
                            smem.record_state[ID][module_state_object.bit_block_ID].parameter=junbo_receive_packet[4];
                        }

            */






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

    }catch(...){}
}

void junbo_lane_adj_light::light_timeout_control(int control_parameter)
{
    try
    {

        light_timeout.parameter[0]=control_parameter;
     for(int i=0;i<8;i++)
     junbo_light_send_reference_select(i,light_timeout);


    }catch(...){}
}
void junbo_lane_adj_light::brightness_control(int bright_parameter)
{
    try
    {

     brightness.parameter[0]=control_parameter;
     for(int i=0;i<8;i++)
     junbo_light_send_reference_select(i,brightness);


    }catch(...){}
}
void junbo_lane_adj_light::module_query()
{
    try
    {




    }
    catch(...){}
}
