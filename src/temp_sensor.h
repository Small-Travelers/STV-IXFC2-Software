#pragma once
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// ============================================================
// DS18B20 温度センサーモジュール (1-Wire)
// ============================================================

namespace TempSensor {

static OneWire          _ow(PIN_DS18B20);
static DallasTemperature _dt(&_ow);
static float            _lastTemp    = TEMP_ERROR_VALUE;
static uint32_t         _lastReadMs  = 0;
static bool             _available   = false;

void begin() {
    _dt.begin();
    _available = (_dt.getDeviceCount() > 0);
    if (_available) {
        _dt.setResolution(12);      // 12bit精度 (0.0625℃)
        _dt.setWaitForConversion(false); // 非同期変換
        _dt.requestTemperatures();  // 最初の変換リクエスト
    }
}

// 5秒毎に温度を更新 (true=更新あり)
bool update() {
    if (!_available) return false;

    uint32_t now = millis();
    if (now - _lastReadMs < TEMP_READ_MS) return false;
    _lastReadMs = now;

    float t = _dt.getTempCByIndex(0);
    if (t != DEVICE_DISCONNECTED_C && t > -50.0f && t < 100.0f) {
        _lastTemp = t;
    } else {
        _lastTemp = TEMP_ERROR_VALUE;
    }

    // 次回変換リクエスト
    _dt.requestTemperatures();
    return true;
}

float getTemp()       { return _lastTemp; }
bool  isAvailable()   { return _available; }
bool  isError()       { return (_lastTemp <= TEMP_ERROR_VALUE + 1.0f); }

} // namespace TempSensor
