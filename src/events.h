#pragma once

#include <Arduino.h>

enum EventType : uint16_t {
  WIFI_CONNECTING,
  WIFI_CONNECTED,
  WIFI_DISCONNECTED,
  WIFI_CONNECT_FAILED,

  SPOTIFY_AUTH_ERROR,
  SPOTIFY_UPDATE,
};