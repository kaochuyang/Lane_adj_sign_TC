#ifndef JUNBO_LANE_ADJ_LIGHT_H
#define JUNBO_LANE_ADJ_LIGHT_H
#include "pthread.h"
#include "var.h"
#include "RS232.h"
#include "stdio.h"
#include "var.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <signal.h>

class junbo_lane_adj_light
{
public:
    void test_LAS_function(int commnd,int parameter_1);
    void test_LAS_function(int commnd,int parameter_1,int ID);
    void test_step();

    static pthread_mutex_t _junbo_mutex;
    RS232 junbo_lane_adj_port;

    junbo_lane_adj_light();
    virtual ~junbo_lane_adj_light();

    void initial_junbo_control(char *output_tty_name);
    junbo_packet  junbo_Packeted(s_junbo_lane_adj Action);
    void junbo_light_send(junbo_packet send);
    void junbo_light_receive(MESSAGEOK messageIn);
    void junbo_light_send_reference_select(BYTE ID,s_junbo_lane_adj Action);
    void light_exec_check(int ID,int segment_type);
    void light_mistake_warning();


    void step_control(int segment_type);//************ important
    void determind_weekday_specialday();


    bool vClearMsg(MESSAGEOK *messageIn, unsigned short int msgID);
    bool ParseBlock(int receiveBlockLength,BYTE *block,MESSAGEOK *messageIn,int *lastPacketIndex);


    void light_timeout_control(int control_parameter);
    void report_light_timeout();

    void brightness_control(int bright_parameter);
//   void report_light_brightness();
    void auto_minus_bright();

    void do_query_module();
    void query_module_state_1();
    void query_module_state_2();
    void query_module_state_3();
    BYTE *report_module_state();
    void link_ID_check();

    void store_lane_adj_setting(junbo_lane_adj_memory_object object);
    void read_lane_adj_setting(junbo_lane_adj_memory_object *object);
    void clear_s_junbo_lane_adj(s_junbo_lane_adj *object);
    void delete_record_before_15day();




protected:
private:



    s_junbo_lane_adj
    light_on,
    light_off,
    straight,
    left,
    right,
    straight_left,
    straight_right,
    query_state,
    light_timeout,
    module_query[4],
    brightness,
    light_black,
    straight_flash,
    left_flash,
    right_flash,
    straight_left_flash,
    straight_right_flash;


    unsigned char junbo_send_packet[8];
    unsigned char junbo_receive_packet[8];
    BYTE module_err[9];

    char *tty_name;
    char input_string[25];

    char filename[80];//record file director

};

#endif // JUNBO_LANE_ADJ_LIGHT_H
