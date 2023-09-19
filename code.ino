#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <FastLED.h>
#include <arduinoFFT.h>
FASTLED_USING_NAMESPACE
/////////////////////////////////////////////////////////////////////////////
#include <SoftwareSerial.h>
SoftwareSerial btSerial(0,1); // RX, TX pins of the Bluetooth module
// How many leds are in the strip?
#define NUM_LEDS 48
#define BRIGHTNESS 56
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define FRAMES_PER_SECOND 60
#define MAX_POWER_MILLIAMPS 500
#define LED_PIN 5
#define LED_COUNT 120
#define NUM_LEDS 48
#define FRAMES_PER_SECOND 120
/*
const int trigPin = 7;
const int echoPin = 6;
*/
bool gReverseDirection = false;
#define UPDATES_PER_SECOND 100
// For led chips like WS2812, which have a data line, ground, and power, you 
just
// need to define DATA_PIN. For led chipsets that are SPI based (four wires 
- data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 5
#define CLOCK_PIN 13
#define bottonPin 3
int bt=0;
// This is an array of leds. One item for each led in your strip.
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
#define COOLING 55
#define SPARKING 120
arduinoFFT FFT = arduinoFFT();
double realComponent[64];
double imagComponent[64];
int spectralHeight[] = {0, 32, 48, 56, 60, 62, 63, 64};
int index, c;
const int micPin = A5;
Adafruit_NeoPixel led = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + 
NEO_KHZ800);
Adafruit_NeoPixel pixels(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);
void Fire2012()
{
// Array of temperature readings at each simulation cell
 static uint8_t heat[NUM_LEDS];
 // Step 1. Cool down every cell a little
 for( int i = 0; i < NUM_LEDS; i++) {
 heat[i] = qsub8( heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 
2));
 }
 
 // Step 2. Heat from each cell drifts 'up' and diffuses a little
 for( int k= NUM_LEDS - 1; k >= 2; k--) {
 heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
 }
 
 // Step 3. Randomly ignite new 'sparks' of heat near the bottom
 if( random8() < SPARKING ) {
 int y = random8(7);
 heat[y] = qadd8( heat[y], random8(160,255) );
 }
 // Step 4. Map from heat cells to LED colors
 for( int j = 0; j < NUM_LEDS; j++) {
 CRGB color = HeatColor( heat[j]);
 int pixelnumber;
 if( gReverseDirection ) {
 pixelnumber = (NUM_LEDS-1) - j;
 } else {
 pixelnumber = j;
 }
 leds[pixelnumber] = color;
 }
}
void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
 uint8_t brightness = 255;
 
 for( int i = 0; i < NUM_LEDS; ++i) {
 leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, 
currentBlending);
 colorIndex += 3;
 }
}
void ChangePalettePeriodically()
{
 uint8_t secondHand = (millis() / 1000) % 60;
 static uint8_t lastSecond = 99;
 
 if( lastSecond != secondHand) {
 lastSecond = secondHand;
 if( secondHand == 0) { currentPalette = RainbowColors_p; 
currentBlending = LINEARBLEND; }
 if( secondHand == 10) { currentPalette = RainbowStripeColors_p; 
currentBlending = NOBLEND; }
 if( secondHand == 15) { currentPalette = RainbowStripeColors_p; 
currentBlending = LINEARBLEND; }
 if( secondHand == 20) { SetupPurpleAndGreenPalette(); 
currentBlending = LINEARBLEND; }
 if( secondHand == 25) { SetupTotallyRandomPalette(); 
currentBlending = LINEARBLEND; }
 if( secondHand == 30) { SetupBlackAndWhiteStripedPalette(); 
currentBlending = NOBLEND; }
 if( secondHand == 35) { SetupBlackAndWhiteStripedPalette(); 
currentBlending = LINEARBLEND; }
 if( secondHand == 40) { currentPalette = CloudColors_p; 
currentBlending = LINEARBLEND; }
 if( secondHand == 45) { currentPalette = PartyColors_p; 
currentBlending = LINEARBLEND; }
 if( secondHand == 50) { currentPalette = myRedWhiteBluePalette_p; 
currentBlending = NOBLEND; }
 if( secondHand == 55) { currentPalette = myRedWhiteBluePalette_p; 
currentBlending = LINEARBLEND; }
 }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
 for( int i = 0; i < 16; ++i) {
 currentPalette[i] = CHSV( random8(), 255, random8());
 }
}
void SetupBlackAndWhiteStripedPalette()
{
 // 'black out' all 16 palette entries...
 fill_solid( currentPalette, 16, CRGB::Black);
 // and set every fourth one to white.
 currentPalette[0] = CRGB::White;
 currentPalette[4] = CRGB::White;
 currentPalette[8] = CRGB::White;
 currentPalette[12] = CRGB::White;
 
}
// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
 CRGB purple = CHSV( HUE_PURPLE, 255, 255);
 CRGB green = CHSV( HUE_GREEN, 255, 255);
 CRGB black = CRGB::Black;
 
 currentPalette = CRGBPalette16(
 green, green, black, black,
 purple, purple, black, black,
green, green, black, black,
purple, purple, black, black );
}
.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
 CRGB::Red,
 CRGB::Gray, // 'white' is too bright compared to red and blue
 CRGB::Blue,
 CRGB::Black,
 
 CRGB::Red,
 CRGB::Gray,
 CRGB::Blue,
 CRGB::Black,
 
 CRGB::Red,
 CRGB::Red,
 CRGB::Gray,
 CRGB::Gray,
 CRGB::Blue,
 CRGB::Blue,
 CRGB::Black,
 CRGB::Black
};
CRGBPalette16 pacifica_palette_1 = 
 { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 
0x000117, 
 0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x14554B, 
0x28AA50 };
CRGBPalette16 pacifica_palette_2 = 
 { 0x000507, 0x000409, 0x00030B, 0x00030D, 0x000210, 0x000212, 0x000114, 
0x000117, 
 0x000019, 0x00001C, 0x000026, 0x000031, 0x00003B, 0x000046, 0x0C5F52, 
0x19BE5F };
CRGBPalette16 pacifica_palette_3 = 
 { 0x000208, 0x00030E, 0x000514, 0x00061A, 0x000820, 0x000927, 0x000B2D, 
0x000C33, 
 0x000E39, 0x001040, 0x001450, 0x001860, 0x001C70, 0x002080, 0x1040BF, 
0x2060FF };
void pacifica_loop()
{
 // Increment the four "color index start" counters, one for each wave 
layer.
 // Each is incremented at a different speed, and the speeds vary over time.
 static uint16_t sCIStart1, sCIStart2, sCIStart3, sCIStart4;
 static uint32_t sLastms = 0;
 uint32_t ms = GET_MILLIS();
 uint32_t deltams = ms - sLastms;
 sLastms = ms;
 uint16_t speedfactor1 = beatsin16(3, 179, 269);
 uint16_t speedfactor2 = beatsin16(4, 179, 269);
 uint32_t deltams1 = (deltams * speedfactor1) / 256;
 uint32_t deltams2 = (deltams * speedfactor2) / 256;
 uint32_t deltams21 = (deltams1 + deltams2) / 2;
 sCIStart1 += (deltams1 * beatsin88(1011,10,13));
 sCIStart2 -= (deltams21 * beatsin88(777,8,11));
 sCIStart3 -= (deltams1 * beatsin88(501,5,7));
 sCIStart4 -= (deltams2 * beatsin88(257,4,6));
 // Clear out the LED array to a dim background blue-green
 fill_solid( leds, NUM_LEDS, CRGB( 2, 6, 10));
 // Render each of four layers, with different scales and speeds, that vary 
over time
 pacifica_one_layer( pacifica_palette_1, sCIStart1, beatsin16( 3, 11 * 256, 14 * 256), beatsin8( 10, 70, 130), 0-beat16( 301) );
 pacifica_one_layer( pacifica_palette_2, sCIStart2, beatsin16( 4, 6 * 256, 9 * 256), beatsin8( 17, 40, 80), beat16( 401) );
 pacifica_one_layer( pacifica_palette_3, sCIStart3, 6 * 256, beatsin8( 9, 10,38), 0-beat16(503));
 pacifica_one_layer( pacifica_palette_3, sCIStart4, 5 * 256, beatsin8( 8, 10,28), beat16(601));
 // Add brighter 'whitecaps' where the waves lines up more
 pacifica_add_whitecaps();
 // Deepen the blues and greens a bit
 pacifica_deepen_colors();
}
// Add one layer of waves into the led array
void pacifica_one_layer( CRGBPalette16& p, uint16_t cistart, uint16_t 
wavescale, uint8_t bri, uint16_t ioff)
{
 uint16_t ci = cistart;
 uint16_t waveangle = ioff;
 uint16_t wavescale_half = (wavescale / 2) + 20;
 for( uint16_t i = 0; i < NUM_LEDS; i++) {
 waveangle += 250;
 uint16_t s16 = sin16( waveangle ) + 32768;
 uint16_t cs = scale16( s16 , wavescale_half ) + wavescale_half;
 ci += cs;
 uint16_t sindex16 = sin16( ci) + 32768;
 uint8_t sindex8 = scale16( sindex16, 240);
 CRGB c = ColorFromPalette( p, sindex8, bri, LINEARBLEND);
 leds[i] += c;
 }
}
// Add extra 'white' to areas where the four layers of light have lined up 
brightly
void pacifica_add_whitecaps()
{
 uint8_t basethreshold = beatsin8( 9, 55, 65);
 uint8_t wave = beat8( 7 );
 
 for( uint16_t i = 0; i < NUM_LEDS; i++) {
 uint8_t threshold = scale8( sin8( wave), 20) + basethreshold;
 wave += 7;
 uint8_t l = leds[i].getAverageLight();
 if( l > threshold) {
 uint8_t overage = l - threshold;
 uint8_t overage2 = qadd8( overage, overage);
 leds[i] += CRGB( overage, overage2, qadd8( overage2, overage2));
 }
 }
}
// Deepen the blues and greens
void pacifica_deepen_colors()
{
 for( uint16_t i = 0; i < NUM_LEDS; i++) {
 leds[i].blue = scale8( leds[i].blue, 145); 
 leds[i].green= scale8( leds[i].green, 200); 
 leds[i] |= CRGB( 2, 5, 7);
 }
}
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride() 
{
 static uint16_t sPseudotime = 0;
 static uint16_t sLastMillis = 0;
 static uint16_t sHue16 = 0;
 uint8_t sat8 = beatsin88( 87, 220, 250);
 uint8_t brightdepth = beatsin88( 341, 96, 224);
 uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
 uint8_t msmultiplier = beatsin88(147, 23, 60);
 uint16_t hue16 = sHue16;//gHue * 256;
 uint16_t hueinc16 = beatsin88(113, 1, 3000);
 
 uint16_t ms = millis();
 uint16_t deltams = ms - sLastMillis ;
 sLastMillis = ms;
 sPseudotime += deltams * msmultiplier;
 sHue16 += deltams * beatsin88( 400, 5,9);
 uint16_t brightnesstheta16 = sPseudotime;
 
 for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
 hue16 += hueinc16;
 uint8_t hue8 = hue16 / 256;
 brightnesstheta16 += brightnessthetainc16;
 uint16_t b16 = sin16( brightnesstheta16 ) + 32768;
 uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
 uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
 bri8 += (255 - brightdepth);
 
 CRGB newcolor = CHSV( hue8, sat8, bri8);
 
 uint16_t pixelnumber = i;
 pixelnumber = (NUM_LEDS-1) - pixelnumber;
 
 nblend( leds[pixelnumber], newcolor, 64);
 }
}
void twinkle() {
 // Set a random LED to a random color
 leds[random(NUM_LEDS)] = CRGB(random(0,255), random(0,255), random(0,255));
 
31
 // Fade out all LEDs
 for (int i = 0; i < NUM_LEDS; i++) {
 leds[i].fadeToBlackBy(20);
 }
}
// List of patterns to cycle through. Each is defined as a separate function 
below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, 
sinelon, juggle, bpm };
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is 
current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
 
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
void nextPattern()
{
 // add one to the current pattern number, and wrap around at the end
 gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( 
gPatterns);
}
void rainbow() 
{
 // FastLED's built-in rainbow generator
 fill_rainbow( leds, NUM_LEDS, gHue, 7);
}
void rainbowWithGlitter() 
{
 // built-in FastLED rainbow, plus some random sparkly glitter
 rainbow();
 addGlitter(80);
}
void addGlitter( fract8 chanceOfGlitter) 
{
 if( random8() < chanceOfGlitter) {
 leds[ random16(NUM_LEDS) ] += CRGB::White;
 }
}
void confetti() 
{
 // random colored speckles that blink in and fade smoothly
 fadeToBlackBy( leds, NUM_LEDS, 10);
 int pos = random16(NUM_LEDS);
32
 leds[pos] += CHSV( gHue + random8(64), 200, 255);
}
void sinelon()
{
 // a colored dot sweeping back and forth, with fading trails
 fadeToBlackBy( leds, NUM_LEDS, 20);
 int pos = beatsin16( 13, 0, NUM_LEDS-1 );
 leds[pos] += CHSV( gHue, 255, 192);
}
void bpm()
{
 // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
 uint8_t BeatsPerMinute = 62;
 CRGBPalette16 palette = PartyColors_p;
 uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
 for( int i = 0; i < NUM_LEDS; i++) { //9948
 leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
 }
}
void juggle() {
 // eight colored dots, weaving in and out of sync with each other
 fadeToBlackBy( leds, NUM_LEDS, 20);
 uint8_t dothue = 0;
 for( int i = 0; i < 8; i++) {
 leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
 dothue += 32;
 }
}
// This function sets up the ledsand tells the controller about them
void setup() {
 // Initialize the ultrasonic sensor pins
 //pinMode(trigPin, OUTPUT);
 //pinMode(echoPin, INPUT);
 
 // Initialize the serial communication
 Serial.begin(9600);
 btSerial.begin(9600); // initialize Bluetooth communication
 // sanity check delay - allows reprogramming if accidently blowing power 
w/leds
 delay(2000);
 pinMode(bottonPin,INPUT);
 // Uncomment/edit one of the following lines for your leds arrangement.
 // ## Clockless types ##
 
 FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
 // ## Clocked (SPI) types ##
pixels.begin(); // Initialize LED strip
}
33
// This function runs over and over, and is where you do the magic to light
// your leds.
int t=6;
void loop() {
if(Serial.available() > 0) 
 {3333333330000000000000006
 t = Serial.read(); 
 Serial.print(t); 
 Serial.print("\n"); }
bt=digitalRead(bottonPin); 
if (bt==1){
 if (t==9){t=1;}
 else{t++;}
 delay(500);}
 
 
if (t==1){
 
 // Move a single white led 
 for(int whiteLed = 0; whiteLed < NUM_LEDS; whiteLed = whiteLed + 1) {
 // Turn our current led on to white, then show the leds
 leds[whiteLed] = CRGB(2, 248, 35);
 // Show the leds (only one of which is set to white, from above)
 FastLED.show();
 // Wait a little bit
 delay(10);
 // Turn our current led back to black for the next loop around
 leds[whiteLed] = CRGB::Black;}}
 else if(t==2)
 { //twinkle
 twinkle();
 FastLED.show();
 FastLED.delay(1000 / FRAMES_PER_SECOND);
 }
else if(t==5)
 { 
/// Pacifica
 EVERY_N_MILLISECONDS( 20) {
 pacifica_loop();
 FastLED.show();
 }
}
34
 else if(t==3){
 //Add entropy to random number generator; we use a lot of it.
 // random16_add_entropy( random());
 Fire2012(); // run simulation frame
 
 FastLED.show(); // display this frame
 FastLED.delay(1000 / FRAMES_PER_SECOND);
 }
else if(t==4)
{ //Pride2015
 pride();
 FastLED.show(); 
}
 else if(t==6){
// rainbow
 static uint8_t hue = 0;
 // Fill the entire strip with a rainbow of colors
 for (int i = 0; i < NUM_LEDS; i++) {
 leds[i] = CHSV(hue + (i * 255 / NUM_LEDS), 255, 255);
 }
 // Move the rainbow along the strip
 for (int i = 0; i < NUM_LEDS; i++) {
 leds[i] = leds[(i + 1) % NUM_LEDS];
 }
 FastLED.show();
 hue++;
 }
else if(t==7){
 // DemoReel100.
 // Call the current pattern function once, updating the 'leds' array
 gPatterns[gCurrentPatternNumber]();
 // send the 'leds' array out to the actual LED strip
 FastLED.show(); 
 // insert a delay to keep the framerate modest
 FastLED.delay(1000/FRAMES_PER_SECOND); 
 // do some periodic updates
 EVERY_N_MILLISECONDS( 20 ) { gHue++; } 
// slowly cycle the "base color" through the rainbow
 EVERY_N_SECONDS( 10 ) { nextPattern(); } 
35
// change patterns periodically
 }
else if(t==8){
 
 ChangePalettePeriodically();
 
 static uint8_t startIndex = 0;
 startIndex = startIndex + 1; /* motion speed */
 
 FillLEDsFromPaletteColors( startIndex);
 
 FastLED.show();
 FastLED.delay(1000 / UPDATES_PER_SECOND);}
else if (t==9)
{
 int sensitivity = map(analogRead(A6), 0, 1023, 50, 100);
 Serial.println(analogRead(A6));
 for (int i = 0; i < 64; i++)
 {
 realComponent[i] = analogRead(micPin) / sensitivity; // Read from the 
microphone sensor pin
 imagComponent[i] = 0;
 }
 FFT.Windowing(realComponent, 64, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
 FFT.Compute(realComponent, imagComponent, 64, FFT_FORWARD);
 FFT.ComplexToMagnitude(realComponent, imagComponent, 64);
 for (int i = 0; i < 16; i++)
 {
 int ledIndex = i % 4 * 4 + i / 4;
 realComponent[i] = constrain(realComponent[i], 0, 80);
 realComponent[i] = map(realComponent[i], 0, 80, 0, 7);
 index = realComponent[i];
 c = spectralHeight[index];
 led.setPixelColor(ledIndex, c, 0, 0); // Set RGB values accordingly 
(e.g., Red)
 led.setPixelColor(ledIndex+16, c, 0, 0); 
 led.setPixelColor(ledIndex+32, c, 0, 0); 
 
 }
 led.show();
}
pixels.show(); 
}