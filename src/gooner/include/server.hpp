#pragma once

#include <cstdint>

#define GOONER_PORT 6969

namespace GoonerServer
{
    enum class Action : std::uint8_t
    {
        _NULL,
        OPEN,
        CLOSE,
    };

    class Instance
    {
    public:
        Instance();

        ~Instance();

        void start();

        void shutoff();

    private:
    };
};
