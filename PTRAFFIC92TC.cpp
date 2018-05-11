// vGet0FCommandSet
// 0: Basic (wireless)
// 1: Base
// 2: Basic & Advance
// 3: Basic & Advance & Option

//---------------------------------------------------------------------------
#include "PTRAFFIC92TC.h"

#include "CDataToMessageOK.h"
#include "WRITEJOB.h"
#include "CTIMER.h"
#include "DIGITALIO.h"
#include "CHexToInt.h"
#include "CSTC.h"
#include "CTools.h"

#include "SCREENLast92TCPlanSegmentUpdate.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

#include "CTIMER.h"

//---------------------------------------------------------------------------
PTRAFFIC92TC::PTRAFFIC92TC(void)
{
}
//---------------------------------------------------------------------------
PTRAFFIC92TC::~PTRAFFIC92TC(void)
{
}
//---------------------------------------------------------------------------
bool PTRAFFIC92TC::DoWorkViaPTraffic92(MESSAGEOK message)
{
    try
    {

        switch (message.packet[8])
        {
        case 0x10:
            vWriteControlStrategy5F10(message);
            break;

        case 0x40:
            vQurryControlStrategy5F40(message);
            break;

        case 0x11:
            vWriteReverseTimeData5F11(message);
            break;

        case 0x41:
            vQueryReverseTimeData5F41(message);
            break;

        case 0x12:
            vWriteReverseTimeData5F12(message);
            break;

        case 0x42:
            vQueryReverseTimeData5F42(message);
            break;

        case 0x14:
            vWritePlan5F14(message);
            break;

        case 0x44:
            vQueryPlan5F44(message);
            break;

        case 0x13:
            vWritePhase5F13(message);
            break;

        case 0x43:
            vQueryPhase5F43(message);
            break;

        case 0x15:
            vWritePlanDBMS5F15(message);
            break;

        case 0x45:
            vQueryPlanDBMS5F45(message);
            break;

        case 0x16:
            vWriteSegment5F16(message);
            break;

        case 0x46:
            vQuerySegment5F46(message);
            break;

        case 0x17:
            vWriteSpecialSegment5F17(message);
            break;

        case 0x47:
            vQueryHolidaySegment5F47(message);
            break;

        case 0x18:
            vWriteRunPlan5F18(message);
            break;

        case 0x48:
            vQueryRunPlan5F48(message);
            break;

        case 0x2F:
            vWritePhaseByStep5F2F(message);
            break;

//OT Debug 4010
        case 0x5F:
            vQueryPhase5F5F(message);
            break;


        case 0x3F:
            vTransmitCycleSetting_5F3F(message);
            break;

        case 0x6F:
            vTransmitCycleQuery_5F6F(message);
            break;

        case 0x1C:
            vGoToNextPhaseStepControl_5F1C(message);
            break;

        case 0x4C:
            vQueryPhaseStepControl_5F4C(message);
            break;

//主動回報
        case 0x00:
        case 0x03:
        case 0x0C:
        case 0x08:
        case 0x0A:
        case 0x0B:
        case 0x0F:
            writeJob.WritePhysicalOut(message.packet, message.packetLength, DEVICECENTER92);
            break;

        //OT20140415
        case 0x1A:
            vSetDynSegParameter(message);
            break;
        case 0x4A:
            vQueryDynSegParameter(message);
            break;
        case 0x1B:
            vSetDynSegSwitch(message);
            break;

        default:
            vReturnToCenterNACK(message.packet[7], message.packet[8], 0x0, 0x0);
            break;

        }

        return true;

    }
    catch (...) {}
}
//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWriteControlStrategy5F10(MESSAGEOK DataMessageIn)
{
    try
    {

        unsigned short int usiAmegidsDevLCN;
        DATA_Bit _ControlStrategy;
        int EffectTime;
        int iTmp;
        unsigned short int usiTmp;

        unsigned char data[32];
        MESSAGEOK _MsgOK;
        printf("[get TC 5F10]\n");

        int iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level

        if(iReturnCommandSet <= 1)
        {
            vReturnToCenterNACK(0x5F, 0x10, 0x80, 0x00);
            return false;
        } // not include level "A""
        if( DataMessageIn.packetLength < 14 )
        {
            vReturnToCenterNACK(0x5F, 0x10, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 14 )
        {
            vReturnToCenterNACK(0x5F, 0x10, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        _ControlStrategy.DBit = DataMessageIn.packet[9];
        EffectTime = DataMessageIn.packet[10] * 60;

        if(iReturnCommandSet <= 1)
        {
            vReturnToCenterNACK(0x5F, 0x10, 0x80, 0x00);    // not include level "A""
            return false;
        }
        if( DataMessageIn.packetLength < 14 )
        {
            vReturnToCenterNACK(0x5F, 0x10, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 14 )
        {
            vReturnToCenterNACK(0x5F, 0x10, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        if(1)    //not amegids mode.
        {

            printf("get TC 5F10, smem.vGetUCData(amegidsDynEnable) <= 0\n");
            int iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level
            vReturnToCenterACK(0x5F, 0x10);

            _ControlStrategy.DBit = DataMessageIn.packet[9];
            EffectTime = DataMessageIn.packet[10] * 60;                                   //EffectTime save using Sec, but protocal using Min.

            data[0] = 0x5F;
            data[1] = 0x91;
            data[2] = DataMessageIn.packet[9];
            data[3] = DataMessageIn.packet[10];

            smem.vSetUCData(TC92_ucControlStrategy, _ControlStrategy.DBit);
            smem.vSetINTData(TC92_iEffectTime, EffectTime);

            smem.vSet5F18EffectTime(EffectTime);

            smem.vSetINTData(TC92_iEffectTime, EffectTime);

            if(_ControlStrategy.switchBit.b1 == true &&
                    _ControlStrategy.switchBit.b5 == false &&
                    _ControlStrategy.switchBit.b6 == false    )
            {
//OT1000218        if( stc.Lock_to_LoadControlStrategy() == STRATEGY_ALLDYNAMIC )
//OT1000218          _intervalTimer.vAllDynamicToTODCount(10);

                if( stc.Lock_to_LoadControlStrategy() == STRATEGY_ALLDYNAMIC )   //OT20111020
                {
                    _intervalTimer.vAllDynamicToTODCount(10);
                }

                smem.vSet5F18EffectTime(2);
            }
            else if(_ControlStrategy.DBit == 0x30)
            {
                printf("Get Amegids 0F 10 30\n\n");
                if( stc.Lock_to_LoadControlStrategy() == STRATEGY_TOD )
                {
                    printf("Get Amegids, now is TOD\n\n");
//OT20111020
                    //OT20111107 stc.Lock_to_Set_Control_Strategy(STRATEGY_ALLDYNAMIC);
                    printf("Get Amegids, now changing to ALLDYNAMIC\n\n");

                    //OI20111107
                    _intervalTimer.vAllDynamicToTODCount(EffectTime);

                    /*
                              int usiCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);
                              if(usiCurrentSubphaseStep == 0) {
                                if(Protocal5F1CStopStep == usiCurrentSubphaseStep) {
                                  printf("Set Sec:%d, stepID:%d\n", EffectTime, usiCurrentSubphaseStep);
                                  _intervalTimer.vAllDynamicToTODCount(EffectTime);
                                }
                                else {
                                  usiTmp = stc.vGetUSIData(CSTC_exec_plan_green_time);
                                  printf("Set Sec:%d, StepID:%d\n", usiTmp, usiCurrentSubphaseStep);
                                  _intervalTimer.vAllDynamicToTODCount(usiTmp);
                                }
                              }
                              else if(usiCurrentSubphaseStep == 1) {
                                usiTmp = stc.vGetUSIData(CSTC_exec_plan_pedgreenflash_time);
                                printf("Set Sec:%d, StepID:%d\n", usiTmp, usiCurrentSubphaseStep);
                                _intervalTimer.vAllDynamicToTODCount(usiTmp);
                              }
                              else if(usiCurrentSubphaseStep == 2) {
                                if(Protocal5F1CStopStep == usiCurrentSubphaseStep) {
                                  printf("Set Sec:%d, StepID:%d, in Protocal5F1CStopStep\n", EffectTime, usiCurrentSubphaseStep);
                                  _intervalTimer.vAllDynamicToTODCount(EffectTime);
                                }
                                else {
                                  usiTmp = stc.vGetUSIData(CSTC_exec_plan_pedred_time);
                                  printf("Set Sec:%d, StepID:%d\n", usiTmp, usiCurrentSubphaseStep);
                                  _intervalTimer.vAllDynamicToTODCount(usiTmp);
                                }
                              }
                              else if(usiCurrentSubphaseStep == 3) {
                                usiTmp = stc.vGetUSIData(CSTC_exec_plan_yellow_time);
                                printf("Set Sec:%d, StepID:%d\n", usiTmp, usiCurrentSubphaseStep);
                                _intervalTimer.vAllDynamicToTODCount(usiTmp);
                              }
                              else if(usiCurrentSubphaseStep == 4) {
                                usiTmp = stc.vGetUSIData(CSTC_exec_plan_allred_time);
                                printf("Set Sec:%d, StepID:%d\n", usiTmp, usiCurrentSubphaseStep);
                                _intervalTimer.vAllDynamicToTODCount(usiTmp);
                              }
                    */

                }
            }



//OT1000218
            /* disable all

              //  _ControlStrategy.DBit = smem.vGetUCData(TC92_ucControlStrategy);              //Read Data from share memory
            //  EffectTime = smem.vGetINTData(TC92_iEffectTime);
                  printf("get TC 5F10, smem.vGetUCData(amegidsDynEnable) <= 0\n");
                  int iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level

                  vReturnToCenterACK(0x5F, 0x10);

                  _ControlStrategy.DBit = DataMessageIn.packet[9];
                  EffectTime = DataMessageIn.packet[10] * 60;                                   //EffectTime save using Sec, but protocal using Min.

                  data[0] = 0x5F;
                  data[1] = 0x91;
                  data[2] = DataMessageIn.packet[9];
                  data[3] = DataMessageIn.packet[10];

                  smem.vSetUCData(TC92_ucControlStrategy, _ControlStrategy.DBit);
                  smem.vSetINTData(TC92_iEffectTime, EffectTime);
                //then save!?

            //For disable Dynamic

                  if(MACHINELOCATE == MACHINELOCATEATYULIN)                                     // For CCJ
                  {

                    _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 4, true);
                    _MsgOK.InnerOrOutWard = cOutWard;
                    writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECCJDYNCTL);

                  } else {

                    if(_ControlStrategy.switchBit.b1 == true &&
                       _ControlStrategy.switchBit.b5 == false &&
                       _ControlStrategy.switchBit.b6 == false    )
                    {
                      if( stc.Lock_to_LoadControlStrategy() == STRATEGY_ALLDYNAMIC )
                        _intervalTimer.vAllDynamicToTODCount(10);
                //      _intervalTimer.vAllDynamicToTODCount(EffectTime);

                    }

                    else if(_ControlStrategy.DBit == 0x30)
                    {
                      printf("Get Amegids 0F 10 30\n\n");
                      if( stc.Lock_to_LoadControlStrategy() == STRATEGY_TOD ) {
                        printf("Get Amegids, now is TOD\n\n");
                        stc.Lock_to_Set_Control_Strategy(STRATEGY_ALLDYNAMIC);
                        printf("Get Amegids, now changing to ALLDYNAMIC\n\n");

                        int usiCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);
                        if(usiCurrentSubphaseStep == 0) {
                          if(Protocal5F1CStopStep == usiCurrentSubphaseStep) {
                            _intervalTimer.vAllDynamicToTODCount(EffectTime);
                          }
                          else {
                            _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_green_time));
                          }
                        }
                        else if(usiCurrentSubphaseStep == 1)
                          _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_pedgreenflash_time));
                        else if(usiCurrentSubphaseStep == 2)
                          if(Protocal5F1CStopStep == usiCurrentSubphaseStep) {
                            _intervalTimer.vAllDynamicToTODCount(EffectTime);
                          }
                          else {
                            _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_pedred_time));
                          }
                        else if(usiCurrentSubphaseStep == 3)
                          _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_yellow_time));
                        else if(usiCurrentSubphaseStep == 4)
                          _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_allred_time));

                      }
                    }
                  }
                  */


        }

        return true;

    }
    catch(...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQurryControlStrategy5F40(MESSAGEOK DataMessageIn)
{
    try
    {
        DATA_Bit _ControlStrategy;
        int EffectTime;
        int iReturnCommandSet;

        _ControlStrategy.DBit = smem.vGetUCData(TC92_ucControlStrategy);              //Read Data from share memory
        EffectTime = smem.vGetINTData(TC92_iEffectTime);
        iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level

        if(iReturnCommandSet <= 1)
        {
            vReturnToCenterNACK(0x5F, 0x40, 0x80, 0x00);    // not include level "A""
            return false;
        }
        if( DataMessageIn.packetLength < 12 )
        {
            vReturnToCenterNACK(0x5F, 0x40, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 12 )
        {
            vReturnToCenterNACK(0x5F, 0x40, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }
//Don't respond  vReturnToCenterACK(0x5F, 0x40);

        unsigned char data[6];
        data[0] = 0x5F;
        data[1] = 0xC0;
        data[2] = _ControlStrategy.DBit;
        data[3] = EffectTime / 60;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 4, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);
        return true;

    }
    catch(...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWriteReverseTimeData5F11(MESSAGEOK DataMessageIn)
{
    try
    {

        int iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level
        if(iReturnCommandSet <= 1)
        {
            vReturnToCenterNACK(0x5F, 0x11, 0x80, 0x00);    // not include level "A""
            return false;
        }

        // here have error, should check first.
        vReturnToCenterACK(0x5F, 0x11);



        int i=0;

//  stc.Lock_to_Reset_Segment_for_Center(DataMessageIn.packet[9],DataMessageIn.packet[10]);
//  stc.Lock_to_Load_WeekDaySegment_for_Center();

// need check first
        stc._for_center_reversetime.ucNonRevLight = 0;

        stc._for_center_reversetime.usiDirectIn    = DataMessageIn.packet[9];
        stc._for_center_reversetime.usiHourStartIn = DataMessageIn.packet[10];
        stc._for_center_reversetime.usiMinStartIn  = DataMessageIn.packet[11];
        stc._for_center_reversetime.usiHourEndIn   = DataMessageIn.packet[12];
        stc._for_center_reversetime.usiMinEndIn    = DataMessageIn.packet[13];

        stc._for_center_reversetime.usiDirectOut    = DataMessageIn.packet[14];
        stc._for_center_reversetime.usiHourStartOut = DataMessageIn.packet[15];
        stc._for_center_reversetime.usiMinStartOut  = DataMessageIn.packet[16];
        stc._for_center_reversetime.usiHourEndOut   = DataMessageIn.packet[17];
        stc._for_center_reversetime.usiMinEndOut    = DataMessageIn.packet[18];

        stc._for_center_reversetime.usiClearTime    = DataMessageIn.packet[19]*10;
        stc._for_center_reversetime.usiFlashGreen = DataMessageIn.packet[20]*10;
        stc._for_center_reversetime.usiGreenTime  = DataMessageIn.packet[21]*10;

        stc._for_center_reversetime._reverse_time_type  = DataMessageIn.packet[22];

        if(stc._for_center_reversetime.usiHourStartIn*60+stc._for_center_reversetime.usiMinStartIn != 0 || stc._for_center_reversetime.usiHourEndIn*60+stc._for_center_reversetime.usiMinEndIn != 0)
        {
            if(stc._for_center_reversetime.usiHourStartOut*60+stc._for_center_reversetime.usiMinStartOut == 0 && stc._for_center_reversetime.usiHourEndOut*60+stc._for_center_reversetime.usiMinEndOut == 0)
            {
                stc._for_center_reversetime.ucNonRevLight = 1;
            }
        }
        else if(stc._for_center_reversetime.usiHourStartOut*60+stc._for_center_reversetime.usiMinStartOut != 0 || stc._for_center_reversetime.usiHourEndOut*60+stc._for_center_reversetime.usiMinEndOut != 0)
        {
            if(stc._for_center_reversetime.usiHourStartIn*60+stc._for_center_reversetime.usiMinStartIn == 0 && stc._for_center_reversetime.usiHourEndIn*60+stc._for_center_reversetime.usiMinEndIn == 0)
            {
                stc._for_center_reversetime.ucNonRevLight = 2;
            }
        }
        else      //unknow type.
        {
            stc._for_center_reversetime.ucNonRevLight = 0;
        }


        stc.Lock_to_Save_ReverseTime_Step1(0);


//OTMARKPRINTF  printf("segment_type%d\n",stc._for_center_segment._segment_type);
//OTMARKPRINTF  printf("segment_Count%d\n",stc._for_center_segment._segment_count);


        int numWeekDay = DataMessageIn.packet[23];

//OTMARKPRINTF  printf("numWeekDay:%d\n",numWeekDay);

        for (int j=0; j<numWeekDay; j++)
        {

            //OTMARKPRINTF  printf("%3x--%d\n",DataMessageIn.packet[12+3*i+j],stc._for_center_segment._segment_type);

            switch (DataMessageIn.packet[24+j])
            {
            case 0x01:
                stc._for_center_weekdayrev[0]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x02:
                stc._for_center_weekdayrev[1]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x03:
                stc._for_center_weekdayrev[2]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x04:
                stc._for_center_weekdayrev[3]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x05:
                stc._for_center_weekdayrev[4]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x06:
                stc._for_center_weekdayrev[5]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x07:
                stc._for_center_weekdayrev[6]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x11:
                stc._for_center_weekdayrev[7]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x12:
                stc._for_center_weekdayrev[8]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x13:
                stc._for_center_weekdayrev[9]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x14:
                stc._for_center_weekdayrev[10]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x15:
                stc._for_center_weekdayrev[11]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x16:
                stc._for_center_weekdayrev[12]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x17:
                stc._for_center_weekdayrev[13]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            default:
                break;
            }
        }

//  stc.Lock_to_Save_Segment_from_Center();
        stc.Lock_to_Save_WeekDayReversetime_from_Center();

        return true;

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryReverseTimeData5F41(MESSAGEOK DataMessageIn)
{
    try
    {
        unsigned char data[256];

        unsigned char ucQueryReverseTimeType = DataMessageIn.packet[9];
        unsigned char ucWeekDay = DataMessageIn.packet[10];

        if( DataMessageIn.packetLength < 14)
        {
            vReturnToCenterNACK(0x5F, 0x41, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 14)
        {
            vReturnToCenterNACK(0x5F, 0x41, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        if(ucQueryReverseTimeType == 0xFF)
        {
            stc.Lock_to_Load_ReverseTime_Step1(2, ucWeekDay);
        }
        else
        {

            /*
            bSegStatus = smem.vGetTCPhasePlanSegTypeData(TC_SegType, iQuerySegmentType);
            if(bSegStatus == false) {
              vReturnToCenterNACK(0x5F, 0x46, 0x02, 0x0); return false;
            }
            */
            stc.Lock_to_Load_ReverseTime_Step1(0, ucQueryReverseTimeType);              //Zero for center
        }

        stc.Lock_to_Load_WeekDayReverseTime_for_Center();
        ;
        int iDataPtr;
        int iNumWeekDayPrt = 0;
        int iNumWeekDay = 0;
        data[0] = 0x5F;
        data[1] = 0xC1;
        data[2] = stc._for_center_reversetime.usiDirectIn;
        data[3] = stc._for_center_reversetime.usiHourStartIn;
        data[4] = stc._for_center_reversetime.usiMinStartIn;
        data[5] = stc._for_center_reversetime.usiHourEndIn;
        data[6] = stc._for_center_reversetime.usiMinEndIn;
        data[7] = stc._for_center_reversetime.usiDirectOut;
        data[8] = stc._for_center_reversetime.usiHourStartOut;
        data[9] = stc._for_center_reversetime.usiMinStartOut;
        data[10] = stc._for_center_reversetime.usiHourEndOut;
        data[11] = stc._for_center_reversetime.usiMinEndOut;

        data[12] = (stc._for_center_reversetime.usiClearTime / 10);
        data[13] = (stc._for_center_reversetime.usiFlashGreen / 10);
        data[14] = (stc._for_center_reversetime.usiGreenTime / 10);
        data[15] = stc._for_center_reversetime._reverse_time_type;

        iNumWeekDayPrt = 16;
        iDataPtr = 16;

        if(stc._for_center_weekdayrev[0]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x01;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[1]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x02;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[2]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x03;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[3]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x04;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[4]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x05;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[5]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x06;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[6]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x07;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[7]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x11;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[8]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x12;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[9]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x13;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[10]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x14;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[11]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x15;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[12]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x16;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[13]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x17;
            iNumWeekDay++;
            iDataPtr++;
        }
        data[iNumWeekDayPrt] = iNumWeekDay;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;
    }
    catch (...) {}
}


//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWriteReverseTimeData5F12(MESSAGEOK DataMessageIn)
{
    try
    {

        int iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level
        if(iReturnCommandSet <= 1)
        {
            vReturnToCenterNACK(0x5F, 0x12, 0x80, 0x00);    // not include level "A""
            return false;
        }

        // here have error, should check first.
        vReturnToCenterACK(0x5F, 0x12);


        int i=0;

//  stc.Lock_to_Reset_Segment_for_Center(DataMessageIn.packet[9],DataMessageIn.packet[10]);
//  stc.Lock_to_Load_WeekDaySegment_for_Center();

// need check first

        stc._for_center_reversetime.usiDirectIn    = DataMessageIn.packet[9];
        stc._for_center_reversetime.usiHourStartIn = DataMessageIn.packet[10];
        stc._for_center_reversetime.usiMinStartIn  = DataMessageIn.packet[11];
        stc._for_center_reversetime.usiHourEndIn   = DataMessageIn.packet[12];
        stc._for_center_reversetime.usiMinEndIn    = DataMessageIn.packet[13];

        stc._for_center_reversetime.usiDirectOut    = DataMessageIn.packet[14];
        stc._for_center_reversetime.usiHourStartOut = DataMessageIn.packet[15];
        stc._for_center_reversetime.usiMinStartOut  = DataMessageIn.packet[16];
        stc._for_center_reversetime.usiHourEndOut   = DataMessageIn.packet[17];
        stc._for_center_reversetime.usiMinEndOut    = DataMessageIn.packet[18];

        stc._for_center_reversetime.usiClearTime    = DataMessageIn.packet[19];
        stc._for_center_reversetime.usiFlashGreen = DataMessageIn.packet[20];
        stc._for_center_reversetime.usiGreenTime  = DataMessageIn.packet[21];

        stc._for_center_reversetime._reverse_time_type  = DataMessageIn.packet[22];

        stc.Lock_to_Save_ReverseTime_Step1(0);                                        // (0) means for center

        stc._for_center_holidayrev._reverse_time_type  = DataMessageIn.packet[22];
        stc._for_center_holidayrev._start_year  = DataMessageIn.packet[23] + 1911;
        stc._for_center_holidayrev._start_month = DataMessageIn.packet[24];
        stc._for_center_holidayrev._start_day   = DataMessageIn.packet[25];
        stc._for_center_holidayrev._end_year    = DataMessageIn.packet[26] + 1911;
        stc._for_center_holidayrev._end_month   = DataMessageIn.packet[27];
        stc._for_center_holidayrev._end_day     = DataMessageIn.packet[28];

        stc.Lock_to_Save_HoliDayReverseTime_Step1(0);

        return true;

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryReverseTimeData5F42(MESSAGEOK DataMessageIn)
{
    try
    {
        unsigned char data[256];

        unsigned char ucQueryReverseTimeType = DataMessageIn.packet[9];

        if( DataMessageIn.packetLength < 13)
        {
            vReturnToCenterNACK(0x5F, 0x42, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 13)
        {
            vReturnToCenterNACK(0x5F, 0x42, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        stc.Lock_to_Load_HoliDayReverseTime_for_Center(ucQueryReverseTimeType);

        ;
        int iDataPtr;
        int iNumWeekDayPrt = 0;
        int iNumWeekDay = 0;
        data[0] = 0x5F;
        data[1] = 0xC2;
        data[2] = stc._for_center_reversetime.usiDirectIn;
        data[3] = stc._for_center_reversetime.usiHourStartIn;
        data[4] = stc._for_center_reversetime.usiMinStartIn;
        data[5] = stc._for_center_reversetime.usiHourEndIn;
        data[6] = stc._for_center_reversetime.usiMinEndIn;
        data[7] = stc._for_center_reversetime.usiDirectOut;
        data[8] = stc._for_center_reversetime.usiHourStartOut;
        data[9] = stc._for_center_reversetime.usiMinStartOut;
        data[10] = stc._for_center_reversetime.usiHourEndOut;
        data[11] = stc._for_center_reversetime.usiMinEndOut;

        data[12] = stc._for_center_reversetime.usiClearTime;
        data[13] = stc._for_center_reversetime.usiFlashGreen;
        data[14] = stc._for_center_reversetime.usiGreenTime;
        data[15] = stc._for_center_reversetime._reverse_time_type;

        data[16] = stc._for_center_holidayrev._start_year - 1911;
        data[17] = stc._for_center_holidayrev._start_month;
        data[18] = stc._for_center_holidayrev._start_day;
        data[19] = stc._for_center_holidayrev._end_year - 1911;
        data[20] = stc._for_center_holidayrev._end_month;
        data[21] = stc._for_center_holidayrev._end_day;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 22, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;
    }
    catch (...) {}
}



//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWritePhase5F13(MESSAGEOK DataMessageIn)
{
    try
    {
//OTMARKPRINTF  printf("[OT] Go to 5F13.\n");
        int i=0;
        bool bMollacOK=false;
        unsigned char ucStepSignal[8];
        unsigned short int usiTMP;
        unsigned char ucTMP;

        //OT Debug Dir
        int iLightOutNo;
        unsigned short int usiShift = 0;
        unsigned char ucLightConvert[8][8];
        for(int i = 0; i < 8; i++)                                                    //init
            for(int j = 0; j < 8; j++)
                ucLightConvert[i][j] = 0x0;

//OT Debug 950322

        int iPhaseOrder = DataMessageIn.packet[9];
        DATA_Bit _SignalMap;
        _SignalMap.DBit = DataMessageIn.packet[10];
        int iSignalCount = DataMessageIn.packet[11];
        int iSubPhaseCount = DataMessageIn.packet[12];

        if( DataMessageIn.packetLength < 16+(iSignalCount*iSubPhaseCount) )
        {
            vReturnToCenterNACK(0x5F, 0x13, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 16+(iSignalCount*iSubPhaseCount) )
        {
            vReturnToCenterNACK(0x5F, 0x13, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        int iSignalMapCountSignal = 0;
        if(_SignalMap.switchBit.b1) iSignalMapCountSignal++;
        if(_SignalMap.switchBit.b2) iSignalMapCountSignal++;
        if(_SignalMap.switchBit.b3) iSignalMapCountSignal++;
        if(_SignalMap.switchBit.b4) iSignalMapCountSignal++;
        if(_SignalMap.switchBit.b5) iSignalMapCountSignal++;
        if(_SignalMap.switchBit.b6) iSignalMapCountSignal++;
        if(_SignalMap.switchBit.b7) iSignalMapCountSignal++;
        if(_SignalMap.switchBit.b8) iSignalMapCountSignal++;
        if( iSignalMapCountSignal != iSignalCount)
        {
            vReturnToCenterNACK(0x5F, 0x13, 0x04, 0x02);    //signalmap error
            return false;
        }

        vReturnToCenterACK(0x5F, 0x13);


        //OT Debug Dir  copy data to convert array
//======================
        for(int i = 0; i < iSubPhaseCount; i++)
            for(int j = 0; j < iSignalCount; j++)
                ucLightConvert[i][j] = DataMessageIn.packet[i*iSignalCount + j + 13];

        if(_SignalMap.switchBit.b1 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirN);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b2 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirNE);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b3 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirE);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b4 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirES);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b5 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirS);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b6 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirSW);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b7 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirW);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b8 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirWN);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                    DataMessageIn.packet[i*iSignalCount + 13 + iLightOutNo] = ucLightConvert[i][usiShift];
                usiShift++;
            }
        }

//======================


        bMollacOK = stc.Lock_to_Reset_Phase_for_Center(iPhaseOrder, iSubPhaseCount, iSignalCount);

        if (bMollacOK)
        {

            stc._for_center_phase._phase_order=iPhaseOrder;
            stc._for_center_phase._signal_map=_SignalMap.DBit;
            stc._for_center_phase._signal_count=iSignalCount;
            stc._for_center_phase._subphase_count=iSubPhaseCount;
            stc._for_center_phase._total_step_count = iSubPhaseCount * 5;  //五個步階

            for(int j = 0; j < iSubPhaseCount; j++)
            {

                for(int qq=0; qq<iSignalCount; qq++)
                {
                    ucStepSignal[qq] = DataMessageIn.packet[iSignalCount*j+13+qq];   //先把同一時相之燈號放到array
                }

                for( int k=0; k< stc._for_center_phase._ptr_subphase_step_count[j]; k++ )    //先把所有步階都填全紅
                {
                    for(int l=0; l<stc._for_center_phase._signal_count; l++)
                    {
                        stc._for_center_phase._ptr_subphase_step_signal_status[j][k][l] = 0x3003;
                    }
                }

                for( int k=0; k< stc._for_center_phase._ptr_subphase_step_count[j]; k++ )
                {
                    for(int l=0; l<stc._for_center_phase._signal_count; l++)
                    {
                        if(k==0)   //step 1
                        {
                            ucTMP = ucStepSignal[l];
                        }
                        else if(k==1)
                        {
                            ucTMP = oTools.vGuessLightStep(2, ucStepSignal[l]);
                        }
                        else if(k==2)
                        {
                            ucTMP = oTools.vGuessLightStep(2, ucStepSignal[l]);
                            ucTMP = oTools.vGuessLightStep(3, ucTMP);
                        }
                        else if(k==3)
                        {
                            ucTMP = oTools.vGuessLightStep(2, ucStepSignal[l]);
                            ucTMP = oTools.vGuessLightStep(3, ucTMP);
                            ucTMP = oTools.vGuessLightStep(4, ucTMP);
                        }
                        else if(k==4)
                        {
                            ucTMP = oTools.vGuessLightStep(2, ucStepSignal[l]);
                            ucTMP = oTools.vGuessLightStep(3, ucTMP);
                            ucTMP = oTools.vGuessLightStep(4, ucTMP);
                            ucTMP = oTools.vGuessLightStep(5, ucTMP);
                        }
                        //OTMARKPRINTF  printf("ucTMP = %X.\n",ucTMP);

                        usiTMP = oTools.vVer30LightToCCTLight(ucTMP);
                        stc._for_center_phase._ptr_subphase_step_signal_status[j][k][l] = usiTMP;

                    }
                }
            }
        }

        //OTMARKPRINTF  printf("[OT] Make OK.\n");

        smem.vWriteMsgToDOM("WritePhaseBy5F13");

        stc.Lock_to_Save_Phase_from_Center();             //儲存回去
        smem.vSetINTData(TC92_iUpdatePhaseData, 1);
        smem.vSetTCPhasePlanSegTypeData(TC_Phase, iPhaseOrder, true);
        //OTMARKPRINTF  printf("[OT] Save Ok.\n");

//OTMARKPRINTF  printf("SAVE Phase:%d OK!!\n",iPhaseOrder);

        return true;

    }
    catch(...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryPhase5F43(MESSAGEOK DataMessageIn)
{
    try
    {
        //CHeck

        //OT Debug Dir
        int iLightOutNo;
        unsigned short int usiShift = 0;
        unsigned char ucLightConvert[8][8];
        DATA_Bit _SignalMap;
        for(int i = 0; i < 8; i++)                                                    //init
            for(int j = 0; j < 8; j++)
                ucLightConvert[i][j] = 0x0;

//Don't respond  vReturnToCenterACK(0x5F, 0x43);

        int iQueryPhaseOrder = DataMessageIn.packet[9];

        bool bPhaseStatus;
        bPhaseStatus = smem.vGetTCPhasePlanSegTypeData(TC_Phase, iQueryPhaseOrder);

        if(bPhaseStatus == false)
        {
            vReturnToCenterNACK(0x5F, 0x43, 0x02, 0x0);
            return false;
        }

        stc.Lock_to_Load_Phase_for_Center(iQueryPhaseOrder);

        unsigned char data[256];
        int iDataPtr = 6; //Form 6 Start to send SignalStatus
        data[0] = 0x5F;
        data[1] = 0xC3;
        data[2] = stc._for_center_phase._phase_order;
        data[3] = stc._for_center_phase._signal_map;
        data[4] = stc._for_center_phase._signal_count;
        data[5] = stc._for_center_phase._subphase_count;

        int usiTMP;
        _SignalMap.DBit = data[3];
        int iSignalCount = data[4];
        int iSubPhaseCount = data[5];

        for(int i = 0; i < stc._for_center_phase._subphase_count; i++)
        {
            for(int j = 0; j < stc._for_center_phase._signal_count; j++)
            {
                usiTMP = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][j];
                data[iDataPtr] = oTools.vCCTLightToVer30Light(usiTMP);
                iDataPtr++;
            }
        }

        //OT Debug Dir  copy data to convert array
//======================
        for(int i = 0; i < iSubPhaseCount; i++)                                              //stc._for_center_phase._subphase_count
            for(int j = 0; j < iSignalCount; j++)                                            //stc._for_center_phase._signal_count
                ucLightConvert[i][j] = data[i*data[4] + j + 6];

        /*
          printf("printfMsg _SignalMap.switchBit.b1:%d\n", _SignalMap.switchBit.b1);
          printf("printfMsg _SignalMap.switchBit.b2:%d\n", _SignalMap.switchBit.b2);
          printf("printfMsg _SignalMap.switchBit.b3:%d\n", _SignalMap.switchBit.b3);
          printf("printfMsg _SignalMap.switchBit.b4:%d\n", _SignalMap.switchBit.b4);
          printf("printfMsg _SignalMap.switchBit.b5:%d\n", _SignalMap.switchBit.b5);
          printf("printfMsg _SignalMap.switchBit.b6:%d\n", _SignalMap.switchBit.b6);
          printf("printfMsg _SignalMap.switchBit.b7:%d\n", _SignalMap.switchBit.b7);
          printf("printfMsg _SignalMap.switchBit.b8:%d\n", _SignalMap.switchBit.b8);
          printf("printfMsg usiShift:%d\n", usiShift);
        */
        if(_SignalMap.switchBit.b1 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirN);
            printf("printfMsg dirN iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
            if(iLightOutNo < iSignalCount)
            {
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                    printf("printfMsg dirN locate:%d\n", i*iSignalCount + 6 + usiShift);
                }
                usiShift++;
            }
            printf("printfMsg go re-defg.\n");
        }
        if(_SignalMap.switchBit.b2 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirNE);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg dirNE iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                    printf("printfMsg dirNE locate:%d\n", i*iSignalCount + 6 + usiShift);
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b3 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirE);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg dirE iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                    printf("printfMsg dirE locate:%d\n", i*iSignalCount + 6 + usiShift);
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b4 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirES);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg dirES iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                    printf("printfMsg dirES locate:%d\n", i*iSignalCount + 6 + usiShift);
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b5 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirS);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg dirS iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                    printf("printfMsg dirS locate:%d\n", i*iSignalCount + 6 + usiShift);
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b6 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirSW);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg dirSW iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
                for(int i = 0; i < iSubPhaseCount; i++)
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b7 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirW);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg dirW iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
                for(int i = 0; i < iSubPhaseCount; i++)
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b8 == true && usiShift < data[4])
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirWN);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg dirWN iLightOutNo:%d usiShift:%d\n", iLightOutNo, usiShift);
                for(int i = 0; i < iSubPhaseCount; i++)
                    data[i*iSignalCount + 6 + usiShift] = ucLightConvert[i][iLightOutNo];
                usiShift++;
            }
        }


        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;

    }
    catch(...) {}
}


//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWritePlan5F14(MESSAGEOK DataMessageIn)
{
    try
    {

        if( DataMessageIn.packetLength < 14+(7*DataMessageIn.packet[10]) )
        {
            vReturnToCenterNACK(0x5F, 0x14, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 14+(7*DataMessageIn.packet[10]) )
        {
            vReturnToCenterNACK(0x5F, 0x14, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        if(DataMessageIn.InnerOrOutWard != cComingFromScreen)
        {
            vReturnToCenterACK(0x5F, 0x14);
        }
        screenLast92TCPlanSegmentUpdate.DisplayPlanUpdate();

        /*
        PlanID(時制計劃):[9]
        SubPhaseCount(分相數):[10]

        MinGreen(短綠):[11]
        MaxGreen(長綠):[12],[13]
        Yellow(黃):[14]
        AllRed(全紅):[15]
        PedGreenFlash(行人綠閃):[16]
        PedRed(行人紅):[17]
        */

        int i=0;
        bool bMollacOK=false;

//OT Debug 0523
        if (DataMessageIn.packet[10]==0)                    //閃光時相
            bMollacOK=stc.Lock_to_Reset_Plan_for_Center(DataMessageIn.packet[9],1);
        else  bMollacOK=stc.Lock_to_Reset_Plan_for_Center(DataMessageIn.packet[9],DataMessageIn.packet[10]);

        if (bMollacOK)
        {

            stc._for_center_plan._planid=DataMessageIn.packet[9];
            stc._for_center_plan._subphase_count=DataMessageIn.packet[10];

            //OTMARKPRINTF  printf("Plan:%d, PhaseCount:%d\n",stc._for_center_plan._planid,stc._for_center_plan._subphase_count);

            if (stc._for_center_plan._subphase_count==0)     //閃光時相
            {
                stc._for_center_plan._subphase_count=1;      //虛擬一個Phase

                stc._for_center_plan._ptr_subplaninfo[0]._min_green=0;
                stc._for_center_plan._ptr_subplaninfo[0]._max_green=0;
                stc._for_center_plan._ptr_subplaninfo[0]._yellow=0;
                stc._for_center_plan._ptr_subplaninfo[0]._allred=0;
                stc._for_center_plan._ptr_subplaninfo[0]._pedgreen_flash=0;
                stc._for_center_plan._ptr_subplaninfo[0]._pedred=0;

            }
            else
            {

                for (i=0; i<stc._for_center_plan._subphase_count; i++)
                {

                    //OTMARKPRINTF  printf("Phase:%d\n",i+1);

                    stc._for_center_plan._ptr_subplaninfo[i]._min_green=DataMessageIn.packet[11+7*i];
                    stc._for_center_plan._ptr_subplaninfo[i]._max_green=DataMessageIn.packet[12+7*i]*256+DataMessageIn.packet[13+7*i];
                    stc._for_center_plan._ptr_subplaninfo[i]._yellow=DataMessageIn.packet[14+7*i];
                    stc._for_center_plan._ptr_subplaninfo[i]._allred=DataMessageIn.packet[15+7*i];
                    stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash=DataMessageIn.packet[16+7*i];
                    stc._for_center_plan._ptr_subplaninfo[i]._pedred=DataMessageIn.packet[17+7*i];

                    //OTMARKPRINTF  printf("minGreen:%d,maxGreen:%d,yellow:%d,allred:%d,pedGFlash:%d,pedRed%d\n",
//OTMARKPRINTF             stc._for_center_plan._ptr_subplaninfo[i]._min_green,stc._for_center_plan._ptr_subplaninfo[i]._max_green,stc._for_center_plan._ptr_subplaninfo[i]._yellow,
//OTMARKPRINTF             stc._for_center_plan._ptr_subplaninfo[i]._allred,stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash,stc._for_center_plan._ptr_subplaninfo[i]._pedred);

                }
            }

            stc.Lock_to_Save_Plan_from_Center();             //儲存回去

            //OTMARKPRINTF  printf("SAVE Plan:%d OK!!\n",DataMessageIn.packet[9]);


            return true;
        }
    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryPlan5F44(MESSAGEOK DataMessageIn)
{
    try
    {

        int iQueryPlan = DataMessageIn.packet[9];

        bool bPlanStatus;
        bPlanStatus = smem.vGetTCPhasePlanSegTypeData(TC_Plan, iQueryPlan);
        if(bPlanStatus == false)
        {
            vReturnToCenterNACK(0x5F, 0x44, 0x02, 0x0);
            return false;
        }
        if( DataMessageIn.packetLength < 13)
        {
            vReturnToCenterNACK(0x5F, 0x44, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 13)
        {
            vReturnToCenterNACK(0x5F, 0x44, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }


        stc.Lock_to_Load_Plan_for_Center(iQueryPlan);

        unsigned char data[256];
        int iDataPtr = 4; //Form 6 Start to send SignalStatus
        data[0] = 0x5F;
        data[1] = 0xC4;
        data[2] = stc._for_center_plan._planid;
        data[3] = stc._for_center_plan._subphase_count;
        for(int i = 0; i < stc._for_center_plan._subphase_count; i++)
        {
            data[iDataPtr] = stc._for_center_plan._ptr_subplaninfo[i]._min_green;
            iDataPtr++;
//OT Debug 950410  -2 line
            data[iDataPtr] = stc._for_center_plan._ptr_subplaninfo[i]._max_green / 256;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_plan._ptr_subplaninfo[i]._max_green % 256;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_plan._ptr_subplaninfo[i]._yellow;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_plan._ptr_subplaninfo[i]._allred;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_plan._ptr_subplaninfo[i]._pedred;
            iDataPtr++;
        }

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWritePlanDBMS5F15(MESSAGEOK DataMessageIn)
{
    try
    {

        int iPhaseOrder = DataMessageIn.packet[11];
        int iSubphaseCount = DataMessageIn.packet[12];

        if(smem.vGetTCPhasePlanSegTypeData(TC_Phase, iPhaseOrder) == false)
        {
            vReturnToCenterNACK(0x5F, 0x15, 0x04, 0x03);
            return false;
        }
        stc.Lock_to_Load_Phase_for_Center(iPhaseOrder);

        printf("5F15: stc._for_center_phase._subphase_count:%d\n", stc._for_center_phase._subphase_count);
        printf("5F15: iSubphaseCount:%d\n", iSubphaseCount);

        if( stc._for_center_phase._subphase_count != iSubphaseCount && iSubphaseCount != 0)
        {
            vReturnToCenterNACK(0x5F, 0x15, 0x04, 0x04);
            return false;
        }

        if( DataMessageIn.packetLength < 20+(2*DataMessageIn.packet[12]))
        {
            vReturnToCenterNACK(0x5F, 0x15, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 20+(2*DataMessageIn.packet[12]))
        {
            vReturnToCenterNACK(0x5F, 0x15, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

//OT Debug 0523
//  vReturnToCenterACK(0x5F, 0x15);

        /*
        PlanID(時制計劃):[9]
        Direct(基準方向):[10]
        PhaseOrder(時相編號):[11]
        SubPhaseCount(分相數):[12]

        Green(綠燈時間):[13][14],[15][16],[17][18]
        CycleTime(週期):[19][20]
        Offset(時差):[21][22]
        */

        int i=0;
        bool bMollacOK=false;

        //OT Debug 0523
        bool bRet;

        if (iSubphaseCount == 0)      //閃光時相
        {
            bMollacOK = stc.Lock_to_Reset_Plan_for_Center(DataMessageIn.packet[9],1);
        }
        else
        {
            bMollacOK = stc.Lock_to_Reset_Plan_for_Center(DataMessageIn.packet[9], iSubphaseCount);
        }

        if (bMollacOK)
        {

            stc._for_center_plan._planid=DataMessageIn.packet[9];
            stc._for_center_plan._dir=DataMessageIn.packet[10];
            printf("Plan:%d,Dir:%d\n",stc._for_center_plan._planid,stc._for_center_plan._dir);

            /*
                  if (iPhaseOrder == 0x30) iPhaseOrder = 0x20;
                  else if (iPhaseOrder==0xB0) iPhaseOrder = 0x80;
            */

            stc._for_center_plan._phase_order = iPhaseOrder;
            stc._for_center_plan._subphase_count= iSubphaseCount;
            printf("Phase_Order:%d,Phase_Count:%d\n",stc._for_center_plan._phase_order,stc._for_center_plan._subphase_count);

            for (i=0; i<stc._for_center_plan._subphase_count; i++)
            {
                printf("Phase:%d\n",i+1);
                stc._for_center_plan._ptr_subplaninfo[i]._green=DataMessageIn.packet[13+2*i]*256+DataMessageIn.packet[14+2*i];
                printf("Green-A:%d\n",stc._for_center_plan._ptr_subplaninfo[i]._green);
                if(stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash > 0 && stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash < stc._for_center_plan._ptr_subplaninfo[i]._green)
                {
                    stc._for_center_plan._ptr_subplaninfo[i]._green = stc._for_center_plan._ptr_subplaninfo[i]._green - stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash;
                    printf("Special A!\n");
                }
                if(stc._for_center_plan._ptr_subplaninfo[i]._pedred > 0 && stc._for_center_plan._ptr_subplaninfo[i]._pedred < stc._for_center_plan._ptr_subplaninfo[i]._green)
                {
                    stc._for_center_plan._ptr_subplaninfo[i]._green = stc._for_center_plan._ptr_subplaninfo[i]._green - stc._for_center_plan._ptr_subplaninfo[i]._pedred;
                    printf("Special B!\n");
                }
                printf("Green-B:%d\n",stc._for_center_plan._ptr_subplaninfo[i]._green);
            }

            stc._for_center_plan._cycle_time=DataMessageIn.packet[13+2*i]*256+DataMessageIn.packet[14+2*i];
            printf("cycle_time:%d\n",stc._for_center_plan._cycle_time);
            stc._for_center_plan._offset=DataMessageIn.packet[15+2*i]*256+DataMessageIn.packet[16+2*i];
            printf("offset:%d\n",stc._for_center_plan._offset);

            if (stc._for_center_plan._subphase_count==0)              //閃光時相
            {
                stc._for_center_plan._subphase_count=1;               //虛擬一個Phase

                //OT Debug 0523
                stc._for_center_plan._ptr_subplaninfo[i]._green=10;
            }

//OT Debug 0523
            bRet = stc.Lock_to_Save_Plan_from_Center();                      //儲存回去
            if(bRet) smem.vSetTCPhasePlanSegTypeData(TC_Plan, DataMessageIn.packet[9], true);
            else
            {
                vReturnToCenterNACK(0x5F, 0x15, 0x04, 0x05);
                return false;
            }
            vReturnToCenterACK(0x5F, 0x15);
//      smem.vSaveLast92TC_5F15Time();
            screenLast92TCPlanSegmentUpdate.DisplayPlanUpdate();

            printf("SAVE Plan:%d OK!\n",DataMessageIn.packet[9]);

        }
    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryPlanDBMS5F45(MESSAGEOK DataMessageIn)
{
    try
    {
        int iQueryPlan = DataMessageIn.packet[9];
        unsigned short int usiGreenTMP;

        bool bPlanStatus;
        bPlanStatus = smem.vGetTCPhasePlanSegTypeData(TC_Plan, iQueryPlan);
        if(bPlanStatus == false)
        {
            vReturnToCenterNACK(0x5F, 0x45, 0x02, 0x0);
            return false;
        }
        if( DataMessageIn.packetLength < 13)
        {
            vReturnToCenterNACK(0x5F, 0x45, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 13)
        {
            vReturnToCenterNACK(0x5F, 0x45, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }


        stc.Lock_to_Load_Plan_for_Center(iQueryPlan);

        unsigned char data[256];
        int iDataPtr = 6;
        data[0] = 0x5F;
        data[1] = 0xC5;
        data[2] = stc._for_center_plan._planid;
        data[3] = stc._for_center_plan._dir;
        data[4] = stc._for_center_plan._phase_order;
        data[5] = stc._for_center_plan._subphase_count;

        for(int i = 0; i < data[5]; i++)
        {
            usiGreenTMP = 0;

            //OT Debug 0523 -3line
            if(stc._for_center_plan._phase_order != 0xB0)                               //Flash PhaseOrder In HsinChu
            {
                usiGreenTMP += stc._for_center_plan._ptr_subplaninfo[i]._green;
            }
            else { }
            usiGreenTMP += stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash;
            usiGreenTMP += stc._for_center_plan._ptr_subplaninfo[i]._pedred;

            data[iDataPtr] = HI(usiGreenTMP);
            iDataPtr++;
            data[iDataPtr] = LO(usiGreenTMP);
            iDataPtr++;
        }

        //OT Debug 0523 -3line
        if(stc._for_center_plan._phase_order != 0xB0)                               //Flash PhaseOrder In HsinChu
        {
            data[iDataPtr] = stc._for_center_plan._cycle_time / 256;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_plan._cycle_time % 256;
            iDataPtr++;
        }
        else
        {
            data[iDataPtr] = 0;
            data[iDataPtr+1] = 0;
            iDataPtr+=2;
        }

        data[iDataPtr] = stc._for_center_plan._offset / 256;
        iDataPtr++;
        data[iDataPtr] = stc._for_center_plan._offset % 256;
        iDataPtr++;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWriteSegment5F16(MESSAGEOK DataMessageIn)
{
    try
    {

        /*
        SegmentType(時段型態):[9]
        SegmentCount(時段分相數):[10]

        Hour(起始時):[11][14]
        Min(起始分):[12][15]
        PlanID(時制計劃):[13][16]

        NumWeekDay(有幾天要照這Segtype):[]
        WeekDay(哪幾天?):[][][][]
        */

        int i=0;

        stc.Lock_to_Reset_Segment_for_Center(DataMessageIn.packet[9],DataMessageIn.packet[10]);
        stc.Lock_to_Load_WeekDaySegment_for_Center();

        stc._for_center_segment._segment_type=DataMessageIn.packet[9];

//OTMARKPRINTF  printf("segment_type%d\n",stc._for_center_segment._segment_type);

        stc._for_center_segment._segment_count=DataMessageIn.packet[10];

//OTMARKPRINTF  printf("segment_Count%d\n",stc._for_center_segment._segment_count);


        for (i=0; i<stc._for_center_segment._segment_count; i++)
        {
            stc._for_center_segment._ptr_seg_exec_time[i]._hour=DataMessageIn.packet[11+3*i];
            stc._for_center_segment._ptr_seg_exec_time[i]._minute=DataMessageIn.packet[12+3*i];
            stc._for_center_segment._ptr_seg_exec_time[i]._planid=DataMessageIn.packet[13+3*i];

            if(smem.vGetTCPhasePlanSegTypeData(TC_Plan, DataMessageIn.packet[13+3*i]) == false)
            {
                vReturnToCenterNACK(0x5F, 0x16, 0x04, 3*i + 5);
                return false;
            }


            //OTMARKPRINTF  printf("%d:%d--%d\n",stc._for_center_segment._ptr_seg_exec_time[i]._hour,stc._for_center_segment._ptr_seg_exec_time[i]._minute,stc._for_center_segment._ptr_seg_exec_time[i]._planid);

        }

        int numWeekDay=DataMessageIn.packet[11+3*i];

//OTMARKPRINTF  printf("numWeekDay:%d\n",numWeekDay);

        if( DataMessageIn.packetLength < 15+(3*DataMessageIn.packet[10])+numWeekDay)
        {
            vReturnToCenterNACK(0x5F, 0x16, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 15+(3*DataMessageIn.packet[10])+numWeekDay)
        {
            vReturnToCenterNACK(0x5F, 0x16, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }
        vReturnToCenterACK(0x5F, 0x16);


        for (int j=0; j<numWeekDay; j++)
        {

            //OTMARKPRINTF  printf("%3x--%d\n",DataMessageIn.packet[12+3*i+j],stc._for_center_segment._segment_type);

            switch (DataMessageIn.packet[12+3*i+j])
            {
            case 0x01:
                stc._for_center_weekdayseg[0]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x02:
                stc._for_center_weekdayseg[1]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x03:
                stc._for_center_weekdayseg[2]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x04:
                stc._for_center_weekdayseg[3]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x05:
                stc._for_center_weekdayseg[4]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x06:
                stc._for_center_weekdayseg[5]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x07:
                stc._for_center_weekdayseg[6]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x11:
                stc._for_center_weekdayseg[7]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x12:
                stc._for_center_weekdayseg[8]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x13:
                stc._for_center_weekdayseg[9]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x14:
                stc._for_center_weekdayseg[10]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x15:
                stc._for_center_weekdayseg[11]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x16:
                stc._for_center_weekdayseg[12]._segment_type=stc._for_center_segment._segment_type;
                break;
            case 0x17:
                stc._for_center_weekdayseg[13]._segment_type=stc._for_center_segment._segment_type;
                break;
            default:
                break;
            }
        }

        stc.Lock_to_Save_Segment_from_Center();
        stc.Lock_to_Save_WeekDaySegment_from_Center();
        smem.vSetTCPhasePlanSegTypeData(TC_SegType, DataMessageIn.packet[9], true);
        screenLast92TCPlanSegmentUpdate.DisplaySegmentUpdate();

        return true;

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQuerySegment5F46(MESSAGEOK DataMessageIn)
{
    try
    {
        bool bSegStatus;
        int iQuerySegmentType = DataMessageIn.packet[9];
        int iWeekDay = DataMessageIn.packet[10];

        if( DataMessageIn.packetLength < 14)
        {
            vReturnToCenterNACK(0x5F, 0x46, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 14)
        {
            vReturnToCenterNACK(0x5F, 0x46, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        if(iQuerySegmentType == 0xFF)
        {
            stc.Lock_to_Load_Segment_for_Center_inWeekDay(iWeekDay);
        }
        else
        {
            bSegStatus = smem.vGetTCPhasePlanSegTypeData(TC_SegType, iQuerySegmentType);
            if(bSegStatus == false)
            {
                vReturnToCenterNACK(0x5F, 0x46, 0x02, 0x0);
                return false;
            }

            stc.Lock_to_Load_Segment_for_Center(iQuerySegmentType);
        }

        stc.Lock_to_Load_WeekDaySegment_for_Center();

        unsigned char data[256];
        int iDataPtr = 4; //Form 6 Start to send SignalStatus
        int iNumWeekDayPrt = 0;
        int iNumWeekDay = 0;
        data[0] = 0x5F;
        data[1] = 0xC6;
        data[2] = stc._for_center_segment._segment_type;
        data[3] = stc._for_center_segment._segment_count;

        for (int i=0; i<stc._for_center_segment._segment_count; i++)
        {
            data[iDataPtr] = stc._for_center_segment._ptr_seg_exec_time[i]._hour;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_segment._ptr_seg_exec_time[i]._minute;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_segment._ptr_seg_exec_time[i]._planid;
            iDataPtr++;
        }
        iNumWeekDayPrt = iDataPtr;
        iDataPtr++;

        if(stc._for_center_weekdayseg[0]._segment_type == data[2])    // data[2] = stc._for_center_segment._segment_type
        {
            data[iDataPtr] = 0x01;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[1]._segment_type == data[2])
        {
            data[iDataPtr] = 0x02;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[2]._segment_type == data[2])
        {
            data[iDataPtr] = 0x03;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[3]._segment_type == data[2])
        {
            data[iDataPtr] = 0x04;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[4]._segment_type == data[2])
        {
            data[iDataPtr] = 0x05;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[5]._segment_type == data[2])
        {
            data[iDataPtr] = 0x06;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[6]._segment_type == data[2])
        {
            data[iDataPtr] = 0x07;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[7]._segment_type == data[2])
        {
            data[iDataPtr] = 0x11;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[8]._segment_type == data[2])
        {
            data[iDataPtr] = 0x12;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[9]._segment_type == data[2])
        {
            data[iDataPtr] = 0x13;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[10]._segment_type == data[2])
        {
            data[iDataPtr] = 0x14;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[11]._segment_type == data[2])
        {
            data[iDataPtr] = 0x15;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[12]._segment_type == data[2])
        {
            data[iDataPtr] = 0x16;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayseg[13]._segment_type == data[2])
        {
            data[iDataPtr] = 0x17;
            iNumWeekDay++;
            iDataPtr++;
        }

        data[iNumWeekDayPrt] = iNumWeekDay;                                           //最後補上NumWeekDay之值

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;
    }
    catch (...) {}
}


//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWriteSpecialSegment5F17(MESSAGEOK DataMessageIn)
{
    try
    {

        if( DataMessageIn.packetLength < 20+(3*DataMessageIn.packet[10]))
        {
            vReturnToCenterNACK(0x5F, 0x17, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 20+(3*DataMessageIn.packet[10]))
        {
            vReturnToCenterNACK(0x5F, 0x17, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }
        vReturnToCenterACK(0x5F, 0x17);

        /*
        SegmentType(時段型態):[9]
        SegmentCount(時段分相數):[10]

        Hour(起始時):[11]
        Min(起始分):[12]
        PlanID(時制計劃):[13]

        Year():[][]
        Month():[][]
        Day():[][]
        */

        int i=0;

        stc.Lock_to_Reset_Segment_for_Center(DataMessageIn.packet[9],DataMessageIn.packet[10]);
        stc.Lock_to_Load_HoliDaySegment_for_Center(DataMessageIn.packet[9]);

        stc._for_center_segment._segment_type=DataMessageIn.packet[9];

//OTMARKPRINTF  printf("segment_type%d\n",stc._for_center_segment._segment_type);

        stc._for_center_segment._segment_count=DataMessageIn.packet[10];

//OTMARKPRINTF  printf("segment_Count%d\n",stc._for_center_segment._segment_count);


        for (i=0; i<stc._for_center_segment._segment_count; i++)
        {
            stc._for_center_segment._ptr_seg_exec_time[i]._hour=DataMessageIn.packet[11+3*i];
            stc._for_center_segment._ptr_seg_exec_time[i]._minute=DataMessageIn.packet[12+3*i];
            stc._for_center_segment._ptr_seg_exec_time[i]._planid=DataMessageIn.packet[13+3*i];

            if(smem.vGetTCPhasePlanSegTypeData(TC_Plan, DataMessageIn.packet[13+3*i]) == false)
            {
                vReturnToCenterNACK(0x5F, 0x17, 0x04, 3*i + 5);
                return false;
            }


            //OTMARKPRINTF  printf("%d:%d--%d\n",stc._for_center_segment._ptr_seg_exec_time[i]._hour,stc._for_center_segment._ptr_seg_exec_time[i]._minute,stc._for_center_segment._ptr_seg_exec_time[i]._planid);

        }

        stc._for_center_holidayseg._segment_type=DataMessageIn.packet[9];
        stc._for_center_holidayseg._start_year=DataMessageIn.packet[11+3*i]+1911;
        stc._for_center_holidayseg._start_month=DataMessageIn.packet[12+3*i];
        stc._for_center_holidayseg._start_day=DataMessageIn.packet[13+3*i];
        stc._for_center_holidayseg._end_year=DataMessageIn.packet[14+3*i]+1911;
        stc._for_center_holidayseg._end_month=DataMessageIn.packet[15+3*i];
        stc._for_center_holidayseg._end_day=DataMessageIn.packet[16+3*i];


//OTMARKPRINTF  printf("%d-%d-%d  %d-%d-%d ::%d\n",stc._for_center_holidayseg._start_year,stc._for_center_holidayseg._start_month,stc._for_center_holidayseg._start_day,
//OTMARKPRINTF  stc._for_center_holidayseg._end_year,stc._for_center_holidayseg._end_month,stc._for_center_holidayseg._end_day,stc._for_center_holidayseg._segment_type);


        stc.Lock_to_Save_Segment_from_Center();              //save
        stc.Lock_to_Save_HoliDaySegment_from_Center();       //save
        smem.vSetTCPhasePlanSegTypeData(TC_SegType, DataMessageIn.packet[9], true);
        screenLast92TCPlanSegmentUpdate.DisplaySegmentUpdate();

        return true;

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryHolidaySegment5F47(MESSAGEOK DataMessageIn)
{
    try
    {
        int iQueryHolidaySegmentType = DataMessageIn.packet[9];

        bool bSegStatus;
        bSegStatus = smem.vGetTCPhasePlanSegTypeData(TC_SegType, iQueryHolidaySegmentType);
        if(bSegStatus == false)
        {
            vReturnToCenterNACK(0x5F, 0x47, 0x02, 0x0);
            return false;
        }
        if( DataMessageIn.packetLength < 13)
        {
            vReturnToCenterNACK(0x5F, 0x47, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 13)
        {
            vReturnToCenterNACK(0x5F, 0x47, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }


        if( stc.Lock_to_Load_HoliDaySegment_for_Center(iQueryHolidaySegmentType) )
        {
        }
        else
        {
            //NACK
            return false;
        }


        unsigned char data[256];
        int iDataPtr = 4; //Form 6 Start to send SignalStatus

        data[0] = 0x5F;
        data[1] = 0xC7;
        data[2] = stc._for_center_holidayseg._segment_type;
        data[3] = stc._for_center_segment._segment_count;

        for (int i=0; i<stc._for_center_segment._segment_count; i++)
        {
            data[iDataPtr] = stc._for_center_segment._ptr_seg_exec_time[i]._hour;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_segment._ptr_seg_exec_time[i]._minute;
            iDataPtr++;
            data[iDataPtr] = stc._for_center_segment._ptr_seg_exec_time[i]._planid;
            iDataPtr++;
        }

        data[iDataPtr] = stc._for_center_holidayseg._start_year - 1911;
        iDataPtr++;
        data[iDataPtr] = stc._for_center_holidayseg._start_month;
        iDataPtr++;
        data[iDataPtr] = stc._for_center_holidayseg._start_day;
        iDataPtr++;
        data[iDataPtr] = stc._for_center_holidayseg._end_year - 1911;
        iDataPtr++;
        data[iDataPtr] = stc._for_center_holidayseg._end_month;
        iDataPtr++;
        data[iDataPtr] = stc._for_center_holidayseg._end_day;
        iDataPtr++;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;
    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWriteRunPlan5F18(MESSAGEOK DataMessageIn)
{
    try
    {

        DATA_Bit _ControlStrategy;
        _ControlStrategy.DBit = smem.vGetUCData(TC92_ucControlStrategy);
        printf("[OTDebug] in 5F18, _ControlStrategy.DBit:%X\n", _ControlStrategy.DBit);


        unsigned int uiTimeOutPlan;
        int iSetPlanID = DataMessageIn.packet[9];

        if( DataMessageIn.packetLength < 13)
        {
            vReturnToCenterNACK(0x5F, 0x18, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 13)
        {
            vReturnToCenterNACK(0x5F, 0x18, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }
        vReturnToCenterACK(0x5F, 0x18);

        //OT1000310
        //OT20110729   if(_ControlStrategy.DBit == 0x02) {
//OT20111107 if((_ControlStrategy.DBit & 0x02) > 0 ) {  //OT20110729
        if(1)    //OT20111107
        {
            smem.vSetINTData(TC92_PlanOneTime5F18_PlanID, iSetPlanID);

            //OT1000218
            uiTimeOutPlan = stc.vDetermineTimeToNextPlan();
            smem.vSet5F18EffectTime(uiTimeOutPlan);

            printf("uiTimeOutPlan:%d\n", uiTimeOutPlan);
            printf("iSetPlanID:%d\n", iSetPlanID);

        }

    }
    catch (...) {}
}

//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryRunPlan5F48(MESSAGEOK DataMessageIn)                   //then return 5FC8
{
    try
    {
        unsigned char data[256];
        int iDataPtr = 6; //Form 6 Start to send SignalStatus

        unsigned short int usiGreen;

        unsigned short int usiRunningPlanID = stc.vGetUSIData(CSTC_exec_plan_plan_ID);

        printf("usiRunningPlanID:%d\n", usiRunningPlanID);

        stc.Lock_to_Load_Plan_for_Center(usiRunningPlanID);
//  unsigned short int iCurrentPhaseID  = stc.vGetUSIData(CSTC_exec_plan_phase_order);
//  stc.Lock_to_Load_Phase_for_Center(iCurrentPhaseID);

        /*
          DATA_Bit _SignalMap, _GSignalMap;
          _SignalMap.DBit = stc._for_center_phase._signal_map;
          int iSignalMapWayCount;
          unsigned short int usiSignalStatus;
          int iGSignalMap[8];

          for(int i = 0; i < stc._for_center_phase._subphase_count; i++) {
            _GSignalMap.DBit = 0;
            iSignalMapWayCount = 0;
            for(int j = 0; j < 8; j++)                                                  //Init
              iGSignalMap[j] = 0;

            if(_SignalMap.switchBit.b1 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[0] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }
            if(_SignalMap.switchBit.b2 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[1] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }
            if(_SignalMap.switchBit.b3 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[2] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }
            if(_SignalMap.switchBit.b4 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[3] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }
            if(_SignalMap.switchBit.b5 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[4] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }
            if(_SignalMap.switchBit.b6 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[5] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }
            if(_SignalMap.switchBit.b7 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[6] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }
            if(_SignalMap.switchBit.b8 == true) {                                         //從SignalMap對應CCT的燈點
              usiSignalStatus = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][iSignalMapWayCount];
              iGSignalMap[7] = oTools.vCCTLightToVer30SignalMapLight_5F0F(0, usiSignalStatus);
              iSignalMapWayCount++;
            }

            for(int j = 0; j < 8; j++) {
              if(iGSignalMap[j] == 1) {
                if(j == 0) _GSignalMap.switchBit.b1 = true;
                if(j == 1) _GSignalMap.switchBit.b2 = true;
                if(j == 2) _GSignalMap.switchBit.b3 = true;
                if(j == 3) _GSignalMap.switchBit.b4 = true;
                if(j == 4) _GSignalMap.switchBit.b5 = true;
                if(j == 5) _GSignalMap.switchBit.b6 = true;
                if(j == 6) _GSignalMap.switchBit.b7 = true;
                if(j == 7) _GSignalMap.switchBit.b8 = true;
              }
            }
            data[iDataPtr] = _GSignalMap.DBit;
            iDataPtr++;
          }
        */

//  int iDataPtr = 6; //Form 6 Start to send SignalStatus
        data[0] = 0x5F;
        data[1] = 0xC8;
        data[2] = usiRunningPlanID;
        data[3] = stc._for_center_plan._dir;
        data[4] = stc._for_center_plan._phase_order;
        data[5] = stc._for_center_plan._subphase_count;

        /*
        data[6] = have beed place before
        */


//OT Debug CECI
        for(int i = 0; i < data[5]; i++)                                              //data[5] = subphasecount
        {
            usiGreen = 0;
            if(stc._for_center_plan._phase_order != 0xB0)                               //Flash PhaseOrder In HsinChu
            {
                usiGreen += stc._for_center_plan._ptr_subplaninfo[i]._green;
            }
            else { }
            usiGreen += stc._for_center_plan._ptr_subplaninfo[i]._pedgreen_flash;
            usiGreen += stc._for_center_plan._ptr_subplaninfo[i]._pedred;

            data[iDataPtr] = HI(usiGreen);
            iDataPtr++;
            data[iDataPtr] = LO(usiGreen);
            iDataPtr++;
        }

        if(stc._for_center_plan._phase_order != 0xB0)                                 //Flash PhaseOrder In HsinChu
        {
            data[iDataPtr] = HI(stc._for_center_plan._cycle_time);
            iDataPtr++;
            data[iDataPtr] = LO(stc._for_center_plan._cycle_time);
            iDataPtr++;
        }
        else
        {
            data[iDataPtr] = 0;
            data[iDataPtr+1] = 0;
            iDataPtr+=2;
        }

        data[iDataPtr] = HI(stc._for_center_plan._offset);
        iDataPtr++;
        data[iDataPtr] = LO(stc._for_center_plan._offset);
        iDataPtr++;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

    }
    catch (...) {}
}


//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vWritePhaseByStep5F2F(MESSAGEOK DataMessageIn)
{
    try
    {
//OTMARKPRINTF  printf("[OT] Go to 5F2F.\n");
        int i=0;
        bool bMollacOK=false;
        unsigned char ucStepSignal[8];
        unsigned short int usiTMP;
        unsigned char ucTMP;
        int iDataPtr = 13;  //From DataMessageIn.packet[13] start read
        int iSubPhaseStepCount;

        //OT Debug Dir
        int iLightOutNo;
        unsigned short int usiShift = 0;
        unsigned char ucLightConvert[8][5][8];
        int iConvertPtr = 13;
        for(int i = 0; i < 8; i++)                                                    //init
            for(int j = 0; j < 5; j++)
                for(int k = 0; k < 8; k++)
                    ucLightConvert[i][j][k] = 0x0;

        vReturnToCenterACK(0x5F, 0x2F);

        int iPhaseOrder = DataMessageIn.packet[9];
        DATA_Bit _SignalMap;
        _SignalMap.DBit = DataMessageIn.packet[10];
        int iSignalCount = DataMessageIn.packet[11];
        int iSubPhaseCount = DataMessageIn.packet[12];

        printf("Write 5F2F phase, iPhaseOrder:%2X iSignalCount:%d iSubPhaseCount:%d", iPhaseOrder, iSignalCount, iSubPhaseCount);

//Copy data to convert array
        for(int i = 0; i < iSubPhaseCount; i++)
        {
            iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
            iConvertPtr++;
            if(i < 8)
            {
                for(int j = 0; j < iSubPhaseStepCount; j++)
                {
                    if(j < 5)
                    {
                        for(int k = 0; k < iSignalCount; k++)
                        {
                            if(k < 8)
                            {
                                ucLightConvert[i][j][k] = DataMessageIn.packet[iConvertPtr];
                            }
                            iConvertPtr++;
                        }
                    }
                }
            }
        }

        if(_SignalMap.switchBit.b1 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirN);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b2 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirNE);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b3 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirE);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b4 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirES);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b5 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirS);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b6 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirSW);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b7 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirW);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        if(_SignalMap.switchBit.b8 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirWN);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 13;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = DataMessageIn.packet[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        DataMessageIn.packet[iConvertPtr+1 + j*iSignalCount + iLightOutNo] = ucLightConvert[i][j][usiShift];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }


        bMollacOK = stc.Lock_to_Reset_Phase_for_Center(iPhaseOrder, iSubPhaseCount, iSignalCount);

        if (bMollacOK)
        {

            stc._for_center_phase._phase_order=iPhaseOrder;
            stc._for_center_phase._signal_map=_SignalMap.DBit;
            stc._for_center_phase._signal_count=iSignalCount;
            stc._for_center_phase._subphase_count=iSubPhaseCount;
            stc._for_center_phase._total_step_count = iSubPhaseCount * 5;  //五個步階

            /*
                for(int j = 0; j < iSubPhaseCount; j++) {
                  for( int k=0; k< stc._for_center_phase._ptr_subphase_step_count[j]; k++ ) {  //先把所有步階都填全紅
                    for(int l=0; l<stc._for_center_phase._signal_count; l++) {
                      stc._for_center_phase._ptr_subphase_step_signal_status[j][k][l] = 0x3003;
                    //OTMARKPRINTF  printf("[OT] stc._for_center_phase._ptr_subphase_step_signal_status[%d][%d][%d] = %x\n",j,k,l, stc._for_center_phase._ptr_subphase_step_signal_status[j][k][l]);
                    }
                  }
                }
            */

            for(int j = 0; j < iSubPhaseCount; j++)
            {

                iSubPhaseStepCount = DataMessageIn.packet[iDataPtr];
                iDataPtr++;

                for( int k=0; k< iSubPhaseStepCount; k++ )
                {

                    for(int qq=0; qq<iSignalCount; qq++)
                    {
                        ucStepSignal[qq] = DataMessageIn.packet[iDataPtr];   //先把同一時相之燈號放到array
                        iDataPtr++;
                    }

                    for(int l=0; l<stc._for_center_phase._signal_count; l++)
                    {
                        ucTMP = ucStepSignal[l];

                        //OTMARKPRINTF  printf("ucTMP = %X.\n",ucTMP);

                        usiTMP = oTools.vVer30LightToCCTLight(ucTMP);

                        for(int iForTmp = k; iForTmp < stc._for_center_phase._ptr_subphase_step_count[j]; iForTmp++)
                        {
                            stc._for_center_phase._ptr_subphase_step_signal_status[j][iForTmp][l] = usiTMP;
                            //OTMARKPRINTF  printf("[OT] stc._for_center_phase._ptr_subphase_step_signal_status[%d][%d][%d] = %x\n", j,iForTmp,l,stc._for_center_phase._ptr_subphase_step_signal_status[j][iForTmp][l]);
                        }

                    }
                }
            }

        }

//OTMARKPRINTF  printf("[OT] 5F2F Make OK.\n");

        smem.vWriteMsgToDOM("WritePhaseBy5F2F");

        stc.Lock_to_Save_Phase_from_Center();             //儲存回去
        smem.vSetINTData(TC92_iUpdatePhaseData, 1);
        smem.vSetTCPhasePlanSegTypeData(TC_Phase, iPhaseOrder, true);


//OTMARKPRINTF  printf("SAVE Phase:%d OK!!\n",iPhaseOrder);

        return true;

    }
    catch(...) {}
}

//OT Debug 0410  add 5F5F and 5FDF
//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryPhase5F5F(MESSAGEOK DataMessageIn)
{
    try
    {

        unsigned short int usiTmpCmp0[8];
        unsigned short int usiTmpCmp1[8];
        bool bCmpResult;

        //OT Debug Dir
        int iLightOutNo;
        unsigned short int usiShift = 0;
        unsigned char ucLightConvert[8][5][8];
        int iConvertPtr = 6;
        int iSubPhaseStepCount;
        for(int i = 0; i < 8; i++)                                                    //init
            for(int j = 0; j < 5; j++)
                for(int k = 0; k < 8; k++)
                    ucLightConvert[i][j][k] = 0x0;


        //CHeck

        int iQueryPhaseOrder = DataMessageIn.packet[9];

//OT Debug 950629
        if(smem.vGetTCPhasePlanSegTypeData(TC_Phase, iQueryPhaseOrder) == false)
        {
            vReturnToCenterNACK(0x5F, 0x5F, 0x02, 1);
            return false;
        }


        stc.Lock_to_Load_Phase_for_Center(iQueryPhaseOrder);

        unsigned char data[256];
        int iDataPtr = 6; //Form 6 Start to send SignalStatus
        int iStepCountPtr;
        data[0] = 0x5F;
        data[1] = 0xDF;
        data[2] = stc._for_center_phase._phase_order;
        data[3] = stc._for_center_phase._signal_map;
        data[4] = stc._for_center_phase._signal_count;
        data[5] = stc._for_center_phase._subphase_count;

        int usiTMP;


        for(int i = 0; i < stc._for_center_phase._subphase_count; i++)
        {

            iStepCountPtr = iDataPtr;
            data[iStepCountPtr] = 0;
            iDataPtr++;

            //clear usiTmpCmp
            for(int ii = 0; ii < 8; ii++)
            {
                usiTmpCmp0[ii] = 0;
                usiTmpCmp1[ii] = 0;
            }

            for(int j = 0; j < stc._for_center_phase._ptr_subphase_step_count[i]; j++)
            {

                for(int k = 0; k < stc._for_center_phase._signal_count; k++)
                {
                    usiTmpCmp1[k] = stc._for_center_phase._ptr_subphase_step_signal_status[i][j][k];
                }

//compare old and new signal light
                bCmpResult = false;
                for(int k = 0; k < 8; k++)
                {
                    if(usiTmpCmp0[k] != usiTmpCmp1[k])
                        bCmpResult = true;
                }

//      if(bCmpResult == true) {
                if(1)   //don't care.
                {
                    data[iStepCountPtr]++;
                    for(int k = 0; k < stc._for_center_phase._signal_count; k++)
                    {
                        usiTMP = stc._for_center_phase._ptr_subphase_step_signal_status[i][j][k];
                        data[iDataPtr] = oTools.vCCTLightToVer30Light(usiTMP);
                        iDataPtr++;
                    }
                }

                for(int k =0; k < 8; k++)
                {
                    usiTmpCmp0[k] = usiTmpCmp1[k];
                }


            }
        }

        printf("printfMsg 1iDataPtr:%d\n" ,iDataPtr);


        DATA_Bit _SignalMap;
        _SignalMap.DBit = data[3];
        int iSignalCount = data[4];
        int iSubPhaseCount = data[5];

        printf("printfMsg 2iDataPtr:%d\n" ,iDataPtr);

//Copy data to convert array
        for(int i = 0; i < iSubPhaseCount; i++)
        {
            iSubPhaseStepCount = data[iConvertPtr];
            printf("printfMsg iSubPhaseStepCount data[iConvertPtr]X:%x\n", data[iConvertPtr]);
            iConvertPtr++;
            for(int j = 0; j < iSubPhaseStepCount; j++)
            {
                for(int k = 0; k < iSignalCount; k++)
                {
                    ucLightConvert[i][j][k] = data[iConvertPtr];
                    iConvertPtr++;
                }
            }
        }
        printf("printfMsg 3iDataPtr:%d\n" ,iDataPtr);

        if(_SignalMap.switchBit.b1 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirN);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        printf("printfMsg iSubPhaseStepCount:%d at:%d\n", iSubPhaseStepCount, iConvertPtr);
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                    printf("iSignalCount:%d iSubPhaseStepCount:%d\n", iSignalCount, iSubPhaseStepCount);
                }
                usiShift++;
            }
        }
        printf("printfMsg 4iDataPtr:%d\n" ,iDataPtr);
        if(_SignalMap.switchBit.b2 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirNE);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        printf("printfMsg 5iDataPtr:%d\n" ,iDataPtr);
        if(_SignalMap.switchBit.b3 == true && usiShift < iSignalCount)
        {
            printf("printfMsg 5a_iDataPtr:%d\n" ,iDataPtr);
            iLightOutNo = smem.vGetSignamMapMappingDir(dirE);
            printf("printfMsg 5b_iDataPtr:%d\n" ,iDataPtr);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg 5c_iDataPtr:%d\n" ,iDataPtr);
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    printf("printfMsg 5d_iDataPtr:%d\n" ,iDataPtr);
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        printf("printfMsg 5e_iDataPtr:%d\n" ,iDataPtr);
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    printf("printfMsg 5f_iDataPtr:%d\n" ,iDataPtr);
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                printf("printfMsg 5g_iDataPtr:%d\n" ,iDataPtr);
                usiShift++;
            }
        }
        printf("printfMsg 6iDataPtr:%d\n" ,iDataPtr);
        if(_SignalMap.switchBit.b4 == true && usiShift < iSignalCount)
        {
            printf("printfMsg 6a_iDataPtr:%d\n" ,iDataPtr);
            iLightOutNo = smem.vGetSignamMapMappingDir(dirES);
            if(iLightOutNo < iSignalCount)
            {
                printf("printfMsg 6b_iDataPtr:%d\n" ,iDataPtr);
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    printf("printfMsg 6c_iDataPtr:%d\n" ,iDataPtr);
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        printf("printfMsg 6d_iDataPtr:%d\n" ,iDataPtr);
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    printf("printfMsg 6e_iDataPtr:%d\n" ,iDataPtr);
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        printf("printfMsg 7iDataPtr:%d\n" ,iDataPtr);
        if(_SignalMap.switchBit.b5 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirS);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        printf("printfMsg 8iDataPtr:%d\n" ,iDataPtr);
        if(_SignalMap.switchBit.b6 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirSW);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        printf("printfMsg 9iDataPtr:%d\n" ,iDataPtr);
        if(_SignalMap.switchBit.b7 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirW);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }
        printf("printfMsg 10iDataPtr:%d\n" ,iDataPtr);
        if(_SignalMap.switchBit.b8 == true && usiShift < iSignalCount)
        {
            iLightOutNo = smem.vGetSignamMapMappingDir(dirWN);
            if(iLightOutNo < iSignalCount)
            {
                iConvertPtr = 6;                                                         // init
                for(int i = 0; i < iSubPhaseCount; i++)
                {
                    iSubPhaseStepCount = data[iConvertPtr];
                    for(int j = 0; j < iSubPhaseStepCount; j++)
                    {
                        data[iConvertPtr+1 + j*iSignalCount + usiShift] = ucLightConvert[i][j][iLightOutNo];
                    }
                    iConvertPtr += iSignalCount*iSubPhaseStepCount+1;
                }
                usiShift++;
            }
        }


        /*
          for(int i = 0; i < stc._for_center_phase._subphase_count; i++) {
            for(int j = 0; i < stc._for_center_phase._subphase_count
            for(int j = 0; j < stc._for_center_phase._signal_count; j++) {
              usiTMP = stc._for_center_phase._ptr_subphase_step_signal_status[i][0][j];
              data[iDataPtr] = oTools.vCCTLightToVer30Light(usiTMP);
              iDataPtr++;
            }
          }
        */
        printf("printfMsg 11iDataPtr:%d\n" ,iDataPtr);
        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        return true;

    }
    catch(...) {}
}


//-------------------------------------------------------------------
bool PTRAFFIC92TC::vTransmitCycleSetting_5F3F(MESSAGEOK DataMessageIn)
{
    try
    {

        printf("printfMsg go to 5F3F.\n");

        short int TT, TC;
        TT = DataMessageIn.packet[9];
        TC = DataMessageIn.packet[10];

        int iReturnCommandSet = smem.vGet0FCommandSet();
        if (iReturnCommandSet == 0 )   //wireless
            if ( TT==2 ||  (TT == 1 && ((TC>=1 && TC<=4) || (TC >= 6 && TC < 0xFF))) )
            {
                vReturnToCenterNACK(0x5F, 0x3F, 0x80, 0x00);    // in wireless
                return false;
            }

        if ( DataMessageIn.packetLength < 14 )
        {
            vReturnToCenterNACK(0x5F, 0x3F, 0x08, 0x00);
            return false;
        }
        else if ( DataMessageIn.packetLength > 14 )
        {
            vReturnToCenterNACK(0x5F, 0x3F, 0x08, DataMessageIn.packetLength - 14);
            return false;
        }

        if(TT >= 3 || TT == 0)
        {
            vReturnToCenterNACK(0x5F, 0x3F, 0x04, 0x01);
            return false;
        }

        //OTMARKPRINTF  printf("Go to Function\n\n");

        if(TC == 3)
            TC = 5;  //Sec
        else if(TC == 4)
            TC = 60;
        else if(TC == 5)
            TC = 300;
        else if(TC == 0)        // send change
            TC = 0;
        else if(TC == 1)
            TC = 1;
        else if(TC == 2)
            TC = 2;
        else if(TC == 0xFF)     // stop send
            TC = 0xFF;

        else
        {
            vReturnToCenterNACK(0x5F, 0x3F, 0x04, 0x02);
            return false;
        }

        if(DataMessageIn.InnerOrOutWard != cComingFromScreen)
        {
            printf("printfMsg go to ACK.\n");
            vReturnToCenterACK(0x5F, 0x3F);
        }
        printf("printfMsg after ACK.\n");

        if(TT == 1)
        {
            smem.vSetINTData(TC92SignalLightStatus_5F0F_IntervalTime, TC);                            //將紀錄間隔設到smem裡
        }
        if(TT == 2)
        {
            smem.vSetINTData(TC92SignalStepStatus_5F03_IntervalTime, TC);                            //將紀錄間隔設到smem裡
        }

        return true;
    }
    catch (...) {}
}

//-------------------------------------------------------------------
bool PTRAFFIC92TC::vTransmitCycleQuery_5F6F(MESSAGEOK DataMessageIn)
{
    try
    {

        short int TT, TC;
        TT = DataMessageIn.packet[9];

        if(TT == 0 || TT > 2)
        {
            vReturnToCenterNACK(0x5F, 0x6F, 0x04, 0x01);
            return false;
        }
        if( DataMessageIn.packetLength < 13 )
        {
            vReturnToCenterNACK(0x5F, 0x6F, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 13 )
        {
            vReturnToCenterNACK(0x5F, 0x6F, 0x08, DataMessageIn.packetLength - 13);
            return false;
        }

        if(TT == 1)
            TC = smem.vGetINTData(TC92SignalLightStatus_5F0F_IntervalTime);
        if(TT == 2)
            TC = smem.vGetINTData(TC92SignalStepStatus_5F03_IntervalTime);

        if(TC == 0)         //系統為傳送變化
            TC = 0;
        else if(TC == 1)
            TC = 1;
        else if(TC == 2)
            TC = 2;
        else if(TC == 5)
            TC = 3;
        else if(TC == 60)
            TC = 4;
        else if(TC == 300)
            TC = 5;
        else if(TC == 0xFF)    //系統的傳送週期為 0 (不傳送)
            TC = 0xFF;

        unsigned char data[6];
        data[0] = 0x5F;
        data[1] = 0xEF;
        data[2] = TT;
        data[3] = TC;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 4, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);
        return true;

    }
    catch(...) {}
}

//OT20111020

/*OT20111107
//-------------------------------------------------------------------
bool PTRAFFIC92TC::vGoToNextPhaseStepControl_5F1C(MESSAGEOK DataMessageIn)
{
try {

    short int siSubPhaseID, siStepID, siEffectTime;
    siSubPhaseID = DataMessageIn.packet[9];
    siStepID = DataMessageIn.packet[10];
    siEffectTime = DataMessageIn.packet[11];

    char msg[256];

    int iReturnCommandSet = smem.vGet0FCommandSet();
    if (iReturnCommandSet <= 2 ) { vReturnToCenterNACK(0x5F, 0x1C, 0x80, 0x00); return false; }

    if ( DataMessageIn.packetLength < 15 ) { vReturnToCenterNACK(0x5F, 0x1C, 0x08, 0x00); return false; }
    else if ( DataMessageIn.packetLength > 15 ) { vReturnToCenterNACK(0x5F, 0x1C, 0x08, DataMessageIn.packetLength - 15); return false; }

    if(siSubPhaseID > 8) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x01); return false; }
    if(siStepID > 8) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x02); return false; }

    DATA_Bit uc5F10_ControlStrategy;
    int i5F10_EffectTime;

    uc5F10_ControlStrategy.DBit = smem.vGetUCData(TC92_ucControlStrategy);
    i5F10_EffectTime = smem.vGetINTData(TC92_iEffectTime);

    memset(msg,0,sizeof(msg));
    sprintf(msg,"[Debug] 5F1C: siSubPhaseID:%d, siStepID:%d, siEffectTime:%d, uc5F10_ControlStrategy:%x, ", siSubPhaseID, siStepID, siEffectTime, uc5F10_ControlStrategy.DBit);
    smem.vWriteMsgToDOM(msg);


    if(uc5F10_ControlStrategy.switchBit.b5 == false && uc5F10_ControlStrategy.switchBit.b6 == false) {
      memset(msg,0,sizeof(msg));
      sprintf(msg,"[Debug] 5F1C: bit 5/6 all false");
      smem.vWriteMsgToDOM(msg);

      vReturnToCenterNACK(0x5F, 0x1C, 0x80, 0x00); return false;
    }

    if(siSubPhaseID == 0 && siStepID == 0 && siEffectTime == 0) {  //Go To Next Step
//      stc.Lock_to_Set_Control_Strategy(STRATEGY_MANUAL);
//      stc.Lock_to_Set_Control`_Strategy(STRATEGY_TOD);

      if (!stc.Lock_to_Set_Control_Strategy(STRATEGY_ALLDYNAMIC)) {
        stc.Lock_to_Set_Next_Step();
      }
      usleep(50);
      int usiCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);

      printf("IN 5F1C, usiCurrentSubphaseStep:%d, Protocal5F1CStopStep:%d\n", usiCurrentSubphaseStep, Protocal5F1CStopStep);

    if(usiCurrentSubphaseStep == 0) {
      if(Protocal5F1CStopStep == usiCurrentSubphaseStep) {
        _intervalTimer.vAllDynamicToTODCount(i5F10_EffectTime);
      }
      else {
        _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_green_time));
      }
    }
    else if(usiCurrentSubphaseStep == 1)
      _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_pedgreenflash_time));
    else if(usiCurrentSubphaseStep == 2) {
      if(Protocal5F1CStopStep == usiCurrentSubphaseStep) {
        _intervalTimer.vAllDynamicToTODCount(i5F10_EffectTime);
      }
      else {
        _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_pedred_time));
      }
    }
    else if(usiCurrentSubphaseStep == 3)
      _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_yellow_time));
    else if(usiCurrentSubphaseStep == 4)
      _intervalTimer.vAllDynamicToTODCount(stc.vGetUSIData(CSTC_exec_plan_allred_time));



//      stc.vLockToSetControlStrategyToManualNotBySignal();
//      usleep(50);
//      stc.vLockToSetControlStrategyToTODNotBySignal();


//OT20111104
      smem.vSet5F18EffectTime(i5F10_EffectTime);

      return true;
    }

    if(siSubPhaseID == 0) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x01); return false; }  //錯誤判斷
    if(siStepID == 0) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x02); return false; }

    unsigned short int usiCSTC_RunningPhaseCurrentPhase = stc.vGetUSIData(CSTC_exec_plan_phase_order);  //Should +1
    unsigned short int usiCSTC_RunningPhaseCurrentSubphase = stc.vGetUSIData(CSTC_exec_phase_current_subphase);  //Should +1
    unsigned short int usiCSTC_RunningPhaseCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);
    stc.Lock_to_Load_Phase_for_Center(usiCSTC_RunningPhaseCurrentPhase);

    return false;

} catch(...) {}
}
*/

//OT20111107
//-------------------------------------------------------------------
bool PTRAFFIC92TC::vGoToNextPhaseStepControl_5F1C(MESSAGEOK DataMessageIn)
{
    try
    {

        short int siSubPhaseID, siStepID, siEffectTime;
        siSubPhaseID = DataMessageIn.packet[9];
        siStepID = DataMessageIn.packet[10];
        siEffectTime = DataMessageIn.packet[11];

        char msg[256];

        int iReturnCommandSet = smem.vGet0FCommandSet();
        if (iReturnCommandSet <= 2 )
        {
            vReturnToCenterNACK(0x5F, 0x1C, 0x80, 0x00);
            return false;
        }

        if ( DataMessageIn.packetLength < 15 )
        {
            vReturnToCenterNACK(0x5F, 0x1C, 0x08, 0x00);
            return false;
        }
        else if ( DataMessageIn.packetLength > 15 )
        {
            vReturnToCenterNACK(0x5F, 0x1C, 0x08, DataMessageIn.packetLength - 15);
            return false;
        }

        if(siSubPhaseID > 8)
        {
            vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x01);
            return false;
        }
        if(siStepID > 8)
        {
            vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x02);
            return false;
        }

        DATA_Bit uc5F10_ControlStrategy;
        int i5F10_EffectTime;

        uc5F10_ControlStrategy.DBit = smem.vGetUCData(TC92_ucControlStrategy);
        i5F10_EffectTime = smem.vGetINTData(TC92_iEffectTime);

        memset(msg,0,sizeof(msg));
        sprintf(msg,"[Debug] 5F1C: siSubPhaseID:%d, siStepID:%d, siEffectTime:%d, uc5F10_ControlStrategy:%x, ", siSubPhaseID, siStepID, siEffectTime, uc5F10_ControlStrategy.DBit);
        smem.vWriteMsgToDOM(msg);


        if(uc5F10_ControlStrategy.switchBit.b5 == false && uc5F10_ControlStrategy.switchBit.b6 == false)
        {
            memset(msg,0,sizeof(msg));
            sprintf(msg,"[Debug] 5F1C: bit 5/6 all false");
            smem.vWriteMsgToDOM(msg);

            vReturnToCenterNACK(0x5F, 0x1C, 0x80, 0x00);
            return false;
        }
        if(smem.vGet5F18EffectTime() <= 0)
        {
            memset(msg,0,sizeof(msg));
            sprintf(msg,"[Debug] 5F18: Effect time Error");
            smem.vWriteMsgToDOM(msg);
            vReturnToCenterNACK(0x5F, 0x1C, 0x80, 0x00);
            return false;
        }

        if(siSubPhaseID == 0 && siStepID == 0 && siEffectTime == 0)    //Go To Next Step
        {
//      stc.Lock_to_Set_Control_Strategy(STRATEGY_MANUAL);
//      stc.Lock_to_Set_Control`_Strategy(STRATEGY_TOD);

//OT20111107      if (!stc.Lock_to_Set_Control_Strategy(STRATEGY_ALLDYNAMIC)) {
            if(1)
            {
                int usiCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);
                if(usiCurrentSubphaseStep <= 2)
                {
                    stc.Lock_to_Set_Next_Step();
                    usleep(50);
                    printf("IN 5F1C, usiCurrentSubphaseStep:%d, Protocal5F1CStopStep:%d\n", usiCurrentSubphaseStep, Protocal5F1CStopStep);
                }
            }

            return true;
        }

        if(siSubPhaseID == 0)
        {
            vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x01);    //錯誤判斷
            return false;
        }
        if(siStepID == 0)
        {
            vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x02);
            return false;
        }

        unsigned short int usiCSTC_RunningPhaseCurrentPhase = stc.vGetUSIData(CSTC_exec_plan_phase_order);  //Should +1
        unsigned short int usiCSTC_RunningPhaseCurrentSubphase = stc.vGetUSIData(CSTC_exec_phase_current_subphase);  //Should +1
        unsigned short int usiCSTC_RunningPhaseCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);
        stc.Lock_to_Load_Phase_for_Center(usiCSTC_RunningPhaseCurrentPhase);

        return false;

    }
    catch(...) {}
}


//OT1000218 Version
//-------------------------------------------------------------------
/*
bool PTRAFFIC92TC::vGoToNextPhaseStepControl_5F1C(MESSAGEOK DataMessageIn)
{
try {

    short int siSubPhaseID, siStepID, siEffectTime;
    siSubPhaseID = DataMessageIn.packet[9];
    siStepID = DataMessageIn.packet[10];
    siEffectTime = DataMessageIn.packet[11];

    char msg[256];

    int iReturnCommandSet = smem.vGet0FCommandSet();
    if (iReturnCommandSet <= 2 ) { vReturnToCenterNACK(0x5F, 0x1C, 0x80, 0x00); return false; }

    if ( DataMessageIn.packetLength < 15 ) { vReturnToCenterNACK(0x5F, 0x1C, 0x08, 0x00); return false; }
    else if ( DataMessageIn.packetLength > 15 ) { vReturnToCenterNACK(0x5F, 0x1C, 0x08, DataMessageIn.packetLength - 15); return false; }

    if(siSubPhaseID > 8) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x01); return false; }
    if(siStepID > 8) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x02); return false; }

    DATA_Bit uc5F10_ControlStrategy;
    int i5F10_EffectTime;

    uc5F10_ControlStrategy.DBit = smem.vGetUCData(TC92_ucControlStrategy);
    i5F10_EffectTime = smem.vGetINTData(TC92_iEffectTime);

    memset(msg,0,sizeof(msg));
    sprintf(msg,"[Debug] 5F1C: siSubPhaseID:%d, siStepID:%d, siEffectTime:%d, uc5F10_ControlStrategy:%x, ", siSubPhaseID, siStepID, siEffectTime, uc5F10_ControlStrategy.DBit);
    smem.vWriteMsgToDOM(msg);


    if(uc5F10_ControlStrategy.switchBit.b5 == false && uc5F10_ControlStrategy.switchBit.b6 == false) {
      memset(msg,0,sizeof(msg));
      sprintf(msg,"[Debug] 5F1C: bit 5/6 all false");
      smem.vWriteMsgToDOM(msg);

      vReturnToCenterNACK(0x5F, 0x1C, 0x80, 0x00); return false;
    }

    if(siSubPhaseID == 0 && siStepID == 0 && siEffectTime == 0) {  //Go To Next Step
//      stc.Lock_to_Set_Control_Strategy(STRATEGY_MANUAL);
//      stc.Lock_to_Set_Control`_Strategy(STRATEGY_TOD);


//OT1000218
    int usiCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);
    if(usiCurrentSubphaseStep <= 2) {
      stc.Lock_to_Set_Next_Step();
    }

    usleep(50);
    usiCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);

    memset(msg,0,sizeof(msg));
    sprintf(msg,"[Debug] 5F1C: new usiCurrentSubphaseStep:%d", usiCurrentSubphaseStep);
    smem.vWriteMsgToDOM(msg);

    //OT1000218

      return true;
    }

    if(siSubPhaseID == 0) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x01); return false; }  //錯誤判斷
    if(siStepID == 0) { vReturnToCenterNACK(0x5F, 0x1C, 0x04, 0x02); return false; }

    unsigned short int usiCSTC_RunningPhaseCurrentPhase = stc.vGetUSIData(CSTC_exec_plan_phase_order);  //Should +1
    unsigned short int usiCSTC_RunningPhaseCurrentSubphase = stc.vGetUSIData(CSTC_exec_phase_current_subphase);  //Should +1
    unsigned short int usiCSTC_RunningPhaseCurrentSubphaseStep = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step);
    stc.Lock_to_Load_Phase_for_Center(usiCSTC_RunningPhaseCurrentPhase);

    return false;

} catch(...) {}
}
*/


//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryPhaseStepControl_5F4C(MESSAGEOK DataMessageIn)
{
    try
    {

//For Amegids of NCKU, not Ver3 Protocal
//  unsigned short int usiEffectTime = _intervalTimer.vGetEffectTime();
        unsigned short int usiStepSec = stc.vGetStepTime();

        unsigned char data[6];
        data[0] = 0x5F;
        data[1] = 0xCC;
        data[2] = smem.vGetUCData(TC92_ucControlStrategy);
        data[3] = stc.vGetUSIData(CSTC_exec_phase_current_subphase) + 1;
        data[4] = stc.vGetUSIData(CSTC_exec_phase_current_subphase_step)  + 1;

//  stc.vGetStepTime();

//  if( stc.Lock_to_LoadControlStrategy == STRATEGY_ALLDYNAMIC )

//For Amegids of NCKU, not Ver3 Protocal
//    data[5] = (usiEffectTime>>8) & 0x00ff;
//    data[6] = usiEffectTime & 0x00ff;
        data[5] = (usiStepSec>>8) & 0x00ff;
        data[6] = usiStepSec & 0x00ff;

//  else
//    data[4] = 0;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 7, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);
        return true;

    }
    catch(...) {}
}

//OT20140415
//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vSetDynSegParameter(MESSAGEOK DataMessageIn)
{
    try
    {
        unsigned char ucAdjType;
        unsigned short int usiAdjSec;
        ucAdjType = DataMessageIn.packet[9];
        usiAdjSec = DataMessageIn.packet[10]*256 + DataMessageIn.packet[11];
        smem.vSetUCData(TCDynSegAdjustType, ucAdjType);
        smem.vSetUSIData(TCDynSegAdjustSec, usiAdjSec);
        smem.vSetUSIData(TCDynSegRemainSec, usiAdjSec);

        vReturnToCenterACK(0x5F, 0x1A);
        return true;

    }
    catch(...) {}
}
//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vQueryDynSegParameter(MESSAGEOK DataMessageIn)
{
    try
    {

        unsigned char ucAdjType = smem.vGetUCData(TCDynSegAdjustType);
        unsigned short int usiAdjSec = smem.vGetUSIData(TCDynSegAdjustSec);
        unsigned short int usiRemainSec = smem.vGetUSIData(TCDynSegRemainSec);
        bool bSW = smem.vGetBOOLData(TCDynSegSwitch);
        bool bStatus = smem.vGetBOOLData(TCDynSegStatus);
        unsigned short int usiPlanCycle = stc.vGetUSIData(CSTC_exec_plan_cycle);
        time_t now = smem.vGetSystemTime();

        unsigned char data[15];
        data[0] = 0x5F;
        data[1] = 0xCA;
        data[2] = ucAdjType;
        data[3] = usiAdjSec / 256;
        data[4] = usiAdjSec % 256;
        data[5] = bSW;
        data[6] = bStatus;
        data[7] = usiRemainSec % 256;
        data[8] = usiRemainSec % 256;

        data[9] = usiPlanCycle % 256;
        data[10] = usiPlanCycle % 256;

        data[11] = ( now >> 24) & 0xff;
        data[12] = ( now >> 16) & 0xff;
        data[13] = ( now >> 8)  & 0xff;
        data[14] = ( now ) & 0xff;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 15, true);
        _MsgOK.InnerOrOutWard = cOutWard;
        //  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);
        return true;

    }
    catch(...) {}
}
//---------------------------------------------------------------------------
bool PTRAFFIC92TC::vSetDynSegSwitch(MESSAGEOK DataMessageIn)
{
    try
    {
        unsigned char ucSW;
        ucSW = DataMessageIn.packet[9];
        if(ucSW > 0)
        {
            smem.vSetBOOLData(TCDynSegSwitch, true);
        }
        else
        {
            smem.vSetBOOLData(TCDynSegSwitch, false);
        }
        vReturnToCenterACK(0x5F, 0x1B);
        return true;
    }
    catch(...) {}
}



//ACK------------------------------------------------------------------------
int PTRAFFIC92TC::vReturnToCenterACK(unsigned char ucDevCode,
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
int PTRAFFIC92TC::vReturnToCenterNACK(unsigned char ucDevCode,
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


bool PTRAFFIC92TC::vWriteReverseTimeData5F11_revAPP(MESSAGEOK DataMessageIn)
{
    try
    {
        unsigned char data[31];

        int iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level
        if(iReturnCommandSet <= 1)
        {
            vReturnToCenterNACK(0x5F, 0x11, 0x80, 0x00);    // not include level "A""
            return false;
        }

        // here have error, should check first.
        vReturnToCenterACK(0x5F, 0x11);

        data[0]=0x5f;
        data[1]=0xf1;

        int i=0;

//  stc.Lock_to_Reset_Segment_for_Center(DataMessageIn.packet[9],DataMessageIn.packet[10]);
//  stc.Lock_to_Load_WeekDaySegment_for_Center();

// need check first
        stc._for_center_reversetime.ucNonRevLight = 0;

        data[2]=stc._for_center_reversetime.usiDirectIn    = DataMessageIn.packet[2];
        data[3]=stc._for_center_reversetime.usiHourStartIn = DataMessageIn.packet[3];
        data[4]=stc._for_center_reversetime.usiMinStartIn  = DataMessageIn.packet[4];
        data[5]=stc._for_center_reversetime.usiHourEndIn   = DataMessageIn.packet[5];
        data[6]=stc._for_center_reversetime.usiMinEndIn    = DataMessageIn.packet[6];

        data[7]=stc._for_center_reversetime.usiDirectOut    = DataMessageIn.packet[7];
        data[8]=stc._for_center_reversetime.usiHourStartOut = DataMessageIn.packet[8];
        data[9]=stc._for_center_reversetime.usiMinStartOut  = DataMessageIn.packet[9];
        data[10]=stc._for_center_reversetime.usiHourEndOut   = DataMessageIn.packet[10];
        data[11]=stc._for_center_reversetime.usiMinEndOut    = DataMessageIn.packet[11];

        /*  data[12]=stc._for_center_reversetime.usiClearTime    = DataMessageIn.packet[12];//2017 07 01 kaochu
          data[13]=stc._for_center_reversetime.usiFlashGreen = DataMessageIn.packet[13];
          data[14]=stc._for_center_reversetime.usiGreenTime  = DataMessageIn.packet[14];*/



        stc._for_center_reversetime.usiClearTime    = DataMessageIn.packet[12]*10;
        stc._for_center_reversetime.usiFlashGreen = DataMessageIn.packet[13]*10;
        stc._for_center_reversetime.usiGreenTime  = DataMessageIn.packet[14]*10;


        data[15]=stc._for_center_reversetime._reverse_time_type  = DataMessageIn.packet[15];

        if(stc._for_center_reversetime.usiHourStartIn*60+stc._for_center_reversetime.usiMinStartIn != 0 || stc._for_center_reversetime.usiHourEndIn*60+stc._for_center_reversetime.usiMinEndIn != 0)
        {
            if(stc._for_center_reversetime.usiHourStartOut*60+stc._for_center_reversetime.usiMinStartOut == 0 && stc._for_center_reversetime.usiHourEndOut*60+stc._for_center_reversetime.usiMinEndOut == 0)
            {
                stc._for_center_reversetime.ucNonRevLight = 1;
            }
        }
        else if(stc._for_center_reversetime.usiHourStartOut*60+stc._for_center_reversetime.usiMinStartOut != 0 || stc._for_center_reversetime.usiHourEndOut*60+stc._for_center_reversetime.usiMinEndOut != 0)
        {
            if(stc._for_center_reversetime.usiHourStartIn*60+stc._for_center_reversetime.usiMinStartIn == 0 && stc._for_center_reversetime.usiHourEndIn*60+stc._for_center_reversetime.usiMinEndIn == 0)
            {
                stc._for_center_reversetime.ucNonRevLight = 2;
            }
        }
        else      //unknow type.
        {
            stc._for_center_reversetime.ucNonRevLight = 0;
        }


        stc.Lock_to_Save_ReverseTime_Step1(0);


//OTMARKPRINTF  printf("segment_type%d\n",stc._for_center_segment._segment_type);
//OTMARKPRINTF  printf("segment_Count%d\n",stc._for_center_segment._segment_count);


        int numWeekDay = DataMessageIn.packet[16];
        data[16]=DataMessageIn.packet[16];
//OTMARKPRINTF  printf("numWeekDay:%d\n",numWeekDay);

        for (int j=0; j<numWeekDay; j++)
        {

            //OTMARKPRINTF  printf("%3x--%d\n",DataMessageIn.packet[12+3*i+j],stc._for_center_segment._segment_type);

            switch (DataMessageIn.packet[17+j])
            {
            case 0x01:
                stc._for_center_weekdayrev[0]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x02:
                stc._for_center_weekdayrev[1]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x03:
                stc._for_center_weekdayrev[2]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x04:
                stc._for_center_weekdayrev[3]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x05:
                stc._for_center_weekdayrev[4]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x06:
                stc._for_center_weekdayrev[5]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x07:
                stc._for_center_weekdayrev[6]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x0b:
                stc._for_center_weekdayrev[7]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x0c:
                stc._for_center_weekdayrev[8]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x0d:
                stc._for_center_weekdayrev[9]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x0e:
                stc._for_center_weekdayrev[10]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x0f:
                stc._for_center_weekdayrev[11]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x10:
                stc._for_center_weekdayrev[12]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            case 0x11:
                stc._for_center_weekdayrev[13]._reverse_time_type=stc._for_center_reversetime._reverse_time_type;
                break;
            default:
                break;
            }
        }
        for(int i=0; i<14; i++ )
            data[17+i]=stc._for_center_weekdayrev[i]._reverse_time_type;





//  stc.Lock_to_Save_Segment_from_Center();
        stc.Lock_to_Save_WeekDayReversetime_from_Center();


        /*       MESSAGEOK _MsgOK;

               _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, 31,true);
               _MsgOK.InnerOrOutWard = cOutWard;
        //    writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
               writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);*/

        return true;

    }
    catch (...) {}
}

bool PTRAFFIC92TC::vWriteReverseTimeData5F12_revAPP(MESSAGEOK DataMessageIn)
{
    try
    {

        int iReturnCommandSet = smem.vGet0FCommandSet();                              //Get Oper Level
        if(iReturnCommandSet <= 1)
        {
            vReturnToCenterNACK(0x5F, 0x12, 0x80, 0x00);    // not include level "A""
            return false;
        }

        // here have error, should check first.
        vReturnToCenterACK(0x5F, 0x12);


        int i=0;

//  stc.Lock_to_Reset_Segment_for_Center(DataMessageIn.packet[9],DataMessageIn.packet[10]);
//  stc.Lock_to_Load_WeekDaySegment_for_Center();

// need check first

        stc._for_center_reversetime.usiDirectIn    = DataMessageIn.packet[2];
        stc._for_center_reversetime.usiHourStartIn = DataMessageIn.packet[3];
        stc._for_center_reversetime.usiMinStartIn  = DataMessageIn.packet[4];
        stc._for_center_reversetime.usiHourEndIn   = DataMessageIn.packet[5];
        stc._for_center_reversetime.usiMinEndIn    = DataMessageIn.packet[6];

        stc._for_center_reversetime.usiDirectOut    = DataMessageIn.packet[7];
        stc._for_center_reversetime.usiHourStartOut = DataMessageIn.packet[8];
        stc._for_center_reversetime.usiMinStartOut  = DataMessageIn.packet[9];
        stc._for_center_reversetime.usiHourEndOut   = DataMessageIn.packet[10];
        stc._for_center_reversetime.usiMinEndOut    = DataMessageIn.packet[11];

        stc._for_center_reversetime.usiClearTime    = DataMessageIn.packet[12];
        stc._for_center_reversetime.usiFlashGreen = DataMessageIn.packet[13];
        stc._for_center_reversetime.usiGreenTime  = DataMessageIn.packet[14];

        stc._for_center_reversetime._reverse_time_type  = DataMessageIn.packet[15];

        stc.Lock_to_Save_ReverseTime_Step1(0);                                        // (0) means for center

        stc._for_center_holidayrev._reverse_time_type  = DataMessageIn.packet[15];
        stc._for_center_holidayrev._start_year  = DataMessageIn.packet[16] + 1911;
        stc._for_center_holidayrev._start_month = DataMessageIn.packet[17];
        stc._for_center_holidayrev._start_day   = DataMessageIn.packet[18];
        stc._for_center_holidayrev._end_year    = DataMessageIn.packet[19] + 1911;
        stc._for_center_holidayrev._end_month   = DataMessageIn.packet[20];
        stc._for_center_holidayrev._end_day     = DataMessageIn.packet[21];

        stc.Lock_to_Save_HoliDayReverseTime_Step1(0);

        return true;

    }
    catch (...) {}
}
bool PTRAFFIC92TC::vQueryReverseTimeData5F41_revAPP(MESSAGEOK DataMessageIn)
{
    try
    {
        unsigned char data[256];

        unsigned char ucQueryReverseTimeType = DataMessageIn.packet[2];
        unsigned char ucWeekDay = DataMessageIn.packet[3];
        /*
                if( DataMessageIn.packetLength < 14)
                {
                    vReturnToCenterNACK(0x5F, 0x41, 0x08, 0x00);
                    return false;
                }
                else if( DataMessageIn.packetLength > 14)
                {
                    vReturnToCenterNACK(0x5F, 0x41, 0x08, DataMessageIn.packetLength - 12);
                    return false;
                }
        */
        if(ucQueryReverseTimeType == 0xFF)
        {
            stc.Lock_to_Load_ReverseTime_Step1(2, ucWeekDay);
        }
        else
        {

            /*
            bSegStatus = smem.vGetTCPhasePlanSegTypeData(TC_SegType, iQuerySegmentType);
            if(bSegStatus == false) {
              vReturnToCenterNACK(0x5F, 0x46, 0x02, 0x0); return false;
            }
            */
            stc.Lock_to_Load_ReverseTime_Step1(0, ucQueryReverseTimeType);              //Zero for center
        }

        stc.Lock_to_Load_WeekDayReverseTime_for_Center();
        ;
        int iDataPtr;
        int iNumWeekDayPrt = 0;
        int iNumWeekDay = 0;
        data[0] = 0x5F;
        data[1] = 0xC1;
        data[2] = stc._for_center_reversetime.usiDirectIn;
        data[3] = stc._for_center_reversetime.usiHourStartIn;
        data[4] = stc._for_center_reversetime.usiMinStartIn;
        data[5] = stc._for_center_reversetime.usiHourEndIn;
        data[6] = stc._for_center_reversetime.usiMinEndIn;
        data[7] = stc._for_center_reversetime.usiDirectOut;
        data[8] = stc._for_center_reversetime.usiHourStartOut;
        data[9] = stc._for_center_reversetime.usiMinStartOut;
        data[10] = stc._for_center_reversetime.usiHourEndOut;
        data[11] = stc._for_center_reversetime.usiMinEndOut;

        /* data[12] = (stc._for_center_reversetime.usiClearTime );//2017 07 01 rev kaochu
         data[13] = (stc._for_center_reversetime.usiFlashGreen );
         data[14] = (stc._for_center_reversetime.usiGreenTime );*/

        data[12] = (stc._for_center_reversetime.usiClearTime / 10);
        data[13] = (stc._for_center_reversetime.usiFlashGreen / 10);
        data[14] = (stc._for_center_reversetime.usiGreenTime / 10);
        data[15] = stc._for_center_reversetime._reverse_time_type;

        iNumWeekDayPrt = 16;
        iDataPtr = 16;

        if(stc._for_center_weekdayrev[0]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x01;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[1]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x02;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[2]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x03;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[3]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x04;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[4]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x05;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[5]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x06;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[6]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x07;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[7]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x0b;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[8]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x0c;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[9]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x0d;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[10]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x0e;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[11]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x0f;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[12]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x10;
            iNumWeekDay++;
            iDataPtr++;
        }
        if(stc._for_center_weekdayrev[13]._reverse_time_type == ucQueryReverseTimeType)
        {
            data[iDataPtr] = 0x11;
            iNumWeekDay++;
            iDataPtr++;
        }
        data[iNumWeekDayPrt] = iNumWeekDay;

        MESSAGEOK _MsgOK;
        _MsgOK = oDataToMessageOK.vPackageINFOTo92Protocol(data, iDataPtr, true);
        _MsgOK.InnerOrOutWard = cOutWard;
//  writeJob.WriteWorkByMESSAGEOUT(_MsgOK);
        // writeJob.WritePhysicalOut(_MsgOK.packet, _MsgOK.packetLength, DEVICECENTER92);

        BYTE revAPP_packet[16];
        for(int i=0; i<16; i++)
        {
            revAPP_packet[i]=data[i];
        }
        writeJob.WritePhysicalOut(revAPP_packet,16,revAPP);
        printf("hello~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        for(int i=0; i<16; i++)
            printf("%x",revAPP_packet[i]);
        return true;
    }
    catch (...) {}
}
bool PTRAFFIC92TC::vQueryReverseTimeData5F42_revAPP(MESSAGEOK DataMessageIn)
{
    try
    {
        unsigned char data[256];

        unsigned char ucQueryReverseTimeType = DataMessageIn.packet[2];

        if( DataMessageIn.packetLength < 13)
        {
            vReturnToCenterNACK(0x5F, 0x42, 0x08, 0x00);
            return false;
        }
        else if( DataMessageIn.packetLength > 13)
        {
            vReturnToCenterNACK(0x5F, 0x42, 0x08, DataMessageIn.packetLength - 12);
            return false;
        }

        stc.Lock_to_Load_HoliDayReverseTime_for_Center(ucQueryReverseTimeType);

        ;
        int iDataPtr;
        int iNumWeekDayPrt = 0;
        int iNumWeekDay = 0;
        data[0] = 0x5F;
        data[1] = 0xC2;
        data[2] = stc._for_center_reversetime.usiDirectIn;
        data[3] = stc._for_center_reversetime.usiHourStartIn;
        data[4] = stc._for_center_reversetime.usiMinStartIn;
        data[5] = stc._for_center_reversetime.usiHourEndIn;
        data[6] = stc._for_center_reversetime.usiMinEndIn;
        data[7] = stc._for_center_reversetime.usiDirectOut;
        data[8] = stc._for_center_reversetime.usiHourStartOut;
        data[9] = stc._for_center_reversetime.usiMinStartOut;
        data[10] = stc._for_center_reversetime.usiHourEndOut;
        data[11] = stc._for_center_reversetime.usiMinEndOut;

        data[12] = stc._for_center_reversetime.usiClearTime;
        data[13] = stc._for_center_reversetime.usiFlashGreen;
        data[14] = stc._for_center_reversetime.usiGreenTime;
        data[15] = stc._for_center_reversetime._reverse_time_type;

        data[16] = stc._for_center_holidayrev._start_year - 1911;
        data[17] = stc._for_center_holidayrev._start_month;
        data[18] = stc._for_center_holidayrev._start_day;
        data[19] = stc._for_center_holidayrev._end_year - 1911;
        data[20] = stc._for_center_holidayrev._end_month;
        data[21] = stc._for_center_holidayrev._end_day;

        writeJob.WritePhysicalOut(data,22, revAPP);

        return true;
    }
    catch (...) {}
}
bool PTRAFFIC92TC::revseg_send_to_revAPP()
{
    try
    {
        BYTE revAPP_weekdayseg[15];
        CWeekDayRevType weekdayrev[AMOUNT_WEEKDAY_REV];
        FILE *reversetime_rfile = NULL;
        char filename[36];



        sprintf( filename,"/Data/WeekDayRevType.bin\0" );
        reversetime_rfile = fopen(filename, "r"); //fopen return NULL if file not exist

        if(reversetime_rfile)
        {

            printf("Reading File WeekDayRevType\n");

            for(int i=0; i<AMOUNT_WEEKDAY_REV; i++)
            {
                fread( &weekdayrev[i]._reverse_time_type, sizeof( unsigned short int ), 1, reversetime_rfile );
                fread( &weekdayrev[i]._weekday,           sizeof( unsigned short int ), 1, reversetime_rfile );
                if(weekdayrev[i]._reverse_time_type>7)
                    perror("ERROR: WeekDayReverseTimeData not in {0-7}\n");
                if(    (i< 7 && (i!=weekdayrev[i]._weekday-1))
                        || (i>=7 && (i!=weekdayrev[i]._weekday-4)) )
                    perror("ERROR: WeekDayFile WeekDay != weekdayrev[i]._weekday\n");
//==          printf("  i:%2d _segment_type:%2d _segment_weekday:%2d\n",i,weekdayseg[i]._segment_type,weekdayseg[i]._weekday);
                if     (i< 7) weekdayrev[i]._weekday=(i+1);
                else if(i>=7) weekdayrev[i]._weekday=(i+4);
            }
            fclose( reversetime_rfile );
        }
        else perror("ERROR: Missing File WeekDayRevType");





        revAPP_weekdayseg[0]=0xab;//let revAPP know TC send rev_seg to it.
        for(int i=0; i<AMOUNT_WEEKDAY_REV; i++)
        {

            revAPP_weekdayseg[i+1] =weekdayrev[i]._reverse_time_type;

        }

        for(int i=0; i<AMOUNT_WEEKDAY_REV; i++)
        {

            printf("%x",revAPP_weekdayseg[i+1]) ;

        }
        writeJob.WritePhysicalOut(revAPP_weekdayseg,15,revAPP);
        return true;
    }
    catch(...) {}
}


