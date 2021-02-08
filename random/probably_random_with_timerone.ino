#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <TimerOne.h>

byte sample = 0;
boolean sample_waiting = false;
byte current_bit = 0;
byte result = 0;

void setup() {
  Serial.begin(115200);
  wdtSetup();
  Timer1.initialize(30000); // set a timer of length somewhat longer than watchdog length
}

void loop()
{
  if (sample_waiting) {
    sample_waiting = false;

    result = rotl(result, 1); // Spread randomness around
    result ^= sample; // XOR preserves randomness

    current_bit++;
    if (current_bit > 7)
    {
      current_bit = 0;
      Serial.write(result); // raw binary
      //Serial.println(result, DEC); // decimal text
      //binprint(result);  // bits
    }
  }
}

// Rotate bits to the left
// https://en.wikipedia.org/wiki/Circular_shift#Implementing_circular_shifts
byte rotl(const byte value, int shift) {
  if ((shift &= sizeof(value)*8 - 1) == 0)
    return value;
  return (value << shift) | (value >> (sizeof(value)*8 - shift));
}

// Setup of the watchdog timer.
void wdtSetup() {
  cli();
  MCUSR = 0;
  
  /* Start timed sequence */
  WDTCSR |= _BV(WDCE) | _BV(WDE);

  /* Put WDT into interrupt mode */
  /* Set shortest prescaler(time-out) value = 2048 cycles (~16 ms) */
  WDTCSR = _BV(WDIE);

  sei();
}

// Watchdog Timer Interrupt
ISR(WDT_vect)
{
  sample = TCNT1L; // Ignore higher bits
  TCNT1 = 0; // Clear Timer 1
  sample_waiting = true;
}

// Print binary numbers
// http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1193507343/5#5
void binprint(int input) {
  for (unsigned int mask = 0x80; mask; mask >>= 1) {
    Serial.print(mask&input?'1':'0');
  }
  Serial.println();
}