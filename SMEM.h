#ifndef SMEMH
#define SMEMH

#include "var.h"
#include "STORAGE.h"
#include "RS232.h"
#include "UDP.h"
#include "junbo_light_control.h"
#include "junli_vd.h"
#include "junbo_cms.h"
#include "power_reboot_group.h"
#include "protocol_9F_m_curve.h"
#include "junbo_lane_adj_light.h"
#include <pthread.h>
//---------------------------------------------------------------------------
typedef struct MESSAGEWAITREPONSE
{
    int sequence;
    BYTE packet[BUFFERSIZE];
    int length;
    int retryCount;
    time_t sendTime;
} MESSAGEWAITREPONSE;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class light_time_control
{
public:
    light_time_control(void);
    ~light_time_control(void);
    bool text_ID_control(int text_ID);
    bool read_textID();
    int light_flash_time;
    bool store_time(int time_sec);
    bool read_time();
    int text_ID;
    bool read_cms_mark_object();
    bool write_cms_mark_object(cms_mark object);
    cms_mark get_cms_record();



private:
    cms_mark cms_record_object;

};


class SMEM
{
public:
    SMEM(void);
    ~SMEM(void);
    junbo_light_control o_Junbo_light;

    light_time_control light_time;
    bool _0F80_packet(BYTE PROTOCOL_TYPE[2]);

    typedef  struct junbo_to_light
    {
        unsigned char ID;
        unsigned char command;
        unsigned char parameter;
    };
    junbo_to_light record_light[9],record_state[3][4],record_timeout[9],record_brightness[9];
    int LAS_week_type_info[8];
    LAS_YMD specialtype[28];

   junbo_lane_adj_memory_object Lane_adj_memo_object;

    s_junbo_lane_adj
    lane_adj_brightness_record[9],
    lane_adj_light_record[9],
    lane_adj_timeout_record[9],
    lane_adj_module_state[9];

   int lane_adj_run_state[9];

    int segmenttype_8f;
    LAS_excute_info LAS_segmenttype[21];//1~3 common day, 7-20 special day, 0 no info.


    // LSC=  Lane adjusted sign controler

 protocol_9F_m_curve protocol_9F_object;
    junli_vd junli_object;
    junbo_cms junbo_object;
    power_reboot_group power_object;
    RS232 power_port;
    UDP revAPP_socket;
junbo_lane_adj_light junbo_LASC_object;
    int count_vd_alive;
    STORAGE disk;                                                           //寫檔案用
    /*OTCombo0713*/
//        RS232 centerPort,lightPort,ssPort,keypadPort,testerPort,rtmsPort;       //交控中心,Light,SmartSensor,面板,手提測試機,RTMS
    RS232 centerPort,testerPort,keypadPort,lightPort;
    RS232 redCountPort;                                                     //紅燈倒數
    RS232 NMEAPort;                                                         //GPS Com2
    RS232 passingPort;
//OTSS++
    RS232 ssPort;
    RS232 com2Port;

    RS232 revLaneManualPort;

    UDP revSyncSocket;

//OTSS++
    UDP centerSocket,testerSocket,tempSocket;                               //交控中心,手提測試機,保留
//        UDP VDBcastForListen5003Socket;                                         //VD廣撥Socket
    UDP DynCalServerInCrossSocket, DynCalServerInCenterSocket;              //全動態計算主機
    UDP SSVD01Socket, SSVD02Socket, SSVD03Socket, SSVD04Socket;

    //OT Pass
    UDP SSVD05Socket, SSVD06Socket, SSVD07Socket, SSVD08Socket, SSVD09Socket, SSVD10Socket,
    SSVD11Socket, SSVD12Socket, SSVD13Socket, SSVD14Socket, SSVD15Socket,
    SSVD16Socket, SSVD17Socket, SSVD18Socket, SSVD19Socket, SSVD20Socket,
    SSVD21Socket, SSVD22Socket, SSVD23Socket, SSVD24Socket, SSVD25Socket,
    SSVD26Socket, SSVD27Socket, SSVD28Socket, SSVD29Socket, SSVD30Socket;
//            SSVD31Socket, SSVD32Socket, SSVD33Socket, SSVD34Socket, SSVD35Socket,
//            SSVD36Socket, SSVD37Socket, SSVD38Socket, SSVD39Socket, SSVD40Socket;

