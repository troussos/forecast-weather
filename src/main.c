#include <pebble.h>

static Window *window;
static BitmapLayer *icon_layer;
static TextLayer *temp_layer;
static TextLayer *city_layer;
static GBitmap *icon_bitmap = NULL;

//Resource Array for holding the different icons to display based on the condition
static const uint32_t WEATHER_ICONS[] = {
	RESOURCE_ID_IMAGE_THERMO, //0
	RESOURCE_ID_IMAGE_SUN, //1
	RESOURCE_ID_IMAGE_MOON, //2
	RESOURCE_ID_IMAGE_CLOUD, //3
	RESOURCE_ID_IMAGE_CLOUD_SUN, //4
	RESOURCE_ID_IMAGE_CLOUD_MOON, //5
	RESOURCE_ID_IMAGE_WIND, //6
	RESOURCE_ID_IMAGE_SNOW, //7
	RESOURCE_ID_IMAGE_FOG, //8
	RESOURCE_ID_IMAGE_SLEET, //9
	RESOURCE_ID_IMAGE_RAIN //10
};

//Maping of icon to Forecast.io condition name, Indexes are used to do the matching
const char *icons[] = {
	"default",
	"clear-day",
	"clear-night",
	"cloudy",
	"partly-cloudy-day",
	"partly-cloudy-night",
	"wind",
	"snow",
	"fog",
	"sleet",
	"rain"
};

//Mapping of the numeric index to the expected data for the Dictionary response
enum {
	KEY_ICON,
	KEY_TEMP,
	KEY_CITY
};

static void refreshHome()
{
	text_layer_set_text(temp_layer, "?\u00B0F");
	icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[0]);
    bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
	text_layer_set_text(city_layer, "N/A");
	
	DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    Tuplet value = TupletInteger(1, 42);
    dict_write_tuplet(iter, &value);
	app_message_outbox_send();
}
/**
 * Do the inital window setup creating and setting the various layers
 */
static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);

	//Setup the icon layer at the top and give a default icon of a thermometer
    icon_layer = bitmap_layer_create(GRect(22, 5, 100, 80));
	icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[0]);
    bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));

	//Setup the tempatures layer and default its value to ?F
    temp_layer = text_layer_create(GRect(0, 87, 144, 68));
    text_layer_set_text_color(temp_layer, GColorWhite);
    text_layer_set_background_color(temp_layer, GColorClear);
    text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
    text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
	text_layer_set_text(temp_layer, "?\u00B0F");
    layer_add_child(window_layer, text_layer_get_layer(temp_layer));
	
	//Add a city layer with a default value of N/A
	city_layer = text_layer_create(GRect(0, 135, 144, 40));
	text_layer_set_text_color(city_layer, GColorWhite);
    text_layer_set_background_color(city_layer, GColorClear);
    text_layer_set_font(city_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
    text_layer_set_text_alignment(city_layer, GTextAlignmentCenter);
	text_layer_set_text(city_layer, "N/A");
    layer_add_child(window_layer, text_layer_get_layer(city_layer));
}

/**
 * This method will update the icon in the icon layer based on the icon name passed in.
 */
static void updateIcon(char * icon_name)
{
	int i;
	for (i=0; i<11; i++)
	{
		if (strcmp(icon_name, icons[i]) == 0)
		{
			icon_bitmap = gbitmap_create_with_resource(WEATHER_ICONS[i]);
		    bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
			break;
		}
	}
}

/**
 * Handler for recieved messages from the Javascript backend.
 */
static void in_received_handler(DictionaryIterator *iter, void *context) {

	//Try to pull out the various parts of the expected message
	Tuple *icon_tuple = dict_find(iter, KEY_ICON);
	Tuple *temp_tuple = dict_find(iter, KEY_TEMP);
	Tuple *city_tuple = dict_find(iter, KEY_CITY);
	
	//Perform UI updates if new data has been returned
	if(icon_tuple)
	{
		updateIcon(icon_tuple->value->cstring);
	}
	
	if(temp_tuple)
	{
		text_layer_set_text(temp_layer, temp_tuple->value->cstring);
	}
	
	if(city_tuple)
	{
		text_layer_set_text(city_layer, city_tuple->value->cstring);
	}
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    refreshHome();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    refreshHome();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    refreshHome();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/**
 * Break down and unload the window
 */
static void window_unload(Window *window) {
    if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
    }

    text_layer_destroy(temp_layer);
    text_layer_destroy(city_layer);
    bitmap_layer_destroy(icon_layer);
}

/**
 * Basic inistalization, setting handles and callbacks
 */
static void init(void) {
    window = window_create();
    window_set_background_color(window, GColorBlack);
    window_set_fullscreen(window, true);
	window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });	
	
	app_message_register_inbox_received(in_received_handler);
	app_message_register_outbox_sent(out_sent_handler);
	
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
