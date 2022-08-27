#ifndef __TEMP__
#define __TEMP__

#include "main.h"

typedef struct _onewire{
		GPIO_TypeDef *gpiox;
		unsigned int pin;
}onewire;

void onewire_setmode(onewire *ptr, unsigned char isout);

void onewire_writebyte(onewire *ptr, unsigned char data);

unsigned char onewire_readbyte(onewire *ptr);

unsigned char onewire_reset(onewire *ptr);


#endif
