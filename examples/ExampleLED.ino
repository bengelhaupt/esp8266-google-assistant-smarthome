#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <GoogleAssistantSmarthome.h>

#define LED_PIN 14

char ssid[] = "YOUR_SSID";
char pass[] = "YOUR_PASS";

ESP8266WebServer server(80);
GoogleAssistantSmarthome smarthome(&server);

bool onState = false;
int brightnessState = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.hostname("GoogleAssistantSmarthome");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    yield();
    delay(1000);
  }

  smarthome.setup(
    "/auth",
    "/token",
    "/",
    "googleClientId",
    "googleClientSecret",
    "projectId",
    "authtoken123",
    "refreshtoken456",
    "accesstoken789",
  []() {
    return String("\"agentUserId\":\"user\",\"devices\":[{\"id\":\"123\",\"type\":\"action.devices.types.LIGHT\",\"traits\":[\"action.devices.traits.OnOff\",\"action.devices.traits.Brightness\"],\"name\":{\"name\":\"Nightlight\"},\"willReportState\":false}]");
  },
  [](String id) {
    if (id == "123") {
      return status();
    }
    return String();
  },
  [](String id, String command, JsonVariant params) {
    if (id == "123") {
      if (command == "action.devices.commands.OnOff") {
        bool param = params["on"];
        toggle(param);
      }
      if (command == "action.devices.commands.BrightnessAbsolute") {
        int param = params["brightness"];
        setBrightness(param);
      }
      return status();
    }
    return String();
  },
  []() {  }
  );

  server.begin();
}

String status() {
  String onString = "false";
  if (onState) {
    onString = "true";
  }
  return String("\"on\":" + onString + ",\"brightness\":" + brightnessState + ",\"online\":true");
}

void toggle(bool on) {
  if (on) {
    analogWrite(LED_PIN, 10 * brightnessState);
    onState = true;
  } else {
    analogWrite(LED_PIN, 0);
    onState = false;
  }
}

void setBrightness(int brightness) {
  onState = brightness != 0;
  brightnessState = brightness;
  analogWrite(LED_PIN, 10 * brightness);
}

void loop() {
  server.handleClient();
}
