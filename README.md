# Google Smarthome Actions for Arduino ESP8266

This project can be used to provide on-device Google Assistant support to Arduino projects based on the ESP8266 without an extra bridge.
The library is intended to be used in a single-user environment as its OAuth 2.0 implementation provides no real user authentication and always uses the same strings as tokens. 
It handles the requests made by the Google servers like described in [their guide](https://developers.google.com/assistant/smarthome/develop/implement-oauth).
As this library handles no authentication it is recommended to place the endpoints behind a reverse proxy with an enabled authentication mechanism (e.g. HTTP Basic).
The intent fulfillment requests are handled by the library while the query handling needs to be implemented.

It uses the [ESP8266WebServer](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer) to process the requests and [ArduinoJson](https://arduinojson.org/) to parse their contents.

## Usage

For a working example see the [examples](./examples) folder of this repository.
To use a device with Google, a Google Actions project has to be created in the [console](https://console.actions.google.com/).

The main function is:
```
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
    )
```

### `authRoute`
A route of the authorization endpoint. Returns the authorization token (`tokenAuth`) to Google. E.g. `/auth`.

### `tokenRoute`
A route of the token exchange endpoint. Exchanges the authorization token for the refresh (`tokenRefresh`) and access (`tokenAccess`) token. E.g. `/token`.

### `fulfillmentRoute`
A route where Google sends intent fulfillment requests to. E.g. `/`.

### `googleClientId`
The string identifying Google as the user of the OAuth server.

### `googleClientSecret`
The secret string sent by Google alongside token exchange requests.

### `projectId`
The project id from the Google Actions Console.

### `tokenAuth`
A string to be used as the authorization token. Should be non-guessable and kept private.

### `tokenRefresh`
A string to be used as the refresh token. Should be non-guessable and kept private.

### `tokenAccess`
A string to be used as the access token. Should be non-guessable and kept private.

### `sync()`
Invoked when Google sends an [action.device.SYNC](https://developers.google.com/assistant/smarthome/reference/intent/sync) query.
Shall return the content of the `payload` object (which includes the device information including their id's which can be made up).

### `query(deviceId)`
Invoked when Google sends an [action.device.QUERY](https://developers.google.com/assistant/smarthome/reference/intent/query) query.
Shall return the content of the `payload` object for the given device (which includes the device state).

### `execute(deviceId, command, params)`
Invoked when Google sends an [action.device.EXECUTE](https://developers.google.com/assistant/smarthome/reference/intent/execute) query.
Shall execute the given command for the given device and return the current device state object.

### `disconnect()`
Invoked when Google sends an [action.device.DISCONNECT](https://developers.google.com/assistant/smarthome/reference/intent/disconnect) query.
Returns nothing.

## Sample nginx reverse proxy configuration

```
location = /myawesomedevice/ {
	proxy_pass http://192.168.0.100;
	proxy_cache off;
	proxy_redirect off;
}

location = /myawesomedevice/token/ {
	proxy_pass http://192.168.0.100/token;
	proxy_cache off;
	proxy_redirect off;
}

location = /myawesomedevice/auth/ {
	proxy_pass http://192.168.0.100/auth;
	proxy_cache off;
	proxy_redirect off;

	auth_basic "MY_AUTHENTICATION_REALM";
	auth_basic_user_file /etc/nginx/.auth;
}
```

## Links
[Google Device Types](https://developers.google.com/assistant/smarthome/guides)

[Google Device Traits](https://developers.google.com/assistant/smarthome/traits)

[Google Developer Guide](https://developers.google.com/assistant/smarthome/develop/create)

