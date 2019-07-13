// Test code for my mega hub75 (take 2) board.

// These two defines are for the RGB Matrix
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define MATRIX_HEIGHT 32
#define MATRIX_WIDTH  64

// Pin defines for the 32x32 RGB matrix.
#define CLK 11  
#define LAT 10
#define OE  9
#define A   A8
#define B   A9
#define C   A10
#define D   A11

/* Other Pin Mappings...hidden in the RGB library:
 *  Sig   Uno  Mega
 *  R0    2    24
 *  G0    3    25
 *  B0    4    26
 *  R1    5    27
 *  G1    6    28
 *  B1    7    29
 */

// Note "false" for double-buffering to consume less memory, or "true" for double-buffered.
// Double-buffered makes updates look smoother.
RGBmatrixPanel matrix(A, B, C,  D,  CLK, LAT, OE, true, MATRIX_WIDTH);

// Potentiometer read pin
#define POT_PIN A4

// Audio inputs from sound board
#define AUDIO_PIN    A0
#define ENVELOPE_PIN A1
#define GATE_PIN     A2


//  Audio samples from the ADC are "centered" around 2.5v, which maps to 512 on the ADC.
#define SAMPLE_BIAS 512

#define SAMPLE_SIZE MATRIX_WIDTH
int sample[SAMPLE_SIZE];

int gain=8;

void collect_samples( void )
{
  int i;
  for (i = 0; i < SAMPLE_SIZE; i++)
  {
    sample[i] = analogRead(AUDIO_PIN);
  }
}

// Mapped sample should give a number between 0 and 31
int map_sample( int input )
{
  int mapped_sample;

  // start by taking out DC bias.  This will make negative #s...
  mapped_sample = input - SAMPLE_BIAS;

  // add in gain.
  mapped_sample = mapped_sample / gain;
  
  // center on 16.
  mapped_sample = mapped_sample + 16;

  // and clip.
  if (mapped_sample > 31) mapped_sample = 31;
  if (mapped_sample < 0) mapped_sample = 0;

  return mapped_sample;
}

void show_samples_lines( void )
{
  int x;
  int y;
  int last_x=0;
  int last_y=16;

  matrix.fillScreen(0);
  
  // a red boarder all the way around.
  matrix.drawRect(0,0,MATRIX_WIDTH,MATRIX_HEIGHT,matrix.Color333(1,0,0));
  
  for (x=0; x < SAMPLE_SIZE; x++)
  {
    y=map_sample(sample[x]);
    matrix.drawLine(last_x,last_y,x,y,matrix.Color333(0,0,1));
    last_x = x;
    last_y = y;
  }
  
  matrix.swapBuffers(true);
}

void setup() 
{
  unsigned long start_time;
  
  Serial.begin(9600);
  matrix.begin();



}
  
  

void loop() 
{
  int pot_value;
  int audio_val;
  int envelope_val;
  int gate_val;
  static unsigned long last_time = 0;
  unsigned long current_time;
  
 

  // Only print these out once per second
  current_time = millis();
  if (current_time > last_time + 1000)
  {
    last_time = current_time;

     // read potentiometer and display out serial
    pot_value = analogRead(POT_PIN);
    Serial.print("Pot: ");
    Serial.println(pot_value);

    // read sound board pins and display out serial
    audio_val = analogRead(AUDIO_PIN);
    envelope_val = analogRead(ENVELOPE_PIN);
    gate_val = analogRead(GATE_PIN);
    Serial.print("audio env gate = ");
    Serial.print(audio_val);
    Serial.print(" ");
    Serial.print(envelope_val);
    Serial.print(" ");
    Serial.println(gate_val);
  }

  collect_samples();
  show_samples_lines();
  
}
