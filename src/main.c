/*
 * main.c
 * Creates a Window, Layer and animates it around the screen 
 * from corner to corner. It uses the `.stopped` callback to schedule the 
 * next stage of the animation.
 */

#include <pebble.h>

#define FISH_W 28
#define FISH_H 16

#define ANIM_DURATION_MAX 2000
#define ANIM_DURATION_MIN 1000
#define ANIM_DELAY_MIN 2000
#define ANIM_DELAY_MAX 5000

static Window *s_main_window;
static BitmapLayer *s_box_layer;
static BitmapLayer *s_box_layer2;
static BitmapLayer *s_box_layer3;
static BitmapLayer *s_background_layer;
static GBitmap *right_fish;
static GBitmap *left_fish;
static PropertyAnimation *s_box_animation;
static Layer *s_time_layer;
static GFont s_time_font;
static GBitmap *s_background_bitmap;
static GBitmap *s_fish_bitmap;
static GBitmap *s_fish_bitmap2;
static GBitmap *s_fish_bitmap3;
char curTime[64];

/*
 *  TIME STUFF
 */

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  strcpy(curTime, "00:00");

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(curTime, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(curTime, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  layer_mark_dirty(s_time_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

/*
 *  ANIMATION STUFF
 */

// Function prototype 
static void next_animation();

static void anim_stopped_handler(Animation *animation, bool finished, void *context) {
#ifdef PBL_PLATFORM_APLITE
  // Free the animation
  property_animation_destroy(s_box_animation);
#endif

  // Schedule the next one, unless the app is exiting
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "animation stopped, finished: %d", finished); // why do some fish stop moving?
  if (finished) {
    next_animation(context); // figure out how to get who just finished
  }
}

static void anim_started_handler(Animation *animation, void *context) {
  BitmapLayer *layer = context;
  GPoint from = GPoint(0,0);
  GPoint to = GPoint(0,0);
  property_animation_get_from_gpoint((PropertyAnimation *) animation, &from);
  property_animation_get_to_gpoint((PropertyAnimation *) animation, &to);
  if (from.x - to.x < 0) {
    // fish is going right
    bitmap_layer_set_bitmap(layer, right_fish);
  }
  else {
    // fish is going left
    bitmap_layer_set_bitmap(layer, left_fish);
  }
}

static void next_animation(Layer *layer) {
  // Determine start and finish positions
  GRect start, finish;
  
  GPoint current = layer_get_frame(layer).origin;
  start = GRect(current.x, current.y, FISH_W, FISH_H);
  
  GPoint next = GPoint(rand() % (144 - FISH_W), rand() % (168 - FISH_H));
  finish = GRect(next.x, next.y, FISH_W, FISH_H);

  // flip the fish around
  
  // Schedule the next animation
  s_box_animation = property_animation_create_layer_frame(layer, &start, &finish);
  animation_set_duration((Animation*)s_box_animation, rand() %ANIM_DURATION_MAX + ANIM_DURATION_MIN);
  animation_set_delay((Animation*)s_box_animation, rand() % ANIM_DELAY_MAX + ANIM_DELAY_MIN);
  animation_set_curve((Animation*)s_box_animation, AnimationCurveEaseOut);
  
  animation_set_handlers((Animation*)s_box_animation, (AnimationHandlers) {
    .started = anim_started_handler,
    .stopped = anim_stopped_handler
  }, layer);
  animation_schedule((Animation*)s_box_animation);

}

/*
static void update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorClear);
  //graphics_fill_rect(ctx, layer_get_bounds(layer), 4, GCornersAll);
}
*/

void draw_time_text(Layer *this_layer, GContext *ctx){
    graphics_context_set_text_color(ctx, GColorBlack);
    
    graphics_draw_text(ctx, curTime, s_time_font, GRect(1,21, 144,32), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, curTime, s_time_font, GRect(3,21, 144,32), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, curTime, s_time_font, GRect(1,23, 144,32), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    graphics_draw_text(ctx, curTime, s_time_font, GRect(3,23, 144,32), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    graphics_context_set_text_color(ctx, GColorWhite);
 
    graphics_draw_text(ctx, curTime, s_time_font, GRect(2,22, 144,32), GTextOverflowModeFill, GTextAlignmentCenter, NULL);

}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  // Create Fish Layers
  s_box_layer = bitmap_layer_create(GRect(0, 0, FISH_W, FISH_H));
  s_box_layer2 = bitmap_layer_create(GRect(50, 50, FISH_W, FISH_H));
  s_box_layer3 = bitmap_layer_create(GRect(100, 100, FISH_W, FISH_H));
  //layer_set_update_proc(bitmap_layer_get_layer(s_box_layer), update_proc);
  //layer_set_update_proc(bitmap_layer_get_layer(s_box_layer2), update_proc);
  //layer_set_update_proc(bitmap_layer_get_layer(s_box_layer3), update_proc);
  
  // Create fish bitmaps
  s_fish_bitmap = right_fish;
  s_fish_bitmap2 = right_fish;
  s_fish_bitmap3 = right_fish;
  bitmap_layer_set_compositing_mode(s_box_layer, GCompOpSet);
  bitmap_layer_set_compositing_mode(s_box_layer2, GCompOpSet);
  bitmap_layer_set_compositing_mode(s_box_layer3, GCompOpSet);
  bitmap_layer_set_bitmap(s_box_layer, s_fish_bitmap);
  bitmap_layer_set_bitmap(s_box_layer2, s_fish_bitmap2);
  bitmap_layer_set_bitmap(s_box_layer3, s_fish_bitmap3);
  
  // set time font FONT_MONTSERRAT_40
  //s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_HANGDJ_40));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_MONTSERRAT_40));
  
  // Create time TextLayer
  s_time_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_time_layer, draw_time_text);
  
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND_IMAGE_AQUARIUM);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // Improve the layout to be more like a watchface
  // Create GFont


  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, s_time_layer);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_box_layer));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_box_layer2));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_box_layer3));
}

static void main_window_unload(Window *window) {
  // Destroy Layer
  bitmap_layer_destroy(s_box_layer);
  bitmap_layer_destroy(s_box_layer2);
  bitmap_layer_destroy(s_box_layer3);
  // Destroy TextLayer
  layer_destroy(s_time_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
}

static void init(void) {
  // Create main Window
  s_main_window = window_create();
  
  #ifdef PBL_COLOR
    window_set_background_color(s_main_window, GColorVividCerulean);
  #else 
    window_set_background_color(s_main_window, GColorWhite);
  #endif
    
  // create fish
  left_fish = gbitmap_create_with_resource(RESOURCE_ID_ORANGE_FISH_LEFT);
  right_fish = gbitmap_create_with_resource(RESOURCE_ID_ORANGE_FISH_RIGHT);
    
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
#ifdef PBL_SDK_2
  window_set_fullscreen(s_main_window, true);
#endif
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Start animation loop
  next_animation(bitmap_layer_get_layer(s_box_layer));
  next_animation(bitmap_layer_get_layer(s_box_layer2));
  next_animation(bitmap_layer_get_layer(s_box_layer3));
}

static void deinit(void) {
  // Stop any animation in progress
  animation_unschedule_all();
  gbitmap_destroy(right_fish);
  gbitmap_destroy(left_fish);
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_fish_bitmap);
  gbitmap_destroy(s_fish_bitmap2);
  gbitmap_destroy(s_fish_bitmap3);
  
  // Destroy main Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}