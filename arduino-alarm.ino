/*
 * Copyright (C) 2014 Mario Sanchez Prada
 * Authors: Mario Sanchez Prada <mario@mariospr.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 3 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>
 *
 */
#define TOGGLE_SWITCH_PIN 13
#define LIGHT_SENSOR_PIN A0

#define STATUS_LED_PIN 12
#define RED_LED_PIN 7
#define BLUE_LED_PIN 4
#define SPEAKER_PIN 2

#define DELAY_DURATION_MS 180

#define LIGHT_SENSOR_TRESHOLD_HIGH 300
#define LIGHT_SENSOR_TRESHOLD_LOW 200

static int switch_value = LOW;
static int status_led_value = LOW;
static int red_led_value = LOW;
static int blue_led_value = LOW;
static int light_sensor_value = 0;
static bool alarm_enabled = false;

typedef enum {
  ALARM_STATE_OFF = LOW,
  ALARM_STATE_ON = HIGH
} AlarmState;

static AlarmState current_state = ALARM_STATE_OFF;

typedef enum {
  PITCH_VALUE_NONE = 0,
  PITCH_VALUE_LOW = 1000,
  PITCH_VALUE_HIGH = 3000
} PitchValue;

static PitchValue current_pitch_value = PITCH_VALUE_NONE;

void setup() {
  pinMode(TOGGLE_SWITCH_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN, INPUT);

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
}

void read_input_values() {
  int previous_switch_value = switch_value;
  switch_value = digitalRead(TOGGLE_SWITCH_PIN);

  if (switch_value == HIGH && previous_switch_value != switch_value) {
    current_state = (current_state == ALARM_STATE_OFF)
      ? ALARM_STATE_ON
      : ALARM_STATE_OFF;
  }

  light_sensor_value = analogRead(LIGHT_SENSOR_PIN);
  if (!alarm_enabled && light_sensor_value > LIGHT_SENSOR_TRESHOLD_HIGH)
    alarm_enabled = true;
  else if (alarm_enabled && light_sensor_value < LIGHT_SENSOR_TRESHOLD_LOW)
    alarm_enabled = false;
}

void update_led_values() {
  status_led_value = current_state == ALARM_STATE_OFF ? HIGH : LOW;
  if (current_state == ALARM_STATE_OFF || !alarm_enabled) {
    red_led_value = LOW;
    blue_led_value = LOW;
  } else if (alarm_enabled) {
    if (red_led_value == blue_led_value) {
      // First time we light these leds up after switching on.
      red_led_value = HIGH;
      blue_led_value = LOW;
    } else {
      red_led_value = !red_led_value;
      blue_led_value = !blue_led_value;
    }
  }
}

void update_speaker_pitch_value() {
  if (current_state == ALARM_STATE_ON && alarm_enabled) {
    current_pitch_value = (current_pitch_value == PITCH_VALUE_HIGH)
      ? PITCH_VALUE_LOW
      : PITCH_VALUE_HIGH;
  } else {
    current_pitch_value = PITCH_VALUE_NONE;
  }
}

void render_result() {
  digitalWrite(STATUS_LED_PIN, status_led_value);
  digitalWrite(RED_LED_PIN, red_led_value);
  digitalWrite(BLUE_LED_PIN, blue_led_value);

  if (current_pitch_value != PITCH_VALUE_NONE)
    tone(SPEAKER_PIN, current_pitch_value, DELAY_DURATION_MS);
}

void loop() {
  read_input_values();
  update_led_values();
  update_speaker_pitch_value();
  render_result();

  delay(DELAY_DURATION_MS);
}
