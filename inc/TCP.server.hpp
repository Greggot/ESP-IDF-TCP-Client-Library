#pragma once

#include "esp_wifi.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#include <string>

// #define TCPdebug

namespace TCP
{
    class Client
    {
        private:
            wifi_config_t WiFiConfig;
            const std::string SSID, Password;
        public:
            Client(std::string SSID, std::string Password);
    }; 
}