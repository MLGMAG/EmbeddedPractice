#ifndef __COMMON_H
#define __COMMON_H

typedef enum
{
  GPIO_PIN_RESET = 0,
  GPIO_PIN_SET
} GPIO_PinState;

void UAL_QueueTimeoutError_Handler(void);

void UAL_Error_Handler(void);


#endif /* __COMMON_H */
