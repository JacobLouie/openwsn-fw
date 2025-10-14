/**
\brief This is a program which shows how to use the bsp modules for the board
       and UART.

\note: Since the bsp modules for different platforms have the same declaration,
       you can use this project with any platform.

Load this program on your board. Open a serial terminal client (e.g. PuTTY or
TeraTerm):
- You will read "Hello World!" printed over and over on your terminal client.
- when you enter a character on the client, the board echoes it back (i.e. you
  see the character on the terminal client) and the "ERROR" led blinks.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
*/

#include "stdint.h"
#include "stdio.h"
#include "string.h"
// bsp modules required
#include "board.h"
#include "uart.h"
#include "sctimer.h"
#include "leds.h"

//=========================== defines =========================================

#define SCTIMER_PERIOD     0xffff // 0xffff@32kHz = 2s
#define RX_BUFFER_SIZE     255 

//=========================== variables =======================================

typedef struct {
              uint8_t uart_lastTxByteIndex;
    volatile  uint8_t uartDone;

              uint8_t rxBuffer[RX_BUFFER_SIZE];
              uint8_t rxIndex;
    volatile  uint8_t msgEnd;
} app_vars_t;

app_vars_t app_vars;

//=========================== prototypes ======================================

void cb_compare(void);
void cb_uartTxDone(void);
uint8_t cb_uartRxCb(void);

//=========================== main ============================================

/**
\brief The program starts executing here.
*/
int mote_main(void) {
   
  // clear local variable
  memset(&app_vars,0,sizeof(app_vars_t));

  // initialize the board
  board_init();

  // setup UART
  uart_setCallbacks(cb_uartTxDone,cb_uartRxCb);
  uart_enableInterrupts();
   
  while(1) {

    // '/n' detected, replay message
    if (app_vars.msgEnd) {
        app_vars.msgEnd = 0;  // clear flag
        app_vars.uart_lastTxByteIndex = 0;

        while(app_vars.uart_lastTxByteIndex < app_vars.rxIndex){
          app_vars.uartDone = 0;
          uart_writeByte(app_vars.rxBuffer[app_vars.uart_lastTxByteIndex]);
          app_vars.uart_lastTxByteIndex++;
          while (app_vars.uartDone==0);
        }

        app_vars.rxIndex = 0;   // reset buffer index
        memset(app_vars.rxBuffer, 0, RX_BUFFER_SIZE);
    }
  }
}

//=========================== callbacks =======================================

void cb_uartTxDone(void) {
    app_vars.uartDone = 1;
}

uint8_t cb_uartRxCb(void) {
  uint8_t byte;

  // read received byte
  byte = uart_readByte();

  // toggle LED to show activity
  leds_error_toggle();

  // end of message
  if (byte == '\n') {   
      app_vars.rxBuffer[app_vars.rxIndex++] = '\n';
      app_vars.rxBuffer[app_vars.rxIndex] = '\r';
      app_vars.msgEnd = 1;
  } 
  else {
    // store byte in buffer
    if (app_vars.rxIndex < RX_BUFFER_SIZE - 1) {
        app_vars.rxBuffer[app_vars.rxIndex++] = byte;
    } else {
        // loop buffer
        app_vars.rxIndex = 0;
    }
  }

  return 0;
}