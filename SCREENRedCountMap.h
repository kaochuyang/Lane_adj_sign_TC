#ifndef SCREENRedCountMapH
#define SCREENRedCountMapH

#include "SCREEN.h"
//---------------------------------------------------------------------------
class SCREENRedCountMap:public SCREEN
{
    public:
        SCREENRedCountMap(void);
        ~SCREENRedCountMap(void);

        void DisplayRedCountMapping(void);                                              //���
        void DoKeyWork(BYTE);                                                   //�ھ�KEY���ȧ@��

    private:
        BYTE ucRedCountMappingBitmap[3840];                                               //����

        DISP_WORD title[8];
        DISP_WORD lane[8];
        unsigned int cPosition;                                                 //�{�b����m
//        int laneCount;
        unsigned short int usiMappingValue[8];                                                       //�n�O�����������D

        void LoadBitmapFromFile(void);                                          //�N����Load�i�O����
        void InitDispWord(void);                                                //��l�Ʀ��e�����y�е��Ѽ�

        void DoKey0Work(void);
        void DoKey1Work(void);
        void DoKey2Work(void);
        void DoKey3Work(void);
        void DoKey4Work(void);
        void DoKey5Work(void);
        void DoKey6Work(void);
        void DoKey7Work(void);

        void DoKeyF1Work(void);
        void DoKeyF2Work(void);
        void DoKeyF4Work(void);

        void DoKeyLEFTWork(void);
        void DoKeyRIGHTWork(void);

        void DoKeyEnterWork(void);
};
//---------------------------------------------------------------------------
extern SCREENRedCountMap screenRedCountMap;
#endif