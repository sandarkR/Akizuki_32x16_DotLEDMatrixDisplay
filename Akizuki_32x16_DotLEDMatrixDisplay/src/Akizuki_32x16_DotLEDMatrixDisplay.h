#ifndef _Akizuki_32x16_DotLEDMatrixDisplay_H_
#define _Akizuki_32x16_DotLEDMatrixDisplay_H_

/* Use for Graphic drawing */
#include <Adafruit_GFX.h>
 
/* Use for Shinonome Font (include the Japanese font) */
#include <ESP32_SD_ShinonomeFNT.h>
#include <ESP32_SD_UTF8toSJIS.h>

/* Number of LEDs of a panel */
const uint8_t DOT_PANEL_WIDTH  = 32;
const uint8_t DOT_PANEL_HEIGHT = 16;

/* Color */
const uint16_t DOT_BLACK  = 0x0000;  /* Black  */
const uint16_t DOT_RED    = 0xF800;  /*  Red   */

/* Class definition */
class Akizuki_32x16_DotLEDMatrixDisplay : public Adafruit_GFX {
  public:
    /* Constructor */
    Akizuki_32x16_DotLEDMatrixDisplay(
      uint8_t n_panel,    /* Number of panels */
      uint8_t SIN_1_PIN,  /* 2p */
      uint8_t SIN_2_PIN,  /* 3p */
      uint8_t SIN_3_PIN,  /* 4p */
      uint8_t CLOCK_PIN,  /* 5p */
      uint8_t LATCH_PIN,  /* 6p */
      uint8_t STROBE_PIN  /* 7p */
    );

    /* Destructor */
    ~Akizuki_32x16_DotLEDMatrixDisplay(void);

    /* Adafruit_GFX library : Must be overridden in subclass. */
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    /* Adafruit_GFX library : TRANSACTION API / CORE DRAW API */
    void startWrite(void);
    void writePixel(int16_t x, int16_t y, uint16_t color);
    void endWrite(void);

    /* Original method */
    void begin();
    void ShinonomeFNT_SD_init(
      uint8_t pin_SD_CLK,
      uint8_t pin_SD_MISO,
      uint8_t pin_SD_MOSI,
      uint8_t pin_SD_CS,
      const uint32_t SD_Frep,
      const char* UTF8SJIS_filePath,
      const char* Shino_Zen_Font_filePath,
      const char* Shino_Half_Font_filePath,
      ESP32_SD_ShinonomeFNT* Shino
    );
    void shnmFntBegin(uint8_t max_word);
    uint16_t shnmFntStrConv(String str, uint8_t *strBmp);
    //
    void shnmFntDbg_FontDisp(String str);
    //
    // uint16_t shnmFntGetTxtWidth(String str);
    //
    void shnmFntWrite(uint8_t *strBmp);
    void shnmFntPrint(String str);
    void shnmFntScroll(String str, uint8_t speed_ms);
    //
    void drawGIMP_CSourceImageDump(int16_t x, int16_t y, const uint8_t bitmap[],
                                   int16_t w, int16_t h);


  private:
    uint8_t pinSIN1, pinSIN2, pinSIN3, pinCLK, pinLAT, pinSTB;
    uint8_t nPanel;
    uint32_t transactionCounter;
    uint32_t *buffer;
    
    /* Use of shinonome font */
    uint8_t MAX_WORD;   /* Maximum number of words setting */
    ESP32_SD_ShinonomeFNT *SF;
    uint8_t   *sjCode;        /* Shift-JIS code storing */
    boolean   *ZorH;          /* Zenkaku:1 or Hankaku:0 */
    uint8_t   *fntBmp;        /* Font bitmap data 8x16  */
    uint16_t  *fntBmpColor;   /* Font bitmap color data */
    uint16_t  wordCnt;        /* Word count */

    /* Private methods */
    uint8_t shnmFntZenOrHan(uint16_t len);
    void shnmFntDbg_FontDispLine8(uint8_t buf);
    void shnmFntDbg_FontDispLine16(uint8_t buf_l, uint8_t buf_r);
};

#endif // _Akizuki_32x16_DotLEDMatrixDisplay_H_