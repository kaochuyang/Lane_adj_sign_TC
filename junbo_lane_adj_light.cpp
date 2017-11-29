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

        if(smem.lane_adj_run_state[ID]==1)
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


            if((junbo_receive_packet[3]==0xb1)
                    ||(junbo_receive_packet[3]==0xb0)
              )
            {
                ID=junbo_receive_packet[3];
                smem.lane_adj_light_record[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_light_record[ID].command=junbo_receive_packet[4];
                smem.lane_adj_light_record[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_light_record[ID].parameter[1]=junbo_receive_packet[6];


            }
            else if(junbo_receive_packet[4]==0xb2)
            {
                ID=junbo_receive_packet[3];
                smem.lane_adj_timeout_record[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_timeout_record[ID].command=junbo_receive_packet[4];
                smem.lane_adj_timeout_record[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_timeout_record[ID].parameter[1]=junbo_receive_packet[6];
            }
            else if(junbo_receive_packet[4]==0xb5)
            {
                ID=junbo_receive_packet[3];
                smem.lane_adj_brightness_record[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_brightness_record[ID].command=junbo_receive_packet[4];
                smem.lane_adj_brightness_record[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_brightness_record[ID].parameter[1]=junbo_receive_packet[6];
            }
            else if(junbo_receive_packet[4]==0xb4)
            {
                ID=junbo_receive_packet[3];
                smem.lane_adj_module_state[ID].ID=junbo_receive_packet[3];//ID
                smem.lane_adj_module_state[ID].command=junbo_receive_packet[4];
                smem.lane_adj_module_state[ID].parameter[0]=junbo_receive_packet[5];
                smem.lane_adj_module_state[ID].parameter[1]=junbo_receive_packet[6];
            }

            else if(junbo_receive_packet[4]==0xb6)
            {

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
        memset(smem.lane_adj_run_state,-1,sizeof(smem.lane_adj_run_state));

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

        for(int i=0; i<8; i++)
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
        for(int i=0; i<8; i++)
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
        char filename[256]="/cct/Data/SETTING/cms_mark_object.bin";
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
