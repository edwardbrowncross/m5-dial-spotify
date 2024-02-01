#include <Arduino.h>
#include <WiFi.h>
#define _TASK_OO_CALLBACKS
#define _TASK_STATUS_REQUEST
#include <EventHandler.h>
#include <TaskSchedulerDeclarations.h>

#include "events.h"

class WifiTask : public Task, public TSEvents::EventHandler {
 public:
  WifiTask(Scheduler& s, TSEvents::EventBus& e, const char* _ssid, const char* _pass)
      : Task(1000 * TASK_MILLISECOND, TASK_FOREVER, &s, false),
        EventHandler(&s, &e) {
    ssid = _ssid;
    pass = _pass;
  }

  bool OnEnable() {
    WiFi.disconnect(true);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    delay(200);
    connect();
    return true;
  }

  bool Callback() {
    switch (state) {
      case CONNECTING:
        switch (WiFi.status()) {
          case WL_CONNECTED: {
            // Connecting complete
            const char* ip = WiFi.localIP().toString().c_str();
            const char* dns = WiFi.dnsIP().toString().c_str();
            Serial.print("IP: ");
            Serial.println(ip);
            Serial.print("DNS: ");
            Serial.println(dns);
            dispatch(WIFI_CONNECTED, dns, strlen(dns));
            state = CONNECTED;
            setInterval(1 * TASK_SECOND);
            break;
          }
          case WL_NO_SSID_AVAIL:
            // Connecting failed
            dispatch(WIFI_CONNECT_FAILED, "No SSID Available");
            state = DISCONNECTED;
            setInterval(1 * TASK_MINUTE);
            break;
          case WL_CONNECT_FAILED:
            // Connecting failed
            dispatch(WIFI_CONNECT_FAILED, "Failed");
            state = DISCONNECTED;
            setInterval(1 * TASK_MINUTE);
            break;
          default:
            if (millis() - connectTime > 30e3) {
              // Connection timed out
              dispatch(WIFI_CONNECT_FAILED, "Connection timeout");
              state = DISCONNECTED;
              setInterval(1 * TASK_MINUTE);
            }
        }
        break;
      case CONNECTED:
        if (WiFi.status() != WL_CONNECTED) {
          // Connection lost
          dispatch(WIFI_DISCONNECTED);
          state = DISCONNECTED;
          setInterval(1 * TASK_MINUTE);
          connect();
          break;
        }
        break;
      case DISCONNECTED:
        connect();
        break;
    }
    return true;
  }

  bool connect() {
    int apIndex = findAPIndex(ssid);
    if (apIndex == -1) {
      dispatch(WIFI_CONNECT_FAILED, "SSID not detectable");
      return false;
    }
    int32_t channel = WiFi.channel(apIndex);
    uint8_t* bssid = WiFi.BSSID(apIndex);
    WiFi.config(IPAddress(192, 168, 50, 203), IPAddress(192, 168, 50, 1), IPAddress(255, 255, 255, 0), IPAddress(192, 168, 50, 3));
    WiFi.begin(ssid, pass, channel, bssid, true);
    state = CONNECTING;
    connectTime = millis();
    setInterval(100 * TASK_MILLISECOND);
    dispatch(WIFI_CONNECTING);
    return true;
  }

  void disconnect() {
    WiFi.disconnect(true);
    disable();
    if (state != DISCONNECTED) {
      dispatch(WIFI_DISCONNECTED);
      state = DISCONNECTED;
    }
  }

  void HandleEvent(TSEvents::Event event) {
  }

 private:
  int findAPIndex(String ssid) {
    int index = -1;
    int32_t rssi = 0;
    int n = WiFi.scanNetworks();
    Serial.println("wifi in range:");
    for (int i = 0; i < n; i++) {
      Serial.print(i);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(", ");
      Serial.print(WiFi.channel(i));
      Serial.print(", ");
      Serial.print(WiFi.BSSIDstr(i));
      Serial.println(")");
      if (WiFi.SSID(i) == ssid) {
        int32_t newRSSI = WiFi.RSSI(i);
        if (index == -1 || newRSSI > rssi) {
          index = i;
          rssi = newRSSI;
        }
      }
    }
    Serial.print("selecting ");
    Serial.println(index);
    return index;
  }

  enum State {
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
  };

  unsigned long connectTime;
  State state;
  const char* ssid;
  const char* pass;
};