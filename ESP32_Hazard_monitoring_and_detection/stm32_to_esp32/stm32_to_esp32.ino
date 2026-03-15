#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Tien";
const char* password = "tien12345";
const String FIREBASE_HOST = "https://hazard-monitoring-detection-default-rtdb.firebaseio.com";

HardwareSerial mySerial(1);  // UART1 (LoRa)

float tempthreshold = 0, humidthreshold = 0;
float ppmthreshold = 0, pgathreshold = 0;
int buzzerstate_node1 = 0, buzzerstate_node2 = 0;

float old_tempthreshold = -1;
float old_humidthreshold = -1;
float old_ppmthreshold = -1;
float old_pgathreshold = -1;
int old_buzzerstate_node1 = -1;
int old_buzzerstate_node2 = -1;

float temp_value = 0, humid_value = 0;
float ppm_value = 0, pga_value = 0;

char recvBuffer[128];
int idx = 0;

unsigned long lastSendMillis = 0;
unsigned long sendInterval = 2500;
unsigned long lastGetMillis = 0;
unsigned long GetInterval = 3000;

unsigned long lastSendToNode1Millis = 0;
unsigned long lastSendToNode2Millis = 0;

int sendCountNode1 = 0;
int sendCountNode2 = 0;

bool needSendNode1 = false;
bool needSendNode2 = false;

bool firstSendDoneNode1 = false;
bool firstSendDoneNode2 = false;

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 16, 17);  // RX=16, TX=17

    WiFi.begin(ssid, password);
    Serial.println("Đang kết nối WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\n✅ Đã kết nối WiFi!");

    // Gửi ngưỡng và trạng thái còi ngay khi kết nối WiFi lần đầu
    sendNode1(ppmthreshold, pgathreshold, buzzerstate_node1);
    sendNode2(tempthreshold, humidthreshold, buzzerstate_node2);
    firstSendDoneNode1 = true;
    firstSendDoneNode2 = true;
    lastSendToNode1Millis = millis();
    lastSendToNode2Millis = millis();

    lastSendMillis = millis();
    lastGetMillis = millis();
}


const int maxSendCount = 5;

void loop() {
    unsigned long currentMillis = millis();

    // Gửi giá trị cảm biến lên Firebase định kỳ
    if (currentMillis - lastSendMillis >= sendInterval) {
        lastSendMillis = currentMillis;
        Serial.println("📤 Gửi dữ liệu cảm biến lên Firebase");
        postSensorValuesToFirebase();
    }

    // Lấy ngưỡng và trạng thái còi từ Firebase định kỳ
    if (currentMillis - lastGetMillis >= GetInterval) {
        lastGetMillis = currentMillis;

        float old_tempthreshold_local = tempthreshold;
        float old_humidthreshold_local = humidthreshold;
        float old_ppmthreshold_local = ppmthreshold;
        float old_pgathreshold_local = pgathreshold;
        int old_buzzerstate_node1_local = buzzerstate_node1;
        int old_buzzerstate_node2_local = buzzerstate_node2;

        Serial.println("🔄 Lấy ngưỡng và trạng thái còi từ Firebase");
        getThresholdsAndBuzzer();

        if (!firstSendDoneNode1 || !firstSendDoneNode2) {
            Serial.println("📌 Lần đầu khởi tạo gửi ngưỡng và trạng thái");
            needSendNode1 = true;
            needSendNode2 = true;
            sendCountNode1 = 0;
            sendCountNode2 = 0;
            firstSendDoneNode1 = true;
            firstSendDoneNode2 = true;
            lastSendToNode1Millis = currentMillis;
            lastSendToNode2Millis = currentMillis;
        } else {
            if ((ppmthreshold != old_ppmthreshold_local) || (pgathreshold != old_pgathreshold_local) || (buzzerstate_node1 != old_buzzerstate_node1_local)) {
                Serial.println("🔁 Ngưỡng Node 1 thay đổi → cần gửi lại");
                needSendNode1 = true;
                sendCountNode1 = 0;
                lastSendToNode1Millis = currentMillis;
            }
            if ((tempthreshold != old_tempthreshold_local) || (humidthreshold != old_humidthreshold_local) || (buzzerstate_node2 != old_buzzerstate_node2_local)) {
                Serial.println("🔁 Ngưỡng Node 2 thay đổi → cần gửi lại");
                needSendNode2 = true;
                sendCountNode2 = 0;
                lastSendToNode2Millis = currentMillis;
            }
        }
    }

    // Gửi lại Node1 10 lần mỗi 5s nếu cần
    if (needSendNode1 && currentMillis - lastSendToNode1Millis >= 510) {
        if (sendCountNode1 < 10) {
            Serial.print("📡 Gửi lại Node1 lần ");
            Serial.println(sendCountNode1 + 1);
            sendNode1(ppmthreshold, pgathreshold, buzzerstate_node1);
            sendCountNode1++;
            lastSendToNode1Millis = currentMillis;
        } else {
            Serial.println("✅ Đã gửi Node1 đủ 10 lần");
            needSendNode1 = false;
            sendCountNode1 = 0;
        }
    }

    // Gửi lại Node2 10 lần mỗi 5s nếu cần
    if (needSendNode2 && currentMillis - lastSendToNode2Millis >= 500) {
        if (sendCountNode2 < maxSendCount) {
            Serial.print("📡 Gửi lại Node2 lần ");
            Serial.println(sendCountNode2 + 1);
            sendNode2(tempthreshold, humidthreshold, buzzerstate_node2);
            sendCountNode2++;
            lastSendToNode2Millis = currentMillis;
        } else {
            Serial.println("✅ Đã gửi Node2 đủ 5 lần");
            needSendNode2 = false;
            sendCountNode2 = 0;
        }
    }

    // Nhận dữ liệu từ Serial USB và gửi qua LoRa (UART1)
    if (Serial.available()) {
        String userData = Serial.readStringUntil('\n');
        mySerial.println(userData);
        Serial.print("🛠️ Gửi tới STM32: ");
        Serial.println(userData);
    }

    // Nhận dữ liệu từ UART1 (LoRa)
    receive_UART1_data();
}


