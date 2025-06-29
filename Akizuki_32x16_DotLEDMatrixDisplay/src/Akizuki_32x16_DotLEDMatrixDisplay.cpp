#include "Akizuki_32x16_DotLEDMatrixDisplay.h"

/* Constructor */
Akizuki_32x16_DotLEDMatrixDisplay::Akizuki_32x16_DotLEDMatrixDisplay(
  uint8_t n_panel,
  uint8_t pin_sin1, uint8_t pin_sin2, uint8_t pin_sin3,
  uint8_t pin_clk, uint8_t pin_lat, uint8_t pin_stb) :
  Adafruit_GFX(DOT_PANEL_WIDTH * n_panel, DOT_PANEL_HEIGHT),
  pinSIN1(pin_sin1), pinSIN2(pin_sin2), pinSIN3(pin_sin3),
  pinCLK(pin_clk), pinLAT(pin_lat), pinSTB(pin_stb),
  nPanel(n_panel), transactionCounter(0) {
  if (!buffer) {
    buffer = (uint32_t*) calloc(n_panel * DOT_PANEL_HEIGHT, sizeof(uint32_t));
  }
}

/* Destructor */
Akizuki_32x16_DotLEDMatrixDisplay::~Akizuki_32x16_DotLEDMatrixDisplay(void) {
  if (buffer) {
    free(buffer);
  }
  if (sjCode) {
    free(sjCode);
  }
  if (ZorH) {
    free(ZorH);
  }
  if (fntBmp) {
    free(fntBmp);
  }
  if (fntBmpColor) {
    free(fntBmpColor);
  }
}

/* Initialize */
void Akizuki_32x16_DotLEDMatrixDisplay::begin() {
  pinMode(pinSIN1, OUTPUT);
  pinMode(pinSIN2, OUTPUT);
  pinMode(pinSIN3, OUTPUT);
  pinMode(pinLAT,  OUTPUT);
  pinMode(pinCLK,  OUTPUT);
  pinMode(pinSTB,  OUTPUT);

  digitalWrite(pinSIN1, LOW);
  digitalWrite(pinSIN2, LOW);
  digitalWrite(pinSIN3, LOW);
  digitalWrite(pinLAT,  HIGH);  // Hold data
  digitalWrite(pinCLK,  LOW);
  digitalWrite(pinSTB,  LOW);   // Output enable

  fillScreen(DOT_BLACK);

  /* default */
  textcolor = DOT_RED;
  textbgcolor = DOT_BLACK;
}

/* Draw pixel */
void Akizuki_32x16_DotLEDMatrixDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
  startWrite();
  writePixel(x, y, color);
  endWrite();
}

/* Begin write data */
void Akizuki_32x16_DotLEDMatrixDisplay::startWrite() {
  transactionCounter++;
}

/* Finish write data */
void Akizuki_32x16_DotLEDMatrixDisplay::endWrite() {
  if (--transactionCounter) {
    return;
  }
  if (!buffer) {
    return;
  }

  // ########################################################################################
  for (uint16_t y = 0; y < HEIGHT; y++) {
    // digitalWrite(pinSTB, LOW);  // Output enable  これがあると下部行が1列遅れる

    /* Repeat to write X axis dots as many panels as */
    for (uint8_t n = 1; n <= nPanel; n++) {

      /* Write X axis dots of a panel to shift register */
      for (uint8_t x = 0; x < (DOT_PANEL_WIDTH / 2); x++) {

        uint16_t i = (x >> 5) + (y * nPanel) + (nPanel - n);  // Buffer address
        uint8_t panelX = x & 31;
        
        /* Select row to write */
        if(y == x) {
          digitalWrite(pinSIN1, HIGH);
        } else {
          digitalWrite(pinSIN1, LOW);
        }

        /* Write data*/
        digitalWrite(pinSIN2, (buffer[i] >> (DOT_PANEL_WIDTH - 1 - panelX + 16)) & 1);  // LED1
        digitalWrite(pinSIN3, (buffer[i] >> (DOT_PANEL_WIDTH - 1 - panelX +  0)) & 1);  // LED2

        digitalWrite(pinCLK, HIGH);
        delayMicroseconds(1);
        digitalWrite(pinCLK, LOW);
        delayMicroseconds(1);
      }
    }

    digitalWrite(pinSIN1, LOW);
    digitalWrite(pinSIN2, LOW);  // LED1
    digitalWrite(pinSIN3, LOW);  // LED2

    digitalWrite(pinLAT, LOW);
    delayMicroseconds(2);
    digitalWrite(pinSTB, LOW);  // Output enable  これがないと表示されない
    digitalWrite(pinLAT, HIGH);
  }

  /* delay*/
  delayMicroseconds(100*2);  // これがないと下部行が薄くなる 400:OK　100:若干薄い

  digitalWrite(pinSTB, LOW);   // Output enable これがないと全体が薄い。下部行が明るくなる。
  delayMicroseconds(1);
  digitalWrite(pinSTB, HIGH);  // Output disable
  delayMicroseconds(1);
}
// ########################################################################################

