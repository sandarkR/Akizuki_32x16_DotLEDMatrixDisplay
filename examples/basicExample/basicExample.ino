#include <Akizuki_32x16_DotLEDMatrixDisplay.h>
#include "kaomoji_64x16_01_Csource.c"


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

/*** MicroSD card ****************************************/
/* Usage on ESP32 Breakout board with MicroSD Socket */
const uint8_t SD_CS   = 13;  // CS(SS)
const uint8_t SD_CLK  = 14;
const uint8_t SD_MISO =  2;
const uint8_t SD_MOSI = 15;

const uint32_t sd_freq = 80000000;
/*********************************************************/

/*** Shinonome Font **************************************************************************/
ESP32_SD_ShinonomeFNT SFR(SD_CS, sd_freq);

const char* UTF8SJIS_file = "/font/Utf8Sjis.tbl";  // UTF8 Shift_JIS 変換テーブルファイル名を記載しておく
const char* Shino_Zen_Font_file = "/font/shnmk16.bdf";  // 全角フォントファイル名を定義
const char* Shino_Half_Font_file = "/font/shnm8x16.bdf";  // 半角フォントファイル名を定義
/*********************************************************************************************/

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

/*** Application *****************************************************************************/
//bool scl_pause = false;       // scroll pause flag
//bool disp = true;             // display flag
//bool play_all_demo = false;   // play all demo flag
//boolean state = 1;      // 0: Setting  1: Display
//uint8_t demo_mode = 1;  // demo number
//uint8_t demo_num = 6;   // number of demo
//uint16_t tWidth = 0;        // text width
//int16_t tX0 = panelWidth;   // the origin of X axis of text
//uint16_t scrollDelay = 20;  // default : 20
//String displayText = "日本語表示も可能です。";  // 11 characters

/* Limit number of characters to display */
const uint8_t MAX_WORD = 200;  // Maximum word count (shoud to set 4 or more)

/* Buffer of string bitmap */
uint8_t *strBmp;
/*********************************************************************************************/





void setup() {
  Serial.begin(115200);
  while (!Serial);

  /*** Akizuki_32x16_DotLEDMatrixDisplay ********************************************************/
  matrix.begin();
  Serial.println("Initialized LED panel successfully.");
  
  matrix.ShinonomeFNT_SD_init(SD_CLK, SD_MISO, SD_MOSI, SD_CS, sd_freq,
                              UTF8SJIS_file, Shino_Zen_Font_file, Shino_Half_Font_file, &SFR);
  matrix.shnmFntBegin(MAX_WORD);  // max number of characters
  matrix.setTextColor(DOT_RED, DOT_BLACK);  // text color, background color
  strBmp = (uint8_t*) calloc(MAX_WORD * 2 * 16, sizeof(uint8_t));
  /********************************************************************************************/

  /************** Switches ********************************************************************/
  pinMode(SW_1, INPUT);
  pinMode(SW_2, INPUT);
  pinMode(SW_3, INPUT);
  pinMode(SW_4, INPUT);
  /********************************************************************************************/

  delay(1000);


  
  /***** Test of library methods *****/
  /* origin of drawing coordinates */
  /*  supported only some methods  */
  matrix.setCursor(0, 0);

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

  /* draw bitmap with color */
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.drawGIMP_CSourceImageDump(16,  // x axis
                                     0,   // y axis
                                     KAOMOJI_64X16_01_CSOURCE_pixel_data,
                                     KAOMOJI_64X16_01_CSOURCE_WIDTH,
                                     KAOMOJI_64X16_01_CSOURCE_HEIGHT
                                    );
  }

  
  /* display Japanese characters by convert and write with same time */
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.setTextColor(DOT_RED, DOT_BLACK);  // text color, background color
    matrix.setCursor(24, 0);
    matrix.shnmFntPrint("表示１");  // Note: Refresh rate is dependent on length of string
  }
  
  /* display Japanese characters by write after convert 1 */
  matrix.fillScreen(DOT_BLACK);
  matrix.setTextColor(DOT_RED, DOT_BLACK);  // text color, background color
  matrix.setCursor(24, 0);
  matrix.shnmFntStrConv("表示２", strBmp);
  matrix.startWrite();
  matrix.shnmFntWrite(strBmp);  // write
  matrix.endWrite();  // display
  for(int frame = 0; frame < 1000; frame++) {
    matrix.startWrite();
    matrix.endWrite();  // display
    delay(10);  // Recommended 1usec or more [1usec: 0.43A  10usec:0.19A  (none: 0.48A)]
  }
  
  /* display Japanese characters by write after convert 2 */
  for(int frame = 0; frame < 500; frame++) {
    matrix.fillScreen(DOT_BLACK);
    matrix.setTextColor(DOT_RED, DOT_BLACK);  // text color, background color
    matrix.setCursor(24, 0);
    matrix.shnmFntStrConv("表示３", strBmp);
    matrix.startWrite();
    matrix.shnmFntWrite(strBmp);  // write
    matrix.endWrite();  // display
  }

  /* scroll Japanese characters with color by convert and write with same time */
  matrix.fillScreen(DOT_BLACK);
  matrix.setTextColor(DOT_RED, DOT_BLACK);  // text color, background color
  uint8_t delay_scroll_1 = 20;  // 20msec/dot
  matrix.shnmFntScroll("日本語スクロールも可能です。", delay_scroll_1);
//  /* example: https://x.com/akizuki_yashio/status/1926465607211331780 */
//  matrix.shnmFntScroll("八潮店限定!!!　電光掲示板キット!　1480円!!　案内サインの表示や列車の電光掲示板の再現に!! 文字列をシリアル通信で転送したり、ROM内に書き込んで表示が可能です!　追加ディスプレイは3枚まで追加可能です。", delay_scroll_1);

  /* debug for using shinonome font */
  /*     output Serial monitor      */
  String debugText = "abcABCＡＢＣひらがなカタカナﾊﾝｶｸ漢字";
  matrix.shnmFntDbg_FontDisp(debugText);
}


void loop() {
  
}
