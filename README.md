# IX3315 Fan Controller (STV-IXFC2) Firmware

- これは、ESP32-DevKitC V4 (ESP32-WROOM-32E)を用いて開発したIX3315 向けファン制御ファームウェアです。
- ハードウェア構成はこのリポジトリか、別のリポジトリで公開するかもしれませんが、いまはありません。
- また、本ドキュメントはClaudeが作成しました。

## 検証済ハードウェア構成

| 部品 | 型番 |
|------|------|
| MCU | ESP32-DevKitC V4 |
| 電源 | OKI-78SR-5/1.5-W36E-C (12V→5V) |
| ファン | Noctua NF-A4x20 PWM ×2 (GroupA) / NF-A4x10 PWM ×2 (GroupB) |
| 温度センサー | DS18B20 (1-Wire) |

## GPIOアサイン

| 用途 | GPIO | DevKitCピン |
|------|------|-------------|
| Fan1 PWM出力 | GPIO14 | J2-12 |
| Fan2 PWM出力 | GPIO27 | J2-11 |
| Fan3 PWM出力 | GPIO26 | J2-10 |
| Fan4 PWM出力 | GPIO25 | J2-9 |
| Fan1 TACH入力 | GPIO4 | J3-13 |
| Fan2 TACH入力 | GPIO5 | J3-10 |
| Fan3 TACH入力 | GPIO18 | J3-9 |
| Fan4 TACH入力 | GPIO19 | J3-8 |
| TACH偽装A → IX J7 | GPIO32 | J2-7 |
| TACH偽装B → IX J8 | GPIO33 | J2-8 |
| DS18B20 1-Wire | GPIO21 | J3-6 |
| LED2 (ファン異常) | GPIO22 | J3-3 |

## ファイル構成

```
ix3315_firmware/
├── platformio.ini      # PlatformIO設定
└── src/
    ├── main.cpp        # メインループ
    ├── config.h        # 全定数・設定値
    ├── fan_pwm.h       # ファンPWM制御 (LEDC 25kHz)
    ├── tach.h          # TACH計測 (割り込みカウント)
    ├── tach_fake.h     # TACH偽装出力 (200Hz / 停止)
    ├── temp_sensor.h   # DS18B20温度センサー (1-Wire)
    └── serial_output.h # シリアルCSV出力
```
## 書き込み前に

`platformio.ini`に追記が必要です。追記する場所は次の2つ。
```
upload_port = ESP32のCOMポート (例：COM3)
monitor_port = ESP32のCOMポート　(例：COM3)
```
この通り、ESP32のCOMポートを追記してあげてください。分からない場合はデバイスマネージャから確認できます（Windowsの場合）。

## 書き込み方法

PlatformIO IDE (VSCode拡張) または PlatformIO CLI を使用

```bash
# ビルド
pio run

# 書き込み
pio run --target upload

# シリアルモニタ
pio device monitor --baud 115200
```

## シリアル出力フォーマット

```
# IX3315 Fan Controller - ESP32-DevKitC
# timestamp_ms,temp_c,fan1_rpm,fan2_rpm,fan3_rpm,fan4_rpm,tach_fake,fault
1234,42.5,3200,3180,4100,4090,RUN,OK
2234,42.6,3210,3185,4110,4095,RUN,OK
```

| フィールド | 説明 |
|-----------|------|
| timestamp_ms | 起動からのミリ秒 |
| temp_c | CPU付近温度 (℃)、読み取り失敗時は `ERR` |
| fan1〜4_rpm | 各ファン回転数 (rpm) |
| tach_fake | `RUN`=偽装パルス出力中 / `STOP`=パルス停止(異常) |
| fault | `OK`=正常 / `FAULT`=いずれかのファンがエラー |

## PWM制御ロジック

温度に応じてリニア補間でDutyを制御します。

| 温度 | Duty |
|------|------|
| 35℃以下 | 20% (Noctua推奨最低Duty) |
| 35〜60℃ | 線形増加 |
| 60℃以上 | 100% |

設定値は `config.h` の以下の定数で変更可能です。
- `TEMP_PWM_MIN_C` / `TEMP_PWM_MAX_C`
- `PWM_DUTY_MIN` / `PWM_DUTY_MAX`

## TACH偽装ロジック

| 状態 | GPIO32/33出力 | IX3315の認識 |
|------|-------------|-------------|
| 全Fan正常 | 200Hz パルス (50% duty) | 6000rpm → 正常 |
| 1台以上異常 | LOW固定 | 0rpm → エラー通知 |

## 注意事項

- DS18B20が未接続の場合、温度エラーとなりPWMはデフォルト50%で動作します
- ウォッチドッグタイマーが30秒でリセットします（`config.h` の `WDT_TIMEOUT_S` で変更可能）
- TACH計測は割り込みを使用しています。IRAM_ATTRにより高速化しています

## 本ファームウェアの活用について
- 本ファームウェアはApache-2.0 Licenceに基づいて配布されています。
- ライセンスに基づいてご自由に活用してください。
- なお、制作を行いましたSmall Travelersにこれのサポートは期待しないでください。

## お問い合わせは・・・
- 重大な欠陥などがありましたら、Small Travelers事務局までご連絡ください。
  - admin@smalltravelers.jp