/* Write pixel data */
void Akizuki_32x16_DotLEDMatrixDisplay::writePixel(int16_t x, int16_t y, uint16_t color) {
  if (!buffer || x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
    return;
  }

  uint16_t i = (x >> 5) + (y * nPanel);
  uint8_t panelX = x & 31;

  switch (color) {
    case DOT_RED:
      buffer[i] |= 1 << panelX;
      break;
    default:
      buffer[i] &= ~(1 << panelX);
      break;
  }
}

/* Use for shinonome font */
void Akizuki_32x16_DotLEDMatrixDisplay::ShinonomeFNT_SD_init(
    uint8_t   pin_SD_CLK,
    uint8_t   pin_SD_MISO,
    uint8_t   pin_SD_MOSI,
    uint8_t   pin_SD_CS,
    const uint32_t  SD_Frep,
    const char* UTF8SJIS_filePath,
    const char* Shino_Zen_Font_filePath,
    const char* Shino_Half_Font_filePath,
    ESP32_SD_ShinonomeFNT* Shino
  ) {
    
    SF = Shino;

    /* SD card Initialization */
    Serial.print("Initializing SD card...");

    SPI.begin(pin_SD_CLK, pin_SD_MISO, pin_SD_MOSI, pin_SD_CS);  // CLK, MISO, MOSI, SS

    if (!SD.begin(pin_SD_CS, SPI)) {
      Serial.println("initialization failed!");
      return;
    }
    Serial.println("initialization done.");

    /* Open font file in SD card */
    SF->SD_Shinonome_Init3F(UTF8SJIS_filePath, Shino_Half_Font_filePath, Shino_Zen_Font_filePath);
}

/* Setup for use shinonome font */
void Akizuki_32x16_DotLEDMatrixDisplay::shnmFntBegin(uint8_t max_word) {
  if (max_word < 4) {
    /* Minimum number of character is 4 */
    MAX_WORD = 4;
  } else {
    MAX_WORD = max_word;
  }

  /* Buffers of shinonome font */
  if (!sjCode) {
    sjCode = (uint8_t*) calloc(MAX_WORD * 2, sizeof(uint8_t));
  }
  if (!ZorH) {
    ZorH = (boolean*) calloc(MAX_WORD, sizeof(boolean));
  }
  if (!fntBmp) {
    /* Number of character is MAX_WORD when Zenkaku, MAX_WORD * 2 when Hankaku */
    fntBmp = (uint8_t*) calloc(MAX_WORD * 2 * 16, sizeof(uint8_t));
  }
  if(!fntBmpColor) {
    fntBmpColor = (uint16_t*) calloc(MAX_WORD * 8 * 2 * 16, sizeof(uint16_t));
  }
}

/* Serial print Hankaku font bitmap line 8bit */
void Akizuki_32x16_DotLEDMatrixDisplay::shnmFntDbg_FontDispLine8(uint8_t buf) {
  for(int i=0; i<8; i++) {
    if(((128>>i)&buf)>>(7-i)) {
      Serial.print("#");
    }else{
      Serial.print(" ");
    }
  }
}

/* Serial print Zenkaku font bitmap line 16bit */
void Akizuki_32x16_DotLEDMatrixDisplay::shnmFntDbg_FontDispLine16(uint8_t buf_l, uint8_t buf_r) {
  shnmFntDbg_FontDispLine8(buf_l);
  shnmFntDbg_FontDispLine8(buf_r);
}

// http://dencha.ojaru.jp/programs_sub/zenkaku.html
// Shift-JIS コードから全角・半角を判断する関数
// 引数： Shift-JISコードの配列の長さ 配列のインデックス
uint8_t Akizuki_32x16_DotLEDMatrixDisplay::shnmFntZenOrHan(uint16_t len) {
  
  uint8_t cnt = 0;   // Number of characters

  for(int i=0; i<len; i++) {
    if ( (sjCode[i] >=0x81 && sjCode[i]<=0x9f) || (sjCode[i]>=0xe0 && sjCode[i]<=0xfc)){    // 全角の第１バイト
        if (sjCode[i+1]>=0x40 && sjCode[i+1]<=0xfc) {    // 全角の第２バイト
          ZorH[cnt] = 1;  // Zenkaku
          cnt++;
          i++;
        }
    }else{
      ZorH[cnt] = 0;  // Hankaku
      cnt++;
    }
  }
  
  return cnt;
}

