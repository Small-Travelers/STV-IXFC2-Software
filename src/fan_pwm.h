#pragma once
#include <Arduino.h>
#include "config.h"

// ============================================================
// ファン PWM 制御モジュール
// LEDC 25kHz / 8bit
// ============================================================

namespace FanPwm {

void setDuty(uint8_t ch, uint8_t duty) {
    ledcWrite(ch, duty);
}

void setAll(uint8_t duty) {
    ledcWrite(PWM_CH_FAN1, duty);
    ledcWrite(PWM_CH_FAN2, duty);
    ledcWrite(PWM_CH_FAN3, duty);
    ledcWrite(PWM_CH_FAN4, duty);
}

// 温度からDutyを計算（リニア補間）
uint8_t calcDutyFromTemp(float tempC) {
    if (tempC <= TEMP_PWM_MIN_C) return PWM_DUTY_MIN;
    if (tempC >= TEMP_PWM_MAX_C) return PWM_DUTY_MAX;

    float ratio = (tempC - TEMP_PWM_MIN_C) / (TEMP_PWM_MAX_C - TEMP_PWM_MIN_C);
    return (uint8_t)(PWM_DUTY_MIN + ratio * (PWM_DUTY_MAX - PWM_DUTY_MIN));
}

// begin()はsetAll()より後に定義（前方参照回避）
void begin() {
    ledcSetup(PWM_CH_FAN1, PWM_FREQ_HZ, PWM_RESOLUTION);
    ledcSetup(PWM_CH_FAN2, PWM_FREQ_HZ, PWM_RESOLUTION);
    ledcSetup(PWM_CH_FAN3, PWM_FREQ_HZ, PWM_RESOLUTION);
    ledcSetup(PWM_CH_FAN4, PWM_FREQ_HZ, PWM_RESOLUTION);

    ledcAttachPin(PIN_FAN1_PWM, PWM_CH_FAN1);
    ledcAttachPin(PIN_FAN2_PWM, PWM_CH_FAN2);
    ledcAttachPin(PIN_FAN3_PWM, PWM_CH_FAN3);
    ledcAttachPin(PIN_FAN4_PWM, PWM_CH_FAN4);

    setAll(PWM_DUTY_DEFAULT);
}

} // namespace FanPwm
