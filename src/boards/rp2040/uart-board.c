/*!
 * \file      uart-board.c
 *
 * \brief     Target board UART driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */

#include "pico/stdlib.h"
#include "utilities.h"
#include "board.h"
#include "uart-board.h"

/*!
 * Number of times the UartPutBuffer will try to send the buffer before
 * returning ERROR
 */
#define TX_BUFFER_RETRY_COUNT                       10

// static UART_HandleTypeDef UartHandle;
// uint8_t RxData = 0;
// uint8_t TxData = 0;

// extern Uart_t Uart2;

void UartMcuInit( Uart_t *obj, UartId_t uartId, PinNames tx, PinNames rx )
{
    gpio_set_function(tx, GPIO_FUNC_UART);
    gpio_set_function(rx, GPIO_FUNC_UART);
    uart_init(uart1, 115200);
}

void UartMcuConfig( Uart_t *obj, UartMode_t mode, uint32_t baudrate, WordLength_t wordLength, StopBits_t stopBits, Parity_t parity, FlowCtrl_t flowCtrl )
{
//     if( obj->UartId == UART_USB_CDC )
//     {
// #if defined( USE_USB_CDC )
//         UartUsbConfig( obj, mode, baudrate, wordLength, stopBits, parity, flowCtrl );
// #endif
//     }
//     else
//     {
//         UartHandle.Instance = USART2;
//         UartHandle.Init.BaudRate = baudrate;

//         if( mode == TX_ONLY )
//         {
//             if( obj->FifoTx.Data == NULL )
//             {
//                 assert_param( FAIL );
//             }
//             UartHandle.Init.Mode = UART_MODE_TX;
//         }
//         else if( mode == RX_ONLY )
//         {
//             if( obj->FifoRx.Data == NULL )
//             {
//                 assert_param( FAIL );
//             }
//             UartHandle.Init.Mode = UART_MODE_RX;
//         }
//         else if( mode == RX_TX )
//         {
//             if( ( obj->FifoTx.Data == NULL ) || ( obj->FifoRx.Data == NULL ) )
//             {
//                 assert_param( FAIL );
//             }
//             UartHandle.Init.Mode = UART_MODE_TX_RX;
//         }
//         else
//         {
//             assert_param( FAIL );
//         }

//         if( wordLength == UART_8_BIT )
//         {
//             UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
//         }
//         else if( wordLength == UART_9_BIT )
//         {
//             UartHandle.Init.WordLength = UART_WORDLENGTH_9B;
//         }

//         switch( stopBits )
//         {
//         case UART_2_STOP_BIT:
//             UartHandle.Init.StopBits = UART_STOPBITS_2;
//             break;
//         case UART_1_STOP_BIT:
//         default:
//             UartHandle.Init.StopBits = UART_STOPBITS_1;
//             break;
//         }

//         if( parity == NO_PARITY )
//         {
//             UartHandle.Init.Parity = UART_PARITY_NONE;
//         }
//         else if( parity == EVEN_PARITY )
//         {
//             UartHandle.Init.Parity = UART_PARITY_EVEN;
//         }
//         else
//         {
//             UartHandle.Init.Parity = UART_PARITY_ODD;
//         }

//         if( flowCtrl == NO_FLOW_CTRL )
//         {
//             UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
//         }
//         else if( flowCtrl == RTS_FLOW_CTRL )
//         {
//             UartHandle.Init.HwFlowCtl = UART_HWCONTROL_RTS;
//         }
//         else if( flowCtrl == CTS_FLOW_CTRL )
//         {
//             UartHandle.Init.HwFlowCtl = UART_HWCONTROL_CTS;
//         }
//         else if( flowCtrl == RTS_CTS_FLOW_CTRL )
//         {
//             UartHandle.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
//         }

//         UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

//         if( HAL_UART_Init( &UartHandle ) != HAL_OK )
//         {
//             assert_param( FAIL );
//         }

//         HAL_NVIC_SetPriority( USART2_IRQn, 1, 0 );
//         HAL_NVIC_EnableIRQ( USART2_IRQn );

//         /* Enable the UART Data Register not empty Interrupt */
//         HAL_UART_Receive_IT( &UartHandle, &RxData, 1 );
//     }
}

void UartMcuDeInit( Uart_t *obj )
{
    if( obj->UartId == UART_USB_CDC )
    {
#if defined( USE_USB_CDC )
        UartUsbDeInit( obj );
#endif
    }
    else
    {
        // __HAL_RCC_USART2_FORCE_RESET( );
        // __HAL_RCC_USART2_RELEASE_RESET( );
        // __HAL_RCC_USART2_CLK_DISABLE( );

        // GpioInit( &obj->Tx, obj->Tx.pin, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
        // GpioInit( &obj->Rx, obj->Rx.pin, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    }
}

uint8_t UartMcuPutChar( Uart_t *obj, uint8_t data )
{
    uart_putc(uart1, data);
}

uint8_t UartMcuGetChar( Uart_t *obj, uint8_t *data )
{
    *data = uart_getc(uart1);
}

uint8_t UartMcuPutBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size )
{
    uart_puts(uart1, buffer);
    return 0;
}

uint8_t UartMcuGetBuffer( Uart_t *obj, uint8_t *buffer, uint16_t size, uint16_t *nbReadBytes )
{
    uint16_t localSize = 0;

    while( localSize < size )
    {
        if( UartGetChar( obj, buffer + localSize ) == 0 )
        {
            localSize++;
        }
        else
        {
            break;
        }
    }

    *nbReadBytes = localSize;

    if( localSize == 0 )
    {
        return 1; // Empty
    }
    return 0; // OK
}
