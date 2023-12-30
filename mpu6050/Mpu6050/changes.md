# Changes for my DMP
### in [inv_mpu.c](./driver/eMPL/inv_mpu.c)
1. #define MPU6050
2. #defined EMPL_TARGET_STM32F4
3. #define delay_ms    HAL_Delay

   #define get_ms      mget_ms

   #define log_i(...)       do {} while (0)

   #define log_e(...)       do {} while (0)

   #define min(a,b) ((a<b)?a:b)

   #define fabs        fabsf
4. Write i2c_write(),i2c_read() [line121-135](./driver/eMPL/inv_mpu.c)
5. Write mget_ms() [line 137-142](./driver/eMPL/inv_mpu.c)