/* Serial print str font */
void Akizuki_32x16_DotLEDMatrixDisplay::shnmFntDbg_FontDisp(String str) {
  
  /* String(UTF-8)->Shift-JIS code. and get sj_code array length */
  uint16_t sj_length = SF->UTF8toSJIS_convert(str, sjCode);
  /* String(UTF-8)->BitMap(8bitx16bit) */
  uint8_t fntBmpTmp[MAX_WORD][16] ={0};
  SF->StrDirect_ShinoFNT_readALL(str, fntBmpTmp);
  for(int i=0; i<MAX_WORD; i++) {
    for(int line=0; line<16; line++) {
      fntBmp[i*16 + line] = fntBmpTmp[i][line];
    }
  }

  /* Zenkaku or Hankaku by Shift-JIS code, and get nubmer of character */
  wordCnt = shnmFntZenOrHan(sj_length);

  Serial.println("\n ### Font Display Test ###");
  // Serial.printf("index = %d\n", index);
  Serial.print("str: "); Serial.println(str); 
  Serial.printf("sj_length = %d\n", sj_length);
  Serial.printf("Word count = %d\n\n", wordCnt);

  /* font bitmap index */
  int font_index = 0;
  
  for(uint8_t i=0; i<wordCnt; i++) {
    Serial.printf("%d=>\n", i);
    
    if(ZorH[i]) { // Zenkaku
      
      for(int j=0; j<16; j++) {
        shnmFntDbg_FontDispLine16(fntBmp[font_index*16 + j], fntBmp[(font_index+1)*16 + j]);
        Serial.println();
      }

      font_index = font_index + 2;
      
    }else{  // Hankaku
      
      for(int j=0; j<16; j++) {
        shnmFntDbg_FontDispLine8(fntBmp[font_index*16 + j]);
        Serial.println();
      }
      
      font_index++;
      
    }
  }
}

// /* Get str bitmap width  */
// uint16_t Akizuki_32x16_DotLEDMatrixDisplay::shnmFntGetTxtWidth(String str) {
//   // String(UTF-8)->Shift-JIS code. and get sj_code array length
//   uint16_t sjLen = SF->UTF8toSJIS_convert(str, sjCode);
//   // Zenkaku or Hankaku by Shift-JIS code. and get nubmer of character
//   wordCnt = shnmFntZenOrHan(sjLen);
//   // Text length limit
//   if (wordCnt > MAX_WORD) {
//     Serial.println("str ->");
//     Serial.println(str);
//     Serial.print("MAX_WORD ->");
//     Serial.print(MAX_WORD);
//     Serial.print("  wordCnt ->");
//     Serial.println(wordCnt);
//     Serial.println("エラー：文字数が設定された表示可能文字数を超えています。");

//     str = "ｴﾗｰ";  // Display error message

//     // String(UTF-8)->Shift-JIS code. and get sj_code array length
//     sjLen = SF->UTF8toSJIS_convert(str, sjCode);
//     // Zenkaku or Hankaku by Shift-JIS code. and get nubmer of character
//     wordCnt = shnmFntZenOrHan(sjLen);
//   }
  
//   uint16_t width = 0;  // width of shinonome font bitmap data

//   // Culculate text width
//   for(int i=0; i<wordCnt; i++) {
//     width += ZorH[i] + 1;  // 全角1 半角0
//   }

//   return width;
// }

