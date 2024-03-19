#ifndef __LCDCONTROLLER_H
#define __LCDCONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

// Includes ------------------------------------------------------------------//
#include "main.h"
#include "lvgl.h"
// DEFINES -------------------------------------------------------------------//

// TYPEDEFS ------------------------------------------------------------------//

// GLOBAL PROTOTYPES---------------------------------------------------------//
void lv_port_disp_init(void); // Initialize low level display driver 
void disp_enable_update(void); //Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
void disp_disable_update(void); //Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
lv_span_t * lv_span_init (void);
void lv_2leds_init (lv_obj_t * led1, lv_obj_t * led2);
lv_obj_t * button_init (void);
// MACROS--------------------------------------------------------------------//


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif 

