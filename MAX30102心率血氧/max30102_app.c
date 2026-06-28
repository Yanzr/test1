#include "max30102_app.h"

// 初始化数据结构
MAX30102_Data max30102_data = {
    .buffer_length = BUFFER_LENGTH, // 缓冲区长度
    .min_value = 0x3FFFF,           // 初始最小值
    .max_value = 0,                 // 初始最大值
    .brightness = 0                 // 初始亮度值
};

/********************************** 滤波算法的全局变量 *************************************************/

// 滑动平均滤波器缓存
int hr_buffer[WINDOW_SIZE] = {0};   // 用于心率的滑动窗口
int spo2_buffer[WINDOW_SIZE] = {0}; // 用于血氧的滑动窗口
int hr_index = 0, spo2_index = 0;   // 缓存索引

// 低通滤波器的先前值
int prev_hr = 0, prev_spo2 = 0; // 上一时刻的平滑心率和血氧值

/********************************** 滤波算法的全局变量 *************************************************/

/********************************** 滤波算法 *************************************************/
// 滑动平均滤波函数
int SmoothData(int new_value, int *buffer, int *index)
{
  buffer[*index] = new_value;          // 更新滑动窗口
  *index = (*index + 1) % WINDOW_SIZE; // 循环索引

  int sum = 0;
  for (int i = 0; i < WINDOW_SIZE; i++)
  {
    sum += buffer[i]; // 计算窗口内的平均值
  }

  return sum / WINDOW_SIZE; // 返回平均值
}

// 低通滤波函数
int LowPassFilter(int new_value, int previous_filtered_value)
{
  return (int)(ALPHA * new_value + (1 - ALPHA) * previous_filtered_value); // 按公式进行滤波
}

/********************************** 滤波算法 *************************************************/

// 读取MAX30102传感器数据，并进行心率补偿
void MAX30102_Read_Data(void)
{
  volatile uint32_t un_prev_data;
  uint8_t temp[6];
  // 读取前500个样本，确定信号范围
  for (int i = 0; i < max30102_data.buffer_length; i++)
  {
    while (MAX30102_INT == 1)
      ; // 等待中断信号

    max30102_FIFO_ReadBytes(REG_FIFO_DATA, temp); // 从FIFO读取数据
    max30102_data.red_buffer[i] = (long)((long)((long)temp[0] & 0x03) << 16) | (long)temp[1] << 8 | (long)temp[2];
    max30102_data.ir_buffer[i] = (long)((long)((long)temp[3] & 0x03) << 16) | (long)temp[4] << 8 | (long)temp[5];

    // 更新信号的最小值和最大值
    if (max30102_data.min_value > max30102_data.red_buffer[i])
      max30102_data.min_value = max30102_data.red_buffer[i];
    if (max30102_data.max_value < max30102_data.red_buffer[i])
      max30102_data.max_value = max30102_data.red_buffer[i];
  }

  // 更新上一数据点
  un_prev_data = max30102_data.red_buffer[max30102_data.buffer_length - 1];

  // 调整补偿值：减少补偿并应用在心率计算前
  max30102_data.heart_rate += HEART_RATE_COMPENSATION; // 可减少补偿值
}

// 计算心率和血氧值
void Calculate_Heart_Rate_and_SpO2(void)
{
  maxim_heart_rate_and_oxygen_saturation(max30102_data.ir_buffer, max30102_data.buffer_length,
                                         max30102_data.red_buffer, &max30102_data.spO2, &max30102_data.spO2_valid,
                                         &max30102_data.heart_rate, &max30102_data.heart_rate_valid);
}

// 更新信号的最小值和最大值，并应用滤波
void Update_Signal_Min_Max(void)
{
  uint32_t un_prev_data = max30102_data.red_buffer[max30102_data.buffer_length - 1];

  for (int i = 100; i < max30102_data.buffer_length; i++)
  {
    // 移位数据
    max30102_data.red_buffer[i - 100] = max30102_data.red_buffer[i];
    max30102_data.ir_buffer[i - 100] = max30102_data.ir_buffer[i];

    // 低通滤波处理
    max30102_data.red_buffer[i - 100] = LowPassFilter(max30102_data.red_buffer[i - 100], un_prev_data);
    max30102_data.ir_buffer[i - 100] = LowPassFilter(max30102_data.ir_buffer[i - 100], un_prev_data);

    // 滑动平均滤波
    max30102_data.red_buffer[i - 100] = SmoothData(max30102_data.red_buffer[i - 100], hr_buffer, &hr_index);
    max30102_data.ir_buffer[i - 100] = SmoothData(max30102_data.ir_buffer[i - 100], spo2_buffer, &spo2_index);

    // 更新最小值和最大值
    if (max30102_data.min_value > max30102_data.red_buffer[i - 100])
      max30102_data.min_value = max30102_data.red_buffer[i - 100];
    if (max30102_data.max_value < max30102_data.red_buffer[i - 100])
      max30102_data.max_value = max30102_data.red_buffer[i - 100];
  }
}

// 显示相关字符串
uint8_t dis_hr = 0;   // 显示的心率值
uint8_t dis_spo2 = 0; // 显示的血氧值

// 数据处理与显示
void Process_And_Display_Data(void)
{
  if (max30102_data.heart_rate_valid == 1 && max30102_data.heart_rate < 150)
  {
    dis_hr = max30102_data.heart_rate;
    dis_spo2 = max30102_data.spO2;
    printf("dis_hr:%d  ,dis_spo2:%d\r\n",dis_hr,dis_spo2);

  }
  
  else
  {
    dis_hr = 0;
    dis_spo2 = 0;
    printf("0000:dis_hr:%d  ,dis_spo2:%d\r\n",dis_hr,dis_spo2);
  }
}

void max30102_proc(void)
{
    MAX30102_Read_Data();
    Calculate_Heart_Rate_and_SpO2();
    Update_Signal_Min_Max();
    Process_And_Display_Data();
}