    /* OT ADD 950927 */
    UDP CCJDynCtlSocket;
    UDP CCJDynCtl2Socket;

    UDP amegidsSocket;

    int GetcFace(void);                                                     //得到目前LCD上是哪個畫面
    int GetLastFace(void);                                                  //得到上次LCD上是哪個畫面
    int GetAddress(void);                                                   //得到目前路口編號
    int GetOperMode(void);                                                  //得到目前操作模式 1:遠端操作 0:現場操作
    int GetSequence(void);                                                  //得到機器上的流水編號
    bool GetKeypadReturn(void);                                             //得到keypad是否有回傳了
    char * GetPassword(void);                                               //得到控制器密碼
    int GetDbOperStat(void);                                                //得到資料庫操作設定(0:允許現場操作,1:不允許現場操作,2:只允許現場查看)
    bool GetSSComm(void);                                                   //得到與SmartSensor的通訊

    bool SetcFace(int);                                                     //設定目前LCD是哪個畫面
    bool SetLastFace(int);                                                  //設定上次LCD是哪個畫面
    bool SetAddress(int);                                                   //設定目前路口編號
    bool SetOperMode(int);                                                  //設定目前操作模式 1:遠端操作 0:現場操作
    bool SetKeypadReturn(bool);                                             //設定keypad是否有回傳了
    bool SetPassword(char,char,char,char,char,char);                        //設定控制器密碼
    bool SetDbOperStat(int);                                                //設定資料庫操作設定(0:允許現場操作,1:不允許現場操作,2:只允許現場查看)
    bool SetSSComm(bool);                                                   //設定與SmartSensor的通訊

    bool SetWaitMsg(int,BYTE *,int);                                        //記錄要等待回應的msg
    bool ClearWaitMsg(int);                                                 //消掉等候回應的msg

    time_t GetLastKeypadTime(void);                                         //得到最後一次按鍵盤的時間
    bool SetLastKeypadTime(void);                                           //設定最後一次按鍵盤的時間

    time_t GetStartTime(void);                                              //得到此程式啟動時間
    time_t GetCurrentTime(void)
    {
        return time(NULL);
    }


    struct itimerspec _ShareSimCycle;

    bool vSaveShareMemoryDataToDOM(void);

    bool vSetSimIntervalTime(itimerspec);
    bool bSimIntervalTimeChange;
    bool vGetSimIntervalTimeChangeStatus(void);
    bool vSetSimIntervalTimeChangeStatus(bool);

    bool vSet0FHardwareCycle(itimerspec);
    itimerspec _HWCycleReportInterval;
    bool vGet0FHardwareCycleChangeStatus(void);
    bool bHWCycleReportIntervalChange;
    bool vSet0FHardwareCycleChangeStatus(bool);
    itimerspec vGet0FHardwareCycle(void);
    bool vSetHWCycleCodeFor_0F14_0FC4(unsigned char);
    unsigned char vGetHWCycleCodeFor_0F14_0FC4(void);
    unsigned char HWCycleCode;


    unsigned char vGetHardwareStatus(int);                                 //只有vGet沒有vSet，故意的，由vGet自行判斷


    unsigned char ucCommandSet;                                             //for 0F should save
    bool vSet0FCommandSet(unsigned char);
    unsigned char vGet0FCommandSet(void);

    time_t vGetSSCommTime(void);

    DATA_Bit _DIOByte;
    DATA_Bit Get_DIOByte(void);

    SmartSensorINFO _SSInfoInShareMem;
    bool vSyncSSInfo(SmartSensorINFO);                                      //把SmartSensor之S0的Info紀錄到smem
    /*new add */
    SmartSensorINFO vGetSSInfo(void);                                       //讀出S0data

