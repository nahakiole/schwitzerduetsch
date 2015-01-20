#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_hour_layer;
static TextLayer *s_min_layer;

static char hours[13][8] = {"zwöufi", "eis", "zwöi", "drü", "vieri", "füfi", "sächsi", "sibni", "achti", "nüni", "zäni", "eufi"};

static char minutes[13][20] = {"öpä","füf ab","zäh ab","viertu ab", "zwänzg ab", "füfäzwängsab", "haubi", "füf ab haubi", "zwänzg vor", "viertu vor", "zäh vor", "füf vor", "öpä"};


static int get_aprox_minute(int minute) {
    return ((minute+2)/5);
}

static char* get_minute_string(int minute) {
    return minutes[minute]; 
}

static void update_time() {
    time_t temp = time(NULL); 
    struct tm *tick_time = localtime(&temp);
    int hour = tick_time->tm_hour;
    int minute = tick_time->tm_min;
    int second = tick_time->tm_sec;
	
    // If over half of the minute has passed we round up the minute
    if (second > 30){
        minute++;
    }
    minute = get_aprox_minute(minute);

    // From index seven on we use the next hour to show the time eg. "viertuvor achti"
    if (minute >= 6){
        hour++;
    }

    text_layer_set_text(s_min_layer, get_minute_string(minute));
    text_layer_set_text(s_hour_layer, hours[hour%12]);
}

static void main_window_load(Window *window) {
    s_hour_layer = text_layer_create(GRect(0, 80, 144, 50));

    s_min_layer = text_layer_create(GRect(0, 40, 144, 50));

    text_layer_set_background_color(s_min_layer, GColorClear);
    text_layer_set_text_color(s_min_layer, GColorWhite);

    text_layer_set_background_color(s_hour_layer, GColorClear);
    text_layer_set_text_color(s_hour_layer, GColorWhite);

    text_layer_set_text(s_hour_layer, "");

    text_layer_set_text(s_min_layer, "");


    window_set_background_color(window, GColorBlack);

    text_layer_set_font(s_hour_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_hour_layer, GTextAlignmentCenter);

    text_layer_set_font(s_min_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
    text_layer_set_text_alignment(s_min_layer, GTextAlignmentCenter);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_hour_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_min_layer));
    update_time();
}

static void main_window_unload(Window *window) {
    text_layer_destroy(s_hour_layer);
    text_layer_destroy(s_min_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void init() {
    s_main_window = window_create();

    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    window_stack_push(s_main_window, true);

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
