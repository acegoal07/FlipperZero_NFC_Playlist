#pragma once
#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/scene_manager.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/file_browser.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/modules/dialog_ex.h>

#include <notification/notification_messages.h>
#include <storage/storage.h>
#include <toolbox/path.h>
#include <toolbox/stream/file_stream.h>
#include <toolbox/stream/stream.h>

#include "nfc_playlist_icons.h"
#include "scenes/nfc_playlist_scene.h"
#include "lib/led_worker/nfc_playlist_led_worker.h"
#include "lib/playlist_worker/nfc_playlist_worker.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PLAYLIST_LOCATION     "/ext/apps_data/nfc_playlist/"
#define PLAYLIST_DIR          "/ext/apps_data/nfc_playlist"
#define NFC_ITEM_LOCATION     "/ext/nfc/"
#define MAX_PLAYLIST_NAME_LEN 50

typedef enum {
   NfcPlaylistView_Submenu,
   NfcPlaylistView_Widget,
   NfcPlaylistView_FileBrowser,
   NfcPlaylistView_VariableItemList,
   NfcPlaylistView_TextInput,
   NfcPlaylistView_Dialog
} NfcPlaylistViews;

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
   Widget* widget;
   NfcPlaylistFileBrowserView file_browser;
   NfcPlaylistTextInputView text_input;
   VariableItemList* variable_item_list;
   DialogEx* dialog;
} NfcPlaylistView;

typedef struct {
   NfcPlaylistWorkerSettings* settings;
   NfcPlaylistWorker* worker;
} NfcPlaylistWorkerInfo;

typedef struct {
   SceneManager* scene_manager;
   ViewDispatcher* view_dispatcher;
   NotificationApp* notification_app;
   NfcPlaylistView views;
   NfcPlaylistWorkerInfo worker_info;
} NfcPlaylist;

#ifdef __cplusplus
}
#endif
