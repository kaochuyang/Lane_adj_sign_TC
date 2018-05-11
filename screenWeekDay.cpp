#include "screenWeekDay.h"
#include "screenWeekDayEditF2.h"
#include "SCREENMain.h"
#include "screenSegtype.h"
#include "screenSegtypeMenu.h"
//#include "faceDefine.h"
#include "LCD240x128.h"
#include "CSTC.h"
#include <stdio.h>

#include "SMEM.h"

//---------------------------------------------------------------------------
ScreenWeekDay screenWeekDay;
//---------------------------------------------------------------------------
ScreenWeekDay::ScreenWeekDay(void)
{
    loadBitmapFromFile();
    initDispWord();
    for (int i=0; i<14; i++)
    {
        weekDaySegtype[i]._segment_type=0;
        weekDaySegtype[i]._weekday=0;
        //CCJ++
        WeekTemp[i*2] = 0;
        WeekTemp[i*2+1] = 0;
        //CCJ--
    }
    cDatePosition=0;
    cSegPosition=0;
}
//---------------------------------------------------------------------------
ScreenWeekDay::~ScreenWeekDay(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::loadBitmapFromFile(void)
{
    try
    {
        FILE *bitmap;
        bitmap=fopen("//cct//bitmap//backGround//TC5F//weekDay.bit","rb");
        if (bitmap)
        {
            fread(weekDayBitmap,3840,1,bitmap);
            fclose(bitmap);
        }
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::DisplayWeekDay(void)
{
    try
    {
        smem.SetcFace(cWEEKDAY);
        lcd240x128.DISPLAY_GRAPHIC(0,weekDayBitmap,128,30);
        DisplayDate();
        DisplayDate();
        LoadWeekDaySegtype();
        DisplayWeekDaySegtype();
        DisplayWeekDaySegtype();

        time_t currentTime=time(NULL);
        struct tm *now=localtime(&currentTime);

        if(   ((((((now->tm_mday-1)/7)+1)%2)==0 ) && (now->tm_wday>=((now->tm_mday-1)%7)))  //Even week
                || ((((((now->tm_mday-1)/7)+1)%2)> 0 ) && (now->tm_wday< ((now->tm_mday-1)%7))) )
        {
            printf("printfMsg Even Week.\n");
            if (now->tm_wday==0)  cSegPosition=26;
            else cSegPosition=((now->tm_wday-1)*2)+14;
        }
        else
        {
            printf("printfMsg odd Week.\n");
            if (now->tm_wday==0)  cSegPosition=12;
            else cSegPosition=(now->tm_wday-1)*2;
        }

        setCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::initDispWord(void)
{
    try
    {
        for (int i=0; i<8; i++)
        {
            if (i>=0 && i<=3) dateWord[i].X=128+i*8;                               //xxxx�~
            else if (i>=4 && i<=5) dateWord[i].X=176+(i-4)*8;                      //xx��
            else if (i>=6 && i<=7) dateWord[i].X=208+(i-6)*8;                      //xx��
            dateWord[i].Y=2;
            dateWord[i].width=8;
            dateWord[i].height=16;
        }
        for (int i=0; i<28; i++)
        {
            if (i>=0 && i<=1) segtypeWord[i].X=56+i*8;
            else if (i>=2 && i<=3) segtypeWord[i].X=80+(i-2)*8;
            else if (i>=4 && i<=5) segtypeWord[i].X=104+(i-4)*8;
            else if (i>=6 && i<=7) segtypeWord[i].X=128+(i-6)*8;
            else if (i>=8 && i<=9) segtypeWord[i].X=152+(i-8)*8;
            else if (i>=10 && i<=11) segtypeWord[i].X=176+(i-10)*8;
            else if (i>=12 && i<=13) segtypeWord[i].X=200+(i-12)*8;
            else if (i>=14 && i<=15) segtypeWord[i].X=56+(i-14)*8;
            else if (i>=16 && i<=17) segtypeWord[i].X=80+(i-16)*8;
            else if (i>=18 && i<=19) segtypeWord[i].X=104+(i-18)*8;
            else if (i>=20 && i<=21) segtypeWord[i].X=128+(i-20)*8;
            else if (i>=22 && i<=23) segtypeWord[i].X=152+(i-22)*8;
            else if (i>=24 && i<=25) segtypeWord[i].X=176+(i-24)*8;
            else if (i>=26 && i<=27) segtypeWord[i].X=200+(i-26)*8;


            if (i>=0 && i<=13)  segtypeWord[i].Y=52;
            else if (i>=14 && i<=27)  segtypeWord[i].Y=72;
            segtypeWord[i].width=8;
            segtypeWord[i].height=16;
        }
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::DisplayDate(void)
{
    try
    {
        unsigned short cYear=0,cMonth=0,cDay=0;
        time_t currentTime=time(NULL);
        struct tm *now=localtime(&currentTime);
        cYear=now->tm_year+1900;
        cMonth=now->tm_mon+1;
        cDay=now->tm_mday;


        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[0].X,dateWord[0].Y,word8x16[cYear/1000],dateWord[0].height,dateWord[0].width/8);
        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[1].X,dateWord[1].Y,word8x16[(cYear/100)%10],dateWord[1].height,dateWord[1].width/8);
        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[2].X,dateWord[2].Y,word8x16[(cYear/10)%10],dateWord[2].height,dateWord[2].width/8);
        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[3].X,dateWord[3].Y,word8x16[cYear%10],dateWord[3].height,dateWord[3].width/8);
        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[4].X,dateWord[4].Y,word8x16[cMonth/10],dateWord[4].height,dateWord[4].width/8);
        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[5].X,dateWord[5].Y,word8x16[cMonth%10],dateWord[5].height,dateWord[5].width/8);
        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[6].X,dateWord[6].Y,word8x16[cDay/10],dateWord[6].height,dateWord[6].width/8);
        lcd240x128.DISPLAY_GRAPHIC_XY(dateWord[7].X,dateWord[7].Y,word8x16[cDay%10],dateWord[7].height,dateWord[7].width/8);
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::LoadWeekDaySegtype(void)
{
    try
    {
        stc.Lock_to_Load_WeekDaySegment_for_Panel();
        for (int i=0; i<14; i++)
        {
            weekDaySegtype[i]._segment_type=stc._panel_weekdayseg[i]._segment_type;
            weekDaySegtype[i]._weekday=stc._panel_weekdayseg[i]._weekday;
        }
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::DisplayWeekDaySegtype(void)
{
    try
    {
        for (int i=0; i<28; i+=2)
        {
            lcd240x128.DISPLAY_GRAPHIC_XY(segtypeWord[i].X,segtypeWord[i].Y,word8x16[weekDaySegtype[i/2]._segment_type/10],segtypeWord[i].height,segtypeWord[i].width/8);
            lcd240x128.DISPLAY_GRAPHIC_XY(segtypeWord[i+1].X,segtypeWord[i+1].Y,word8x16[weekDaySegtype[i/2]._segment_type%10],segtypeWord[i+1].height,segtypeWord[i+1].width/8);
        }
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyWork(BYTE key)
{
    try
    {
        switch (key)
        {
        case 0x80:
            doKey0Work();
            break;
        case 0x81:
            doKey1Work();
            break;
        case 0x82:
            doKey2Work();
            break;
        case 0x83:
            doKey3Work();
            break;
        case 0x84:
            doKey4Work();
            break;
        case 0x85:
            doKey5Work();
            break;
        case 0x86:
            doKey6Work();
            break;
        case 0x87:
            doKey7Work();
            break;
        case 0x88:
            doKey8Work();
            break;
        case 0x89:
            doKey9Work();
            break;
        case 0x8A:
            doKeyAWork();
            break;
        case 0x8B:
            doKeyBWork();
            break;
        case 0x8C:
            doKeyCWork();
            break;
        case 0x8D:
            doKeyDWork();
            break;
        case 0x8E:
            doKeyEWork();
            break;
        case 0x8F:
            doKeyFWork();
            break;
        case 0x90:
            doKeyF1Work();
            break;
        case 0x91:
            doKeyF2Work();
            break;
        case 0x92:
            doKeyF3Work();
            break;
        case 0x93:
            doKeyF4Work();
            break;
        case 0x94:
            doKeyUPWork();
            break;
        case 0x95:
            doKeyDOWNWork();
            break;
        case 0x96:
            doKeyLEFTWork();
            break;
        case 0x97:
            doKeyRIGHTWork();
            break;
        case 0x98:
            doKeyEnterWork();
            break;
        default:
            break;
        }
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey0Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey1Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey2Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey3Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey4Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey5Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey6Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey7Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey8Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKey9Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyAWork(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyBWork(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyCWork(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyDWork(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyEWork(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyFWork(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyF1Work(void)
{
    screenMain.DisplayMain();
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyF2Work(void)
{
//    screenSegTypeEditStep1.DisplaySegTypeEditStep1_keySegTypeID();
    screenWeekDayEditF2.DisplayWeekDay();
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyF3Work(void)
{
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyF4Work(void)
{
    screenSegtypeMenu.DisplaySegtypeMenu();
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyUPWork(void)
{
    clearCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
    if (cSegPosition>=14 && cSegPosition<=27) cSegPosition-=14;
    setCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyDOWNWork(void)
{
    clearCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
    if (cSegPosition>=0 && cSegPosition<=13) cSegPosition+=14;
    setCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyLEFTWork(void)
{
    try
    {
        clearCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);

        if (cSegPosition>=0 && cSegPosition<=13)
        {
            if (cSegPosition==0) cSegPosition=1;
        }
        else
        {
            if (cSegPosition==14) cSegPosition=15;
        }

        cSegPosition--;
        setCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyRIGHTWork(void)
{
    try
    {
        clearCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
        cSegPosition++;
        if (cSegPosition>=0 && cSegPosition<=14)
        {
            if (cSegPosition>=13) cSegPosition=13;
        }
        else
        {
            if (cSegPosition>=27) cSegPosition=27;
        }
        setCursor8x16(segtypeWord[cSegPosition].X,segtypeWord[cSegPosition].Y+16);
    }
    catch (...) {}
}
//---------------------------------------------------------------------------
void ScreenWeekDay::doKeyEnterWork(void)
{
    int x=weekDaySegtype[cSegPosition/2]._segment_type;
    if (x>=0 && x<=20) screenSegtype.DisplaySegtype(x, cWEEKDAY);
}
//---------------------------------------------------------------------------
