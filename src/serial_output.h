#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================================
// シリアル出力モジュール
// 既存サーバーへのUSBシリアル経由モニタリング出力
// フォーマット: CSV (パース容易)
// ============================================================

namespace SerialOutput {

static uint32_t _lastOutputMs = 0;

void begin() {
    Serial.begin(SERIAL_BAUD);
    // ヘッダ出力
    Serial.println(F("# IX3315 Fan Controller - ESP32-DevKitC"));
    Serial.println(F("# timestamp_ms,temp_c,fan1_rpm,fan2_rpm,fan3_rpm,fan4_rpm,tach_fake,fault"));
}

// 5秒毎に出力 (true=出力あり)
bool update(float tempC,
            uint32_t rpm1, uint32_t rpm2,
            uint32_t rpm3, uint32_t rpm4,
            bool tachRunning, bool fault) {

    uint32_t now = millis();
    if (now - _lastOutputMs < SERIAL_OUTPUT_MS) return false;
    _lastOutputMs = now;

    // CSV行出力
    Serial.print(now);
    Serial.print(',');

    if (tempC > TEMP_ERROR_VALUE + 1.0f) {
        Serial.print(tempC, 1);
    } else {
        Serial.print(F("ERR"));
    }

    Serial.print(','); Serial.print(rpm1);
    Serial.print(','); Serial.print(rpm2);
    Serial.print(','); Serial.print(rpm3);
    Serial.print(','); Serial.print(rpm4);
    Serial.print(','); Serial.print(tachRunning ? F("RUN") : F("STOP"));
    Serial.print(','); Serial.println(fault ? F("FAULT") : F("OK"));

    return true;
}

} // namespace SerialOutput
