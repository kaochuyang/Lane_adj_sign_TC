//---------------------------------------------------------------------------
#pragma hdrstop

#include "var.h"
#include "SMEM.h"
#include "PARSETRAFFIC.h"
#include "PARSERTMS.h"
#include "PARSENMEA.h"
#include "PARSESS.h"

#include "READJOB.h"
#include "WRITEJOB.h"

#include "LCD240x128.h"
#include "DIGITALIO.h"

#include "CTIMER.h"
#include "CDataToMessageOK.h"

#include "SCREENLogo.h"

#include "STORAGE.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/*Combo++*/
#include "CSTC.h"
#include "CSTORINI.h"
/*Combo--*/
#include "PTRAFFIC92TC.h"
#include "PTRAFFIC92COMM.h"
//---------------------------------------------------------------------------
//void *timerHandle (void *);                                                     //¥t¤@­Óthread,±Mªù±±¨îTimer
//---------------------------------------------------------------------------
void SendRequestToKeypad(void)
{
    try
    {

        MESSAGEOK messageOut;
        messageOut.protocol=PROTOCOLKEYPAD;
        messageOut.packetLength=6;
        messageOut.cksStatus=true;
        messageOut.success=true;
        messageOut.ReadorWrite=cWRITE;

        messageOut.packet[0]=0xAA;
        messageOut.packet[1]=0xBB;
        messageOut.packet[2]=0x03;
        messageOut.packet[3]=0xAA;
        messageOut.packet[4]=0xCC;
        messageOut.packet[5]=0;
        for (int i=0; i<5; i++) messageOut.packet[5]^=messageOut.packet[i];

        writeJob.WriteWorkByMESSAGEOUT(messageOut);

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char* argv[])
{
    try
    {
        bool notexit=true;                                                          //while loop ¥Ã¤£Â÷¶}
        unsigned char ucTmp;

        int iFWYearTmp;
        int iFWMonthTmp;
        int iFWDayTmp;
        char cTMP[128];

        int rec=-1;                                                                 //Select Result
        fd_set readfs;                                                              //File Descriptor Set
        int maxport=0,tempmax=0;                                                    //maxnum file desciptor used
        struct timeval timeout;                                                     //°²¨Ï­Y¤z¬íselect³£¨S¦³¸ê®Æ¶i¨Ó,´N°µtimeout­n§@ªº¨Æ±¡
        int readSelectLength=0;                                                     //§PÂ_Åª¨ìªºªø«×

        PARSETRAFFIC parseAABB;                                                     //¸ÑªR©Ò¦³AABB¶}ÀYAACCµ²§Àªºª«¥ó
        PARSERTMS parseRtms;                                                        //¸ÑªRRTMSªºª«¥ó
        PARSESS parseSS;                                                            //¸ÑªRSmartSensorªºª«¥ó
        PARSENMEA parseNMEA;
        tsUDPMappingLCN _sUDPMappingTMP;
        tsUDPInfo _sUDPMappingInfoTMP;

        int iCom2Type;
        iCom2Type = smem.vGetINTData(Com2_TYPE);

        //remove tc reset file
        system("rm -rf /tmp/resetTCmyself.txt");
        system("sync");

        int iTmp;




        /*
          ³q°T°ð¤À§G

          RS232,422,485 ³q°T°ð
          /dev/ttyS0(RS232)         ¥æ³q±±¨î¤¤¤ß-GPRS MODEM-(9600 N 8 1)
          /dev/ttyS1(RS232)         Resrever
          /dev/ttyS2(RS232)         KEYPAD-(9600 N 8 1)
          /dev/ttyS3(RS232)         ¿OÂI±±¨î(9600 N 8 1)
          ??????????(RS485)         RTMS-(9600 N 8 1)

          UDP SOCKET ³q°T°ð
          192.168.1.101:6003        ¥æ³q±±¨î¤¤¤ß
          192.168.1.101:6789        «O¯d´ú¸Õ¥Î
          192.168.1.102:6003        ²{³õ´ú¸Õ¾÷


          IO ³q°T°ð
          LPT1  LCD(240x128)  ¦ì§}-0x378
          DGIO  Light         ¦ì§}-In:0x200 Out:0x208
        */


        //¶}±ÒRS232,422,485 ³q°T°ð
        //¶}±Ò¥æ³q±±¨î¤¤¤ß³q°T°ð
        /*
                ucTmp = smem.vGetUCData(TC_ReverseLane_Manual_Enable_Status);
                printf("smem.vGetUCData(TC_ReverseLane_Manual_Enable_Status):%d\n", ucTmp);

        */

        smem.junbo_LASC_object.initial_junbo_control("/dev/ttyS0");

        if (smem.lightPort.SetConnDevice(DEVICETRAFFICLIGHT))
            if ((tempmax=smem.lightPort.OpenRs232Port("/dev/ttyS3",9600,false))>0)
            {
                if (tempmax>maxport)  maxport=tempmax;
                printf("open TrafficLightPort-/dev/ttyS3 (fdValue:%d) Success!!\n",tempmax);
            }
            else printf("open LightPort Fail!!\n");


        int localPort1=smem.GetLocalIP1(5);
        int distPort0=smem.GetdistIp0(5);
        int distPort=smem.GetDistIP(5);

        char localIP1[20]= {0},distIP0[20]= {0},distIP[20]= {0};
        char tmp[5]= {0};

        for (int ip=1; ip<=4; ip++)
        {
            sprintf(tmp,"%d",smem.GetLocalIP1(ip));
            strcat(localIP1,tmp);
            if (ip<4) strcat(localIP1,".");
            sprintf(tmp,"%d",smem.GetdistIp0(ip));
            strcat(distIP0,tmp);
            if (ip<4) strcat(distIP0,".");
            sprintf(tmp,"%d",smem.GetDistIP(ip));
            strcat(distIP,tmp);
            if (ip<4) strcat(distIP,".");
        }


        printf("\nIP1 : %s:%d\n",localIP1,localPort1);
        printf("DISTIP0 : %s:%d\n",distIP0, distPort0);
        printf("DISTIP : %s:%d\n\n",distIP,distPort);


        //¶}±ÒUDP SOCKET ³q°T°ð
        //¶}±Ò»P¥æ³q±±¨î¤¤¤ß³q°TªºUDP SOCKET


        if (smem.centerSocket.SetConnDevice(DEVICECENTER92))
            if ((tempmax=smem.centerSocket.OpenUdpSocket(localIP1,localPort1,distIP0,distPort0))>0)
            {
                if (tempmax>maxport)  maxport=tempmax;
                printf("open CenterSocket-%s:%d (fdValue:%d) Success!!\n",localIP1,localPort1,tempmax);
            }
            else printf("open CenterSocket-%s:%d Fail!!\n",localIP1,localPort1);

   if (smem.centerSocket2.SetConnDevice(DEVICETESTER92))
    if ((tempmax=smem.centerSocket2.OpenUdpSocket(localIP1,localPort1+64,distIP,distPort))>0)  {
         if (tempmax>maxport)  maxport=tempmax;
         printf("open centerSocket2-%s:%d (fdValue:%d) Success!!\n",localIP1,localPort1+64,tempmax);
    } else printf("open centerSocket2-%s:%d Fail!!\n",localIP1,localPort1+64);

////-----------------------------------------------------------------------------------------------------//
       /* system("ifconfig eth1 192.168.2.1");//for VSX-6156
        if ( smem.revAPP_socket.SetConnDevice(1000))
            if ((tempmax= smem.revAPP_socket.OpenUdpSocket("192.168.2.1",7002,"192.168.2.2",7002))>0)
            {
                if (tempmax>maxport)  maxport=tempmax;
                printf("open rev_socket-%s:%d (fdValue:%d) Success!!\n","192.168.2.1",7002,tempmax);
            }
            else printf("open CenterSocket-%s:%d Fail!!\n","192.168.2.2",7002);*/
        PTRAFFIC92TC oRev_protocol_5F;
        PTRAFFIC92COMM oRev_protocol_0F;
        MESSAGEOK revAPP_messagein;
////-----------------------------------------------------------------------------------------------------//



        maxport=maxport+1;                                                          //important


        //¶}±ÒIO ³q°T°ð
        //LPT1(0x378) LCD­±ªO
        //±o¨ìkernalÅv­­,¥i¥H±±¨îLPT1
        if (lcd240x128.GetAuthority(0x378))                                         //±o¨ìkernalÅv­­,¥i¥H±±¨îLPT1
        {
            lcd240x128.GRAPHIC_MODE();                                              //±NLcd³]©w¬°Graphicµe­±
            //OTMARKPRINTF  printf("LCD GetKernalAuthority Successful!!\n");
        }
        else printf("LCD GetKernalAuthority Fail!!\n");

        //Digital Input Output
        if (digitalIO.GetAuthority(0x200,0x208))
        {
            //OTMARKPRINTF  printf("MotherBoard Digital I/O GetKernalAuthority Successful!!\n");
        }
        else printf("MotherBoard Digital I/O GetKernalAuthority Failure!!\n");

        screenLogo.DisplayLogo();                                                   //Åã¥ÜLogo­¶

        printf("Main 1 OK! \n");

        _intervalTimer.ThreadsGenerate();                                           //°_Interval Timer Thread

    stc.ThreadsGenerate();

        //OT Fix 950727   LCN0000
//   SendRequestToKeypad();                                                      //°Ýkeypad¥Ø«e­±ªO³]©w

        printf("Main 2 OK! \n");

        bzero(cTMP, 128);
        iFWYearTmp = smem.vGetFirmwareInfo_0FC3(0);
        iFWMonthTmp = smem.vGetFirmwareInfo_0FC3(1);
        iFWDayTmp = smem.vGetFirmwareInfo_0FC3(2);
        sprintf(cTMP, "TC Booting, FW Ver:%02d-%02d-%02d", iFWYearTmp, iFWMonthTmp, iFWDayTmp);
        smem.vWriteMsgToDOM(cTMP);

        while (notexit)
        {

            FD_ZERO(&readfs);

            if(smem.revAPP_socket.GetPortAlreadyOpen())FD_SET(smem.revAPP_socket.Getfd(),&readfs);//for revAPP

            if (smem.lightPort.GetPortAlreadyOpen()) FD_SET(smem.lightPort.Getfd(),&readfs);

            if (smem.centerSocket.GetPortAlreadyOpen()) FD_SET(smem.centerSocket.Getfd(),&readfs);
            if (smem.centerSocket2.GetPortAlreadyOpen()) FD_SET(smem.centerSocket2.Getfd(),&readfs);
            if(smem.junbo_LASC_object.junbo_lane_adj_port.GetPortAlreadyOpen())FD_SET(smem.junbo_LASC_object.junbo_lane_adj_port.Getfd(),&readfs);

            timeout.tv_sec=60;                                                      //timeout¬í¼Æ
            timeout.tv_usec=0;                                                      //³o­Ó¬O²@¬í,¼È¤£¨Ï¥Î

            rec=select(maxport,&readfs,NULL,NULL,&timeout);                         //wait block for read

            if (rec<0)                                                              //Select Error
            {

            }
            else if (rec==0)                                                        //Select Time Out
            {
                //OTMARKPRINTF  printf("Select Time Out!!\n");
            }
            else
            {


         if(smem.junbo_LASC_object.junbo_lane_adj_port.GetPortAlreadyOpen())
                { //printf("receive messgae LASC  111~~~~~~~~~~~\n");
                    if(FD_ISSET(smem.junbo_LASC_object.junbo_lane_adj_port.Getfd(),&readfs))
                    {     //printf("receive messgae LASC~~~~~~~~~~~\n");
                        readSelectLength=smem.junbo_LASC_object.junbo_lane_adj_port.Rs232Read();
                        if(readSelectLength>0)
                        {   // printf("receive messgae LASC  parse~~~~~~~~~~~\n");
                             /* for(int i =0; i<readSelectLength; i++)
                                  printf("%x ",smem.junbo_LASC_object.junbo_lane_adj_port.block[i]);

                              printf("\n\n");*/

                            smem.junbo_LASC_object.ParseBlock(readSelectLength,
                                                              smem.junbo_LASC_object.junbo_lane_adj_port.block,
                                                              smem.junbo_LASC_object.junbo_lane_adj_port.messageIn,
                                                         &smem.junbo_LASC_object.junbo_lane_adj_port.lastPacketIndex);

                        }
                    }

                }




                /*****************************************************************/
                if(smem.revAPP_socket.GetPortAlreadyOpen())
                {
                    if(FD_ISSET(smem.revAPP_socket.Getfd(),&readfs))
                    {
                        readSelectLength=smem.revAPP_socket.UdpRead();

                        printf("receive messgae REVAPP~~~~~~~~~~~\n");
                        if(readSelectLength>0)
                        {
                            //printf("readSelectLength=%d\n",readSelectLength);
                            if(smem.vGetCommEnable()==true)
                            {
                                int rev_select=0;
                                revAPP_messagein=smem.o_Junbo_light.revAPP_packet(readSelectLength,smem.revAPP_socket.block);
                                for(int g=0; g<revAPP_messagein.packetLength; g++)printf("%x ",revAPP_messagein.packet[g]);
                                printf("\n");


                                if(revAPP_messagein.packet[0]==0x5F)
                                {
                                    switch(revAPP_messagein.packet[1])
                                    {

                                    default:
                                        printf("wrong revAPP 5F protocol!!\n");
                                        break;
                                    }
                                }
                                else if(revAPP_messagein.packet[0]==0x0F)
                                {
                                    switch(revAPP_messagein.packet[1])
                                    {

                                           case (0x10):
                                        smem.vWriteMsgToDOM("vRebootIPC_0F10_revAPP by app");// 0f 10 52 52
                                        oRev_protocol_0F.vRebootIPC_0F10_revAPP(revAPP_messagein);
                                        break;

                                    case (0x4a):
                                        smem.vWriteMsgToDOM("WriteNetworkSetConfirm_RevAPP by app");
                                        smem.disk.WriteNetworkSetConfirm_RevAPP(revAPP_messagein);
                                        break;
                                    case (0x18):
                                        smem.vWriteMsgToDOM("dispalyLCN_NetworkIP_to_RevAPP by app");
                                        smem.disk.dispalyLCN_NetworkIP_to_RevAPP();
                                        break;
                                    case(0xc1):
                                        smem.vWriteMsgToDOM("vReportIPCTime_0FC2_revAPP by app");
                                        oRev_protocol_0F.vReportIPCTime_0FC2_revAPP();
                                        printf("hello\n\n\n");
                                        break;
                                    case(0x01):
                                        smem.vWriteMsgToDOM("check link state by app");
                                        if((revAPP_messagein.packet[2]==0x02)&&(revAPP_messagein.packet[3]=0x03))
                                            oRev_protocol_0F.check_link_revAPP();
                                        else printf("check link packet miss\n");

                                    default:
                                        smem.vWriteMsgToDOM("wrong revAPP 0F protocol!! by app");
                                        printf("wrong revAPP 0F protocol!!\n");
                                        break;
                                    }
                                }
                                memset(revAPP_messagein.packet,0x0,sizeof(revAPP_messagein.packet));
                            }
                        }
                    }
                }
                /********************************************************************/


                if (smem.centerSocket.GetPortAlreadyOpen())
                {
                    if (FD_ISSET(smem.centerSocket.Getfd(),&readfs))
                    {
                        readSelectLength=smem.centerSocket.UdpRead();
                        if (readSelectLength>0)
                        {
                            if(smem.vGetCommEnable() == true)
                            {
                               /* printf("receive message  =");
                                for(int i=0;i<readSelectLength;i++)printf("%x ",smem.centerSocket.block[i]);
                                printf("\n");*/                                //OT20110526
                                smem.vSetLastGetProtocolTime();

                                parseAABB.ParseBlock(readSelectLength,smem.centerSocket.block,smem.centerSocket.messageIn,&smem.centerSocket.lastPacketIndex,&smem.centerSocket.maxMessageIndex);
                                parseAABB.CheckSum(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                //enable vJudgeProtocolAndCheckLength
                                parseAABB.vJudgeProtocolAndCheckLength(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                parseAABB.DecideProtocol(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn,smem.centerSocket.GetConnDevice());
                                parseAABB.CheckReasonable(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                parseAABB.AssignLcn(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                readJob.SetInterfaceAndReadFlag(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn,cUDP);
                                readJob.vCheckIfBcastingForwardToUDP(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                readJob.CheckLcn(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                readJob.SetCenterComm(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                readJob.DoWorkByMESSAGEIN(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn);
                                parseAABB.EchoToGUI(&smem.centerSocket.maxMessageIndex,smem.centerSocket.messageIn,"192.168.1.101:6003");
                                parseAABB.MoveLastData(&smem.centerSocket.maxMessageIndex,&smem.centerSocket.lastPacketIndex,smem.centerSocket.messageIn);
                            }
                        }
                    }
                }
            if (smem.centerSocket2.GetPortAlreadyOpen()) {
                if (FD_ISSET(smem.centerSocket2.Getfd(),&readfs)) {
                    readSelectLength=smem.centerSocket2.UdpRead();
                    if (readSelectLength>0) {
                      if(smem.vGetCommEnable() == true){                       //OT20110526
                        smem.vSetLastGetProtocolTime();

                        parseAABB.ParseBlock(readSelectLength,smem.centerSocket2.block,smem.centerSocket2.messageIn,&smem.centerSocket2.lastPacketIndex,&smem.centerSocket2.maxMessageIndex);
                        parseAABB.CheckSum(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        //enable vJudgeProtocolAndCheckLength
                        parseAABB.vJudgeProtocolAndCheckLength(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        parseAABB.DecideProtocol(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn,smem.centerSocket2.GetConnDevice());
                        parseAABB.CheckReasonable(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        parseAABB.AssignLcn(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        readJob.SetInterfaceAndReadFlag(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn,cUDP);
                        readJob.vCheckIfBcastingForwardToUDP(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        readJob.CheckLcn(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        readJob.SetCenterComm(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        readJob.DoWorkByMESSAGEIN(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn);
                        parseAABB.EchoToGUI(&smem.centerSocket2.maxMessageIndex,smem.centerSocket2.messageIn,"192.168.1.102:6003");
                        parseAABB.MoveLastData(&smem.centerSocket2.maxMessageIndex,&smem.centerSocket2.lastPacketIndex,smem.centerSocket2.messageIn);
                      }
                    }
                }
            }

            }

        }

        //pthread_join(tidTimerHandle,NULL);                                          //µ²§ô¥t¤@­Óthread


        //Ãö³¬RS232,422,485 ³q°T°ð

        if (smem.centerSocket.CloseUdpSocket()) printf("Close CenterSocket Successful!!\n");
 if (smem.centerSocket2.CloseUdpSocket()) printf("Close centerSocket2 Successful!!\n");

        //Ãö³¬IO ³q°T°ð
        lcd240x128.ReleaseAuthority();
        digitalIO.ReleaseAuthority();


        return 0;

    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void *timerHandle (void *)
{
}
//---------------------------------------------------------------------------
