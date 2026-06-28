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
    uint8_t length=usart_rb.itemCount;
    // 从环形缓冲区读取数据到读取缓冲区
    ringbuffer_read(&usart_rb, usart_read_buffer, usart_rb.itemCount);
    ////////////////处理函数//////////////////
    // 打印读取缓冲区中的数据
  //  printf("ringbuffer data: %s\n", usart_read_buffer);
	
	flexible_parse(usart_read_buffer, length, &parsed_data);
	
    //////////////////////////////////////////////
    // 清空读取缓冲区
    memset(usart_read_buffer, 0, sizeof(uint8_t) * BUUFER_SIZE);
}

///////////////////DMA缓存区END


////////////////////////上传解析///////////////
#include <stdint.h>
#include <stdio.h>

// 定义帧头和功能码
#define FRAME_HEADER 0xAB
#define SOURCE_ADDRESS 0xDC   // 源地址
#define TARGET_ADDRESS 0xFE  // 目标上位机地址
#define FUNCTION_CODE 0x03   // 飞控姿态：欧拉角格式

// USART 发送函数原型
void usart_send_euler_angles(float roll, float pitch, float yaw, float fusion_state) {
    // 将角度乘以100，转换为int16格式
  int16_t  roll_int = (int16_t )(roll * 100.0f);    // 将 ROL 转换为 int16 格式
  int16_t  pitch_int = (int16_t)(pitch * 100.0f);  // 将 PIT 转换为 int16 格式
  int16_t  yaw_int =  (int16_t)(yaw * 100.0f);      // 将 YAW 转换为 int16 格式

    // 计算数据长度
    uint16_t data_length = 7; // ROL + PIT + YAW + FUSION_STA

    // 创建数据帧
    uint8_t data_frame[10 + data_length]; // 10字节的固定头部 + 数据长度
    uint16_t index = 0;

    // 填充帧头
    data_frame[index++] = FRAME_HEADER;
    
    // 填充源地址
    data_frame[index++] = SOURCE_ADDRESS;
    
    // 填充目标地址
    data_frame[index++] = TARGET_ADDRESS;
    
    // 填充功能码
    data_frame[index++] = FUNCTION_CODE;

    // 填充数据长度（小端模式）
    data_frame[index++] = (data_length & 0xFF);       // 低字节
    data_frame[index++] = (data_length >> 8) & 0xFF;  // 高字节

    // 填充数据内容（按顺序填充欧拉角和融合状态）
    data_frame[index++] = (roll_int & 0xFF);       // ROL 低字节
    data_frame[index++] = (roll_int >> 8) & 0xFF;  // ROL 高字节

    data_frame[index++] = (pitch_int & 0xFF);      // PIT 低字节
    data_frame[index++] = (pitch_int >> 8) & 0xFF; // PIT 高字节

    data_frame[index++] = (yaw_int & 0xFF);        // YAW 低字节
    data_frame[index++] = (yaw_int >> 8) & 0xFF;   // YAW 高字节

    data_frame[index++] = fusion_state;         // 融合状态

    // 计算和校验
    uint8_t sum_check = 0;
    uint8_t add_check = 0;
    
    for (uint16_t i = 0; i < index; i++) {
        sum_check += data_frame[i];
        add_check += sum_check; // 进行累加以计算附加校验
    }

    // 填充和校验字节
    data_frame[index++] = sum_check;
    data_frame[index++] = add_check;


	for(int i=0;i<index;i++)
	{
    // 发送缓存区内容
    printf("%c", data_frame[i]); // 打印缓存区内容
	}

    // 也可以在此处添加代码将数据发送到实际的 USART 硬件
}

/////////////////////接收解析欧拉角----未测试

//#define MAX_DATA_LENGTH 7 // 实际数据长度
//#define FRAME_SIZE (10 + MAX_DATA_LENGTH) // 假设最大数据帧大小

//// usart_parse 函数定义
//uint8_t usart_parse_euler_angles(uint8_t *buffer, uint16_t length) {
//    uint8_t index = 0; // 使用 index 作为索引

//    // 1. 判断长度
//    if (length < FRAME_SIZE) { // 检查接收到的长度是否小于最小数据帧长度
//        printf("错误: 数据长度不足\n");
//        return 0; // 失败返回 0
//    }

//    // 2. 判断帧头
//    if (buffer[index++] != FRAME_HEADER) {
//        printf("错误: 帧头不匹配 (expected: 0x%02X, got: 0x%02X)\n", FRAME_HEADER, buffer[0]);
//        return 0; // 失败返回 0
//    }
//    
//    // 3. 读取源地址、目标地址、功能码
//    uint8_t source_address = buffer[index++];
//    uint8_t target_address = buffer[index++];
//    uint8_t function_code = buffer[index++];

//    // 4. 读取数据长度
//    uint16_t data_length = (buffer[index++] | (buffer[index++] << 8));
//    
//    // 5. 判断长度是否正确
//    if (data_length != MAX_DATA_LENGTH) {
//        printf("错误: 数据长度不匹配 (expected: %d, got: %d)\n", MAX_DATA_LENGTH, data_length);
//        return 0; // 失败返回 0
//    }

