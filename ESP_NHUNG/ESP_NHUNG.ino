#define BLYNK_TEMPLATE_ID "TMPL62dEsfygC"
#define BLYNK_TEMPLATE_NAME "GAS WARNING"
#define BLYNK_AUTH_TOKEN "LiVx1HRk4QC-5_qqnTlathcC9Ej-u0tU"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// UART2 pins
#define RXD2 16  // ESP32 RX từ STM32 TX
#define TXD2 17  // ESP32 TX đến STM32 RX

// WiFi credentials
// char ssid[] = "";
char ssid[] = "UET-Wifi-Office-Free 2.4Ghz";
char pass[] = "";

// Biến nhận dữ liệu
float gas = 0.0;
int gas_alert_state = 0;
String inputBuffer = "";
String gas_state_string = "";

// Biến điều khiển hệ thống
bool system_active = false;

// UART2
HardwareSerial mySerial(2); // UART2

BlynkTimer timer;

// Xử lý switch bật/tắt từ Blynk (V0)
BLYNK_WRITE(V0) {
    system_active = param.asInt(); // Lấy trạng thái switch (0 hoặc 1)
    if (system_active) {
        mySerial.print("1\n");
        Serial.println("Gửi lệnh 1 đến STM32");
    } else {
        mySerial.print("0\n");
        Serial.println("Gửi lệnh 0 đến STM32");
    }
}

// Gửi dữ liệu lên Blynk
void sendToBlynk() {
    Blynk.virtualWrite(V4, gas);           // Gửi gas lên V4
    Blynk.virtualWrite(V1, gas_state_string); // Gửi trạng thái khí gas dạng chuỗi lên V1
    Blynk.virtualWrite(V0, system_active); // Cập nhật trạng thái switch V0

    // Kiểm tra ngưỡng gas và gửi cảnh báo
    if (gas > 1000.0) {
        Blynk.logEvent("gas_warning", "Nồng độ khí gas vượt quá 1000 ppm: " + String(gas) + " ppm");
    }
}

void setup() {
    Serial.begin(115200); // Serial monitor
    mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2); // UART2 với RXD2 và TXD2

    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

    timer.setInterval(500L, sendToBlynk); // Gửi dữ liệu mỗi 1s

    Serial.println("ESP32 đã sẵn sàng. Đợi dữ liệu từ STM32...");
}

void loop() {
    Blynk.run();
    timer.run();

    while (mySerial.available()) {
        char c = mySerial.read();
        if (c == '\n') {
            // Kiểm tra loại dữ liệu
            if (inputBuffer.startsWith("S")) {
                // Dữ liệu trạng thái hệ thống: "S0" hoặc "S1"
                int state;
                sscanf(inputBuffer.c_str(), "S%d", &state);
                system_active = (state == 1);
                Serial.print("Nhận trạng thái hệ thống từ STM32: ");
                Serial.println(system_active ? "ON" : "OFF");
            } else {
                // Dữ liệu cảm biến: "%.1f %d"
                sscanf(inputBuffer.c_str(), "%f %d", &gas, &gas_alert_state);

                // Chuyển đổi gas_alert_state thành chuỗi giống LCD
                switch (gas_alert_state) {
                    case -1: gas_state_string = "Error"; break;
                    case 0: gas_state_string = "No Gas"; break;
                    case 1: gas_state_string = "Low Gas"; break;
                    case 2: gas_state_string = "High Gas"; break;
                    case 3: gas_state_string = "Danger!"; break;
                    default: gas_state_string = "Error"; break;
                }

                // In dữ liệu cảm biến để debug
                Serial.println("Đã nhận chuỗi: " + inputBuffer);
                Serial.print("GAS: "); Serial.println(gas);
                Serial.print("Trạng thái khí gas: "); Serial.println(gas_state_string);
            }

            inputBuffer = "";
        } else {
            inputBuffer += c;
        }
    }
}