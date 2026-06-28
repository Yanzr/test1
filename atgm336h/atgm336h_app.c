#include "atgm336h_app.h"
#include "usart.h"
#include "string.h"

///////////////////DMA缓存区BEGIN
//uartapp.c与.h文件解除注释，uart.c文件初始化中解除相应注释 it文件解除相应注释,主函数初始化,调度器
#define BUUFER_SIZE 1000
ringbuffer_t usart_rb2;
uint8_t usart_read_buffer2[BUUFER_SIZE];
uint8_t uart_rx_dma_buffer2[1000]={0};

char longitude[64] = "123N"; // 经度
char latitude[64] = "456E";  // 纬度

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


 void atgm336h_process(unsigned char *uartReadBuff)
{

    // 查找字符串 "$GNGGA" 和 "\r\n$GNGLL"，分别表示开始和结束的标志
    char *start = strstr((const char *)uartReadBuff, "$GNGGA");
    char *end = strstr((const char *)uartReadBuff, "\r\n$GNGLL");

    // 如果没有找到这两个标志，说明没有找到有效的 GPS 数据
    if (start == NULL || end == NULL)
    {
        // 如果需要调试，可以打印出来
		my_printf(&huart1,"[GPS]$GNGGA line NOT found\r\n");
    }
    else
    {
        // 成功找到开始和结束标志，提取数据
        // 创建一个足够存储 $GNGGA 数据的缓冲区
        char gngga[100];

        // 从 uartReadBuff 中提取从 $GNGGA 到 $GNGLL 之间的数据
        strncpy(gngga, start, end - start);
        gngga[end - start] = '\0'; // 确保字符串以 NULL 结尾

        // 打印提取的 GNGGA 数据（调试用）
		my_printf(&huart1,"---%s\r\n", gngga);

        // 定义分隔符和一个数组来存储解析出的字段
        char *token;
        token = strtok(gngga, ","); // 使用逗号分隔每个字段
        char *nmea_fields[15];      // 最多支持 15 个字段
        int i = 0;

        // 逐个字段提取并存入 nmea_fields 数组中
        while (token != NULL)
        {
            nmea_fields[i] = token;
            token = strtok(NULL, ","); // 获取下一个字段
            i++;
            if (i >= 15)
                break; // 限制字段数为 15
        }

        // 如果提取到的字段大于 6，说明数据有效
        if (i > 6)
        {
            // 成功提取到数据，处理经纬度
			my_printf(&huart1,"[GPS]data found\r\n");

//            // 将提取到的经度和纬度数据拼接到字符串中
//            sprintf(longitude, "%s %s", nmea_fields[2], nmea_fields[3]);
//            sprintf(latitude, "%s %s", nmea_fields[4], nmea_fields[5]);

//            // 打印经纬度数据（调试用）
//            printf("[GPS]%s %s,%s %s\r\n", nmea_fields[2], nmea_fields[3], nmea_fields[4], nmea_fields[5]);
            
            // 解析纬度
            int lat_deg = (int)(atof(nmea_fields[2]) / 100); // 取出度数（例如：2056.122314 -> 20）
            double lat_min = atof(nmea_fields[2]) - (lat_deg * 100); // 取出分数（例如：2056.122314 - 20*100 = 56.122314）

            // 计算纬度
            float latitude_decimal = lat_deg + (lat_min / 60);
            if (nmea_fields[3][0] == 'S') // 如果是南纬，取负
                latitude_decimal = -latitude_decimal;

            // 解析经度
            float lon_deg = atof(nmea_fields[4]) / 100; // 取出度数（例如：11002.398438 -> 110）
            float lon_min = atof(nmea_fields[4]) - (lon_deg * 100); // 取出分数（例如：11002.398438 - 110*100 = 2.398438）

            // 计算经度
            float longitude_decimal = lon_deg + (lon_min / 60);
            if (nmea_fields[5][0] == 'W') // 如果是西经，取负
                longitude_decimal = -longitude_decimal;
            
             // 打印转换后的经纬度数据（调试用）
			my_printf(&huart1,"[GPS]Latitude: %.6f, Longitude: %.6f\r\n", latitude_decimal, longitude_decimal);
		
        }
        else
        {
            // 如果字段数不够，说明数据无效
			my_printf(&huart1,"[GPS]data NOT found\r\n");
		
        }
    }
    // printf("---GPS_END---\r\n"); // 可选的调试输出
}

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
    // 打印数据
	my_printf(&huart1,"ringbuffer2 data: %s\n", usart_read_buffer2);
    atgm336h_process((unsigned char *)usart_read_buffer2);
    //////////////////////////////////////////////
    // 清空读取缓冲区
    memset(usart_read_buffer2, 0, sizeof(uint8_t) * BUUFER_SIZE);
}

///////////////////DMA缓存区END





