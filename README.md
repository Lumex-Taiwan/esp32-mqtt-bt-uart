# esp32-mqtt-bt-uart

## Feature Highlights are:
* Wireless Internet Connection with MQTT Client. 
* Live Configurable through Bluetooth Serial - Wifi(SSID, KEY), MQTT(Server, Topic)
* Bluetooth Serial is able to be turn off through MQTT.
  * Bluetooth connection is enable after esp32 power on.
  * MQTT message "Bluetooth off" or "bluetooth off" will turn Bluetooth connection off.
  * MQTT message "Bluetooth on" or "bluetooth on" will turn Bluetooth connection on.
* Sending TEXT from MQTT to UART connection.

## Don't Forget Change Those Settings

```C
    char* ssid = "wifi_ssid";
    char* password = "wifi_password";
    char* Bluetooth_ssid = "ID of This Device";
    char* mqtt_server = "broker.hivemq.com"; 
```

* Those settings are able to be changed and saved into EEPROM through Bluetooth Serial connection by following command
  * Wifi SSID(ssid) can be changed by inputing "ssid=(your own SSID)"
  * Wifi Password(password) can be changed by inputing "password=(your own password)"
  * Bluetooth device name(Bluetooth_ssid) can be changed by inputing "bluetooth name=(your own Bluetooth ID)"
  *  MQTT server(mqtt_server) can be changed by inputing "broker=(your own broker name)"


### Note:
**PubSubClient library is required.  Please installed this library through "Include Library > Manage Libraries".**
