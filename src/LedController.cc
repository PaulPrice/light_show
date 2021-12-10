#include "light_show/LedController.h"



#ifndef RASPBERRY_PI
// Dummy replacements for the controller
#define RPI_PWM_CHANNELS 2

ws2811_return_t ws2811_init(ws2811_t *controller) {
    std::cerr << "ws2811: Initializing" << std::endl;
    for (std::size_t ii = 0; ii < RPI_PWM_CHANNELS; ++ii) {
        std::size_t const num = controller->channel[ii].count;
        if (num > 0) {
            std::cerr << "Allocating " << num << " pixels for channel " << ii << std::endl;
            controller->channel[ii].leds = reinterpret_cast<light_show::ColorPixel *>(
                new light_show::Pixel[4*controller->channel[ii].count]);
        }
    }
    return WS2811_SUCCESS;
}

void ws2811_fini(ws2811_t *controller) {
    std::cerr << "ws2811: finalizing" << std::endl;
    for (std::size_t ii = 0; ii < RPI_PWM_CHANNELS; ++ii) {
        if (controller->channel[ii].count > 0) {
            delete[] controller->channel[ii].leds;
        }
    }
}

ws2811_return_t ws2811_render(ws2811_t *) {
    std::cerr << "ws2811: rendering" << std::endl;
    return WS2811_SUCCESS;
}

ws2811_return_t ws2811_wait(ws2811_t *) {
    std::cerr << "ws2811: waiting" << std::endl;
    return WS2811_SUCCESS;
}

const char * ws2811_get_return_t_str(const ws2811_return_t) {
    return "ws2811: error message";
}

#endif  // RASPBERRY_PI


namespace light_show {


LedController::LedController(
    int gpio,
    int num,
    StripType type,
    unsigned int dma
) : LedController(dma) {
    _setChannel(0, gpio, num, type);
    for (std::size_t ii = 1; ii < RPI_PWM_CHANNELS; ++ii) {
        _setChannel(ii, 0, 0, StripType(0));
    }
    _start(1);
}


LedController::LedController(
    ndarray::Array<int, 1, 1> const& gpio,
    ndarray::Array<int, 1, 1> const& num,
    ndarray::Array<StripType, 1, 1> const& types,
    unsigned int dma
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


LedController::~LedController() {
    for (int ii = 0; ii < size(); ++ii) {
        _channels[ii].clear();
    }
    render();
    ws2811_fini(&_controller);
}


void LedController::render(bool wait) const {
    _execute(&ws2811_render, "ws2811_render");
    if (wait) {
        _execute(&ws2811_wait, "ws2811_wait");
    }
}


void LedController::off() {
    for (auto & strip : _channels) {
        strip.clear();
    }
    render();
}


void LedController::_execute(std::function<ws2811_return_t(ws2811_t*)> func, char const* name) const {
    ws2811_return_t code = func(const_cast<ws2811_t*>(&_controller));
    if (code != WS2811_SUCCESS) {
        throw Ws2811Exception(std::string(name), code);
    }
}


void LedController::_setChannel(int index, int gpio, int num, StripType type) {
    assert(index >= 0 && index < RPI_PWM_CHANNELS);
    if (num < 0) {
        throw std::length_error("Negative length specified");
    }
    _controller.channel[index].gpionum = gpio;
    _controller.channel[index].count = num;
    _controller.channel[index].invert = 0;
    _controller.channel[index].brightness = 255;
    _controller.channel[index].strip_type = type;
    _controller.channel[index].leds = nullptr;
    _controller.channel[index].gamma = nullptr;
}


void LedController::_start(int numChannels) {
    _execute(ws2811_init, "ws2811_init");
    _channels.reserve(numChannels);
    for (int ii = 0; ii < numChannels; ++ii) {
        _channels.emplace_back(_controller.channel[ii].count, _controller.channel[ii].leds);
    }
}


}  // namespace light_show