    bool vSetLastResetTime(long);                                           //將DOM紀錄之最後時間寫入smem
    YMDHMS vGetLastResetTime(void);                                         //從smem讀出上次關機之時間

    int vGetConnectedSendIntervalTime(void);

    bool vGetCenterConnectOldStatus(void);                                  //上次連線狀態


    bool vCheckConnectStatus(void);

    bool bConnectWithCenter;                                                //紀錄與比較用的 old
    bool bConnectWithCenterStatus;                                          //連線中心的status
    bool vLoadCenterConnectStatus(void);
    bool vSaveCenterConnectStatus(bool);
    bool vSetTimeInterVal(int, int);
//OT Debug 0706        bool vSyncSSTimeInterVal(void);
    bool GetConnectCenterStatus(void);

    bool vGet0F11CommuncationResetChangeStatus();
    bool vSet0F11CommuncationResetChangeStatus(bool);
    bool b0F11CommuncationResetChangeStatus;

    bool vGet0F16LockDBRequestStatus();
    bool vSet0F16LockDBRequestStatus(bool);
    bool b0F16LockDBRequestStatus;


    bool vSet_DIOByte(DATA_Bit);

    DATA_Bit _OldDIOByte;
    DATA_Bit Get_OldDIOByte(void);

    itimerspec vReadSimIntervalTime(void);


    bool vWriteMsgToDOM(char *);
    bool vWriteReverseLog(char *);      //jacky20140507

    bool vSaveLastAliveStatus();                                             //當SS斷線時，用interval來紀錄時間toDOM
    int vGetFirmwareInfo_0FC3(int);

    int vGetAdjudicateReturnAddressBCDorHEX(void);
    bool vSetAdjudicateReturnAddressBCDorHEX(int);

    //OTCombo
    unsigned char vGetUCData(int);
    int vGetINTData(int);

    bool vSetUCData(const int , const unsigned char);
    bool vSetINTData(const int, const int);

//OT Debug 0523
    bool vSetUSIData(const int, const unsigned short int);
    unsigned short int vGetUSIData(const int);

    bool vSetBOOLData(const int, const bool);
    bool vGetBOOLData(const int);

    bool vGetTCPhasePlanSegTypeData(const int, const int);
    bool vSetTCPhasePlanSegTypeData(const int, const int, const bool);

    tsUDPMappingLCN vGetUDPMappingLCNData(int);

    unsigned short int vGetWayMappingRedCount(unsigned short int);
    unsigned short int vSetWayMappingRedCount(unsigned short int, unsigned short int);

//OT Pass
    bool vSetACK_W2W(unsigned char, unsigned char);
    unsigned char vGetACK_W2W(unsigned char);
    bool vSetPassMode(unsigned char);
    unsigned char vGetPassMode(void);
    bool vSetPassServerLCN(unsigned short int);
    unsigned short int vGetPassServerLCN(void);


//---------------------------------------------------------------------------
//OT Debug Direct
    unsigned short int vGetSignamMapMappingDir(unsigned short int);
    unsigned short int vSetSignamMapMappingDir(unsigned short int, unsigned short int);
    unsigned short int vGetSignamMapMappingLightBoard(unsigned short int);
    unsigned short int vSetSignamMapMappingLightBoard(unsigned short int, unsigned short int);

    unsigned short int vGetScreenCopyPlanID(void);
    bool vSetScreenCopyPlanID(unsigned short int);
    unsigned short int vGetScreenCopySegID(void);
    bool vSetScreenCopySegID(unsigned short int);

    tsUDPMappingLCN vGetCCJDynCtlIPData(void);
    tsUDPMappingLCN vGetCCJDynCtl2IPData(void);

    bool vReadLCNFromDisk(void);

    bool vSaveLast92TC_5F15Time(void);
    bool vLoadLast92TC_5F15Time(void);
    YMDHMS vGetLast92TC_5F15Time(void);

