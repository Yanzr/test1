#include "uart_app.h"
#include "usart.h"
#include "string.h"


///////////////////DMA缓存区BEGIN
//uartapp.c与.h文件解除注释，uart.c文件初始化中解除相应注释 it文件解除相应注释,主函数初始化,调度器
#define BUUFER_SIZE 1000
ringbuffer_t usart_rb;
uint8_t usart_read_buffer[BUUFER_SIZE];
uint8_t uart_rx_dma_buffer[1000]={0};


/**
 * @brief UART DMA接收完成回调函数
 *        将接收到的数据写入环形缓冲区，并清空DMA缓冲区
 * @param huart UART句柄
 * @param Size 接收到的数据大小
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart1)  // 假设 huart1 是串口1的句柄
    {		
        // 处理串口1的数据
        if (!ringbuffer_is_full(&usart_rb))
        {
            ringbuffer_write(&usart_rb, uart_rx_dma_buffer, Size);
//			my_printf(&huart1, "111right\n");
        }
        memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));
    }
    else if (huart == &huart2)  // 假设 huart2 是串口2的句柄
    {	
        // 处理串口2的数据
        if (!ringbuffer_is_full(&usart_rb2))
        {
            ringbuffer_write(&usart_rb2, uart_rx_dma_buffer2, Size);
//			  my_printf(&huart1, "Raw received: %s\n", uart_rx_dma_buffer2); // 打印原始接收数据
//			my_printf(&huart2, "222right\n");
        }
        memset(uart_rx_dma_buffer2, 0, sizeof(uart_rx_dma_buffer2));
    }
}


/**
 * @brief  处理UART接收缓冲区中的数据。
 *         如果在100ms内没有接收到新的数据，将清空缓冲区。
 * @param  None
 * @retval None
 */
// 处理UART接收缓冲区中的数据
void uart_proc(void)
{

    // 如果环形缓冲区为空，直接返回
    if(ringbuffer_is_empty(&usart_rb)) return;
    
    // 从环形缓冲区读取数据到读取缓冲区
    ringbuffer_read(&usart_rb, usart_read_buffer, usart_rb.itemCount);
    ////////////////处理函数//////////////////
    // 打印读取缓冲区中的数据
    my_printf(&huart1,"ringbuffer data: %s\n", usart_read_buffer);
    //////////////////////////////////////////////
    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(uint8_t) * BUUFER_SIZE);
}

///////////////////DMA缓存区END





