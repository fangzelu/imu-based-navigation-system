#ifndef _HBM2X1M_H_
#define _HBM2X1M_H_

#include "pinmacros.h"
/*
#define BTRST				P1_5
#define BTRTS				P1_6
#define BTCTS				P1_7
#define BT_LDO_CE			P3_2

*/


MAKE_PORT_MACRO(BTRST,		1,5);
MAKE_PORT_MACRO(BTRTS,		1,6);
MAKE_PORT_MACRO(BTCTS,		1,7);
MAKE_PORT_MACRO(BTLDOCE,	3,2);


extern void HBM2X1M_reset();
extern void HBM2X1M_init();
extern void InitBTUart();
extern void bt_send_cmd(const char *cmd_str);
#endif

