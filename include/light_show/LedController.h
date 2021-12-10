#ifndef LIGHT_SHOW_LEDCONTROLLER_H
#define LIGHT_SHOW_LEDCONTROLLER_H

#include <string>
#include <stdexcept>

#include "ndarray_fwd.h"

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
    LedController(int gpio, int num, StripType type, unsigned int dma=10);
    LedController(
        ndarray::Array<int, 1, 1> const& gpio,
        ndarray::Array<int, 1, 1> const& num,
        ndarray::Array<StripType, 1, 1> const& types,
        unsigned int dma=10
    );

    ~LedController();

    void render(bool wait=true) const;

    int size() { return _channels.size(); }

    LedStrip const& operator[](int index) const {
        return _channels[checkIndex(index, _channels.size())];
    }
    LedStrip & operator[](int index) {
        return _channels[checkIndex(index, _channels.size())];
    }

    LedStripSet getAll() {
        std::cerr << "Getting all " << _channels.size() << " channels" << std::endl;
        return LedStripSet(_channels);
    }

    void off();

  private:

    void _execute(std::function<ws2811_return_t(ws2811_t*)> func, char const* name) const;

    LedController(unsigned int dma=10) {
        _controller.freq = WS2811_TARGET_FREQ;
        _controller.dmanum = dma;
    }

    void _setChannel(int index, int gpio, int num, StripType type);

    void _start(int numChannels);

    ws2811_t _controller;
    std::vector<LedStrip> _channels;
};


}  // namespace light_show

#endif  // include guard
