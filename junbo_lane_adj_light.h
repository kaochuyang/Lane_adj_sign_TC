#ifndef JUNBO_LANE_ADJ_LIGHT_H
#define JUNBO_LANE_ADJ_LIGHT_H


class junbo_lane_adj_light
{
public:


    static pthread_mutex_t _junbo_mutex;


    junbo_lane_adj_light();
    virtual ~junbo_lane_adj_light();

    void initial_junbo_control(char *output_tty_name);

    void junbo_light_send(unsigned char junbo_send_packet[8]);
    void junbo_light_receive(MESSAGEOK messageIn);
    void junbo_light_send_reference_select();


    bool vClearMsg(MESSAGEOK *messageIn, unsigned short int msgID);
    bool ParseBlock(int receiveBlockLength,BYTE *block,MESSAGEOK *messageIn,int *lastPacketIndex);


    typedef  struct s_junbo_lane_adj
    {

        unsigned char command;
        unsigned char parameter[2];
    };


protected:
private:

    unsigned char junbo_send_packet[8];
    unsigned char junbo_receive_packet[8];


    s_junbo_lane_adj light_on,light_off,text_ID[5],light_timeout,module_query[4],brightness[4],query_textID



    char input_string[25];

    char filename[80];//record file director

};

#endif // JUNBO_LANE_ADJ_LIGHT_H
