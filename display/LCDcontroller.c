
#if 1

// Includes -----------------------------------------------------------------------------------------//
#include 	"LCDcontroller.h"
#include "lvgl.h"
#include 	<stdbool.h>
#include 	"st7735.h"
#include "touch.h"

// DEFINES ------------------------------------------------------------------------------------------//
#define DISP_HOR_RES    160
#define DISP_VER_RES    128

// TYPEDEFS ----------------------------------------------------------------------------------------//

// STATIC PROTOTYPES--------------------------------------------------------------------------------//
static void disp_init(void);
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t * data);

// STATIC VARIABLES---------------------------------------------------------------------------------//
static lv_disp_drv_t disp_drv;	//Descriptor of a display driver

static int32_t x1_flush;
static int32_t y1_flush;
static int32_t x2_flush;
static int32_t y2_fill;
static int32_t y_fill_act;

static const lv_color_t * buf_to_flush;

// MACROS-------------------------------------------------------------------------------------------//

//FUNCTIONS-----------------------------------------------------------------------------------------//
void lv_port_disp_init(void)
{
	disp_init(); //Initialize your display
	//Create a buffer for drawing
	/**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
	*/

    // Example for 1
   // static lv_disp_draw_buf_t draw_buf_dsc_1;
   // static lv_color_t buf_1[DISP_HOR_RES	* 10];                          			//A buffer for 10 rows
   // lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, DISP_HOR_RES * 10);   //Initialize the display buffer

    // Example for 2
    static lv_disp_draw_buf_t draw_buf_dsc_2;
    static lv_color_t buf_2_1[DISP_HOR_RES * 10];                        /*A buffer for rows*/
    static lv_color_t buf_2_2[DISP_HOR_RES * 10];                        /*An other buffer for rows*/
		lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, DISP_HOR_RES * 10);   /*Initialize the display buffer*/
   
	 // Example for 3 also set disp_drv.full_refresh = 1 below
  //  static lv_disp_draw_buf_t draw_buf_dsc_3;
  //  static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];			//A screen sized buffer
   // static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];			//Another screen sized buffer
   // lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,	MY_DISP_VER_RES * LV_VER_RES_MAX);	//Initialize the display buffer*/


    
		//Register the display in LVGL
    lv_disp_drv_init(&disp_drv);		//Basic initialization

    //Set up the functions to access to your display
    disp_drv.hor_res = DISP_HOR_RES;   //Set the resolution of the display
    disp_drv.ver_res = DISP_VER_RES;

    disp_drv.flush_cb = disp_flush;  //Used to copy the buffer's content to the display

    disp_drv.draw_buf = &draw_buf_dsc_2;  //Set a display buffer
	//	disp_drv.draw_buf = &draw_buf_dsc_1;
    /*Required for Example 3)*/
    //disp_drv.full_refresh = 1;

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

		lv_disp_drv_register(&disp_drv);  //Finally register the driver
		
		static lv_indev_drv_t indev_drv;
		#if 0
		lv_indev_drv_init(&indev_drv);      //Basic initialization
		indev_drv.type = LV_INDEV_TYPE_POINTER; //touchpad or mouse
		indev_drv.read_cb = my_input_read;
		
		#endif
		
}

//STATIC FUNCTIONS---------------------------------------------------------//

//--------------------------Initialize your display and the required peripherals--------------------------//
static void disp_init(void)
{ lcdInit();	}

volatile bool disp_flush_enabled = true;

//----------Enable updating the screen (the flushing process) when disp_flush() is called by LVGL----------//
void disp_enable_update(void)
{
	disp_flush_enabled = true;
}

//---------Disable updating the screen (the flushing process) when disp_flush() is called by LVGL---------//
void disp_disable_update(void)
{
	disp_flush_enabled = false;
}

//-------------------------------------------------------------------------------------------------//
/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
	
  //Set the drawing region
	if (area->x2 < 0)
		return;
	if (area->y2 < 0)
		return;
	if (area->x1 > DISP_HOR_RES)
		return;
	if (area->y1 > DISP_VER_RES)
		return;
	
	st7735SetAddrWindow(area->x1, area->y1, area->x2, area->y2);
	int height = area->y2 - area->y1 + 1;
	int width = area->x2 - area->x1 + 1;
	LCD_DrawBitmapDMA(width, height, (uint8_t *)color_p);
	//LCD_DrawBitmap(width, height, (uint8_t *)color_p); //без DMA
	//IMPORTANT!!!  Inform the graphics library that you are ready with the flushing//
//	lv_disp_flush_ready(disp_drv);
}

#if 0	
//-------------------------------------------------------------------------------------------------//
static void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
  if(isTouch() == TRUE) 
	{
    data->point.x = touchpad_x;
    data->point.y = touchpad_y;
    data->state = LV_INDEV_STATE_PRESSED;
  } 
	else 
	{
    data->state = LV_INDEV_STATE_RELEASED; 
  }
}
#endif

//-------------------------------------------------------------------------------------------------//
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	lv_disp_flush_ready(&disp_drv);
}