//    // 6. 读取数据内容（ROL、PIT、YAW 和融合状态）
//    int16_t roll_int = (buffer[index++] | (buffer[index++] << 8));
//    int16_t pitch_init = (buffer[index++] | (buffer[index++] << 8));
//    int16_t yaw_init = (buffer[index++] | (buffer[index++] << 8));
//    uint8_t fusion_state = buffer[index++];
//	
//	float roll=roll_int/100.f;
//	float pitch=pitch_init/100.f;
//	float yaw=yaw_init/100.f;
//	

//    // 7. 读取校验位
//    uint8_t sum_check = buffer[index++];
//    uint8_t add_check = buffer[index++];

//    // 校验和
//    uint8_t calculated_sum = 0;
//    uint8_t calculated_add = 0;

//    for (uint8_t i = 0; i < index; i++) { // 计算前13个字节的和
//        calculated_sum += buffer[i];
//        calculated_add += calculated_sum; // 累加和
//    }

//    // 校验
//    if (calculated_sum != sum_check || calculated_add != add_check) {
//        printf("错误: 校验失败 (expected sum: 0x%02X, got: 0x%02X; expected add: 0x%02X, got: 0x%02X)\n", sum_check, calculated_sum, add_check, calculated_add);
//        return 0; // 失败返回 0
//    }

//    // 8. 打印结果
//    printf("解析成功:\n");
//    printf("源地址: 0x%02X\n", source_address);
//    printf("目标地址: 0x%02X\n", target_address);
//    printf("功能码: 0x%02X\n", function_code);
//    printf("横滚角: %.2f\n", roll );
//    printf("俯仰角: %.2f\n", pitch );
//    printf("航向角: %.2f\n", yaw );
//    printf("融合状态: %d\n", fusion_state);
//    printf("和校验: 0x%02X\n", sum_check);
//    printf("附加校验: 0x%02X\n", add_check);

//    return 1; // 成功返回 1
//}

#define FRAME_HEADER 0xAB
#define MAX_DATA_LENGTH 10 // 假设最大数据长度
#define FRAME_SIZE (10 + MAX_DATA_LENGTH) // 假设最大数据帧大小



//// 定义数据结构体
//typedef struct {
//    int16_t int16_value; // int16 数据
//    int8_t int8_value;   // int8 数据
//    float float_value;   // float 数据
//} DataPacket;

DataPacket parsed_data;

float bytes_to_float(uint8_t *bytes) {
	float value;
    memcpy(&value, bytes, sizeof(float)); // 将字节转换为 float
	return value;
}

// usart_parse 函数定义
uint8_t flexible_parse(uint8_t *buffer, uint16_t length, DataPacket *data) {
    uint8_t index = 0; // 使用 index 作为索引

    // 1. 判断长度
    if (length < 15) { // 至少要有帧头、数据长度字段和校验位
        printf("错误: 数据长度不足\n");
        return 0;
    }

    // 2. 判断帧头
    if (buffer[index++] != FRAME_HEADER) {
        printf("错误: 帧头不匹配 (expected: 0x%02X, got: 0x%02X)\n", FRAME_HEADER, buffer[0]);
        return 0;
    }

    // 3. 读取源地址、目标地址、功能码
    uint8_t source_address = buffer[index++];
    uint8_t target_address = buffer[index++];
    uint8_t function_code = buffer[index++];

    // 4. 读取数据长度
    uint16_t data_length = (buffer[index++] | (buffer[index++] << 8));
    
    // 5. 判断长度是否正确
    if (data_length + index > length) {
        printf("错误: 数据长度不匹配 (expected: %d, available: %d)\n", data_length, length - index);
        return 0;
    }

    // 6. 读取数据内容
    printf("解析数据内容:\n");

    // 假设顺序为 int16, int8, float
    data->int16_value = (buffer[index++] | (buffer[index ++] << 8));

    data->int8_value = buffer[index++];

	data->float_value =bytes_to_float( &buffer[index]);
    index += sizeof(float); // 增加索引

    // 7. 读取校验位
    if (index >= length) {
        printf("错误: 校验位缺失\n");
        return 0;
    }
    uint8_t sum_check = buffer[index++];
    uint8_t calculated_sum = 0;

    // 计算和
    for (uint8_t i = 0; i < index - 1; i++) {
        calculated_sum += buffer[i];
    }

    // 校验
    if (calculated_sum != sum_check) {
        printf("错误: 校验失败 (expected: 0x%02X, got: 0x%02X)\n", sum_check, calculated_sum);
        return 0;
    }
	printf("数据类型: int16, 值: %d\n", data->int16_value);
	printf("数据类型: int8, 值: %d\n", data->int8_value);
	printf("数据类型: float, 值: %.2f\n", data->float_value);

    // 8. 返回成功
    printf("解析成功，数据长度: %d\n", data_length);
    return 1;
}







