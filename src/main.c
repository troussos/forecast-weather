#include <pebble.h>

static Window *window;
static BitmapLayer *icon_layer;
static TextLayer *temp_layer;
static GBitmap *icon_bitmap = NULL;

static const uint32_t WEATHER_ICONS[] = {
    RESOURCE_ID_IMAGE_CLOUD //0
};

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);

    icon_layer = bitmap_layer_create(GRect(22, 16, 100, 74));
	icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[0]);
    bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));

    temp_layer = text_layer_create(GRect(0, 95, 144, 68));
    text_layer_set_text_color(temp_layer, GColorWhite);
    text_layer_set_background_color(temp_layer, GColorClear);
    text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
	text_layer_set_text(temp_layer, "-25\u00B0F");
    layer_add_child(window_layer, text_layer_get_layer(temp_layer));
}
//FONT_KEY_ROBOTO_CONDENSED_21 - Good for "In the next hour"

static void window_unload(Window *window) {
    if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
    }

    text_layer_destroy(temp_layer);
    bitmap_layer_destroy(icon_layer);
}

static void init(void) {
    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_set_fullscreen(window, true);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });	
	
    const int inbound_size = 64;
    const int outbound_size = 64;
    app_message_open(inbound_size, outbound_size);

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
