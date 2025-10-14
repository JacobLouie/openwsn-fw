/**
\brief This program shows the use of the "sctimer" bsp module.

Since the bsp modules for different platforms have the same declaration, you
can use this project with any platform.

Load this program onto your board, and start running. It will enable the sctimer. 
The sctimer is periodic, of period SCTIMER_PERIOD ticks. Each time it elapses:
    - the frame debugpin toggles
    - the error LED toggles

\author Tengfei Chang <tengfei.chang@eecs.berkeley.edu>, April 2017.
*/

#include "stdint.h"
#include "string.h"
#include "board.h"
#include "debugpins.h"
#include "leds.h"
#include "sctimer.h"

//=========================== defines =========================================

#define SCTIMER_500ms      16384 // @32kHz = 500ms

//=========================== variables =======================================

typedef struct {
    uint8_t led2_count;
    uint8_t led3_count;
    uint8_t led4_count;
} app_vars_t;

app_vars_t app_vars;

//=========================== prototypes ======================================

void cb_compare(void);
void some_delay(void);

//=========================== main ============================================

/**
\brief The program starts executing here.
*/
int mote_main(void) {  
   
  // initialize board. 
  board_init();
  memset(&app_vars, 0, sizeof(app_vars_t));

  sctimer_set_callback(cb_compare);
  sctimer_setCompare(sctimer_readCounter()+SCTIMER_500ms);

  while (1) {
    board_sleep();
  }
}

//=========================== callbacks =======================================

void cb_compare(void){
  uint32_t currentTime = sctimer_readCounter();
  // toggle pin
  //debugpins_frame_toggle();

  app_vars.led2_count++;
  app_vars.led3_count++;
  app_vars.led4_count++;
  
  // LED 1 every 500ms
  leds_error_on();

  // LED 2 every 1000ms
  if (app_vars.led2_count == 2){
    // toggle error led
    leds_sync_on();
    app_vars.led2_count = 0;
  };

  // LED 3 every 1500ms
  if (app_vars.led3_count == 3){
    // toggle sync led
    leds_radio_on();
    app_vars.led3_count = 0;
  };

  // LED 4 every 2000ms
  if (app_vars.led4_count == 4){
    // toggle radio led
    leds_debug_on();
    app_vars.led4_count = 0;
  };
 
  // schedule again
  sctimer_setCompare(sctimer_readCounter()+SCTIMER_500ms);

  some_delay();
  leds_all_off();
}

void some_delay(void) {
   volatile uint32_t delay;
   for (delay=0x186A00;delay>0;delay--);
}
