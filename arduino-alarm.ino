#define TOGGLE_SWITCH_PIN 13
#define LIGHT_SENSOR_PIN A0

#define STATUS_LED_PIN 12
#define RED_LED_PIN 7
#define BLUE_LED_PIN 4
#define SPEAKER_PIN 2

#define DELAY_DURATION_MS 180

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

static int switch_value = LOW;
static int status_led_value = LOW;
static int red_led_value = LOW;
static int blue_led_value = LOW;
static int previous_switch_value = LOW;

void setup() {
  pinMode(TOGGLE_SWITCH_PIN, INPUT);
  /* pinMode(LIGHT_SENSOR_PIN, INPUT); */

  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(SPEAKER_PIN, OUTPUT);
}

void toggle_state_if_needed() {
  if (switch_value != HIGH || previous_switch_value == switch_value)
    return;

  current_state = (current_state == ALARM_STATE_OFF)
    ? ALARM_STATE_ON
    : ALARM_STATE_OFF;
}

void update_led_values() {
  if (current_state == ALARM_STATE_OFF) {
    status_led_value = HIGH;
    red_led_value = LOW;
    blue_led_value = LOW;
  } else {
    status_led_value = LOW;
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
  if (current_state == ALARM_STATE_OFF)
    current_pitch_value = PITCH_VALUE_NONE;
  else {
    current_pitch_value = (current_pitch_value == PITCH_VALUE_HIGH)
      ? PITCH_VALUE_LOW
      : PITCH_VALUE_HIGH;
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
  previous_switch_value = switch_value;
  switch_value = digitalRead(TOGGLE_SWITCH_PIN);

  toggle_state_if_needed();
  update_led_values();
  update_speaker_pitch_value();
  render_result();

  delay(DELAY_DURATION_MS);
}
