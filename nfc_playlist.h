#pragma once
#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/popup.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/submenu.h>
#include <gui/modules/file_browser.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>

#include <notification/notification_messages.h>

#include <storage/storage.h>

#include <toolbox/stream/stream.h>
#include <toolbox/stream/file_stream.h>
#include <toolbox/path.h>

#include "nfc_playlist_icons.h"

#include "scenes/nfc_playlist_scene.h"

#include "lib/emulation_worker/nfc_playlist_emulation_worker.h"
#include "lib/led_worker/nfc_playlist_led_worker.h"

#define PLAYLIST_LOCATION     "/ext/apps_data/nfc_playlist/"
#define PLAYLIST_DIR          "/ext/apps_data/nfc_playlist"
#define NFC_ITEM_LOCATION     "/ext/nfc/"
#define MAX_PLAYLIST_NAME_LEN 50

typedef enum {
   NfcPlaylistView_Submenu,
   NfcPlaylistView_Popup,
   NfcPlaylistView_Widget,
   NfcPlaylistView_VariableItemList,
   NfcPlaylistView_FileBrowser,
   NfcPlaylistView_TextInput,
   NfcPlaylistView_Count
} NfcPlaylistViews;

typedef struct {
   FuriString* playlist_path;
   uint8_t playlist_length;
   uint8_t emulate_timeout;
   uint8_t emulate_delay;
   bool emulate_led_indicator;
   bool skip_error;
} NfcPlaylistSettings;

typedef struct {
   FileBrowser* view;
   FuriString* output;
} NfcPlaylistFileBrowserView;

typedef struct {
   TextInput* view;
   char* output;
} NfcPlaylistTextInputView;

typedef struct {
   Submenu* submenu;
   Popup* popup;
   Widget* widget;
   VariableItemList* variable_item_list;
   NfcPlaylistFileBrowserView file_browser;
   NfcPlaylistTextInputView text_input;
} NfcPLaylistView;

typedef struct {
   SceneManager* scene_manager;
   ViewDispatcher* view_dispatcher;
   NfcPlaylistSettings settings;
   NfcPLaylistView views;
   NfcPlaylistEmulationWorker* emulation_worker;
   NotificationApp* notification_app;
   FuriThread* thread;
} NfcPlaylist;

static const int options_emulate_timeout[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const int default_emulate_timeout = 4;
static const int options_emulate_delay[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
static const int default_emulate_delay = 0;
static const bool default_emulate_led_indicator = true;
static const bool default_skip_error = false;
