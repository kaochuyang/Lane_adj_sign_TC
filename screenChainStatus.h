#ifndef screenChainStatusH
#define screenChainStatusH
//---------------------------------------------------------------------------
#include "SCREEN.h"
//---------------------------------------------------------------------------
class ScreenChainStatus:public SCREEN
{
    public:
      ScreenChainStatus(void);
      ~ScreenChainStatus(void);

      void DisplayChainStatus(void);
      void doKeyWork(BYTE);                                                     //�ھ�KEY���ȧ@��

      void vRefreshChainStatusData(void);

    private:
      DISP_WORD _MotherOutputEnable;
      DISP_WORD _MotherOutputSignal;
      DISP_WORD _ChildInputEnable;
      DISP_WORD _ChildInputSignal;
      DISP_WORD _ChildInputReasonable;
      DISP_WORD _ChildCycle[3];

      void initDispWord(void);                                                  //��l�Ʀ��e�����y�е��Ѽ�
      int cSelect;                                                              //�{�b��ܪ��ﶵ

      BYTE ucChainStatusBitmap[3840];
      void loadBitmapFromFile(void);                                            //�N����Load�i�O����

      void doKey0Work(void);
      void doKey1Work(void);
      void doKey2Work(void);
      void doKey3Work(void);
      void doKey4Work(void);
      void doKey5Work(void);
      void doKey6Work(void);
      void doKey7Work(void);
      void doKey8Work(void);
      void doKey9Work(void);
      void doKeyAWork(void);
      void doKeyBWork(void);
      void doKeyCWork(void);
      void doKeyDWork(void);
      void doKeyEWork(void);
      void doKeyFWork(void);
      void doKeyF1Work(void);
      void doKeyF2Work(void);
      void doKeyF3Work(void);
      void doKeyF4Work(void);
      void doKeyUPWork(void);
      void doKeyDOWNWork(void);
      void doKeyLEFTWork(void);
      void doKeyRIGHTWork(void);
      void doKeyEnterWork(void);
};
//---------------------------------------------------------------------------
extern ScreenChainStatus screenChainStatus;
#endif