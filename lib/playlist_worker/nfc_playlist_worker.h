#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_listener.h>

#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include <toolbox/path.h>

static const int options_emulate_timeout[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const int default_emulate_timeout = 4;
static const int options_emulate_delay[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const int default_emulate_delay = 0;
static const bool default_emulate_led_indicator = true;
static const bool default_skip_error = false;

typedef enum {
   NfcPlaylistWorkerState_Ready,
   NfcPlaylistWorkerState_Emulating,
   NfcPlaylistWorkerState_Delaying,
   NfcPlaylistWorkerState_InvalidFileType,
   NfcPlaylistWorkerState_FileDoesNotExist,
   NfcPlaylistWorkerState_FailedToLoadPlaylist,
   NfcPlaylistWorkerState_FailedToLoadNfcCard,
   NfcPlaylistWorkerState_Stopped
} NfcPlaylistWorkerState;

typedef struct {
   FuriString* playlist_path;
   uint8_t playlist_length;
   uint8_t emulate_timeout;
   uint8_t emulate_delay;
   bool emulate_led_indicator;
   bool skip_error;
} NfcPlaylistWorkerSettings;

typedef struct {
   FuriThread* thread;
   NfcPlaylistWorkerState state;
   NfcListener* nfc_listener;
   NfcDevice* nfc_device;
   NfcProtocol nfc_protocol;
   Nfc* nfc;
   NfcPlaylistWorkerSettings* settings;
   FuriString* nfc_card_path;
   int ms_counter;
} NfcPlaylistWorker;

NfcPlaylistWorker* nfc_playlist_worker_alloc(NfcPlaylistWorkerSettings* settings);
void nfc_playlist_worker_free(NfcPlaylistWorker* worker);
void nfc_playlist_worker_stop(NfcPlaylistWorker* worker);
void nfc_playlist_worker_start(NfcPlaylistWorker* worker);
