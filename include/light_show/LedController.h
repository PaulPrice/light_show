#ifndef LIGHT_SHOW_LEDCONTROLLER_H
#define LIGHT_SHOW_LEDCONTROLLER_H

#include <string>
#include <stdexcept>
#include "light_show/config.h"
#include "light_show/indexing.h"
#include "light_show/LedStrip.h"
#include "light_show/LedStripSet.h"

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


class Ws2811Exception : public std::runtime_error {
  public:
    Ws2811Exception(ws2811_return_t code) : std::runtime_error(ws2811_get_return_t_str(code)) {}
    Ws2811Exception(std::string const& descr, ws2811_return_t code
        ) : std::runtime_error(descr + ": " + ws2811_get_return_t_str(code)) {}
};


class LedController final {
  public:
    LedController(int gpio, int num, StripType type, unsigned int dma=10
        ) : LedController(dma) {
        _setChannel(0, gpio, num, type);
        for (std::size_t ii = 1; ii < RPI_PWM_CHANNELS; ++ii) {
            _setChannel(ii, 0, 0, StripType(0));
        }
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
        ws2811_fini(&_controller);
    }

    void render(bool wait=true) const {
        _execute(&ws2811_render, "ws2811_render");
        if (wait) {
            _execute(&ws2811_wait, "ws2811_wait");
        }
    }

    int size() { return _channels.size(); }

    LedStrip const& operator[](int index) const {
        return _channels[checkIndex(index, _channels.size())];
    }
    LedStrip & operator[](int index) {
        return _channels[checkIndex(index, _channels.size())];
    }

    LedStripSet getAll() {
        return LedStripSet(_channels);
    }

  private:

    void _execute(std::function<ws2811_return_t(ws2811_t*)> func, char const* name) const {
        ws2811_return_t code = func(const_cast<ws2811_t*>(&_controller));
        if (code != WS2811_SUCCESS) {
            throw Ws2811Exception(std::string(name), code);
        }
    }

    LedController(unsigned int dma=10) {
        _controller.freq = WS2811_TARGET_FREQ;
        _controller.dmanum = dma;
    }

    void _setChannel(int index, int gpio, int num, StripType type) {
        assert(index >= 0 && index < RPI_PWM_CHANNELS);
        if (num < 0) {
            throw std::length_error("Negative length specified");
        }
        _controller.channel[index].gpionum = gpio;
        _controller.channel[index].count = num;
        _controller.channel[index].invert = 0;
        _controller.channel[index].brightness = 255;
        _controller.channel[index].strip_type = type;
    }

    void _start(int numChannels) {
        _execute(ws2811_init, "ws2811_init");
        _channels.reserve(numChannels);
        for (int ii = 0; ii < numChannels; ++ii) {
            _channels.emplace_back(_controller.channel[ii].count, _controller.channel[ii].leds);
        }
    }

    ws2811_t _controller;
    std::vector<LedStrip> _channels;
};


}  // namespace light_show

#endif  // include guard