    bool vCmpGreenConflict(void);

    sChildChain vGetChildChainStruct(void);
    bool vSetChildChainStruct(sChildChain);
    bool vClearChildChainStruct(void);

    int vGetChainOffset(const int, const int);
    bool vSetChainOffset(const int, const int, short int);
    bool vWriteDIO(unsigned char);
    unsigned char vGetWriteDIO(void);

    bool vWriteChainDataFromStroage(void);
    bool vReadChainDataFromStroage(void);

    //---------------------------------------------------------------------------
    bool SetLocalIP1(int,int);
    bool SetdistIp0(int,int);
    bool SetDistIP(int,int);
    bool SetNetmask(int, int);
    bool SetGateway(int, int);
    int GetLocalIP1(int);
    int GetdistIp0(int);
    int GetDistIP(int);
    int GetNetmask(int);
    int GetGateway(int);

    void vSendRequestToKeypad(void);

    bool vSetMotherBoardType(unsigned char);
    unsigned char vGetMotherBoardType(void);

    bool vSetActuatePhaseExtend(unsigned char);
    unsigned char vGetActuatePhaseExtend(void);

    bool vSendTimerUpdateToCCJ_5F9E(void);

    //For TC
    bool vSetVDUpdateDB(int, bool);
    bool vGetVDUpdateDB(int);

    bool vSetSignalConflictErrorVar(unsigned char, unsigned char, unsigned char);
    bool vGetSignalConflictErrorVar(unsigned char *);

    bool vTrafficLightSendAndCheck(void);
    bool vTrafficLightAckAdd(void);

    time_t GetRevManualPadTime(void);
    bool SetRevManualPadTime(void);

    bool vSetTimerMutexCSTC(unsigned char);
    unsigned char vGetTimerMutexCSTC(void);
    bool vSetTimerMutexCTIMER(unsigned char);
    unsigned char vGetTimerMutexCTIMER(void);
    bool vSetTimerMutexRESET(unsigned char);
    unsigned char vGetTimerMutexRESET(void);

    unsigned char vGetW77E58FWVer(unsigned char);
    bool vSetW77E58FWVer(unsigned char, unsigned char);

    bool vSetSystemClockTime(unsigned short int,                            //Year,month,day,hour,min,sec
                             unsigned char,
                             unsigned char,
                             unsigned char,
                             unsigned char,
                             unsigned char
                            );

    bool vSetVDPhaseMapTable(unsigned char, unsigned char, unsigned short int);
    unsigned short int vGetVDPhaseMapTable(unsigned char, unsigned char);
    bool vSetVDPhaseTriggerSwitch(unsigned char, unsigned char);
    unsigned char vGetVDPhaseTriggerSwitch(unsigned char);

    bool vSetForceClockRedCountOneTime(bool);
    bool vGetForceClockRedCountOneTime(void);

    bool vSetStopSend0x22(bool);
    bool vGetStopSend0x22(void);

    bool vSetTC5F08Status(unsigned char);
    unsigned char vGetTC5F08Status(void);

    bool vSetTC5F08StatusChange(bool);
    bool vGetTC5F08StatusChange(void);

    bool vSetThisCycleRunCCJPlan5F18(bool);
    bool vGetThisCycleRunCCJPlan5F18(void);
    bool vSetThisCycleRunCCJActure5FA2(bool);
    bool vGetThisCycleRunCCJActure5FA2(void);

    bool vSaveCurrentAsGreenStartTime(void);
    unsigned int vGetActRunningGreenTime(void);

    bool vSetLastRevSyncTime(time_t);

    bool vCheckRevSyncStatus(void);
    bool vSetRevSyncStatus(sRevSyncStatus);
    sRevSyncStatus vGetRevSyncStatus(void);

    bool vSetRevSyncVar(int);
    int vGetRevSyncVar(void);

