#pragma once

#include <Arduino.h>
#include <SpotifyArduino.h>
#include "utils.h"

typedef struct {
  char trackName[32];
  char trackId[8];
  char artistName[32];
  char albumName[32];
  char albumId[8];
  char albumArtUrl[96];
  bool isPlaying;
  long progressMs;
  long durationMs;

  void setFromCurrentlyPlaying(CurrentlyPlaying* cp) {
    strncpynt(trackName, cp->trackName, 32);
    strncpynt(trackId, cp->trackUri + 14, 8);
    strncpynt(artistName, cp->artists[0].artistName, 32);
    strncpynt(albumName, cp->albumName, 32);
    strncpynt(albumId, cp->albumUri + 14, 8);
    for (int i = 0; i < cp->numImages; i++) {
      if (cp->albumImages[i].width == 300) {
        strncpynt(albumArtUrl, cp->albumImages[i].url, 96);
        break;
      }
    }
    isPlaying = cp->isPlaying;
    progressMs = cp->progressMs;
    durationMs = cp->durationMs;
  }
} SpotifyState;