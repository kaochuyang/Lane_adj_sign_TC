#ifndef PROTOCOL_AD_CMS_TRAVEL_TIME_H
#define PROTOCOL_AD_CMS_TRAVEL_TIME_H
#include "protocol_CF_cms_travel_time.h"

class protocol_AD_cms_travel_time:public protocol_CF_cms_travel_time
{
  public:
    protocol_AD_cms_travel_time();
    virtual ~protocol_AD_cms_travel_time();

    bool DoWorkViaPTraffic92(MESSAGEOK message);
    void _AD10_time_display_set(MESSAGEOK messageIn);
     void _AD10_test_time_display_set(int x,int y,int z);

    void _AD40_time_display_query();
    void _ADC0_time_display_rport_report();
    void _AD00_time_display_auto_report();

    void _AD11_CMS_controler_interrupt_set(MESSAGEOK messageIn);
    void _AD41_CMS_controler_interrupt_query();
    void _ADC1_CMS_controler_interrupt_report();
int getValueRecord(int num);
void setValueRecord(int num,int value);

  Value_Record value_record;
protected:
private:


};

#endif // PROTOCOL_AD_CMS_TRAVEL_TIME_H
