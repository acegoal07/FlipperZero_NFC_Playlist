#include "../nfc_playlist.h"

static FuriString* header_str_static = NULL;
static FuriString* text_str_static = NULL;

void nfc_playlist_emulation_scene_on_enter(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;

   popup_set_context(nfc_playlist->views.popup, nfc_playlist);
   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup);

   nfc_playlist->worker_info.worker =
      nfc_playlist_worker_alloc(nfc_playlist->worker_info.settings);
   nfc_playlist_worker_start(nfc_playlist->worker_info.worker);
}

bool nfc_playlist_emulation_scene_on_event(void* context, SceneManagerEvent event) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   bool consumed = false;

   if(event.type == SceneManagerEventTypeBack) {
      nfc_playlist_worker_stop(nfc_playlist->worker_info.worker);
      consumed = true;
   } else if(event.type == SceneManagerEventTypeTick) {
      switch(nfc_playlist->worker_info.worker->state) {
      case NfcPlaylistWorkerState_Emulating: {
         if(nfc_playlist->worker_info.settings->emulate_led_indicator) {
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Normal);
         }
         if(!header_str_static) header_str_static = furi_string_alloc();
         if(!text_str_static) text_str_static = furi_string_alloc();
         FuriString* nfc_card_name = furi_string_alloc();
         path_extract_filename_no_ext(
            furi_string_get_cstr(nfc_playlist->worker_info.worker->nfc_card_path), nfc_card_name);
         furi_string_set_str(header_str_static, "Emulating:\n");
         furi_string_cat(header_str_static, nfc_card_name);
         furi_string_free(nfc_card_name);
         popup_set_header(
            nfc_playlist->views.popup,
            furi_string_get_cstr(header_str_static),
            64,
            5,
            AlignCenter,
            AlignTop);
         furi_string_printf(
            text_str_static, "%ds", (nfc_playlist->worker_info.worker->ms_counter / 1000));
         popup_set_text(
            nfc_playlist->views.popup,
            furi_string_get_cstr(text_str_static),
            64,
            50,
            AlignCenter,
            AlignTop);
         consumed = true;
         break;
      }
      case NfcPlaylistWorkerState_Delaying: {
         if(nfc_playlist->worker_info.settings->emulate_led_indicator) {
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Normal);
         }
         if(!text_str_static) text_str_static = furi_string_alloc();
         popup_set_header(nfc_playlist->views.popup, "Delaying", 64, 5, AlignCenter, AlignTop);
         furi_string_printf(
            text_str_static, "%ds", (nfc_playlist->worker_info.worker->ms_counter / 1000));
         popup_set_text(
            nfc_playlist->views.popup,
            furi_string_get_cstr(text_str_static),
            64,
            50,
            AlignCenter,
            AlignTop);
         consumed = true;
         break;
      }
      case NfcPlaylistWorkerState_FailedToLoadPlaylist: {
         if(nfc_playlist->worker_info.settings->emulate_led_indicator) {
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Error);
         }
         nfc_playlist_worker_stop(nfc_playlist->worker_info.worker);
         scene_manager_next_scene(
            nfc_playlist->scene_manager, NfcPlaylistScene_FailedToLoadPlaylist);
         consumed = true;
         break;
      }
      case NfcPlaylistWorkerState_FailedToLoadNfcCard: {
         if(nfc_playlist->worker_info.settings->emulate_led_indicator) {
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Error);
         }
         if(!header_str_static) header_str_static = furi_string_alloc();
         if(!text_str_static) text_str_static = furi_string_alloc();
         furi_string_set_str(header_str_static, "Failed to load:\n");
         furi_string_cat(header_str_static, nfc_playlist->worker_info.worker->nfc_card_path);
         popup_set_header(
            nfc_playlist->views.popup,
            furi_string_get_cstr(header_str_static),
            64,
            5,
            AlignCenter,
            AlignTop);
         furi_string_printf(
            text_str_static, "%ds", (nfc_playlist->worker_info.worker->ms_counter / 1000));
         popup_set_text(
            nfc_playlist->views.popup,
            furi_string_get_cstr(text_str_static),
            64,
            50,
            AlignCenter,
            AlignTop);
         consumed = true;
         break;
      }
      case NfcPlaylistWorkerState_InvalidFileType: {
         if(nfc_playlist->worker_info.settings->emulate_led_indicator) {
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Error);
         }
         if(!header_str_static) header_str_static = furi_string_alloc();
         if(!text_str_static) text_str_static = furi_string_alloc();
         furi_string_set_str(header_str_static, "Invalid file type:\n");
         furi_string_cat(header_str_static, nfc_playlist->worker_info.worker->nfc_card_path);
         popup_set_header(
            nfc_playlist->views.popup,
            furi_string_get_cstr(header_str_static),
            64,
            5,
            AlignCenter,
            AlignTop);
         furi_string_printf(
            text_str_static, "%ds", (nfc_playlist->worker_info.worker->ms_counter / 1000));
         popup_set_text(
            nfc_playlist->views.popup,
            furi_string_get_cstr(text_str_static),
            64,
            50,
            AlignCenter,
            AlignTop);
         consumed = true;
         break;
      }
      case NfcPlaylistWorkerState_FileDoesNotExist: {
         if(nfc_playlist->worker_info.settings->emulate_led_indicator) {
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Error);
         }
         if(!header_str_static) header_str_static = furi_string_alloc();
         if(!text_str_static) text_str_static = furi_string_alloc();
         furi_string_set_str(header_str_static, "File does not exist:\n");
         furi_string_cat(header_str_static, nfc_playlist->worker_info.worker->nfc_card_path);
         popup_set_header(
            nfc_playlist->views.popup,
            furi_string_get_cstr(header_str_static),
            64,
            5,
            AlignCenter,
            AlignTop);
         furi_string_printf(
            text_str_static, "%ds", (nfc_playlist->worker_info.worker->ms_counter / 1000));
         popup_set_text(
            nfc_playlist->views.popup,
            furi_string_get_cstr(text_str_static),
            64,
            50,
            AlignCenter,
            AlignTop);
         consumed = true;
         break;
         break;
      }
      case NfcPlaylistWorkerState_Stopped: {
         nfc_playlist_worker_stop(nfc_playlist->worker_info.worker);
         scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_EmulationFinished);
         consumed = true;
         break;
      }
      default:
         break;
      }
   }
   return consumed;
}

void nfc_playlist_emulation_scene_on_exit(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   nfc_playlist_led_worker_stop(nfc_playlist->notification_app);
   popup_reset(nfc_playlist->views.popup);
   nfc_playlist_worker_free(nfc_playlist->worker_info.worker);
   nfc_playlist->worker_info.worker = NULL;
   if(header_str_static) {
      furi_string_free(header_str_static);
      header_str_static = NULL;
   }
   if(text_str_static) {
      furi_string_free(text_str_static);
      text_str_static = NULL;
   }
}
