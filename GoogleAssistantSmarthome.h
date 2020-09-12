#ifndef GOOGLEASSISTANTSMARTHOME_H
#define GOOGLEASSISTANTSMARTHOME_H

#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

class GoogleAssistantSmarthome {
    ESP8266WebServer* server;
  public:
    GoogleAssistantSmarthome(ESP8266WebServer*);
    void setup(
      String authRoute,
      String tokenRoute,
      String fulfillmentRoute,
      String googleClientId,
      String googleClientSecret,
      String projectId,
      String tokenAuth,
      String tokenRefresh,
      String tokenAccess,
      String sync(),
      String query(String id),
      String execute(String id, String command, JsonVariant params),
      void disconnect()
    );
};

#endif
