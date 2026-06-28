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
extern uint8_t uart_rx_dma_buffer[128];
void uart_proc(void);

typedef struct {
    int16_t int16_value; // int16 数据
    int8_t int8_value;   // int8 数据
    float float_value;   // float 数据
} DataPacket;

extern DataPacket parsed_data;
void usart_send_euler_angles(float roll, float pitch, float yaw, float fusion_state);
uint8_t usart_parse_euler_angles(uint8_t *buffer, uint16_t length);
uint8_t flexible_parse(uint8_t *buffer, uint16_t length, DataPacket *data);
///////////////////DMA空闲中断+DMA缓存区END
#endif


