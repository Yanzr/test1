#ifndef _UART_APP_H_
#define _UART_APP_H_
#include "bsp_system.h"


///串口超时解析 BEGIN
//extern uint16_t uart_rx_index;
//extern uint8_t uart_rx_buffer[128];
//extern uint32_t uart_rx_ticks;
//void uart_proc(void);
///串口超时解析 END

///////////////////DMA空闲中断+DMA缓存区BEGIN
extern uint8_t uart_rx_dma_buffer[1000];
void uart_proc(void);
///////////////////DMA空闲中断+DMA缓存区END
#endif


