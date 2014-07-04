#include <pebble.h>

static Window *window;
static TextLayer *top_day_layer, *bottom_day_layer;
static Layer *main_layer;

//Days of week
static const char* const WEEKDAYS[] = {
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

//Days of the month
static const char* const DAYS[] = {
	"",
	"the first",
	"the second",
	"the third",
	"the fourth",
	"the fifth",
	"the sixth",
	"the seventh",
	"the eighth",
	"the ninth",
	"the tenth",
	"the eleventh",
	"the twelfth",
	"the thirteenth",
	"the fourteenth",
	"the fifteenth",
	"the sixteenth",
	"the seventeenth",
	"the eighteenth",
	"the nineteenth",
	"the twentieth",
	"the twenty-first",
	"the twenty-second",
	"the twenty-third",
	"the twenty-fourth",
	"the twenty-fifth",
	"the twenty-sixth",
	"the twenty-seventh",
	"the twenty-eighth",
	"the twenty-ninth",
	"the thirtieth",
	"the thirty-first"
};

//Define things------------------------------------------------------
// The cell column offsets for each digit
#define HourOffset 0
#define MinuteOneOffset 0
#define MinuteTwoOffset 1

// The maximum number of cell columns to display
// (Used so that if a binary digit can never be 1 then no un-filled
// placeholder is shown.)

#define DefaultMaxRows 4
#define MaxRowsMinuteOne 3

//start program
/*----------------------------------------------------------------------------------*/

//Function to draw cells
//input x,y,w,h,filled
void draw_cell(GContext* ctx, int x, int y, int w, int h, bool filled) {
  // Each "cell" represents a binary digit or 0 or 1.

  	graphics_context_set_fill_color(ctx, GColorWhite);  //fill blocks with white

	graphics_fill_rect(ctx, GRect(x,y,w,h), 0, GCornerNone); //draw square white

  if (!filled) { //if not filled color it black
    graphics_context_set_fill_color(ctx, GColorBlack);  

	graphics_fill_rect(ctx, GRect(x,y,w,h), 0, GCornerNone); //draw square black
  }

}




void update_proc(Layer *layer, GContext *ctx)
{
	time_t now = time(NULL);
	struct tm* mTime = localtime(&now);
	
	unsigned short min = mTime-> tm_min;  //minutes
	unsigned short hour = mTime -> tm_hour; //hour
	unsigned short top_day = mTime -> tm_wday; //weekday
	unsigned short bottom_day = mTime -> tm_mday; //day of month
	unsigned short display_hour;
		
//figure which cells to draw for minutes
/*Tens Digit Minute--------------------------------------------------------------*/	
	for (int cell_row_index = 0; cell_row_index < MaxRowsMinuteOne; cell_row_index++) 
		{
			draw_cell(ctx, 75, 9 + (3 - cell_row_index)*36, 18, 18, ((min / 10) >> cell_row_index) & 0x1); 
		}
		
//figure which cells to draw for minutes
/*Ones Digit */	
	for (int cell_row_index = 0; cell_row_index < DefaultMaxRows; cell_row_index++) 
		{
			draw_cell(ctx, 110, 9 + (3 - cell_row_index)*36, 18, 18, ((min % 10) >> cell_row_index) & 0x1); 
		}
		
//Fix 24 hr vs 12 hour clock thing
	if (clock_is_24h_style()) 
	{
		display_hour = hour;
	}

  display_hour = hour % 12;

//if it is twelve, make the hour twelve, not 0
  display_hour = (display_hour == 0 ? 12 : display_hour);
  
	
//draw_cell_column_for_hour_digit(ctx, display_hour, DefaultMaxRows, HourOffset);
	for (int cell_row_index = 0; cell_row_index < DefaultMaxRows; cell_row_index++) 
		{
			draw_cell(ctx, 21, 12 + (3 - cell_row_index)*36, 30, 30, (display_hour >> cell_row_index) & 0x1); 
		}

/*brute force
  //Write day of week
	text_layer_set_text(top_day_layer, WEEKDAYS[t->tm_wday]);
	text_layer_set_text(bottom_day_layer, DAYS[t->tm_mday]);
*/

if(strcmp(text_layer_get_text(top_day_layer), WEEKDAYS[top_day]) != 0)
	{
		text_layer_set_text(top_day_layer, WEEKDAYS[top_day]);
		text_layer_set_text(bottom_day_layer, DAYS[bottom_day]);
	}


}


void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
	layer_mark_dirty(main_layer);
}


static void window_load(Window *window) 
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

	main_layer = layer_create((GRect){.origin = {0, 0}, .size = {bounds.size.w, bounds.size.h}});

//initialize top day layer (weekday)
	top_day_layer = text_layer_create(GRect(57, 136, 72, 15));
	text_layer_set_font(top_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
	text_layer_set_text_color(top_day_layer, GColorWhite);
	text_layer_set_background_color(top_day_layer, GColorClear);
	text_layer_set_text_alignment(top_day_layer, GTextAlignmentLeft);
	text_layer_set_text(top_day_layer, "");

//initialize bottom day layer (month day)
	bottom_day_layer = text_layer_create(GRect(0, 152, 144, 16));
	text_layer_set_font(bottom_day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_color(bottom_day_layer, GColorWhite);
	text_layer_set_background_color(bottom_day_layer, GColorClear);
	text_layer_set_text_alignment(bottom_day_layer, GTextAlignmentCenter);
	text_layer_set_text(bottom_day_layer, "");

	layer_set_update_proc(main_layer, update_proc);
	
//add children
	layer_add_child(window_layer, main_layer);
	layer_add_child(window_layer, text_layer_get_layer(top_day_layer));
	layer_add_child(window_layer, text_layer_get_layer(bottom_day_layer));
	
//tick timer = ok
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

}


static void window_unload(Window *window)
{

	text_layer_destroy(top_day_layer);
	text_layer_destroy(bottom_day_layer);
	layer_destroy(main_layer);
	tick_timer_service_unsubscribe();

}



static void init(void) {
  window = window_create();
  //window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  window_set_background_color(window, GColorBlack);
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}


int main(void) {
  init();

  app_event_loop();
  deinit();
}
