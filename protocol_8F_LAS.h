#ifndef PROTOCOL_8F_LAS_H
#define PROTOCOL_8F_LAS_H
#include "var.h"

class protocol_8F_LAS
{
    public:
        protocol_8F_LAS();
        virtual ~protocol_8F_LAS();
            void send_to_center_3(BYTE head,BYTE type,BYTE value);
    void send_to_center_2(BYTE head,BYTE type);//for act report

        void _8f10_commonday_set(MESSAGEOK messageIn);
        void _8f40_commanday_query(BYTE segmenttype);
        void _8fc0_commonday_report(BYTE segmenttype);

        void _8f11_specialday_set(MESSAGEOK messageIn);
        void _8f41_specialday_query(BYTE segmenttype);
        void _8fc1_specialday_report(BYTE segmenttype);

        void _8f12_brightness_set(BYTE brightness);
        void _8f42_brightness_query();
        void _8fc2_brightness_report();

        void _8f14_module_report_period_set(int cycle);//  0<cycle<0x3c
        void _8f44_module_report_period_query();
        void _8fc4_module_report_period_report();

        void _8f45_module_query();
        void _8fc5_module_report();
        void _8f05_module_act_report();

        void _8f16_default_light_set(MESSAGEOK messageIn);
        void _8f46_default_light_query();
        void _8fc6_default_light_report();


        void _8f47_light_query();
        void _8fc7_light_report();
        void _8f07_light_act_report();

        void _8f18_light_report_period_set(int second);
        void _8f48_light_report_period_query();
        void _8fc8_light_report_preriod_report();



        struct report_memory
        {
            int light_report_second;
            int module_report_hour;
        };

        report_memory LAS_report_object;
        void read_LAS_report_object();
        void store_LAS_report_object();


    protected:
    private:

      int vReturnToCenterACK(unsigned char , unsigned char);
    int vReturnToCenterNACK(unsigned char, unsigned char, unsigned char, unsigned char);
};

#endif // PROTOCOL_8F_LAS_H
