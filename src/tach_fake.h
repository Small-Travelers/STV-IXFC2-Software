#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================================
// TACH偽装出力モジュール
// 正常時: 200Hz (6000rpm相当) パルス出力
// 異常時: パルス停止 (0rpm → IX3315がエラー検知)
// LEDCを使用してハードウェアPWM出力
// ============================================================

namespace TachFake {

static bool _running = false;

// パルス出力開始（正常）
void start() {
    if (_running) return;
    ledcWrite(TACH_FAKE_CH_A, TACH_FAKE_DUTY);
    ledcWrite(TACH_FAKE_CH_B, TACH_FAKE_DUTY);
    _running = true;
}

// パルス停止（異常通知）
// LOW固定 → IX3315が0rpmと判断してエラー
void stop() {
    if (!_running) return;
    ledcWrite(TACH_FAKE_CH_A, 0);
    ledcWrite(TACH_FAKE_CH_B, 0);
    _running = false;
}

void setNormal() { start(); }
void setFault()  { stop();  }

bool isRunning() { return _running; }

// begin()はstart()より後に定義（前方参照回避）
void begin() {
    ledcSetup(TACH_FAKE_CH_A, TACH_FAKE_FREQ_HZ, TACH_FAKE_RESOLUTION);
    ledcSetup(TACH_FAKE_CH_B, TACH_FAKE_FREQ_HZ, TACH_FAKE_RESOLUTION);
    ledcAttachPin(PIN_TACH_FAKE_A, TACH_FAKE_CH_A);
    ledcAttachPin(PIN_TACH_FAKE_B, TACH_FAKE_CH_B);

    // 起動直後は出力開始（ファン状態確認前なのでまず正常扱い）
    start();
}

} // namespace TachFake