// ================= FIREBASE ================= //

void getThresholdsAndBuzzer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Lấy ngưỡng
    String urlThreshold = FIREBASE_HOST + "/Hazard_monitoring_and_detection/threshold.json";
    http.begin(urlThreshold);
    int code1 = http.GET();
    if (code1 == 200) {
      String payloadThreshold = http.getString();
      StaticJsonDocument<1024> docThresh;
      DeserializationError err1 = deserializeJson(docThresh, payloadThreshold);
      if (!err1) {
        if (docThresh.containsKey("Node_1")) {
          auto node1 = docThresh["Node_1"];
          if (node1.containsKey("threshold_ppm")) ppmthreshold = node1["threshold_ppm"].as<float>();
          if (node1.containsKey("threshold_pga")) pgathreshold = node1["threshold_pga"].as<float>();
        }
        if (docThresh.containsKey("Node_2")) {
          auto node2 = docThresh["Node_2"];
          if (node2.containsKey("threshold_temperature")) tempthreshold = node2["threshold_temperature"].as<float>();
          if (node2.containsKey("threshold_humidity")) humidthreshold = node2["threshold_humidity"].as<float>();
        }
      }
    } else {
      Serial.printf("❌ Lỗi GET threshold: %d\n", code1);
    }
    http.end();

    // Lấy trạng thái còi
    String urlDevice = FIREBASE_HOST + "/Hazard_monitoring_and_detection/device.json";
    http.begin(urlDevice);
    int code2 = http.GET();
    if (code2 == 200) {
      String payloadDevice = http.getString();
      StaticJsonDocument<512> docDevice;
      DeserializationError err2 = deserializeJson(docDevice, payloadDevice);
      if (!err2) {
        if (docDevice.containsKey("Node_1")) {
          auto node1 = docDevice["Node_1"];
          if (node1.containsKey("buzzer_1")) buzzerstate_node1 = node1["buzzer_1"].as<int>();
        }
        if (docDevice.containsKey("Node_2")) {
          auto node2 = docDevice["Node_2"];
          if (node2.containsKey("buzzer_2")) buzzerstate_node2 = node2["buzzer_2"].as<int>();
        }
      }
    } else {
      Serial.printf("❌ Lỗi GET device: %d\n", code2);
    }
    http.end();

    Serial.println("✅ Đã đọc ngưỡng và trạng thái còi từ Firebase:");
    Serial.printf("Node1 - ppm: %.2f, pga: %.2f, buzzer: %d\n", ppmthreshold, pgathreshold, buzzerstate_node1);
    Serial.printf("Node2 - temp: %.2f, humid: %.2f, buzzer: %d\n", tempthreshold, humidthreshold, buzzerstate_node2);
  }
}


