#include "atgm336h_app.h"
#include "usart.h"
#include "string.h"


///////////////////DMA缓存区BEGIN
//uartapp.c与.h文件解除注释，uart.c文件初始化中解除相应注释 it文件解除相应注释,主函数初始化,调度器
#define BUUFER_SIZE 64
ringbuffer_t usart_rb2;
uint8_t usart_read_buffer2[BUUFER_SIZE];
uint8_t uart_rx_dma_buffer2[128]={0};


/**
 * @brief UART DMA接收完成回调函数
 *        将接收到的数据写入环形缓冲区，并清空DMA缓冲区
 * @param huart UART句柄
 * @param Size 接收到的数据大小
 * @retval None
 */
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//	
//    // 如果环形缓冲区未满
//    if(!ringbuffer_is_full(&usart_rb2))
//    {
//        // 将DMA缓冲区中的数据写入环形缓冲区
//        ringbuffer_write(&usart_rb2, uart_rx_dma_buffer2, Size);
//		   
//    }
//    // 清空DMA缓冲区
//    memset(uart_rx_dma_buffer2, 0, sizeof(uart_rx_dma_buffer2));
//}

/**
 * @brief  处理UART接收缓冲区中的数据。
 *         如果在100ms内没有接收到新的数据，将清空缓冲区。
 * @param  None
 * @retval None
 */
// 处理UART接收缓冲区中的数据
void atgm336h_proc(void)
{
    // 如果环形缓冲区为空，直接返回
    if(ringbuffer_is_empty(&usart_rb2)) return;
    
    // 从环形缓冲区读取数据到读取缓冲区
    ringbuffer_read(&usart_rb2, usart_read_buffer2, usart_rb2.itemCount);
    ////////////////处理函数//////////////////
    // 打印读取缓冲区中的数据
    my_printf(&huart2,"ringbuffer2 data: %s\n", usart_read_buffer2);
    //////////////////////////////////////////////
    // 清空读取缓冲区
    memset(usart_read_buffer2, 0, sizeof(uint8_t) * BUUFER_SIZE);
}

///////////////////DMA缓存区END





