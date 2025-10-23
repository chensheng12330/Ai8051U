#include "stc.h"
#include "util.h"

WORD reverse2(WORD w)
{
    return (w >> 8) | (w << 8);
}
