**System Description – Underground Mine Monitoring System**

The proposed system is an **IoT-based underground mine monitoring and control system** designed to improve safety and operational awareness in hazardous mining environments. The system continuously monitors environmental parameters and transmits the collected data to a central gateway for remote supervision and control.

The system consists of **two sensor nodes based on STM32 microcontrollers** and a **gateway node using ESP32**. Each STM32 node is responsible for collecting data from environmental sensors such as **gas concentration, temperature, humidity, or vibration sensors** installed in different locations inside the mine. After processing the sensor readings, the STM32 nodes transmit the data wirelessly to the gateway using **LoRa communication**, which provides long-range and low-power data transmission suitable for underground environments.

The **ESP32 acts as the central gateway**, receiving data from both STM32 nodes via LoRa. Thanks to its integrated **WiFi capability**, the ESP32 forwards the collected data to a **web-based monitoring platform**. Through the web interface, users can remotely monitor real-time sensor data, observe system status, and send control commands if necessary.

In addition to the web interface, the ESP32 is also connected to a **local LCD display**, which provides real-time visualization of system parameters directly at the monitoring station. This allows operators to quickly check environmental conditions even without accessing the web dashboard.

Overall, the system provides a **reliable real-time monitoring and control solution**, combining **LoRa long-range communication, IoT connectivity, and local display monitoring**. This architecture enhances safety in underground mining operations by enabling early detection of dangerous environmental conditions and allowing remote supervision through a web-based platform.
