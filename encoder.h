#ifndef NU32__H__
#define NU32__H__

#include <xc.h>                     // processor SFR definitions
#include <sys/attribs.h>            // __ISR macro


void encoder_init(void);
int encoder_counts(void);
int encoder_reset(void);

#endif // NU32__H__
