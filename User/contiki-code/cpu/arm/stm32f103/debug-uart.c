#include <debug-uart.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include <gpio.h>
#include "stm32f10x_dma.h"

#ifndef DBG_UART
#define DBG_UART USART1
#endif

#ifndef DBG_DMA_NO
#define DBG_DMA_NO 1
#endif

#ifndef DBG_DMA_CHANNEL_NO
#define DBG_DMA_CHANNEL_NO 4
#endif


#define _DBG_DMA_NAME(x) DMA##x
#define DBG_DMA_NAME(x) _DBG_DMA_NAME(x)
#define DBG_DMA DBG_DMA_NAME(DBG_DMA_NO)

#define _DMA_CHANNEL_NAME(x,c) DMA ## x ## _Channel ## c
#define DMA_CHANNEL_NAME(x,c) _DMA_CHANNEL_NAME(x,c)
#define DBG_DMA_CHANNEL  DMA_CHANNEL_NAME(DBG_DMA_NO, DBG_DMA_CHANNEL_NO)

#define _DBG_DMA_CHANNEL_IFCR_CGIF(c) DMA_IFCR_CGIF ## c
#define _XDBG_DMA_CHANNEL_IFCR_CGIF(c) _DBG_DMA_CHANNEL_IFCR_CGIF(c)
#define DBG_DMA_CHANNEL_IFCR_CGIF \
_XDBG_DMA_CHANNEL_IFCR_CGIF(DBG_DMA_CHANNEL_NO)


#ifndef DBG_XMIT_BUFFER_LEN
#define DBG_XMIT_BUFFER_LEN 1024
#endif


static unsigned char xmit_buffer[DBG_XMIT_BUFFER_LEN];
#define XMIT_BUFFER_END &xmit_buffer[DBG_XMIT_BUFFER_LEN]
void
dbg_setup_uart_default()
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
   
  //使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA\
                        | RCC_APB2Periph_USART1 ,ENABLE);
 
  //PA9 TX1 复用推挽输出
  GPIO_InitStructure.GPIO_Pin= GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  //PA10 RX1 浮动输入
  GPIO_InitStructure.GPIO_Pin= GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;   
  GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
 
  USART_InitStructure.USART_BaudRate= 9600;
  USART_InitStructure.USART_WordLength= USART_WordLength_8b;
  USART_InitStructure.USART_StopBits= USART_StopBits_1;
  USART_InitStructure.USART_Parity= USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode= USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1,&USART_InitStructure);
 
  //使能USART1
  USART_Cmd(USART1,ENABLE);
}

/* Valid data in head to tail-1 */
/* Read position */
static unsigned char * volatile xmit_buffer_head = xmit_buffer;

/* Write position */
static unsigned char * volatile xmit_buffer_tail = xmit_buffer;

/* xmit_buffer_head == xmit_buffer_tail means empty so we can only store
   DBG_XMIT_BUFFER_LEN-1 characters */

unsigned int
dbg_send_bytes(const unsigned char *seq, unsigned int len)
{
  /* Since each of the pointers should be read atomically
     there's no need to disable interrupts */
  unsigned char *head = xmit_buffer_head;
  unsigned char *tail = xmit_buffer_tail;
  if (tail >= head) {
    /* Free space wraps */
    unsigned int xfer_len = XMIT_BUFFER_END - tail;
    unsigned int free = DBG_XMIT_BUFFER_LEN - (tail - head) - 1;
    if (len > free) len = free;
    if (xfer_len < len) {
      memcpy(tail, seq, xfer_len);
      seq += xfer_len;
      xfer_len = len - xfer_len;
      memcpy(xmit_buffer, seq, xfer_len);
      tail = xmit_buffer + xfer_len;
    } else {
      memcpy(tail, seq, len);
      tail += len;
      if (tail == XMIT_BUFFER_END) tail = xmit_buffer;
    }
  } else {
    /* Free space continuous */
    unsigned int free = (head - tail) - 1;
    if (len > free) len = free;
    memcpy(tail, seq, len);
    tail += len;
  }
  xmit_buffer_tail = tail;

  return len;
}

static unsigned char dbg_write_overrun = 0;

void
dbg_putchar(const char ch)
{
  if (dbg_write_overrun) {
    if (dbg_send_bytes((const unsigned char*)"^",1) != 1) return;
  }
  dbg_write_overrun = 0;
  if (dbg_send_bytes((const unsigned char*)&ch,1) != 1) {
    dbg_write_overrun = 1;
  }
}


void
dbg_blocking_putchar(const char ch)
{
  if (dbg_write_overrun) {
    while (dbg_send_bytes((const unsigned char*)"^",1) != 1);
  }
  dbg_write_overrun = 0;
  while (dbg_send_bytes((const unsigned char*)&ch,1) != 1);
}

void
dbg_drain()
{
  while(xmit_buffer_tail != xmit_buffer_head);
}