//-------------------------------------------------------------------------------------------------//
lv_span_t *  lv_span_init (void)
{
	//	snprintf (time, sizeof (time), "%u:%u:%u", hour, min, sec);
	
    static lv_style_t style;
    lv_style_init(&style); //инициализация стиля
    lv_style_set_border_width(&style, 2); //установка ширины границы
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_ORANGE)); //установка цвета границы
		lv_style_set_border_side(&style, LV_BORDER_SIDE_FULL);
    lv_style_set_pad_all(&style, 1); //установка отступов
    lv_style_set_align(&style, LV_TEXT_ALIGN_CENTER); //Выравнивание текста

    lv_obj_t *spans = lv_spangroup_create(lv_scr_act()); //объект, который используется для отображения форматированного текста. В отличие от объекта label, spangroup может автоматически упорядочивать текст различных шрифтов, цветов и размеров в объект spangroup.
    lv_obj_set_width(spans, 120); //ширина
    lv_obj_set_height(spans, 40); //высота
    //lv_obj_center(spans); //поместить объект в центр
		lv_obj_align(spans, LV_ALIGN_CENTER, 0, 10);
    lv_obj_add_style(spans, &style, 0); //указание используемого объектом стиля

    lv_spangroup_set_align(spans, LV_TEXT_ALIGN_CENTER); //установка режима - Выравнивание текста 
    lv_spangroup_set_overflow(spans, LV_SPAN_OVERFLOW_CLIP); //режим переполнения объекта
    lv_spangroup_set_indent(spans, 0); //отступ первой строки. Все режимы поддерживают единицы измерения в пикселях
    lv_spangroup_set_mode(spans, LV_SPAN_MODE_BREAK); //Set the mode of the spangroup. LV_SPAN_MODE_BREAK - Сохраняйте ширину, разбивайте слишком длинные линии и автоматически увеличивайте высоту.

    lv_span_t * clock_span = lv_spangroup_new_span(spans); //создание строкового дескриптор span
	//	lv_span_set_text(span, time); //Задайте новый текст для spanа. Память будет выделена для хранения текста.
		lv_style_set_text_color(&clock_span->style, lv_palette_main(LV_PALETTE_RED)); //установка цвета шрифта
		lv_style_set_text_font(&clock_span->style, &lv_font_montserrat_24);
 //  lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_UNDERLINE); //выбор декорирования текста - UNDERLINE Подчёркнуто
    lv_style_set_text_opa(&clock_span->style, LV_OPA_90); //Процент непрозрачности


 /*   span = lv_spangroup_new_span(spans);
    lv_span_set_text_static(span, "OOOOOOOOOPs!");
    lv_style_set_text_color(&span->style, lv_palette_main(LV_PALETTE_GREEN));
    //lv_style_set_text_font(&span->style, &lv_font_montserrat_20);
    lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_STRIKETHROUGH );
    lv_style_set_text_opa(&span->style, LV_OPA_60); //Процент непрозрачности*/

    lv_spangroup_refr_mode(spans); //Если режим объекта != LV_SPAN_MODE_FIXED, вы должны вызвать lv_spangroup_refr_mode() после того, как вы изменили стиль span
		return clock_span;
}

//-------------------------------------------------------------------------------------------------//
void lv_2leds_init (lv_obj_t * led1, lv_obj_t * led2)
{
  lv_obj_align(led1, LV_ALIGN_TOP_LEFT, 5, 10);
	lv_obj_set_size(led1, 25, 25);
	lv_led_set_brightness(led1, LV_LED_BRIGHT_MAX);
  lv_led_set_color(led1, lv_palette_main(LV_PALETTE_RED));
  lv_led_off(led1);

  lv_obj_align(led2, LV_ALIGN_TOP_RIGHT, -5, 10);
	lv_obj_set_size(led2, 25, 25);
	lv_led_set_brightness(led2, LV_LED_BRIGHT_MAX);
  lv_led_set_color(led2, lv_palette_main(LV_PALETTE_GREEN));
  lv_led_on(led2);
}

//-------------------------------------------------------------------------------------------------//
lv_obj_t *  button_init (void)
{
	//Init the style for the default state//
  static lv_style_t button_style;
	lv_style_init(&button_style);
	lv_style_set_radius(&button_style, 5);
	lv_style_set_bg_opa(&button_style, LV_OPA_COVER);
	lv_style_set_bg_color(&button_style, lv_palette_lighten(LV_PALETTE_GREY, 2)); //цвет фона
	lv_style_set_border_width(&button_style, 2);
	lv_style_set_border_color(&button_style, lv_palette_main(LV_PALETTE_RED)); //цвет границ контура
	lv_style_set_border_side(&button_style, LV_BORDER_SIDE_FULL);
	lv_style_set_pad_all(&button_style, 3);
	lv_style_set_text_color(&button_style, lv_palette_main(LV_PALETTE_BLUE)); //цвет текста
	lv_style_set_text_font(&button_style, &lv_font_montserrat_14);
	lv_style_set_text_letter_space(&button_style, 5);
	lv_style_set_text_line_space(&button_style, 10);
	
	//button create//
	lv_obj_t * button = lv_btn_create(lv_scr_act());
	lv_obj_set_size(button, 80, 30);
	lv_obj_align(button, LV_ALIGN_BOTTOM_MID, 0, -5);	
	
	//label button create//
	lv_obj_t * label;
	label = lv_label_create(button);
	lv_obj_add_style(label, &button_style, 0); //привязка стиля к надписи на кнопке
	lv_label_set_text(label, "Button");
	lv_obj_center(label);
	return button;
}
#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif