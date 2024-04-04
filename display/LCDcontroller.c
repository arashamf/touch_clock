
// Includes -----------------------------------------------------------------------------------------//
#include "main.h"
#include "gpio.h"
#include "st7735.h"
#include "touch.h"
#include "LCDcontroller.h"
#include "lvgl.h"

// DEFINES ------------------------------------------------------------------------------------------//
#define DISP_HOR_RES    160
#define DISP_VER_RES    128

#define TOUCH

// TYPEDEFS ----------------------------------------------------------------------------------------//

// STATIC PROTOTYPES--------------------------------------------------------------------------------//
static void disp_init(void);
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * drv, lv_indev_data_t * data);
static void event_handler_main_btn(lv_event_t * event);
static void event_handler_date (lv_event_t * e);
static void event_handler_windate(lv_event_t * e);

//VARIABLES-----------------------------------------------------------------------------------------//
static lv_disp_drv_t disp_drv;	//Descriptor of a display driver
volatile bool disp_flush_enabled = true;

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
		
}

//--------------------------Initialize your display and the required peripherals--------------------------//
static void disp_init(void)
{ lcdInit();	}

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

//---------------------------------------------------------------------------------------------------//
void lv_port_indev_init(void)
{
	static lv_indev_drv_t indev_drv;
		
	#ifdef TOUCH
	touchpad_init();	
	lv_indev_drv_init(&indev_drv);      //Basic initialization
	indev_drv.type = LV_INDEV_TYPE_POINTER; //touchpad or mouse
	indev_drv.read_cb = touchpad_read;
	lv_indev_drv_register( &indev_drv ); //регистрация драйвера тачскрина и сохранение его настроек
	#endif
}

//---------------------------------------------------------------------------------------------------//
static void touchpad_init(void)
{
   TouchInit();
}

//-------------------------------------------------------------------------------------------------//
static void touchpad_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
	static lv_coord_t last_x = 0;
  static lv_coord_t last_y = 0;
	#ifdef TOUCH
  if(ILI9341_TouchGetCoordinates(&last_x, &last_y)  == TRUE) 
	{
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = LV_INDEV_STATE_PRESSED;
  } 
	else 
	{	data->state = LV_INDEV_STATE_RELEASED; }
	#endif
}

//-------------------------------------------------------------------------------------------------//
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	lv_disp_flush_ready(&disp_drv);
}

//-----------------------------------screen's background color-----------------------------------//
lv_obj_t * lv_screen_init (lv_obj_t * scr)
{
	lv_obj_set_style_bg_color(scr, lv_color_hex(LV_PALETTE_INDIGO), LV_PART_MAIN);
	lv_obj_set_style_text_color(scr, lv_color_hex(0xffffff), LV_PART_MAIN);
	return scr;
}

//-----------------------------инициализация поля с данными времени-----------------------------//
lv_span_t * lv_span_init (lv_obj_t * scr)
{
	static lv_style_t style;
	lv_style_init(&style); //инициализация стиля
	lv_style_set_border_width(&style, 2); //установка ширины границы
	lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_ORANGE)); //установка цвета границы
	lv_style_set_border_side(&style, LV_BORDER_SIDE_FULL);
  lv_style_set_pad_all(&style, 1); //установка отступов
	lv_style_set_align(&style, LV_TEXT_ALIGN_CENTER); //Выравнивание текста

	//В отличие от объекта label, spangroup может отображать текст, стилизованный под другие шрифты, цвета и размеры, в объект spangroup
	lv_obj_t *spans = lv_spangroup_create(scr); //объект, который используется для отображения форматированного текста. В отличие от объекта label, spangroup может автоматически упорядочивать текст различных шрифтов, цветов и размеров в объект spangroup.
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
	lv_style_set_text_color(&clock_span->style, lv_palette_main(LV_PALETTE_RED)); //установка цвета шрифта
	lv_style_set_text_font(&clock_span->style, &lv_font_montserrat_24);
	//lv_span_set_text(span, time); //Задайте новый текст для spanа. Память будет выделена для хранения текста.
	//lv_style_set_text_decor(&span->style, LV_TEXT_DECOR_UNDERLINE); //выбор декорирования текста - UNDERLINE Подчёркнуто
	lv_style_set_text_opa(&clock_span->style, LV_OPA_90); //Процент непрозрачности

	lv_spangroup_refr_mode(spans); //Если режим объекта != LV_SPAN_MODE_FIXED, вы должны вызвать lv_spangroup_refr_mode() после того, как вы изменили стиль span
	return clock_span;
}

//------------------------------------------Create a spinner------------------------------------------//
lv_obj_t * lv_spinner_init (lv_obj_t * scr)
{
	uint32_t spin_duration  = 1000;
	lv_obj_t * spinner; //Объект Spinner представляет собой вращающуюся дугу внутри круга.	
	spinner = lv_spinner_create(scr, spin_duration, 32);
	lv_obj_set_size(spinner, 48, 48);
	lv_obj_align(spinner,  LV_ALIGN_TOP_MID, 0, 5);
	return spinner;
}

//-----------------------------------------------------------------------------------------------------//
lv_obj_t * lv_redled_init (lv_obj_t * scr)
{
	lv_obj_t * led;
	lv_led_create(led);
  lv_obj_align(led, LV_ALIGN_TOP_LEFT, 5, 10);
	lv_obj_set_size(led, 25, 25);
	lv_led_set_brightness(led, LV_LED_BRIGHT_MAX);
  lv_led_set_color(led, lv_palette_main(LV_PALETTE_RED));
  lv_led_off(led);
	return led;
}

//-----------------------------------------------------------------------------------------------------//
lv_obj_t * lv_greenled_init (lv_obj_t * scr)
{
	lv_obj_t * led;
	lv_led_create(led);
  lv_obj_align(led, LV_ALIGN_TOP_RIGHT, -5, 10);
	lv_obj_set_size(led, 25, 25);
	lv_led_set_brightness(led, LV_LED_BRIGHT_MAX);
  lv_led_set_color(led, lv_palette_main(LV_PALETTE_GREEN));
  lv_led_on(led);
	return led;
}

//-----------------------------------------------------------------------------------------------------//
void button_style_init (lv_style_t button_style)
{	
	lv_style_init(&button_style); //Init the style for the button
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
}

//-----------------------------------------------------------------------------------------------------//
lv_obj_t * lv_button_init (lv_obj_t * scr, lv_style_t* button_style, char * btn_label)
{	
	
	lv_obj_t * main_button = lv_btn_create(scr); 	//button create
	lv_obj_set_size(main_button, 80, 30); //размерв кнопки
	lv_obj_add_event_cb(main_button, event_handler_main_btn, LV_EVENT_CLICKED, NULL); //функция, которая вызывается при нажатии на кнопку
	lv_obj_set_style_align(main_button, LV_ALIGN_BOTTOM_MID, 0); //установка стиля кнопки
  lv_obj_set_pos(main_button, 0, -5); //установка позиции кнопки
		
	lv_obj_t * label; //label button create
	label = lv_label_create(main_button); //создание объекта Надпись как дочерний объект созданной ранее кнопки
	lv_obj_add_style(label, button_style, 0); //привязка стиля к надписи на кнопке
	lv_label_set_text(label, btn_label);
	lv_obj_center(label);
	
	Status.flag_button = MAIN_SCR_ACT;
	
	return main_button;
}

//-------------------------------Функция обработки нажатия основной кнопки-------------------------------//
static void event_handler_main_btn(lv_event_t * event)
{
	lv_obj_t *  button = lv_event_get_target(event);
	lv_obj_t * label = lv_obj_get_child(button, 0);
	Status.flag_button = !Status.flag_button;
	if (Status.flag_button == MAIN_SCR_ACT)
	lv_scr_load(date_scr);
}

//-----------------------------------------------------------------------------------------------------//
lv_obj_t * lv_calendar(lv_obj_t * scr)
{
    lv_obj_t  * calendar = lv_calendar_create(scr); //lv_scr_act() - получение активного дисплея
   // lv_obj_set_size(calendar, (lv_disp_get_hor_res(NULL)-1), (lv_disp_get_ver_res(NULL)-1));
		lv_obj_set_size(calendar, (lv_disp_get_hor_res(NULL)-1), 160);
    lv_obj_align(calendar, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_add_event_cb(calendar, event_handler_date, LV_EVENT_ALL, NULL);

    lv_calendar_set_today_date(calendar, 2021, 02, 23);
    lv_calendar_set_showed_date(calendar, 2021, 02);

    //Highlight a few days
  /*  static lv_calendar_date_t highlighted_days[3]; //Only its pointer will be saved so should be static
    highlighted_days[0].year = 2021;
    highlighted_days[0].month = 02;
    highlighted_days[0].day = 6;

    highlighted_days[1].year = 2021;
    highlighted_days[1].month = 02;
    highlighted_days[1].day = 11;

    highlighted_days[2].year = 2022;
    highlighted_days[2].month = 02;
    highlighted_days[2].day = 22;

    lv_calendar_set_highlighted_dates(calendar, highlighted_days, 3);*/

    lv_calendar_header_dropdown_create(calendar); //календарь с выпадающими списками
  //  lv_calendar_header_arrow_create(calendar);
	return calendar;
}

//-----------------------------------------------------------------------------------------------------//
static void event_handler_date (lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * obj = lv_event_get_current_target(e);
	if(code == LV_EVENT_VALUE_CHANGED) 
	{
		lv_calendar_date_t date;
		if(lv_calendar_get_pressed_date(obj, &date)) 
		{ LV_LOG_USER("Clicked date: %02d.%02d.%d", date.day, date.month, date.year);	}
	}
}

//-----------------------------------------перемотка страницы-----------------------------------------//
void lv_win_date(lv_obj_t * scr)
{
    lv_obj_t * win = lv_win_create(scr, 30);
    lv_calendar(win);
    lv_obj_t * btn;

    lv_win_add_title(win, "Calendar");

/*	btn = lv_win_add_button(win, LV_SYMBOL_LEFT, 40); //добавление кнопки "влево"
    lv_obj_add_event_cb(btn, event_handler_windate, LV_EVENT_CLICKED, NULL);
		btn = lv_win_add_button(win, LV_SYMBOL_RIGHT, 40);  //добавление кнопки "вправо"
    lv_obj_add_event_cb(btn, event_handler_windate, LV_EVENT_CLICKED, NULL);*/

    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 30); //добавление кнопки "закрыть"
    lv_obj_add_event_cb(btn, event_handler_windate, LV_EVENT_CLICKED, NULL);

		/*lv_obj_t * cont = lv_win_get_content(win);  //Content can be added here
		lv_obj_t * label = lv_label_create(cont);
		lv_label_set_text(label, "This is\n"");*/

}

//----------------------------------------------------------------------------------//
static void event_handler_windate(lv_event_t * e)
{
  lv_obj_t * btn = lv_event_get_target(e);
  lv_obj_t * label = lv_obj_get_child(btn, 0);
	lv_scr_load(default_scr);
}