    bool vGetCommEnable(void);
    bool vSetCommEnable(bool);

//OT1000218
    bool vSet5F18EffectTime(unsigned int);
    bool vSet5F18EffectTimeCountDown(void);
    unsigned int vGet5F18EffectTime(void);

    bool vSetLastGetProtocolTime(void);
    time_t vGetLastGetProtocolTime(void);
    bool vSetSystemTime(time_t _tmp);
    time_t vGetSystemTime(void);

    bool vSetShowRedCountVar(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);

    bool vLogTrafficLightTime(void);
    time_t vGetLastTrafficeLightTime(void);
    bool vSetTrafficLightBoardConnectStatus(bool);

    //OT20131225
    bool vAddCCJHeartBeatCount(unsigned short int);

    bool vGetPlanForceTOD(unsigned short int);
    bool vSetPlanForceTOD(unsigned short int, bool);

    //jacky20151210
    unsigned char setReq;
    bool SetRequestKeypad(unsigned char);
    unsigned char GetRequestKeypad(void);

private:
    pthread_mutex_t mutexSmem;                                              //保護記憶體

    void InitialSMem(void);                                                 //初始化記憶體或通訊重置

    int address;                                                            //此設備路口編號
    int cFace;                                                              //目前LCD畫面上的頁面
    int lastFace;                                                           //上一次停留的畫面
    int operMode;                                                           //機器操作模式 1:遠端操作 0:現場操作
    int sequence;                                                           //機器上要送出的流水編號
    char password[6];                                                       //控制器密碼
    int dbOperStat;                                                         //資料庫操作設定(0:允許現場操作,1:不允許現場操作,2:只允許現場查看)

    bool ssComm;                                                            //與SmartSensor的通訊
    time_t ssCommTime;

    time_t startTime;

    bool keypadReturn;                                                      //我們必須借由keypad的回傳值來設定路口編號(address)及操作模式(operMode)
    time_t lastKeypadTime;                                                  //最後一次按鍵盤的時間


    MESSAGEWAITREPONSE msgWaitResponse[MSGWAITREPONSECOUNT];                //重傳機制


    YMDHMS ResetTime;

    unsigned char ucActuateType;
    unsigned char ucOccupyTime;
    unsigned char ucSettingLaneCount;
    unsigned char ucDetectorMap;
    short int siTimeGap;
    unsigned short int usiBigCarLength;
    unsigned short int usiCarLength;
    short int siTransmitType[5];
    long lLastRebootSec;

    bool vSave92COMMToDisk();
    bool vRead92COMMFromDisk();

    int iFirmwareYear;
    int iFirmwareMonth;
    int iFirmwareDay;
    int iFirmwareCompanyID;
    int iFirmwareFirstVersion;
    int iFirmwareSecondVersion;

    int iAdjudicateReturnAddressBCDorHEX;

    //OTCombo

    bool vSave92TCSettingToDisk(void);
    bool vRead92TCSettingFromDisk(void);

    unsigned char ucSmem92TC_ControlStrategy;
    int iSmem92TC_EffectTime;
    int iSmem92TC_PhaseUpdateFlag;
    bool bSmem92TC_TransmitCycle5F03_ChangeFlag;
    bool bSmem92TC_TransmitCycle5F0F_ChangeFlag;
    int iSmem92TC_SignalLightStatus_5F0F_IntervalTime;
    int iSmem92TC_SignalStepStatus_5F03_IntervalTime;

    bool bSmem92TC_ChangePlanOneTime5F18_ChangeFlag;
    int iSmem92TC_ChangePlanOneTime5F18_PlanID;

    int iSmemTC_RedCountVer;                                                //紅燈倒數版本

    bool bSmemTC_PhaseAlive[255];                                           //快速判斷Phase是否存在
    bool bSmemTC_PlanAlive[255];
    bool bSmemTC_SegTypeAlive[255];

    bool bTCDoorStatus;

    bool bHaveTCDoorSwitch;                                                 //have tc hardward door check switch

    bool bTCSignalConflictError;

    bool bSmem92TC_SubPhaseOfPhasePlanIncorrent;                            //當Phase與Plan定義的subphase不同時，此flag為true

//OT Debug 0523
    bool bTC_ActuateTypeFunctionEnable;
    bool bSmemTC_CCT_In_LongTanu_ActuateType_Switch;                        //龍潭特別觸動, 1 cycle change execplan
    unsigned short int usiSmemTC_CCT_In_LongTanu_ActuateType_PlanID;                         //when actuating, change to this plan
    bool bActuateRunningFlag;
    unsigned char ucActuatePhaseExtend;                                     //trash

    bool vReadUDPMappingLCNDataFromStorage();
    tsUDPMappingLCN _sUDPMappingLCN[MaxOpenUDPPort];

    unsigned short int usiWayMappingRedCount[8];

    int iSmem_Com2Type;

    unsigned char ucSignalLightStatus[14];                                  // 7 x 2 Lane Signal

    //OT Debug 0410
    int iSmemTC_SPID;
    int iSmemTC_SID;
    int iSmemTC_ET;

    //OT Pass
    unsigned char ucACKW2W[256];                                            //Seq only have 1 byte
    unsigned char ucPassMode_0F8E8F;
    unsigned short int usiPassServerLCN;

    //OT Debug Direct
    unsigned short int usiSignamMapMappingDir[8];
    unsigned short int usiSignamMapMappingLightBoard[6];

    unsigned short int usiScreenCopyPlanID;
    unsigned short int usiScreenCopySegID;

    tsUDPMappingLCN _sCCJDynCtl;
    tsUDPMappingLCN _sCCJDynCtl2;
    bool vReadCCJDynCtlFromStorage(void);
    bool vReadCCJDynCtl2FromStorage(void);

//OT Debug Signal 951116
    bool bTC_SIGNAL_NEXT_STEP_OK;

    bool bEnableLCNUsingStorage;
    unsigned int uiLCNUsingStorage;

    long lLast92TC_5F15Time;

    //Debug 951128 for dongSignal driver unit
    bool bSignalDriverStatus;

    unsigned char ucGreenConflictDetFromCSTC;
    unsigned char ucGreenConflictDetFromDIO;

    sChildChain sSMEMChildChain;
    unsigned char ucTC92_5F31Manual;
    unsigned char ucTC92_5F31TOD;
    unsigned char ucTC92_5F31StartSubPhaseId;
    unsigned char ucTC92_5F31EndSubPhaseId;
    unsigned char ucTC92_5F32StartSubPhaseId;
    unsigned char ucTC92_5F32EndSubPhaseId;
    unsigned char ucTC_MotherChainStartStepId;
    unsigned char ucTC_MotherChainEndStepId;
    short int siTC92_5F33StartOffset[64];
    short int siTC92_5F33EndOffset[64];

    unsigned char ucSendDigitalIO;

    int localIp1_1,localIp1_2,localIp1_3,localIp1_4,localPort1;
    int distIp0_1,distIp0_2,distIp0_3,distIp0_4,dist0Port;
    int distIp1,distIp2,distIp3,distIp4,distPort;
    //OT20111020
    int netmask1, netmask2, netmask3, netmask4;
    int gateway1, gateway2, gateway3, gateway4;

    unsigned char ucMBType;                                                 // 0:Arbor 1:ICOP6047 2:ICOP6115

    bool bGPSSyncStatus;
    bool bActuateByTFD;

    unsigned char ucTC_KeyPadP6Value;

    bool b92VDUpdateDB_b0;
    bool b92VDUpdateDB_b1;
    bool b92VDUpdateDB_b2;
    bool b92VDUpdateDB_b3;
    bool b92VDUpdateDB_Status;                                              //自已加的，讓Interval判斷比較快

    bool bEnableSendStepToCCJ;

    bool bNextStepForceEnable;
    unsigned short int usiActuateVDID;

    unsigned char ucTC_TrainChainEnable;
    unsigned char ucTC_TrainChainStatusNOW;
    unsigned char ucTC_TrainChainStatusLast;
    unsigned char ucTrainComingBanSubphase;
    unsigned char ucTrainComingBanSubphase2;  //OT20111201
    unsigned char ucTrainComingForceJumpSubphase;
//        unsigned char ucTrainComingBanSubphaseLast;

    unsigned char ucSignalConflictErrorVar[3];

    unsigned int uiTrafficLightSend;
    unsigned int uiTrafficLightACK;

    unsigned char ucReverseLaneManualEnable;
    unsigned char ucReverseLaneControlMode;                                 // 0: auto, 1:Manual, 2:ManualToAuto
    time_t lastRevManualPadTime;

    //OT980420
    unsigned char ucBootingDisplayRedcount;
    unsigned char ucDisplayRedcount;

    bool bEnableUpdateRTC;
    bool bGetResetTCSignal;
    unsigned char ucCSTCControlStrategy;
    unsigned char ucCSTCFieldOperate;
    unsigned char ucTimerCSTC;
    unsigned char ucTimerCTIMER;
    unsigned char ucTimerRESET;

    unsigned char ucW77E58FWVer[3];
    unsigned char ucTC_ActuateTypeByTOD;

    int usiVDPhaseMap[8][8];
    int ucVDPhaseTriggerSwitch[8];
    bool bForceClockRedCountOneTime;

    bool bBugStopSend0x22ForceCtl51Light;  //in version 2009/7/2, when send 0x22, 51 light borad will be controlled immd.
    unsigned char ucTC5F08Status;
    bool bTC5F08StatusChange;
    bool bThisCycleRunCCJPlan5F18;
    bool bThisCycleRunCCJActure5FA2;

    unsigned char ucSmemTC_DynShowRedCountForAct;

    timespec actGreenStartTimeForScreenActuateArwenStatus;
    timespec actGreenCurrentTimeForScreenActuateArwenStatus;

    unsigned char ucSendCCTActArwenProtocal;

    bool bTCSegmentTypeUpdate;
    time_t _RecRevSyncTime;

    unsigned char ucRevSyncEnable;
    unsigned char ucRevSyncInterval;
    unsigned char ucRevSyncTimeDiffTolarence;
    unsigned char ucRevSyncCountineStatusAdj;
    unsigned char ucRevDefaultVehWay;
    unsigned char ucRevLogToFile;
    unsigned char ucRevSendProtocol;
    unsigned char ucRevSyncByteStatus;

    sRevSyncStatus _NewestRevSyncStatus;
    int iRevSyncStatus;  // -1: error, 0:unknow, 1:sync ok
    int iLastRevSyncStatus;  // -1: error, 0:unknow, 1:sync ok

    unsigned char ucAmegidsDynEnable;
    unsigned short int usiAmegidsDynDevLCN;

    unsigned short int usiTODPlanID;
    bool bCommEnableSwitch;

    unsigned int ui5F18EffectTime;

    //OT20110526
    time_t lastGetProtocolTime;
    time_t _SystemTime;
    time_t _SystemTimeTmp;

    unsigned char ucLastShowRedCountVar[8];
    time_t ucLastShowRedCountTime;

    //OT20110607
    unsigned char ucSendCCTPhaseCycleProtocalForCenter;
    unsigned char ucSegmentNoRunning;

    //OT20110624
    unsigned char ucRedcountRepeatCount;

    //OT20110825
    time_t _lastGetTrafficLightTime;
    bool bTrafficeLightBoardConnectStatus;

    //OT20111128
    unsigned char ucMachineLocation;
    //OT20131225
    unsigned short int usiCCJHeartBeatCount;

    //OT20140211
    bool bPlanForceTOD[50];

    //OT20140415
    bool bDynSegSwitch;
    unsigned char ucDynSegAdjustType;
    unsigned short int usiDynSegAdjustSec;
    bool bDynSegStatus;
    unsigned short int usiDynSegRemainSec;

};
//---------------------------------------------------------------------------
extern SMEM smem;
#endif
