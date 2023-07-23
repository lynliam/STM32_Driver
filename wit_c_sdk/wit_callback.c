//
// Created by lynliam on 23-7-6.
//
#include "wit_callback.h"

float fAcc[3], fGyro[3], fAngle[3];
volatile char s_cDataUpdate = 1, s_cCmd = 0xff;
const uint32_t c_uiBaud[10] = {0, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};
unsigned char rx_buffer[11];


void CopeCmdData(unsigned char ucData)
{
    static unsigned char s_ucData[50], s_ucRxCnt = 0;

    s_ucData[s_ucRxCnt++] = ucData;
    if(s_ucRxCnt<3)return;										//Less than three data returned
    if(s_ucRxCnt >= 50) s_ucRxCnt = 0;
    if(s_ucRxCnt >= 3)
    {
        if((s_ucData[1] == '\r') && (s_ucData[2] == '\n'))
        {
            s_cCmd = s_ucData[0];
            memset(s_ucData,0,50);//
            s_ucRxCnt = 0;
        }
        else
        {
            s_ucData[0] = s_ucData[1];
            s_ucData[1] = s_ucData[2];
            s_ucRxCnt = 2;

        }
    }

}
void ShowHelp(void)
{
    printf("\r\n************************	 WIT_SDK_DEMO	************************");
    printf("\r\n************************          HELP           ************************\r\n");
    printf("UART SEND:a\\r\\n   Acceleration calibration.\r\n");
    printf("UART SEND:m\\r\\n   Magnetic field calibration,After calibration send:   e\\r\\n   to indicate the end\r\n");
    printf("UART SEND:U\\r\\n   Bandwidth increase.\r\n");
    printf("UART SEND:u\\r\\n   Bandwidth reduction.\r\n");
    printf("UART SEND:B\\r\\n   Baud rate increased to 115200.\r\n");
    printf("UART SEND:b\\r\\n   Baud rate reduction to 9600.\r\n");
    printf("UART SEND:R\\r\\n   The return rate increases to 10Hz.\r\n");
    printf("UART SEND:r\\r\\n   The return rate reduction to 1Hz.\r\n");
    printf("UART SEND:C\\r\\n   Basic return content: acceleration, angular velocity, angle, magnetic field.\r\n");
    printf("UART SEND:c\\r\\n   Return content: acceleration.\r\n");
    printf("UART SEND:h\\r\\n   help.\r\n");
    printf("******************************************************************************\r\n");
}

void CmdProcess(void)
{
    switch(s_cCmd)
    {
        case 'a':
            if(WitStartAccCali() != WIT_HAL_OK)
                printf("\r\nSet AccCali Error\r\n");
            break;
        case 'm':
            if(WitStartMagCali() != WIT_HAL_OK)
                printf("\r\nSet MagCali Error\r\n");
            break;
        case 'e':
            if(WitStopMagCali() != WIT_HAL_OK)
                printf("\r\nSet MagCali Error\r\n");
            break;
        case 'u':
            if(WitSetBandwidth(BANDWIDTH_5HZ) != WIT_HAL_OK)
                printf("\r\nSet Bandwidth Error\r\n");
            break;
        case 'U':
            if(WitSetBandwidth(BANDWIDTH_256HZ) != WIT_HAL_OK)
                printf("\r\nSet Bandwidth Error\r\n");
            break;
        case 'B':
            if(WitSetUartBaud(WIT_BAUD_115200) != WIT_HAL_OK)
                printf("\r\nSet Baud Error\r\n");
            else
                Usart2Init(c_uiBaud[WIT_BAUD_115200]);
            break;
        case 'b':
            if(WitSetUartBaud(WIT_BAUD_9600) != WIT_HAL_OK)
                printf("\r\nSet Baud Error\r\n");
            else
                Usart2Init(c_uiBaud[WIT_BAUD_9600]);
            break;
        case 'R':
            if(WitSetOutputRate(RRATE_10HZ) != WIT_HAL_OK)
                printf("\r\nSet Rate Error\r\n");
            break;
        case 'r':
            if(WitSetOutputRate(RRATE_1HZ) != WIT_HAL_OK)
                printf("\r\nSet Rate Error\r\n");
            break;
        case 'C':
            if(WitSetContent(RSW_ACC|RSW_GYRO|RSW_ANGLE|RSW_MAG) != WIT_HAL_OK)
                printf("\r\nSet RSW Error\r\n");
            break;
        case 'c':
            if(WitSetContent(RSW_ACC) != WIT_HAL_OK)
                printf("\r\nSet RSW Error\r\n");
            break;
        case 'h':
            ShowHelp();
            break;
    }
    s_cCmd = 0xff;
}

void SensorUartSend(uint8_t *p_data, uint32_t uiSize)
{
    HAL_UART_Transmit(&huart2,p_data, uiSize,0xffff);
}

void Delayms(uint16_t ucMs)
{
    delay_ms(ucMs);
}

void SensorDataUpdata(uint32_t uiReg, uint32_t uiRegNum)
{
    int i;
    for(i = 0; i < uiRegNum; i++)
    {
        switch(uiReg)
        {
//            case AX:
//            case AY:
            case AZ:
                s_cDataUpdate |= ACC_UPDATE;
                break;
//            case GX:
//            case GY:
            case GZ:
                s_cDataUpdate |= GYRO_UPDATE;
                break;
//            case HX:
//            case HY:
            case HZ:
                s_cDataUpdate |= MAG_UPDATE;
                break;
//            case Roll:
//            case Pitch:
            case Yaw:
                s_cDataUpdate |= ANGLE_UPDATE;
                break;
            default:
                s_cDataUpdate |= READ_UPDATE;
                break;
        }
        uiReg++;
    }
}

void AutoScanSensor(void)
{
    int i, iRetry;

    for(i = 1; i < 10; i++)
    {
        Usart2Init(c_uiBaud[i]);
        iRetry = 2;
        do
        {
            s_cDataUpdate = 0;
            WitReadReg(AX, 3);
            delay_ms(100);
            if(s_cDataUpdate != 0)
            {
                printf("%d baud find sensor\r\n\r\n", c_uiBaud[i]);
                ShowHelp();
                return ;
            }
            iRetry--;
        }while(iRetry);
    }
    printf("can not find sensor\r\n");
    printf("please check your connection\r\n");
}

void Usart2Init(unsigned int uiBaud)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = uiBaud;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK)
    {
        Error_Handler();
    }
}
