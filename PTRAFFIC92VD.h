//---------------------------------------------------------------------------

#ifndef PTRAFFIC92VDH
#define PTRAFFIC92VDH

#include "var.h"
#include "SMEM.h"

//---------------------------------------------------------------------------
class PTRAFFIC92VD
{
public:

    PTRAFFIC92VD(void);
    ~PTRAFFIC92VD(void);

    bool DoWorkViaPTraffic92(MESSAGEOK);                                    //根據PTraffic92來的訊息作事情

private:


    int vReturnToCenterACK(unsigned char , unsigned char);
    int vReturnToCenterNACK(unsigned char, unsigned char, unsigned char, unsigned char);



};
//---------------------------------------------------------------------------
#endif