/* Text font convert to shinonome font */
uint16_t Akizuki_32x16_DotLEDMatrixDisplay::shnmFntStrConv(String str, uint8_t *strBmp) {

  /* Initialize */
  for(int i=0; i < MAX_WORD * 2 * 16 ;i++) {
    strBmp[i] = 0;
  }

  /* String(UTF-8)->Shift-JIS code. and get sj_code array length */
  uint16_t sjLen = SF->UTF8toSJIS_convert(str, sjCode);
  /* Zenkaku or Hankaku by Shift-JIS code. and get nubmer of character */
  wordCnt = shnmFntZenOrHan(sjLen);

  /* Text length limit */
  if (wordCnt > MAX_WORD) {
    Serial.println("str ->");
    Serial.println(str);
    Serial.print("MAX_WORD ->");
    Serial.print(MAX_WORD);
    Serial.print("  wordCnt ->");
    Serial.println(wordCnt);
    Serial.println("エラー：文字数が設定された表示可能文字数を超えています。");

    str = "ｴﾗｰ";  // Display error message

    /* String(UTF-8)->Shift-JIS code. and get sj_code array length */
    sjLen = SF->UTF8toSJIS_convert(str, sjCode);
    /* Zenkaku or Hankaku by Shift-JIS code. and get nubmer of character */
    wordCnt = shnmFntZenOrHan(sjLen);
  }
  
  uint16_t numOf8x16 = 0;  // number of 8x16 bitmap data

  /* Culculate text width */
  for(int i=0; i<wordCnt; i++) {
    numOf8x16 += ZorH[i] + 1;
  }

  /* String(UTF-8)->BitMap(8bitx16bit) */
  uint8_t fntBmpTmp[str.length()][16] ={0};

  SF->StrDirect_ShinoFNT_readALL(str, fntBmpTmp);
  for(int i=0; i<numOf8x16; i++) {
    for(int line=0; line<16; line++) {
      // fntBmp[i*16 + line] = fntBmpTmp[i][line];
      strBmp[i*16 + line] = fntBmpTmp[i][line];
    }
  }

  return numOf8x16 * 8;
}

/* Draw shinonome font text */
void Akizuki_32x16_DotLEDMatrixDisplay::shnmFntWrite(uint8_t *strBmp) {

  uint16_t fntBmpX0;
  uint16_t panelX0;
  uint16_t fntBmpIdx_l, fntBmpIdx_r;  // left and right 8bit address of fntBmp
  uint8_t bmp8bit;

  if (cursor_x >= 0) {
    fntBmpX0 = 0;
    panelX0 = cursor_x;
  } else {
    fntBmpX0 = -cursor_x;
    panelX0 = 0;
  }

  for(uint8_t panelIdx=0; panelIdx<nPanel; panelIdx++) {
    for(int line=0; line<16; line++) {
      for(int slot=0; slot<4; slot++) {
        fntBmpIdx_l = (panelIdx * 64) + (slot * 16) + (fntBmpX0 / 8)*16 + line;
        fntBmpIdx_r = (panelIdx * 64) + ((slot+1) * 16) + (fntBmpX0 / 8)*16 + line;

        bmp8bit = 0;
        if (fntBmpIdx_l >= 0 && fntBmpIdx_l < MAX_WORD * 2 * 16) bmp8bit |= strBmp[fntBmpIdx_l] << (fntBmpX0 % 8);
        if (fntBmpIdx_r >= 0 && fntBmpIdx_r < MAX_WORD * 2 * 16) bmp8bit |= strBmp[fntBmpIdx_r] >> (8-(fntBmpX0 % 8));
        
        for(int bit=0; bit<8; bit++) {
          int16_t x = (panelIdx * DOT_PANEL_WIDTH) + (slot * 8) + (7 - bit) + panelX0;
          if((bmp8bit & (0b00000001 << bit)) > 0) {
            writePixel(x, line + cursor_y, textcolor);
          } else {
            writePixel(x, line + cursor_y, textbgcolor);
          }
        }
      }
    }
  }
}

/* Print shinonome font */
void Akizuki_32x16_DotLEDMatrixDisplay::shnmFntPrint(String str) {
  shnmFntStrConv(str, fntBmp);

  startWrite();
  shnmFntWrite(fntBmp);
  endWrite();
}

/* Scroling shinonome text font */
void Akizuki_32x16_DotLEDMatrixDisplay::shnmFntScroll(String str, uint8_t speed_ms) {
  
  uint16_t tWidth;
  tWidth = shnmFntStrConv(str, fntBmp);

  for(int x0 = WIDTH; x0 > -(tWidth + WIDTH); x0--) {
    startWrite();
    setCursor(x0, 0);
    shnmFntWrite(fntBmp);
    delay(speed_ms);
    endWrite();
  }
}

/* Draw Bitmap Image for exported by GIMP C-Souce image dump */
/* Usage: 
   背景は黒でレイヤーを統合して1つのレイヤになった状態でCソースコードのエクスポートを選び、
   適当な「前置名」を入力し、「struct ではなくマクロを使う」のみにチェックを入れてエクスポートする。 */
void Akizuki_32x16_DotLEDMatrixDisplay::drawGIMP_CSourceImageDump(int16_t x, int16_t y,
                                  const uint8_t bitmap[], int16_t w, int16_t h) {
  
  startWrite();
  for (int16_t j = 0; j < h; j++, y++) {
    for (int16_t i = 0; i < w; i++) {
      if (bitmap[(j * 3) * w + i * 3 + 0] > 0) writePixel(x + i, y, DOT_RED);
    }
  }
  endWrite();
}
