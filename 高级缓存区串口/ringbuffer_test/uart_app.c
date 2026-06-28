#include "uart_app.h"

// 默认调试串口
#define DEBUG_UART_HANDLE (&huart1)

// 定义调试等级
#define DEBUG_LEVEL_NONE    0  // 不输出任何信息
#define DEBUG_LEVEL_ERROR   1  // 只输出错误信息
#define DEBUG_LEVEL_WARNING 2  // 输出警告及以上信息
#define DEBUG_LEVEL_INFO    3  // 输出信息及以上信息
#define DEBUG_LEVEL_DEBUG   4  // 输出所有调试信息

// 当前调试等级
int debug_level = DEBUG_LEVEL_DEBUG;

// 获取当前文件名（支持Windows和Unix路径分隔符）
#define _FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define FILENAME (strrchr(_FILENAME, '/') ? strrchr(_FILENAME, '/') + 1 : _FILENAME)

char *get_timestamp()
{
    static char timestamp[16];
    snprintf(timestamp, sizeof(timestamp), "%lu", uwTick);
    return timestamp;
}

// 核心打印函数
int my_printf(UART_HandleTypeDef *huart, const char *format, ...)
{
    char buffer[512];       // 定义缓冲区，用于存储格式化后的字符串
    va_list args;           // 定义参数列表
    va_start(args, format); // 初始化参数列表
    vsnprintf(buffer, sizeof(buffer), format, args); // 格式化字符串
    va_end(args);           // 结束参数列表

    // 发送缓冲区中的数据到指定串口
    HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), 0xFFFF);
    return strlen(buffer);  // 返回发送的数据长度
}

#define ANSI_COLOR_RESET   "\033[0m"
#define ANSI_COLOR_RED     "\033[31m"
#define ANSI_COLOR_YELLOW  "\033[33m"
#define ANSI_COLOR_GREEN   "\033[32m"
#define ANSI_COLOR_BLUE    "\033[34m"
#define ANSI_COLOR_CYAN    "\033[36m"

// 控制是否启用颜色输出
#define ENABLE_COLOR_OUTPUT 0

#if ENABLE_COLOR_OUTPUT
    #define COLOR(color_code) color_code
    #define COLOR_RESET ANSI_COLOR_RESET
#else
    #define COLOR(color_code) ""
    #define COLOR_RESET ""
#endif

