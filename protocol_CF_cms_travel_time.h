#ifndef PROTOCOL_CF_CMS_TRAVEL_TIME_H
#define PROTOCOL_CF_CMS_TRAVEL_TIME_H
#include "var.h"

#include "protocol_8F_LAS.h"


class protocol_CF_cms_travel_time:public protocol_8F_LAS
{
public:
    protocol_CF_cms_travel_time();
    virtual ~protocol_CF_cms_travel_time();


     void store_value(Value_Record object);
    void read_value(Value_Record *object);
    void clear_value_record(Value_Record *object);
    void delete_record_before_15day();

    bool DoWorkViaPTraffic92(MESSAGEOK message);
    void _CF10_time_display_set(MESSAGEOK messageIn);
    void _CF40_time_display_query();
    void _CFC0_time_display_rport_report();
    void _CF00_time_display_auto_report();

    void _CF11_CMS_controler_interrupt_set(MESSAGEOK messageIn);
    void _CF41_CMS_controler_interrupt_query();
    void _CFC1_CMS_controler_interrupt_report();

    void _CF02_hw_state_auto_report();
  Value_Record value_record;
protected:
private:



};

#endif // PROTOCOL_CF_CMS_TRAVEL_TIME_H