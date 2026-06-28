#include "uart_app.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

/////////串口超时解析///////////BEGIN
//uartapp.c与.h文件解除注释，uart.c文件初始化中解除相应注释，调度器
//uint16_t uart_rx_index=0;
//uint8_t  uart_rx_buffer[128]={0};
//uint32_t uart_rx_ticks;


//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance == USART1)
//	{
//		uart_rx_ticks=HAL_GetTick();
//		uart_rx_index++;

//	HAL_UART_Receive_IT(&huart1,&uart_rx_buffer[uart_rx_index],1);
//	}
//	
//}

//void uart_proc(void)
//{
//  if(uart_rx_index==0) return;
//	
//	 if((uwTick-uart_rx_ticks)>100)
//	{
//		////////////////处理函数//////////////////
//		printf("Rxdata: %s\n",uart_rx_buffer);
//		long_count=1;
//		///////////////////////////////////
//		memset(uart_rx_buffer,0,uart_rx_index);
//		uart_rx_index=0;
//		huart1.pRxBuffPtr=uart_rx_buffer;
//	}

//}

/////////串口超时解析///////////END

///////////////////DMA空闲中断BEGIN
//uartapp.c与.h文件解除注释，uart.c文件初始化中解除相应注释 it文件解除相应注释
//uint8_t uart_rx_dma_buffer[128]={0};

//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//	////////////////处理函数//////////////////
//	printf("DMA:%s",uart_rx_dma_buffer);
//	//////////////////////////////////
// memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));
//}

///////////////////DMA空闲中断END

///////////////////DMA缓存区BEGIN
//uartapp.c与.h文件解除注释，uart.c文件初始化中解除相应注释 it文件解除相应注释,主函数初始化,调度器
#define BUUFER_SIZE 64
ringbuffer_t usart_rb;
uint8_t usart_read_buffer[BUUFER_SIZE];
uint8_t uart_rx_dma_buffer[128]={0};


/**
 * @brief UART DMA接收完成回调函数
 *        将接收到的数据写入环形缓冲区，并清空DMA缓冲区
 * @param huart UART句柄
 * @param Size 接收到的数据大小
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
//    printf("dma data:%s\r\n", uart_rx_dma_buffer);
    // 如果环形缓冲区未满
    if(!ringbuffer_is_full(&usart_rb))
    {
        // 将DMA缓冲区中的数据写入环形缓冲区
        ringbuffer_write(&usart_rb, uart_rx_dma_buffer, Size);
    }
    // 清空DMA缓冲区
    memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));
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
    printf("ringbuffer data: %s\n", usart_read_buffer);
    //////////////////////////////////////////////
    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(uint8_t) * BUUFER_SIZE);
}

///////////////////DMA缓存区END





