// Rotary Encoder
// see https://playground.arduino.cc/Main/RotaryEncoders

#define encoder0PinA ROTARY_A_PIN
#define encoder0PinB ROTARY_B_PIN

volatile int encoder0Pos = 0;
volatile unsigned long encodeTime;
int oldPosition = 0;
uint8_t wait_time = 102;

void doEncoderA()
{
  if (millis() > encodeTime + wait_time)
  {
    // look for a low-to-high on channel A
    if (digitalRead(encoder0PinA) == HIGH)
    {

      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == LOW)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }

    else // must be a high-to-low edge on channel A
    {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinB) == HIGH)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }
    encodeTime = millis();
    // Serial.println(encoder0Pos, DEC);
    // use for debugging - remember to comment out
  }
}

void doEncoderB()
{
  if (millis() > encodeTime + wait_time)
  {
    // look for a low-to-high on channel B
    if (digitalRead(encoder0PinB) == HIGH)
    {

      // check channel A to see which way encoder is turning
      if (digitalRead(encoder0PinA) == HIGH)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }

    // Look for a high-to-low on channel B
    else
    {
      // check channel B to see which way encoder is turning
      if (digitalRead(encoder0PinA) == LOW)
      {
        encoder0Pos += 1; // CW
      }
      else
      {
        encoder0Pos += -1; // CCW
      }
    }
    encodeTime = millis();
  }
}

void set_encoder()
{
  if (settings_get_encoder_type())
  {
    attachInterrupt(ROTARY_B_PIN, doEncoderA, CHANGE);
    attachInterrupt(ROTARY_A_PIN, doEncoderB, CHANGE);
  }
  else
  {
    attachInterrupt(ROTARY_A_PIN, doEncoderA, CHANGE);
    attachInterrupt(ROTARY_B_PIN, doEncoderB, CHANGE);
  }
}

bool encoder_is_adjusting()
{
  bool adjusting = encoder0Pos != oldPosition;
  if (adjusting)
  {
    oldPosition = encoder0Pos;
  }
  return adjusting;
}

void resetEncoder()
{
  oldPosition = encoder0Pos = 0;
}

int getEncoderValue()
{
  return (encoder0Pos > 0) ? 1 : -1;
}

void rotary_encoder_begin()
{
  resetEncoder();
  set_encoder(); // sets msb,lsb for two types of encoder
}

/* This section is for the rotary encoder w. LEDs */
typedef enum _rotary_leds_color
{
  ROTARY_LEDS_OFF,
  ROTARY_LEDS_RED,
  ROTARY_LEDS_GREEN,
  ROTARY_LEDS_ORANGE,
} rotary_leds_color_enum;

void rotary_leds_set_color(rotary_leds_color_enum color)
{
  switch (color)
  {
  case ROTARY_LEDS_OFF:
    led_write(LED_ROTARY_RED, LOW);
    led_write(LED_ROTARY_GREEN, LOW);
    break;
  case ROTARY_LEDS_RED:
    led_write(LED_ROTARY_RED, HIGH);
    led_write(LED_ROTARY_GREEN, LOW);
    break;
  case ROTARY_LEDS_GREEN:
    led_write(LED_ROTARY_RED, LOW);
    led_write(LED_ROTARY_GREEN, HIGH);
    break;
  case ROTARY_LEDS_ORANGE:
    led_write(LED_ROTARY_RED, HIGH);
    led_write(LED_ROTARY_GREEN, HIGH);
    break;
  }
}

typedef enum _rotary_leds_recording_state
{
  ROTARY_LEDS_NORMAL,
  ROTARY_LEDS_WAITING_TO_RECORD,
  ROTARY_LEDS_RECORDING,
} rotary_leds_recording_state_enum;

rotary_leds_recording_state_enum rotary_leds_recording_state = ROTARY_LEDS_NORMAL;

void check_rotary_leds()
{
  static int recording_ms, recording_led_state;
  switch (rotary_leds_recording_state)
  {
  case ROTARY_LEDS_NORMAL:
    rotary_leds_set_color(selected_fxn->get_param_active() ? ROTARY_LEDS_GREEN : ROTARY_LEDS_OFF);
    break;
  case ROTARY_LEDS_WAITING_TO_RECORD:
    if (millis() > recording_ms + 300)
    {
      recording_ms = millis();
      if (recording_led_state)
      {
        recording_led_state = 0;
      }
      else
      {
        recording_led_state = 1;
      }
    }
    rotary_leds_set_color(recording_led_state ? ROTARY_LEDS_RED : ROTARY_LEDS_OFF);
    break;
  case ROTARY_LEDS_RECORDING:
    rotary_leds_set_color(ROTARY_LEDS_RED);
    break;
  }
}
