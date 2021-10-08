#ifndef LIGHT_SHOW_LEDCONTROLLER_H
#define LIGHT_SHOW_LEDCONTROLLER_H

#include <string>
#include <stdexcept>
#include "light_show/config.h"
#include "light_show/LedStrip.h"

extern "C" {
#include "ws2811.h"
}

namespace light_show {


enum StripType {
    RGBW = SK6812_STRIP_RGBW,
    RBGW = SK6812_STRIP_RBGW,
    GRBW = SK6812_STRIP_GRBW,
    GBRW = SK6812_STRIP_GBRW,
    BRGW = SK6812_STRIP_BRGW,
    BGRW = SK6812_STRIP_BGRW,
    RGB = WS2811_STRIP_RGB,
    RBG = WS2811_STRIP_RBG,
    GRB = WS2811_STRIP_GRB,
    GBR = WS2811_STRIP_GBR,
    BRG = WS2811_STRIP_BRG,
    BGR = WS2811_STRIP_BGR,
};


class Ws2811Exception : std::runtime_error {
    Ws2811Exception(ws2811_return_t code) : std::runtime_error(ws2811_get_return_t_str(code)) {}
    Ws2811Exception(std::string const& descr, ws2811_return_t code
        ) : std::runtime_error(descr + ": " + ws2811_get_return_t_str(code)) {}
};


class LedController final {
  public:
    LedController(int gpio, int num, StripType type, unsigned int dma=10
        ) : LedController(dma) {
        _setChannel(0, gpio, num, type);
        _start(1);
    }

    LedController(
        ndarray::Array<int, 1, 1> const& gpio,
        ndarray::Array<int, 1, 1> const& num,
        ndarray::Array<StripType, 1, 1> const& types,
        unsigned int dma=10
    ) : LedController(dma) {
        if (gpio.size() != num.size() || gpio.size() != types.size()) {
            throw std::length_error("Length mismatch");
        }
        if (gpio.size() > RPI_PWM_CHANNELS) {
            throw std::length_error("Too many channels");
        }
        for (std::size_t ii = 0; ii < gpio.size(); ++ii) {
            _setChannel(ii, gpio[ii], num[ii], types[ii]);
        }
        _start(gpio.size());
    }

    ~LedController() {
        ws2811_fini(&_leds);
    }

    void render(bool wait=true) const {
        _execute(&ws2811_render, "ws2811_render");
        if (wait) {
            _execute(&ws2811_wait, "ws2811_wait");
        }
    }

    int size() { return _channels.size(); }

    LedStrip & operator[](int index) const {
        if (index < 0) {
            index += _channels.size();
        }
        if (index < 0 || index >= _channels.size()) {
            throw std::out_of_range("Index out of range");
        }
        return _channels[ii];
    }

  private:

    void _execute(void(ws2811_t*) func, char const* name) {
        ws2811_return_t code = func(&_leds);
        if (code != WS2811_SUCCESS) {
            throw Ws2811Exception(std::string(name), code);
        }
    }

    explicit LedController(unsigned int dma=10) {
        _leds.freq = WS2811_TARGET_FREQ;
        _leds.dma = dma;
    }

    void _setChannel(int index, int gpio, int num, StripType type) {
        assert(index >= 0 && index < RPI_PWM_CHANNELS);
        if (num < 0) {
            throw std::length_error("Negative length specified");
        }
        _leds.channel[index].gpionum = gpio;
        _leds.channel[index].count = num;
        _leds.channel[index].brightness = 255;
        _leds.channel[index].strip_type = type;
    }

    void _start(int numChannels) {
        _execute(ws2811_init, "ws2811_init");
        for (int ii = 0; ii < numChannels; ++ii) {
            _channels.emplace_back(_leds.channel[ii].count, _leds.channel[ii].leds);
        }
    }

    ws2811_t _leds;
    std::vector<LedStrip> _channels;
};


}  // namespace light_show

#endif  // include guard