void postSensorValuesToFirebase() {
  if (WiFi.status() == WL_CONNECTED) {
      StaticJsonDocument<512> doc;
      doc["value"]["Node_1"]["value_ppm"] = ppm_value;
      doc["value"]["Node_1"]["value_pga"] = pga_value;
      doc["value"]["Node_2"]["value_temperature"] = temp_value;
      doc["value"]["Node_2"]["value_humidity"] = humid_value;

      String jsonStr;
      serializeJson(doc, jsonStr);

      HTTPClient http;
      String url = FIREBASE_HOST + "/Hazard_monitoring_and_detection.json";
      http.begin(url);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.PATCH(jsonStr);

      if (httpResponseCode > 0) {
      Serial.printf("✅ Đã gửi giá trị cảm biến lên Firebase (code=%d)\n", httpResponseCode);
      } else {
      Serial.printf("❌ Lỗi gửi giá trị cảm biến: %d\n", httpResponseCode);
      }
      http.end();
  }
}

// ================= UART1 (LoRa) ================= //

//========================= UART & LoRa ==========================//

void sendNode1(float ppmthreshold, float pgathreshold, int buzzerstatenode1) {
    String message = "id:node1;";
    message += "ppmthreshold:" + String(ppmthreshold, 2) + ";";
    message += "pgathreshold:" + String(pgathreshold, 2) + ";";
    message += "buzzerstatenode1:" + String(buzzerstatenode1) + ";";
    mySerial.print(message + "\n");
}

void sendNode2(float tempthreshold, float humidthreshold, int buzzerstatenode2) {
    String message = "id:node2;";
    message += "tempthreshold:" + String(tempthreshold, 2) + ";";
    message += "humidthreshold:" + String(humidthreshold, 2) + ";";
    message += "buzzerstatenode2:" + String(buzzerstatenode2) + ";";
    mySerial.print(message + "\n");
}

void send_data_2_node() {
    sendNode1(ppmthreshold, pgathreshold, buzzerstate_node1);
    delay(400);
    sendNode2(tempthreshold, humidthreshold, buzzerstate_node2);
}


void receive_UART1_data() {
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n' || c == '\r') {
      if (idx > 0) {
      recvBuffer[idx] = '\0';
      Serial.print("⬅️ Nhận UART1: ");
      Serial.println(recvBuffer);
      parseUART1Data(recvBuffer);
      idx = 0;
      }
      } else {
      if (idx < sizeof(recvBuffer) - 1) {
      recvBuffer[idx++] = c;
      }
    }
  }
}

void parseUART1Data(char* data) {
  // Dữ liệu có thể nhiều dòng, tách từng dòng
  char* line = strtok(data, "\n");
  while (line != NULL) {
    char* token = strtok(line, ";");
    while (token != NULL) {
      char* sep = strchr(token, ':');
      if (sep != NULL) {
        *sep = '\0';
        char* key = token;
        char* value = sep + 1;
        
        if (strcmp(key, "tempvalue") == 0) {
          temp_value = atof(value);
        } else if (strcmp(key, "humidvalue") == 0) {
          humid_value = atof(value);
        } else if (strcmp(key, "ppmvalue") == 0) {
          ppm_value = atof(value);
        } else if (strcmp(key, "pgavalue") == 0) {
          pga_value = atof(value);
        } else if (strcmp(key, "id") == 0) {
          // Có thể lưu hoặc bỏ qua
        }
      }
      token = strtok(NULL, ";");
    }
    Serial.printf("🌡️ Nhiệt: %.2f, 💧Độ ẩm: %.2f, ☣️ PPM: %.2f, 📊 PGA: %.2f\n",
                  temp_value, humid_value, ppm_value, pga_value);
    line = strtok(NULL, "\n");
  }
}
