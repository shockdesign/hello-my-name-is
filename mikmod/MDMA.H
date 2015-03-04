#ifndef MDMA_H
#define MDMA_H

#include "mtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define READ_DMA                0
#define WRITE_DMA               1
#define INDEF_READ              2
#define INDEF_WRITE             3

void *MDma_AllocMem(UWORD size);
void  MDma_FreeMem(void *w);
int   MDma_Start(int channel,void *pc_ptr,UWORD size,int type);
void  MDma_Stop(int channel);
UWORD MDma_Todo(int channel);

#ifdef __cplusplus
}
#endif

#endif