#define debug_printf_color(level, color, format, ...)                           \
    do {                                                                        \
        if (level <= debug_level) {                                             \
            my_printf(DEBUG_UART_HANDLE, COLOR(color) "[%s][%s:%d] " format COLOR_RESET, \
                      get_timestamp(), FILENAME, __LINE__, ##__VA_ARGS__);      \
        }                                                                       \
    } while (0)

#define debug_error(format, ...)   debug_printf_color(DEBUG_LEVEL_ERROR, ANSI_COLOR_RED, format "\r\n", ##__VA_ARGS__)
#define debug_warning(format, ...) debug_printf_color(DEBUG_LEVEL_WARNING, ANSI_COLOR_YELLOW, format "\r\n", ##__VA_ARGS__)
#define debug_info(format, ...)    debug_printf_color(DEBUG_LEVEL_INFO, ANSI_COLOR_GREEN, format "\r\n", ##__VA_ARGS__)
#define debug_debug(format, ...)   debug_printf_color(DEBUG_LEVEL_DEBUG, ANSI_COLOR_CYAN, format "\r\n", ##__VA_ARGS__)



// 定义环形缓冲区和接收缓冲区
struct rt_ringbuffer ringbuffer;
uint8_t usart_read_buffer[BUUFER_SIZE];

/**
 * @brief UART DMA接收完成回调函数
 *        将接收到的数据写入环形缓冲区，并清空DMA缓冲区
 * @param huart UART句柄
 * @param Size 接收到的数据大小
 * @retval None
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        rt_ringbuffer_put(&ringbuffer, uart_rx_dma_buffer, Size);
        memset(uart_rx_dma_buffer, 0, sizeof(uart_rx_dma_buffer));
    }
}

#include <stdio.h>
#include <string.h>
#include <ringbuffer_test.h>

// struct rt_ringbuffer ringbuffer_test;
// uint8_t ringbuffer_test_pool[8] = {0};
// uint8_t ringbuffer_test_buffer[8] = {0};

ringbuffer_t ringbuffer_test;
uint8_t ringbuffer_test_buffer[8] = {0};

/**
 * @brief  处理UART接收缓冲区中的数据。
 *         如果在100ms内没有接收到新的数据，将清空缓冲区。
 * @param  None
 * @retval None
 */
// 处理UART接收缓冲区中的数据
void uart_proc(void)
{
    uint16_t length = rt_ringbuffer_data_len(&ringbuffer);
    if (length == 0) return;

    // 从环形缓冲区读取数据到读取缓冲区
    rt_ringbuffer_get(&ringbuffer, usart_read_buffer, length);

    if(strncmp((const char *)usart_read_buffer,"init",4) == 0)
    {
        ringbuffer_init(&ringbuffer_test);
        // rt_ringbuffer_init(&ringbuffer_test, ringbuffer_test_pool, sizeof(ringbuffer_test_pool));
        debug_info("ringbuffer init success");
    }
    else if(strncmp((const char *)usart_read_buffer,"put",3) == 0)
    {
        uint8_t data[] = {1,2,3};
        ringbuffer_write(&ringbuffer_test, data, sizeof(data));
        // rt_ringbuffer_put(&ringbuffer_test, data, sizeof(data));
        debug_info("ringbuffer put success");
        for(int i = 0; i < 8; i++)
        {
            // debug_info("pool-data:%d-%d ", ringbuffer_test_pool[i],ringbuffer_test_buffer[i]);
            debug_info("pool-data:%d-%d ", ringbuffer_test.buffer[i],ringbuffer_test_buffer[i]);
        }
    }
    else if(strncmp((const char *)usart_read_buffer,"get",3) == 0)
    {
        ringbuffer_read(&ringbuffer_test, ringbuffer_test_buffer, 2);
        // rt_ringbuffer_get(&ringbuffer_test, ringbuffer_test_buffer, 2);
        debug_info("ringbuffer get success");
        for(int i = 0; i < 8; i++)
        {
            // debug_info("pool-data:%d-%d ", ringbuffer_test_pool[i],ringbuffer_test_buffer[i]);
            debug_info("pool-data:%d-%d ", ringbuffer_test.buffer[i],ringbuffer_test_buffer[i]);
        }       
    }
    else if(strncmp((const char *)usart_read_buffer,"again_put",9) == 0)
    {
        uint8_t data[] = {4,5,6,7,8};
        ringbuffer_write(&ringbuffer_test, data, sizeof(data));
        // rt_ringbuffer_put(&ringbuffer_test, data, sizeof(data));
        debug_info("ringbuffer put again success");
        for (int i = 0; i < 8; i++)
        {
            // debug_info("pool-data:%d-%d ", ringbuffer_test_pool[i],ringbuffer_test_buffer[i]);
            debug_info("pool-data:%d-%d ", ringbuffer_test.buffer[i],ringbuffer_test_buffer[i]);
        }
    }
    else if(strncmp((const char *)usart_read_buffer,"again_get",9) == 0)
    {
        ringbuffer_read(&ringbuffer_test, ringbuffer_test_buffer,6);
        // rt_ringbuffer_get(&ringbuffer_test, ringbuffer_test_buffer, 8);
        debug_info("ringbuffer get success");
        for (int i = 0; i < 8; i++)
        {
            // debug_info("pool-data:%d-%d ", ringbuffer_test_pool[i],ringbuffer_test_buffer[i]);
            debug_info("pool-data:%d-%d ", ringbuffer_test.buffer[i],ringbuffer_test_buffer[i]);
        }
    }
    
    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(uint8_t) * BUUFER_SIZE);
}
