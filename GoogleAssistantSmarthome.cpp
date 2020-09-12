#include "GoogleAssistantSmarthome.h"

GoogleAssistantSmarthome::GoogleAssistantSmarthome(ESP8266WebServer* _server) {
  server = _server;
}

void GoogleAssistantSmarthome::setup(String authRoute, String tokenRoute, String fulfillmentRoute, String googleClientId, String googleClientSecret, String projectId, String tokenAuth, String tokenRefresh, String tokenAccess, String sync(), String query(String), String execute(String, String, JsonVariant), void disconnect()) {
  server->on(authRoute, HTTP_GET, [ =, this]() {
    String clientId = server->arg("client_id");
    String redirectUri = server->arg("redirect_uri");
    String state = server->arg("state");
    //String scope = server->arg("scope");
    String responseType = server->arg("response_type");
    //String userLocale = server->arg("user_locale");

    if (clientId == googleClientId
        && responseType == "code"
        && (redirectUri == "https://oauth-redirect.googleusercontent.com/r/" + projectId || redirectUri == "https://oauth-redirect-sandbox.googleusercontent.com/r/" + projectId)
       ) {
      // implement authorization here
      server->sendHeader("Location", redirectUri + "?code=" + tokenAuth + "&state=" + state);
      server->send(301);
    } else {
      server->send(400);
    }
  });

  server->on(tokenRoute, HTTP_POST, [ =, this]() {
    String clientId = server->arg("client_id");
    String clientSecret = server->arg("client_secret");
    String grantType = server->arg("grant_type");
    String code = server->arg("code");
    String redirectUri = server->arg("redirect_uri");
    String refreshToken = server->arg("refresh_token");

    if (grantType == "authorization_code") {
      if (clientId == googleClientId
          && clientSecret == googleClientSecret
          && code == tokenAuth
          && (redirectUri.startsWith("https://oauth-redirect.googleusercontent.com/r/" + projectId) || redirectUri.startsWith("https://oauth-redirect-sandbox.googleusercontent.com/r/" + projectId))
         ) {
        server->send(200, "application/json", "{\"token_type\":\"Bearer\",\"access_token\":\"" + tokenAccess + "\",\"refresh_token\":\"" + tokenRefresh + "\",\"expires_in\":31536000}");
      } else {
        server->send(400, "application/json", "{\"error\": \"invalid_grant\"}");
      }
    } else if (grantType = "refresh_token") {
      if (clientId == googleClientId
          && clientSecret == googleClientSecret
          && refreshToken == tokenRefresh) {
        server->send(200, "application/json", "{\"token_type\":\"Bearer\",\"access_token\":\"" + tokenAccess + "\",\"expires_in\":31536000}");
      } else {
        server->send(400, "application/json", "{\"error\": \"invalid_grant\"}");
      }
    } else {
      server->send(400, "application/json", "{\"error\": \"invalid_grant\"}");
    }
  });

  server->on(fulfillmentRoute, HTTP_POST, [ =, this]() {
    if (server->header("Authorization") == "Bearer " + tokenAccess) {
      String body = server->arg("plain");

      DynamicJsonDocument doc(2048);
      if (deserializeJson(doc, body)) {
        server->send(500);
      } else {
        String requestId = doc["requestId"];
        JsonVariant input = doc["inputs"][0];

        String intent = input["intent"];
        if (intent == "action.devices.SYNC") {
          server->send(200, "application/json", "{\"requestId\":\"" + requestId + "\",\"payload\":{" + sync() + "}}");
        } else if (intent == "action.devices.QUERY") {
          JsonArray devices = input["payload"]["devices"];
          String response = "{\"requestId\":\"" + requestId + "\",\"payload\":{\"devices\":{";
          for (JsonVariant device : devices) {
            String deviceId = device["id"];
            response += "\"" + deviceId + "\":{" + query(deviceId) + "}";
          }
          response += "}}}";
          server->send(200, "application/json", response);
        } else if (intent == "action.devices.EXECUTE") {
          JsonArray commands = input["payload"]["commands"];
          String response = "{\"requestId\":\"" + requestId + "\",\"payload\":{\"commands\":[";
          for (JsonVariant command : commands) {
            JsonArray devices = command["devices"];
            for (JsonVariant device : devices) {
              String deviceId = device["id"];
              JsonArray executions = command["execution"];
              for (JsonVariant execution : executions) {
                String cmd = execution["command"];
                JsonVariant params = execution["params"];
                response += "{\"ids\":[\"" + deviceId + "\"],\"status\":\"SUCCESS\",\"states\":{" + execute(deviceId, cmd, params) + "}}";
              }
            }
          }
          response += "]}}";
          server->send(200, "application/json", response);
        } else if (intent == "action.devices.DISCONNECT") {
          disconnect();
          server->send(200, "application/json", "{}");
        } else {
          server->send(400);
        }
      }
    } else {
      server->send(401);
    }
  });
}
