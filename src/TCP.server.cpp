#include <TCP.server.hpp>

using namespace TCP;

/**
 * @brief Used to pass data to thread
*/
struct AccessPointdata
{
    const std::string SSID;
    const std::string Password;
};

static void SetupWiFiEvents()
{   
    esp_event_handler_instance_t SPAstartInstance;
    esp_event_handler_instance_t GotIPinstance;

    esp_event_handler_instance_register(WIFI_EVENT, WIFI_EVENT_STA_START,
    [](void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
    {
        esp_wifi_connect();
    }, NULL, &SPAstartInstance);
    
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
    [](void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("got ip: %08X\n", event->ip_info.ip.addr);
    }, NULL, &GotIPinstance);
}

static void ConfigureWiFi(struct AccessPointdata AP)
{
    
    wifi_config_t WiFiConfig;
    memcpy(WiFiConfig.sta.ssid, AP.SSID.c_str(), AP.SSID.length());
    WiFiConfig.sta.ssid[AP.SSID.length()] = 0;
    memcpy(WiFiConfig.sta.password, AP.Password.c_str(), AP.Password.length());
    WiFiConfig.sta.password[AP.Password.length()] = 0;
    WiFiConfig.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    printf("Will try to connect to: %s with %s\n", WiFiConfig.sta.ssid, WiFiConfig.sta.password);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &WiFiConfig);
    esp_wifi_start();
}

void TCPWifiClient(void* pvAccessPointData)
{
    struct AccessPointdata APdat = *(struct AccessPointdata*)pvAccessPointData;

    printf("Get parameters %s(%i), %s(%i)\n", APdat.SSID.c_str(), APdat.SSID.length(), APdat.Password.c_str(),
    APdat.Password.length());

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_t *NETif = esp_netif_create_default_wifi_sta();
    assert(NETif);

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&config);

    SetupWiFiEvents();
    ConfigureWiFi(APdat);

    while(true)
    {

    }

}

Client::Client(std::string SSID, std::string Password) :
    SSID(SSID), Password(Password)
{
    struct AccessPointdata APdat = {SSID, Password};
    xTaskCreate(TCPWifiClient, "TCPWifiClient", 4096, &APdat, 0, NULL);
}