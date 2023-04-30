# phyphoxと連携したESP32の距離センサー

* HC-SR04を使って測定した距離を、phyphoxライブラリを使ってBLE通信でスマートフォン等にインストールされているphyphoxアプリに送信します。

```
// ピン設定
int Echo = 14; // Echoピン
int Trig = 15; // Trigピン
```

