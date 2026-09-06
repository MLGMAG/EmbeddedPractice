#ifndef __GPIO_UTIL_H
#define __GPIO_UTIL_H

#include "common.h"

void UAL_GPIO_UTIL_DisableButtonIt();
void UAL_GPIO_UTIL_EnableButtonIt();
GPIO_PinState UAL_GPIO_UTIL_GetInputButtonState();
GPIO_PinState UAL_GPIO_UTIL_GetLed();
void UAL_GPIO_UTIL_SetLed(GPIO_PinState value);

#endif /* __GPIO_UTIL_H */
