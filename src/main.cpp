#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "fan_pwm.h"
#include "tach.h"
#include "tach_fake.h"
#include "temp_sensor.h"
#include "serial_output.h"

// ============================================================
// IX3315 Fan Controller Firmware
// Board   : ESP32-DevKitC V4
// Version : 1.0.0
// ============================================================

// ------ 状態変数 ------
static bool    g_fault       = false;
static float   g_tempC       = TEMP_ERROR_VALUE;
static uint8_t g_pwmDuty     = PWM_DUTY_DEFAULT;

// ------ LED2制御 ------
static void setFaultLed(bool fault) {
    // Active LOW: fault=true → LOW(点灯)
    digitalWrite(PIN_LED2, fault ? LOW : HIGH);
}

// ------ セットアップ ------
void setup() {
    // ウォッチドッグタイマー設定
    esp_task_wdt_init(WDT_TIMEOUT_S, true);
    esp_task_wdt_add(NULL);

    // LED2初期化（起動時はOFF）
    pinMode(PIN_LED2, OUTPUT);
    setFaultLed(false);

    // 各モジュール初期化
    SerialOutput::begin();
    FanPwm::begin();
    Tach::begin();
    TachFake::begin();
    TempSensor::begin();

    Serial.println(F("# Setup complete"));
    if (!TempSensor::isAvailable()) {
        Serial.println(F("# WARN: DS18B20 not found"));
    }
}

// ------ メインループ ------
void loop() {
    esp_task_wdt_reset();

    // --- TACH計測更新（1秒毎） ---
    if (Tach::update()) {
        uint32_t rpm1 = Tach::getRpm(0);
        uint32_t rpm2 = Tach::getRpm(1);
        uint32_t rpm3 = Tach::getRpm(2);
        uint32_t rpm4 = Tach::getRpm(3);

        // フォルト判定: 1台でもエラーRPM以下
        g_fault = Tach::hasError();

        // TACH偽装出力更新
        if (g_fault) {
            TachFake::setFault();   // パルス停止→IX3315がエラー検知
        } else {
            TachFake::setNormal();  // 200Hz出力継続
        }

        // LED2更新
        setFaultLed(g_fault);
    }

    // --- 温度センサー更新（5秒毎） ---
    if (TempSensor::update()) {
        g_tempC = TempSensor::getTemp();

        // 温度に応じてPWM Duty更新
        if (!TempSensor::isError()) {
            g_pwmDuty = FanPwm::calcDutyFromTemp(g_tempC);
        } else {
            // 温度読み取り失敗時はデフォルトDutyで継続
            g_pwmDuty = PWM_DUTY_DEFAULT;
        }
        FanPwm::setAll(g_pwmDuty);
    }

    // --- シリアル出力（5秒毎） ---
    SerialOutput::update(
        g_tempC,
        Tach::getRpm(0), Tach::getRpm(1),
        Tach::getRpm(2), Tach::getRpm(3),
        TachFake::isRunning(),
        g_fault
    );

    // CPUを少し休ませる（ウォッチドッグリセット間隔を保証）
    delay(10);
}
