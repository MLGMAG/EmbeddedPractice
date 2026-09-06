#ifndef __GPIO_UTIL_H
#define __GPIO_UTIL_H

#include "common.h"

void UAL_GPIO_UTIL_ButtonItDisable();
void UAL_GPIO_UTIL_ButtonItEnable();
GPIO_PinState UAL_GPIO_UTIL_InputButtonStateGet();
GPIO_PinState UAL_GPIO_UTIL_LedStateGet();
void UAL_GPIO_UTIL_LedStateSet(GPIO_PinState value);

#endif /* __GPIO_UTIL_H */
