#pragma once

#include <Arduino.h>
#define _TASK_OO_CALLBACKS
#define _TASK_STATUS_REQUEST
#include <EventHandler.h>
#include <TaskSchedulerDeclarations.h>

// Libraries
#include <ArduinoJson.h>
#include <SpotifyArduino.h>
// #include <SpotifyArduinoCert.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "events.h"

#define SPOTIFY_MARKET "GB"

class SpotifyTask : public Task, public TSEvents::EventHandler {
 public:
  SpotifyTask(Scheduler& s, TSEvents::EventBus& e, const char* clientId, const char* clientSecret, const char* refreshToken)
      : Task(5000 * TASK_MILLISECOND, TASK_FOREVER, &s, false),
        TSEvents::EventHandler(&s, &e) {
    this->clientId = clientId;
    this->clientSecret = clientSecret;
    this->refreshToken = refreshToken;
    // client.setCACert(spotify_server_cert);
    client.setInsecure();

    spotify = new SpotifyArduino(client, clientId, clientSecret, refreshToken);
  }

  ~SpotifyTask() {
    delete spotify;
  }

  CurrentlyPlaying currentlyPlaying;

  bool OnEnable() {
    if (!WiFi.status() == WL_CONNECTED) {
      return false;
    }
    connect();
    return true;
  }

  bool connect() {
    bool ok = spotify->refreshAccessToken();
    if (!ok) {
      dispatch(SPOTIFY_AUTH_ERROR);
      Serial.println("Spotify auth error");
      authorized = false;
      return false;
    }
    authorized = true;
    return true;
  }

  bool Callback() {
    if (!authorized) {
      return true;
    }
    SpotifyTask::reqInstance = this;
    spotify->getCurrentlyPlaying(handleCurrentlyPlaying, SPOTIFY_MARKET);
    return true;
  }

  void onCurrentlyPlaying(CurrentlyPlaying _currentlyPlaying) {
    currentlyPlaying = _currentlyPlaying;
    CurrentlyPlaying* cp = &currentlyPlaying;
    dispatch(SPOTIFY_UPDATE, &cp, sizeof(CurrentlyPlaying*));
  }

  void HandleEvent(TSEvents::Event event) {
    switch (event.id) {
      case WIFI_CONNECTED:
        enableIfNot();
        break;
      case WIFI_DISCONNECTED:
        disable();
        break;
    }
  }

 private:
  const char* clientId;
  const char* clientSecret;
  const char* refreshToken;
  bool authorized = false;
  WiFiClientSecure client;
  SpotifyArduino* spotify;

  static SpotifyTask* reqInstance;
  static void handleCurrentlyPlaying(CurrentlyPlaying currentlyPlaying) {
    assert(reqInstance != NULL);
    reqInstance->onCurrentlyPlaying(currentlyPlaying);
  }
};