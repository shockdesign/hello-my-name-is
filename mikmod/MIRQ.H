#ifndef MIRQ_H
#define MIRQ_H

#include "mtypes.h"

typedef void (interrupt *PVI)(void);

BOOL MIrq_IsEnabled(UBYTE irqno);
BOOL MIrq_OnOff(UBYTE irqno,UBYTE onoff);
PVI  MIrq_SetHandler(UBYTE irqno,PVI handler);
void MIrq_EOI(UBYTE irqno);

#endif
