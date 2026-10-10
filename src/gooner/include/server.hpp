#pragma once

#define GOONER_PORT 6969

namespace GoonerServer
{
    enum Action
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
