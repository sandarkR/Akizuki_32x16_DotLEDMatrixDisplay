# Akizuki_32x16_DotLEDMatrixDisplay
これは、MicroSDソケット付きのESP32開発ボード向けに、秋月電子通商にて販売されていたドットマトリクスLEDを制御するArduinoライブラリです。

Adafruit GFX Libraryに基づいて、日本語表示を実装しました。複数枚並べたり、文字をスクロールしたり、GIMPを使って作った画像を表示したり、その他のグラフィック表示についても、動作を確認したメソッドのサンプルスケッチがあります。



# Get the Panel
- 2024年の年末に秋月電子通商の秋葉原店にて「店舗限定アウトレットセール」として販売されていて、そこで入手しましたが、すでに無いかもしれません。[秋葉原店 2階にて32×16ドットマトリクスLED表示装置パーツセットを1枚200円で発売中！](https://x.com/akizuki_akiba/status/1820287909226070373)

- 上記リンクの参考PDF（p.10～13）のQRコードのリンク： [LED 電光掲示板ボードキット](https://akizukidenshi.com/img/contents/kairo/%E3%83%87%E3%83%BC%E3%82%BF/%E8%A1%A8%E7%A4%BA%E8%A3%85%E7%BD%AE/LED%E9%9B%BB%E5%85%89%E6%8E%B2%E7%A4%BA.pdf)

- その後、2025/5/25に八潮店の公式Xで入荷のポストがありましたが、その後は不明です...->[八潮店に電光掲示板キットが入荷！](https://x.com/akizuki_yashio/status/1926465607211331780)

- もともとは1枚2,800円くらいだったと記憶していますが、高いので手が出せなかったのを覚えています...それでセールの時には「ほぼ確実にドット抜けがある」とあり、1枚たった200円で販売されていました。ところが、Xへの様々な方の投稿を見ると不良は無いようで、実際に6枚ほど使ってみましたが、確かにドット抜けがあるものはありません。八潮店で販売されている店内デモの動画にはドット抜けがあるのですが...



# Usage
- このライブラリは、MicroSDソケット付きのESP32開発ボード用に作っています。
  - 以前、AliExpress内に販売されていました。数はそれほど多くなかったですが、さらに減って今はどこにも売られていません（2025/6/29時点）。
  - というわけで開発ボードも作りましたが、今のところイベント等のみで販売しています。
  - よく売ってあるESP32の開発ボードにSDカードモジュールを接続しても使えると思いますが、動作未確認です。後述のピンアサインを参考にしてください。

- ブレイクアウト基板も作り（これもキット化してイベント等で販売）、余ったGPIOのなかで、入力専用の所にはスイッチの入力（4つ）を付けたり、入出力可能な7つのピンが残っています。良く売られているESP32開発ボードに、MicroSD or SDカードモジュールを接続しても使えると思いますが、動作は未保証です。

- スイッチを付けなくともマトリクスLEDのみを制御するサンプルスケッチもあります。

- このライブラリをダウンロードし、Arduino IDEの"スケッチ"->"ライブラリを管理"->「.ZIP形式のライブラリをインストール」からインストールできます。

- Arduino IDE 1.8.13およびボードマネージャの"esp32"の1.0.6にて動作確認済みです。

- 日本語表示をしない限りはMicroSDカードは必要ありませんが、日本語表示を行うにはMicroSDカードが必要です。ただし、どちらの場合でも、次のライブラリもインストールしてください。
  - [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
  - [ESP32_SD_ShinonomeFNT](https://github.com/mgo-tec/ESP32_SD_ShinonomeFNT)
  - [ESP32_SD_UTF8toSJIS](https://github.com/mgo-tec/ESP32_SD_UTF8toSJIS)



## Pin Assign

| ESP3 | LEDマトリクスパネル |
| :--: | :--: |
| -- | 1: (LED_PWR) |
| 27 | 2: SIN_1_PIN |
| 16 | 3: SIN_2_PIN |
| 33 | 4: SIN_3_PIN |
| 17 | 5: CLOCK_PIN |
| 25 | 6: LATCH_PIN |
| 26 | 7: STROBE_PIN |
| -- | 8: (IC_PWR) |
| -- | 9: (GND) |
| -- | 10: (GND) |

| ESP32 | SDカード |
| :--: | :--: |
| 13 | SD_CS    |
| 14 | SD_CLK  |
|  2 | SD_MISO |
| 15 | SD_MOSI |

| ESP32 | スイッチ |
| :--: | :--: |
| 36 | SW_1 |
| 39 | SW_2 |
| 35 | SW_3 |
| 34 | SW_4 |

※スイッチはすべてプルダウン。
 


# Library samples
- basicExample  
このライブラリで使える全てのメソッドの使用例です。

- methodsCompatibleAdafruitGFX  
Adafruit GFX Libraryにあるメソッドで動作確認済みのメソッドの使用例です。  
参考：[Adafruit GFX Library](http://adafruit.github.io/Adafruit-GFX-Library/html/index.html)

- PortableElectricBulletinBoard  
タクトスイッチ（プルダウン）やスライドスイッチ、スマホからの制御などいろいろ詰め込んだ例です。iPhone6,8,14にてSafariやChromeを使って制御画面を開けることを確認しました。下に書くブレイクアウト基板がないと、動作確認は少しめんどうかも...



# Reference
このライブライリは、いくつかのライブラリや公開されているプログラムを組み込みまたは使っています。

- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [ESP32_SD_ShinonomeFNT](https://github.com/mgo-tec/ESP32_SD_ShinonomeFNT)
- [ESP32_SD_UTF8toSJIS](https://github.com/mgo-tec/ESP32_SD_UTF8toSJIS)
- [ESP32_SD_EasyWebSocket](https://github.com/mgo-tec/ESP32_SD_EasyWebSocket)※一部のサンプルスケッチのみに使用。



# Breakout board
キット化してNT金沢2025にて初販売。今後もイベントで販売する予定ですが、詳細は未定です。



# History
- Ver. 1.0.0  ライブラリ公開。
- Ver. 1.0.1  iPhone8, 14で動作確認できたことをReadmeに追記。