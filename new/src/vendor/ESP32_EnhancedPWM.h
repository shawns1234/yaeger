/**
 * ESP32_EnhancedPWM.h
 *
 * Copied from: https://github.com/CastilloDelSol/ESP32_EnhancedPWM
 * with adaptations for the latest Arduino framework.
 *
 * This is a vendor module — do not edit directly.
 */

#ifndef ESP32_ENHANCED_PWM_H
#define ESP32_ENHANCED_PWM_H

#include <Arduino.h>
#include <driver/ledc.h>

/**
 * @brief A wrapper class for the ESP32 LEDC PWM timer.
 *
 * This class extends the standard LEDC functionality by allowing for extended
 * frequency support (including frequencies below 1 Hz) and fractional frequency
 * adjustments using an enhanced manual divider calculation. It encapsulates both
 * timer and channel configurations, making it easier to initialize and control PWM
 * outputs on the ESP32.
 */
class ESP32_EnhancedPWM
{
private:
    ledc_timer_config_t _timerCfg;
    ledc_channel_config_t _channelCfg;
    uint32_t _maxDuty;
    uint8_t _boardPin;
    float _exactFrequency;

public:
    ESP32_EnhancedPWM() : _timerCfg{}, _channelCfg{}, _maxDuty(0), _boardPin(UINT8_MAX), _exactFrequency(-1.0f) {}

    bool begin(const uint8_t pin, const uint8_t channel, const float frequency,
               const uint8_t resolution = 8, const bool isInverted = false,
               const ledc_clk_cfg_t clk = LEDC_USE_APB_CLK);

    void setDuty(uint32_t dutyValue);
    void setDutyAndResetTimer(uint32_t dutyValue);
    void setDutyNormalized(float dutyNormalized);

    uint8_t getResolution() const { return _timerCfg.duty_resolution; }
    uint32_t getFrequency() const { return ledc_get_freq(_channelCfg.speed_mode, _timerCfg.timer_num); }
    float getExactFrequency() const { return _exactFrequency; }
    uint32_t getDuty() const { return _channelCfg.duty; }
    uint32_t getMaxDuty() const { return _maxDuty; }
    uint8_t getBoardPin() const { return _boardPin; }

    void calculate_ledc_timer_divider(uint32_t f_src, float f_out, uint8_t duty_res, uint32_t &A, uint32_t &B);
    uint32_t extract_fixed_point_divider(uint32_t A, uint32_t B);
    float extract_effective_divider(uint32_t A, uint32_t B);
    float get_ledc_real_frequency(uint32_t f_src, uint32_t A, uint32_t B, uint32_t duty_res);

private:
    uint32_t get_clk_source_frequency(ledc_clk_cfg_t clk_cfg);
};

#endif // ESP32_ENHANCED_PWM_H
