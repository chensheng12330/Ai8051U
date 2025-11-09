/*---------------------------------------------------------------------*/
/* --- Web: www.STCAI.com ---------------------------------------------*/
/*---------------------------------------------------------------------*/

#include "usb.h"
#include "util.h"

DWORD reverse4(DWORD d)
{
    DWORD ret;
    
    ((BYTE *)&ret)[3] = ((BYTE *)&d)[0];
    ((BYTE *)&ret)[2] = ((BYTE *)&d)[1];
    ((BYTE *)&ret)[1] = ((BYTE *)&d)[2];
    ((BYTE *)&ret)[0] = ((BYTE *)&d)[3];

    return ret;
}

WORD reverse2(WORD w)
{
    WORD ret;
    
    ((BYTE *)&ret)[1] = ((BYTE *)&w)[0];
    ((BYTE *)&ret)[0] = ((BYTE *)&w)[1];

    return ret;
}
