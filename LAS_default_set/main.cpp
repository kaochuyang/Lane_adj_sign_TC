#include <iostream>
#include <stdio.h>
#define BYTE unsigned char
#include <string.h>
#include <unistd.h>
using namespace std;

typedef struct LAS_excute_info  // Lane adjusted sign excute information
{
    BYTE segmentcount;
    BYTE lightcount;
    BYTE light_select[9][24];//ID plan
    bool check_ID[9];
    int  hour[24];
    int  min[24];
    BYTE weekcount;




};

typedef struct LAS_YMD
{
    BYTE start_year;
    BYTE start_month;
    BYTE start_day;
    BYTE end_year;
    BYTE end_month;
    BYTE end_day;
};

typedef struct junbo_lane_adj_memory_object
{


    BYTE defaul_light[9];
    BYTE default_timeout;
    BYTE brightness;
};

void store_lane_adj_setting(junbo_lane_adj_memory_object object)
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


        }
        else
        {

            printf("write junbo_lane_adj_memory_object no file\n");

        };

    }
    catch(...) {}
}

void _8f10_sotre(int typeID,LAS_excute_info* pf)
{
    int LAS_week_type_info[7]= {1};
    // LAS_excute_info* pf;
    FILE *pf1=NULL;
    FILE *pf2=NULL;
    char filename[256]= {0};
    char filename2[256]= {0};
    sprintf(filename,"/cct/Data/SETTING/LAS_8f10_setting_%d.bin",typeID);

    sprintf(filename2,"/cct/Data/SETTING/segmenttype_setting.bin");

    pf2=fopen(filename2,"w+");
    if(pf2!=NULL)
    {

        fwrite(LAS_week_type_info,sizeof(int)*7,1,pf2);
        fclose(pf2);
        printf("write segmenttype 8f10 success!.\n");



    }
    else
    {

        printf("write segmenttype 8f10 no file\n");

    };

    pf1=fopen(filename,"w+");
    if(pf1!=NULL)
    {

        fwrite(pf,sizeof(LAS_excute_info),1,pf1);
        fclose(pf1);
        printf("write LAS_excute_info %d success!.\n",typeID);



    }
    else
    {

        printf("write LAS_excute_info no file\n");

    };

}

void _8f11_store(int typeID,LAS_excute_info* pf)
{

    LAS_excute_info* LAS_week_type_info=NULL;
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

        fwrite(LAS_week_type_info,sizeof(LAS_YMD),1,pf2);
        fclose(pf2);
        printf("write special_type_setting 8f11 success!.");
    }
    else
    {

        printf("write special_type_setting 8f11 no file\n");

    };
///////////////////////////////////////////////////////////////////////////////
    pf1=fopen(filename,"w+");
    if(pf1!=NULL)
    {

        fwrite(pf,sizeof(LAS_excute_info),1,pf1);
        fclose(pf1);
        printf("write LAS_excute_info success!.");


    }
    else
    {

        printf("write LAS_excute_info no file\n");

    };
}

int main()
{

 /*   LAS_excute_info A;

    for(int ID=1; ID<9; ID++)
    {
        A.check_ID[ID]=0;
        for(int plan=1; plan<24; plan++)
            A.light_select[ID][plan]=1;



    }
    A.check_ID[1]=1;
    for(int i=0; i<24; i++)
    A.lightcount=2;
    A.segmentcount=1;
    {
        A.hour[i]=0;
        A.min[i]=0;
    }
    A.hour[1]=23;
    A.min[1]=59;
    A.weekcount=7;



   for(int segmenttype=1; segmenttype<8; segmenttype++)
        _8f10_sotre(segmenttype,&A);
    LAS_excute_info* p=NULL;
    FILE *pf=NULL;
    char filename[256]= {0};
    int d=4;
        sprintf(filename,"/cct/Data/SETTING/LAS_8f10_setting_%d.bin",d);

    pf=fopen(filename,"r");
    if(pf!=NULL)
    {
        fread(&A,sizeof(LAS_excute_info),1,pf);
        fclose(pf);
        p=&A;

        printf("read LAS_8f10_setting_%d success\n",d);
        printf("read segmenttype=%d segmentcount=%d  lightcount=%d  weekcount=%d\n",d,p->segmentcount,p->lightcount,p->weekcount);


    }*/
 typedef   struct B
    {

int pack[5];
    };

int pack_[5];

B *A=NULL;
B C;
C.pack[3]=2;
A=&C;

pack_[1]=A->pack[3];

printf("%d",pack_[1]);

    cout << "Hello world!" << endl;
    return 0;
}



