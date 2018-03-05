#include "project.h"
#include <stdio.h>

CY_ISR_PROTO(ISR_UART_rx_handler);
void handleByteReceived(uint8_t byteReceived);

volatile uint16_t result = 0;
volatile double newResult = 0.0;
volatile double reset = 0.0;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    char uartBuffer[256];

    isr_uart_rx_StartEx(ISR_UART_rx_handler);
    UART_1_Start();
    ADC_SAR_1_Start();
    ADC_SAR_1_StartConvert();
    
    UART_1_PutString("Scale application started\r\n");

    for(;;)
    {
        if (ADC_SAR_1_IsEndConversion(ADC_SAR_1_WAIT_FOR_RESULT))
        {
            result = ADC_SAR_1_GetResult16();
            newResult = (double) ((result - 567) / 1.5 - reset);
            snprintf(uartBuffer, sizeof(uartBuffer), "ADC result: %.0f \r\n", newResult);
            UART_1_PutString(uartBuffer);
        }
        CyDelay(1000);
    }
}

CY_ISR(ISR_UART_rx_handler)
{
    uint8_t bytesToRead = UART_1_GetRxBufferSize();
    while (bytesToRead > 0)
    {
        uint8_t byteReceived = UART_1_ReadRxData();
//        UART_1_WriteTxData(byteReceived); // echo back
        
        handleByteReceived(byteReceived);
        
        bytesToRead--;
    }
}

void handleByteReceived(uint8_t byteReceived)
{
    switch(byteReceived)
    {
        case '0' :
        {
            reset = (double) (result - 567) / 1.5;
            
            if (reset < 0) newResult += reset;
            else newResult -= reset;
        }
        break;
    }
}
