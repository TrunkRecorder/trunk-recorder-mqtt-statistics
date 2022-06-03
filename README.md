# Trunk Recorder MQTT Statistics Plugin

This is a plugin for Trunk Recorder that publish the current statistics over MQTT. External programs can use the MQTT messages to display what is going on.

## Install

1. **Build and install the current version of Trunk Recorder** following these [instructions](https://github.com/robotastic/trunk-recorder/blob/master/docs/INSTALL-LINUX.md). Make sure you do a `sudo make install` at the end to install the Trunk Recorder binary and libaries systemwide. The plugin will be built against these libraries.

2. Now, **install the Paho MQTT C & C++ Libraries**. The full documentation for that is [here](https://github.com/eclipse/paho.mqtt.cpp#unix-and-linux)... but the basic commands are as follows:

*Install Paho MQTT C*
```bash
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.8

cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON  -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
```

*Install Paho MQTT C++*
```bash
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp

cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON  -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
```

3. Build and install the plugin:

```bash
mkdir build
cd build
cmake ..
sudo make install
```

## Configure

| Key       | Required | Default Value | Type   | Description                                                  |
| --------- | :------: | ------------- | ------ | ------------------------------------------------------------ |
| broker    |    ✓     |   tcp://localhost:1883            | string | The URL for the MQTT Message Broker. It should include the protocol used: **tcp**, **ssl**, **ws**, **wss** and the port, which is generally 1883 for tcp, 8883 for ssl, and 443 for ws. |
| topic     |    ✓     |               | string | This is the base topic to use. The plugin will create subtopics for the different types of status messages. |
| username  |          |               | string | If a username is required for the broker, add it here. |
| password  |          |               | string | If a password is required for the broker, add it here. |



### Plugin Object Example
See the included [config.json](./config.json) as an example of how to load this plugin.

```yaml
    "plugins": [
    {
        "name": "example plugin",
        "library": "libmqtt_plugin.so",
        "broker": "tcp://io.adafruit.com:1883",
        "topic": "robotastic/feeds",
        "username": "robotastic",
        "password": "" 
    }]
```




### Mosquitto MQTT Broker
This should work with any MQTT Broker, but I have been testing locally with Mosquitto.
The Mosquitto MQTT is an easy way to have a local MQTT broker. It can be installed from a lot of package managers. 


Starting it on a Mac:
```bash
/opt/homebrew/sbin/mosquitto -c /opt/homebrew/etc/mosquitto/mosquitto.conf
```