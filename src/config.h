#pragma once

// ============================================================
// IX3315 Fan Controller Firmware
// Board  : ESP32-DevKitC V4 (ESP32-WROOM-32E)
// Power  : OKI-78SR-5/1.5-W36E-C (12V→5V)
// ============================================================

// ------ GPIO アサイン ------
#define PIN_FAN1_PWM    14
#define PIN_FAN2_PWM    27
#define PIN_FAN3_PWM    26
#define PIN_FAN4_PWM    25

#define PIN_FAN1_TACH    4
#define PIN_FAN2_TACH    5
#define PIN_FAN3_TACH   18
#define PIN_FAN4_TACH   19

#define PIN_TACH_FAKE_A 32   // IX J7 PIN3
#define PIN_TACH_FAKE_B 33   // IX J8 PIN3

#define PIN_DS18B20     21
#define PIN_LED2        22   // Active LOW: ファン異常表示

// ------ PWM 設定 ------
#define PWM_FREQ_HZ     25000   // 25kHz (Noctua PWM規格)
#define PWM_RESOLUTION  8       // 8bit = 0〜255
#define PWM_CH_FAN1     0
#define PWM_CH_FAN2     1
#define PWM_CH_FAN3     2
#define PWM_CH_FAN4     3

// ------ TACH偽装設定 ------
// 6000rpm = 200Hz (2パルス/回転 → 6000/60*2 = 200Hz)
#define TACH_FAKE_FREQ_HZ   200
#define TACH_FAKE_RESOLUTION 8
#define TACH_FAKE_CH_A      4   // LEDCチャンネル4
#define TACH_FAKE_CH_B      5   // LEDCチャンネル5
#define TACH_FAKE_DUTY      128 // 50% duty (255/2)

// ------ TACH 計測設定 ------
#define TACH_MEASURE_MS     1000    // 計測窓 1秒
#define TACH_PULSES_PER_REV 2       // Noctua: 2パルス/回転
#define TACH_ERROR_RPM      200     // これ以下でエラー判定 (rpm)
#define TACH_MAX_RPM        10000   // これ以上は異常値として無視

// ------ 温度センサー設定 ------
#define TEMP_READ_MS        5000    // 5秒毎に読み取り
#define TEMP_ERROR_VALUE    -127.0f // DS18B20エラー値

// ------ PWM制御設定 ------
// 温度に応じたPWM自動制御（簡易リニア補間）
#define TEMP_PWM_MIN_C      35.0f   // この温度以下でPWM最小
#define TEMP_PWM_MAX_C      60.0f   // この温度以上でPWM最大
#define PWM_DUTY_MIN        51      // 20% (Noctua推奨最低Duty)
#define PWM_DUTY_MAX        255     // 100%
#define PWM_DUTY_DEFAULT    128     // 温度読み取り失敗時のデフォルト (50%)

// ------ シリアル出力設定 ------
#define SERIAL_BAUD         115200
#define SERIAL_OUTPUT_MS    5000    // 5秒毎にシリアル出力

// ------ ウォッチドッグ設定 ------
#define WDT_TIMEOUT_S       30      // 30秒でリセット
