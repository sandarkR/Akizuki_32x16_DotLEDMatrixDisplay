#include <Akizuki_32x16_DotLEDMatrixDisplay.h>
#include "kaomoji_64x16_01.c"


/*** Akizuki_32x16_DotLEDMatrixDisplay *******************/
/* pin assign ->        [ESP32]  [PANEL]*/
/*            LED_PWR               1p */
const uint8_t SIN_1_PIN  = 27;  //  2p
const uint8_t SIN_2_PIN  = 16;  //  3p
const uint8_t SIN_3_PIN  = 33;  //  4p
const uint8_t CLOCK_PIN  = 17;  //  5p
const uint8_t LATCH_PIN  = 25;  //  6p
const uint8_t STROBE_PIN = 26;  //  7p
/*            IC_PWR                8p */
/*            GND                   9p */
/*            GND                  10p */

const uint8_t panelNum = 3;  // number of panel
const uint8_t panelWidth  = 32 * panelNum;
const uint8_t panelHeight = 16;

Akizuki_32x16_DotLEDMatrixDisplay matrix(
  panelNum,
  SIN_1_PIN,
  SIN_2_PIN,
  SIN_3_PIN,
  CLOCK_PIN,
  LATCH_PIN,
  STROBE_PIN
);
/*********************************************************/

/*** Switches ********************************************************************************/
/* Switches pin assign */
const int8_t SW_1 = 36;
const int8_t SW_2 = 39;
const int8_t SW_3 = 35;
const int8_t SW_4 = 34;  // slide switch

//volatile uint8_t sw1_Read = 0x00;
//volatile uint8_t sw2_Read = 0x00;
//volatile uint8_t sw3_Read = 0x00;
//volatile uint8_t sw4_Read = 0x00;
//volatile boolean sw1_Pressed = 0;
//volatile boolean sw2_Pressed = 0;
//volatile boolean sw3_Pressed = 0;
//volatile boolean sw4_Changed = 0;
//
///* Hardware Timer */
///* ref: https://garretlab.web.fc2.com/arduino/esp32/examples/ESP32/Timer_RepeatTimer.html */
//hw_timer_t * timer = NULL;
//volatile SemaphoreHandle_t timerSemaphore;
//portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
//
//void IRAM_ATTR onTimer() {
//  /* Increment the counter and set the time of ISR */
//  portENTER_CRITICAL_ISR(&timerMux);
//
//  /* Read the status of switches by use N times matching method */
//  /* SW_1 */
//  sw1_Read = (0xfe & (sw1_Read << 1)) | (0x01 & (uint8_t)digitalRead(SW_1));
//  if (sw1_Read == 0x7f) sw1_Pressed = 1; // 0x7f=0111_1111 : N=7
//  /* SW_2 */
//  sw2_Read = (0xfe & (sw2_Read << 1)) | (0x01 & (uint8_t)digitalRead(SW_2));
//  if (sw2_Read == 0x7f) sw2_Pressed = 1; // 0x7f=0111_1111 : N=7
//  /* SW_3 */
//  sw3_Read = (0xfe & (sw3_Read << 1)) | (0x01 & (uint8_t)digitalRead(SW_3));
//  if (sw3_Read == 0x7f) sw3_Pressed = 1; // 0x7f=0111_1111 : N=7
//  /* SW_4 */
//  sw4_Read = (0xfe & (sw4_Read << 1)) | (0x01 & (uint8_t)digitalRead(SW_4));
//  if (sw4_Read == 0x7f || sw4_Read == 0xfe) sw4_Changed = 1;
//
//  portEXIT_CRITICAL_ISR(&timerMux);
//  // Give a semaphore that we can check in the loop
//  xSemaphoreGiveFromISR(timerSemaphore, NULL);
//  // It is safe to use digitalRead/Write here if you want to toggle an output
//}
/*********************************************************************************************/





void setup() {
  Serial.begin(115200);
  while (!Serial);

  /*** Akizuki_32x16_DotLEDMatrixDisplay ********************************************************/
  matrix.begin();
  Serial.println("Initialized LED panel successfully.");

  matrix.setTextColor(DOT_RED, DOT_BLACK);  // text color, background color
  /********************************************************************************************/

  /************** Switches ********************************************************************/
  pinMode(SW_1, INPUT);
  pinMode(SW_2, INPUT);
  pinMode(SW_3, INPUT);
  pinMode(SW_4, INPUT);
  /********************************************************************************************/

  delay(1000);



  /***** Test of compatible with Adafruit GFX library methods *****/
  /* origin of drawing coordinates */
  /*  supported only some methods  */
  matrix.setCursor(10, 0);

  /* draw pixel */  
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.drawPixel(0, 0, DOT_RED);  // x axis, y axis, color
    matrix.drawPixel(panelWidth - 3, panelHeight - 3, DOT_RED);
  }
  
  /* draw pixel after write to buffer */
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.startWrite();
    matrix.writePixel(0, panelHeight - 1, DOT_RED);  // x axis, y axis, color
    matrix.writePixel(panelWidth - 1, 0, DOT_RED);
    matrix.endWrite();
  }

  /* print characters */
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.setCursor(0, 0);
    matrix.println("abc");
    matrix.print("def");
  }
  
  /* draw filled circle */
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.fillCircle(7, 8, 7, DOT_RED);
  }

  /* draw XBitMap Files (*.xbm)  */
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.drawXBitmap(16, 0, kaomoji_64x16_01_bits, 64, 16, DOT_RED);
  }

  matrix.fillScreen(DOT_BLACK);
}

void loop() {
  
}
