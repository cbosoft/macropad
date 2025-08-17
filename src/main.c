#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "hardware/gpio.h"
#include "tusb.h"

#include "usb_descriptors.h"

static uint BUTTONS_GPIOS[10] = { 19, 18, 17, 22, 21, 20, 26, 27, 28, 16 };
static uint GPIOS_BUTTONS[32] = { 0 };
static volatile bool CHANGED = false;
static volatile uint CURRENT_LAYER = 0;

#define EVENT_LEVEL_LOW  0x1
#define EVENT_LEVEL_HIGH 0x2
#define EVENT_EDGE_FALL  0x4
#define EVENT_EDGE_RISE  0x8

// {{{ Keymap

typedef struct {
  uint8_t keycode;
  uint8_t modifier;
} Key;

#define KEY(NAME) { .keycode = HID_KEY_##NAME, .modifier = 0 }
#define MOD(NAME) { .keycode = 0, .modifier = KEYBOARD_MODIFIER_##NAME }

// https://github.com/hathach/tinyusb/blob/8a78804ff91271fb32008288777fad9fe73f5735/src/class/hid/hid.h
const Key BUTTONS_KEYS[2][9] = {
  { KEY(0), KEY(1), KEY(2), KEY(3), KEY(4), KEY(5), KEY(6), KEY(7), KEY(8), },
  { KEY(ARROW_LEFT), KEY(ARROW_DOWN), KEY(ARROW_RIGHT), KEY(PAGE_DOWN), KEY(ARROW_UP), KEY(PAGE_UP), { .keycode = HID_KEY_TAB, .modifier = KEYBOARD_MODIFIER_LEFTALT }, KEY(7), KEY(8), },
};

static volatile Key keys_in_play[10] = { 0 };

// }}}
// {{{ Function Definitions

void btn_callback(uint pin, uint32_t events);
void update_keystate();

// }}}
// {{{ Main

int main(void) {
  board_init();

  for (int i = 0; i < 10; i++) {
    const uint pin = BUTTONS_GPIOS[i];
    GPIOS_BUTTONS[pin] = i;
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);

    gpio_set_irq_enabled_with_callback(pin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &btn_callback);
  }


  tud_init(BOARD_TUD_RHPORT);

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  while (1) {
    tud_task();
    if (CHANGED) update_keystate();
  }
}

// }}}
// {{{ Key State

void btn_callback(uint pin, uint32_t events) {
  (void) events;
  CHANGED = true;
  const uint i = GPIOS_BUTTONS[pin];

  if (i == 9) {
    if (events & EVENT_EDGE_FALL) {
      // fall -> pressed
      CURRENT_LAYER = 1;
    }
    else {
      // rise -> depressed
      CURRENT_LAYER = 0;
    }
  }
  else {
    if (events & EVENT_EDGE_FALL) {
      // fall -> pressed
      keys_in_play[i] = BUTTONS_KEYS[CURRENT_LAYER][i];
    }
    else {
      // rise -> depressed
      keys_in_play[i].keycode = 0;
      keys_in_play[i].modifier = 0;
    }
  }
}


void layer_select_callback(uint pin, uint32_t events) {
}

void update_keystate() {
  if (!tud_hid_ready()) return;
  
  static uint32_t start_ms = 0;
  static uint32_t interval_ms = 5;

  if (board_millis() - start_ms < interval_ms) return;
  start_ms += interval_ms;

  if (tud_suspended()) {
    tud_remote_wakeup();
  }

  uint8_t j = 0;
  uint8_t keycode[6] = { 0 };
  uint8_t modifier = 0;
  static bool has_key = false;

  for (uint i = 0; (i < 10) && (j < 6); i++) {
    const volatile Key *key = &keys_in_play[i];

    if (key->keycode) {
      keycode[j++] = key->keycode;
    }
    
    if (key->modifier) {
      modifier = modifier | key->modifier;
    }
  }

  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycode);
  CHANGED = false;
}

// }}}
// {{{ USB HID


// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;

  switch (report[0]) {
    case REPORT_ID_KEYBOARD:
      // just sent kb report
      // send mouse report?
      // TODO
      break;

    default:
      break;
  }

}


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  return 0;
}


// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
}

// }}}
