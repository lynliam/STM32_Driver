# WTR IO_retarget Library

WTRobot HITsz

## 介绍

在 STM32 平台，将stdout、stderr和stdin重定向到串口，这样就可以开心地使用 printf() 和 scanf() 了

（当然也可以使用 std::cout、std::cerr 和 std::cin，如果写 C++ 的话）

特性：
- 适配 STM32 HAL 库
- 支持 C 和 C++
- 可以为 stdout、stderr 和 stdin 指定不同的串口
- 对 FreeRTOS 可开启优化
- 适配多个编译器
- 实现 stdin 接收缓冲区，并可配置大小

目前适配的编译器：

- arm-none-eabi-gcc
- ARMCCv5
- ARMCCv6

## 轻松移植
1. 将 `io_retargetToUart.c` 和 `io_retargetToUart.h` 添加到你的工程中
2. 在 CubeMX 里初始化串口
   - 不使用接收缓冲区：不需要打开串口全局中断，不需要额外调用函数
   - 使用接收缓冲区：
     1. 打开串口全局中断
     2. 在代码中调用 `void IORetarget_Uart_Receive_IT()`
     3. 在串口中断函数中调用`IORetarget_Uart_RxCpltCallback()`，例如：
        ```
        void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
        {
            IORetarget_Uart_RxCpltCallback(huart);
        }
        ```
3. 修改 `io_retargetToUart.c` 中的如下内容，改为你要使用的串口
    ```
    static UART_HandleTypeDef *stdout_huart = &huart1;
    static UART_HandleTypeDef *stderr_huart = &huart1;
    static UART_HandleTypeDef *stdin_huart  = &huart1;
    ```
4. 在 `io_retargetToUart.h` 中：
   - 如果使用 FreeRTOS，建议打开 `#define IORETARGET_USE_RTOS`
   - 如果使用 stdin 接收缓冲区，配置 `IORETARGET_STDIN_BUFFER_SIZE` 大小（不使用的话，注释掉那行即可）
   
## 使用方法

移植完成后直接使用 `printf()` 和 `scanf()` 就可以，记得 `#include <stdio.h>`

**不能在中断中使用 `printf()` 和 `scanf()`等函数，否则串口容易锁死，并且 FreeRTOS 相关代码也会出问题**  
**不能在中断中使用 `printf()` 和 `scanf()`等函数，否则串口容易锁死，并且 FreeRTOS 相关代码也会出问题**  
**不能在中断中使用 `printf()` 和 `scanf()`等函数，否则串口容易锁死，并且 FreeRTOS 相关代码也会出问题**

> `io_retargetToUart.h` 只有在调用 `IORetarget_Uart_Receive_IT()` 和 `IORetarget_Uart_RxCpltCallback()` 的地方需要包含，使用 `printf()` 和 `scanf()`并不需要包含。

## Change Log

### 3.6
- 优化：进行了微小的代码格式化

### 3.5
- 修复：修复接收大量数据时，有概率发生 Overrun 错误的问题
- 新增：接收缓冲区

### 3.4
- 优化：对 FreeRTOS 进行优化