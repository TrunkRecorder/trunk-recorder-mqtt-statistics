# trunk-recorder-mqtt
 Publish status using MQTT


## Install Paho MQTT C++ Library

https://github.com/eclipse/paho.mqtt.cpp#unix-and-linux


```bash
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
git checkout v1.3.8

cmake -Bbuild -H. -DPAHO_ENABLE_TESTING=OFF -DPAHO_BUILD_STATIC=ON  -DPAHO_WITH_SSL=ON -DPAHO_HIGH_PERFORMANCE=ON
sudo cmake --build build/ --target install
sudo ldconfig
```


```bash
git clone https://github.com/eclipse/paho.mqtt.cpp
cd paho.mqtt.cpp

cmake -Bbuild -H. -DPAHO_BUILD_STATIC=ON  -DPAHO_BUILD_DOCUMENTATION=TRUE -DPAHO_BUILD_SAMPLES=TRUE
sudo cmake --build build/ --target install
sudo ldconfig
```


/opt/homebrew/sbin/mosquitto -c /opt/homebrew/etc/mosquitto/mosquitto.conf

https://docs.microsoft.com/en-in/azure/iot-hub/iot-hub-mqtt-support

https://github.com/eclipse/paho.mqtt.c/issues/461#issuecomment-667559214

The user name must look like:
tr-hub.azure-devices.net/test/?api-version=2018-06-30

The password must be a shared access signature in this form (including SharedAccessSignature at the start of the password:
SharedAccessSignature sr=<your-hub-name>.azure-devices.net%2Fdevices%2F<your-device-name>&sig=<sig>%3D&se=<timestamp>

HostName=tr-hub.azure-devices.net;DeviceId=test;SharedAccessKey=bOowp7rtaFkjHxOzhRd6i2Phsn5TjK+FKttFH8Ooea8=

HostName=tr-hub.azure-devices.net;SharedAccessKeyName=device;SharedAccessKey=InfNvi9Jjq6CcehPaIEkHdYJ+xPh8UQh+uT3yId4suw=

devices/test/messages/events/

SharedAccessSignature sr=[yourIoTHub].azure-devices.net%2Fdevices%2F[DeviceId]&sig=[tokengeneratedforyourdevice]
SharedAccessSignature sr=tr-hub.azure-devices.net%2Fdevices%2Ftest&sig=bOowp7rtaFkjHxOzhRd6i2Phsn5TjK+FKttFH8Ooea8=


        //"username": "tr-hub.azure-devices.net/test/?api-version=2018-06-30",
        //"password": "SharedAccessSignature sr=tr-hub.azure-devices.net&sig=GNSotqbn3yyr%2BXK0jC4opd%2BOzNkWIrQWniDKDpSuiu8%3D&se=1653834869&skn=iothubowner"
