#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================================
// TACH 計測モジュール
// 割り込みでパルスをカウントし、1秒窓でRPMを算出
// ============================================================

namespace Tach {

// 各ファンのパルスカウンタ（volatile: ISRから更新）
static volatile uint32_t _pulseCount[4] = {0, 0, 0, 0};
static uint32_t _rpm[4]                 = {0, 0, 0, 0};
static uint32_t _lastMeasureMs          = 0;

// ISRハンドラ (インライン展開でオーバーヘッド最小化)
static void IRAM_ATTR isr0() { _pulseCount[0]++; }
static void IRAM_ATTR isr1() { _pulseCount[1]++; }
static void IRAM_ATTR isr2() { _pulseCount[2]++; }
static void IRAM_ATTR isr3() { _pulseCount[3]++; }

void begin() {
    const uint8_t pins[4] = {
        PIN_FAN1_TACH, PIN_FAN2_TACH,
        PIN_FAN3_TACH, PIN_FAN4_TACH
    };
    void (*isrs[4])() = { isr0, isr1, isr2, isr3 };

    for (int i = 0; i < 4; i++) {
        pinMode(pins[i], INPUT);
        attachInterrupt(digitalPinToInterrupt(pins[i]), isrs[i], FALLING);
    }
    _lastMeasureMs = millis();
}

// 1秒経過していればRPMを更新して返す (true=更新済み)
bool update() {
    uint32_t now = millis();
    if (now - _lastMeasureMs < TACH_MEASURE_MS) return false;

    float elapsed = (now - _lastMeasureMs) / 1000.0f;
    _lastMeasureMs = now;

    for (int i = 0; i < 4; i++) {
        // 割り込み一時停止してカウンタ読み取り
        noInterrupts();
        uint32_t cnt = _pulseCount[i];
        _pulseCount[i] = 0;
        interrupts();

        uint32_t rpm = (uint32_t)((cnt / (float)TACH_PULSES_PER_REV / elapsed) * 60.0f);
        // 異常値フィルタ
        _rpm[i] = (rpm > TACH_MAX_RPM) ? 0 : rpm;
    }
    return true;
}

uint32_t getRpm(uint8_t fanIndex) {
    return (fanIndex < 4) ? _rpm[fanIndex] : 0;
}

// いずれか1台でもエラーならtrue
bool hasError() {
    for (int i = 0; i < 4; i++) {
        if (_rpm[i] < TACH_ERROR_RPM) return true;
    }
    return false;
}

} // namespace Tach
