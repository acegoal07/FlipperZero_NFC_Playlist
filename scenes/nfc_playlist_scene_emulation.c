#include "../nfc_playlist.h"

typedef enum NfcPlaylistEmulationState {
   NfcPlaylistEmulationState_Emulating,
   NfcPlaylistEmulationState_Stopped,
   NfcPlaylistEmulationState_Canceled
} NfcPlaylistEmulationState;

static NfcPlaylistEmulationState EmulationState = NfcPlaylistEmulationState_Stopped;

static int32_t nfc_playlist_emulation_task(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;

   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);

   popup_set_context(nfc_playlist->views.popup, nfc_playlist);
   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Popup);

   if(nfc_playlist->settings.playlist_length == 0) {
      popup_set_header(
         nfc_playlist->views.popup,
         "The playlist you have\nselected is empty",
         64,
         5,
         AlignCenter,
         AlignTop);
   } else if(file_stream_open(
                stream,
                furi_string_get_cstr(nfc_playlist->settings.playlist_path),
                FSAM_READ,
                FSOM_OPEN_EXISTING)) {
      EmulationState = NfcPlaylistEmulationState_Emulating;

      FuriString* line = furi_string_alloc();
      FuriString* tmp_header_str = furi_string_alloc();
      FuriString* tmp_counter_str = furi_string_alloc();
      FuriString* tmp_file_name = furi_string_alloc();
      FuriString* tmp_file_ext = furi_string_alloc();

      bool delay_setting_on = nfc_playlist->settings.emulate_delay > 0;
      bool delay_active = false;
      bool skip_delay = false;

      while(stream_read_line(stream, line) &&
            EmulationState == NfcPlaylistEmulationState_Emulating) {
         furi_string_trim(line);

         if(furi_string_empty(line)) {
            continue;
         }

         if(delay_setting_on) {
            if(delay_active && !skip_delay) {
               popup_set_header(
                  nfc_playlist->views.popup, "Delaying", 64, 5, AlignCenter, AlignTop);
               nfc_playlist_led_worker_start(
                  nfc_playlist->notification_app, NfcPlaylistLedState_Error);
               int time_counter_delay_ms =
                  (options_emulate_delay[nfc_playlist->settings.emulate_delay] * 1000);
               while(time_counter_delay_ms > 0 &&
                     EmulationState == NfcPlaylistEmulationState_Emulating) {
                  furi_string_printf(tmp_counter_str, "%ds", (time_counter_delay_ms / 1000));
                  popup_set_text(
                     nfc_playlist->views.popup,
                     furi_string_get_cstr(tmp_counter_str),
                     64,
                     50,
                     AlignCenter,
                     AlignTop);
                  furi_delay_ms(50);
                  time_counter_delay_ms -= 50;
               };
            } else if(!delay_active) {
               delay_active = true;
            } else if(skip_delay) {
               skip_delay = false;
            }
         }

         if(EmulationState != NfcPlaylistEmulationState_Emulating) {
            break;
         }

         path_extract_filename(line, tmp_file_name, false);
         path_extract_ext_str(line, tmp_file_ext);

         int time_counter_ms =
            (options_emulate_timeout[nfc_playlist->settings.emulate_timeout] * 1000);

         if(furi_string_cmpi_str(tmp_file_ext, ".nfc") != 0) {
            // Invalid file type error
            if(nfc_playlist->settings.skip_error) {
               skip_delay = true;
               continue;
            }
            furi_string_printf(
               tmp_header_str,
               "ERROR invalid file type:\n%s",
               furi_string_get_cstr(tmp_file_name));
            popup_set_header(
               nfc_playlist->views.popup,
               furi_string_get_cstr(tmp_header_str),
               64,
               5,
               AlignCenter,
               AlignTop);
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Error);
            while(time_counter_ms > 0 && EmulationState == NfcPlaylistEmulationState_Emulating) {
               furi_string_printf(tmp_counter_str, "%ds", (time_counter_ms / 1000));
               popup_set_text(
                  nfc_playlist->views.popup,
                  furi_string_get_cstr(tmp_counter_str),
                  64,
                  50,
                  AlignCenter,
                  AlignTop);
               furi_delay_ms(50);
               time_counter_ms -= 50;
            };
         } else if(!storage_file_exists(storage, furi_string_get_cstr(line))) {
            // File not found error
            if(nfc_playlist->settings.skip_error) {
               skip_delay = true;
               continue;
            }
            furi_string_printf(
               tmp_header_str, "ERROR file not found:\n%s", furi_string_get_cstr(tmp_file_name));
            popup_set_header(
               nfc_playlist->views.popup,
               furi_string_get_cstr(tmp_header_str),
               64,
               5,
               AlignCenter,
               AlignTop);
            nfc_playlist_led_worker_start(
               nfc_playlist->notification_app, NfcPlaylistLedState_Error);
            while(time_counter_ms > 0 && EmulationState == NfcPlaylistEmulationState_Emulating) {
               furi_string_printf(tmp_counter_str, "%ds", (time_counter_ms / 1000));
               popup_set_text(
                  nfc_playlist->views.popup,
                  furi_string_get_cstr(tmp_counter_str),
                  64,
                  50,
                  AlignCenter,
                  AlignTop);
               furi_delay_ms(50);
               time_counter_ms -= 50;
            };
         } else {
            if(nfc_playlist_emulation_worker_set_nfc_data(
                  nfc_playlist->emulation_worker, (char*)furi_string_get_cstr(line))) {
               if(nfc_playlist_emulation_worker_valid_protocol(nfc_playlist->emulation_worker)) {
                  // Emulate NFC data
                  furi_string_printf(
                     tmp_header_str, "Emulating:\n%s", furi_string_get_cstr(tmp_file_name));
                  popup_set_header(
                     nfc_playlist->views.popup,
                     furi_string_get_cstr(tmp_header_str),
                     64,
                     5,
                     AlignCenter,
                     AlignTop);
                  nfc_playlist_emulation_worker_start(nfc_playlist->emulation_worker);
                  nfc_playlist_led_worker_start(
                     nfc_playlist->notification_app, NfcPlaylistLedState_Normal);
                  while(
                     nfc_playlist_emulation_worker_is_emulating(nfc_playlist->emulation_worker) &&
                     time_counter_ms > 0 &&
                     EmulationState == NfcPlaylistEmulationState_Emulating) {
                     furi_string_printf(tmp_counter_str, "%ds", (time_counter_ms / 1000));
                     popup_set_text(
                        nfc_playlist->views.popup,
                        furi_string_get_cstr(tmp_counter_str),
                        64,
                        50,
                        AlignCenter,
                        AlignTop);
                     furi_delay_ms(50);
                     time_counter_ms -= 50;
                  };
                  nfc_playlist_emulation_worker_stop(nfc_playlist->emulation_worker);
                  nfc_playlist_emulation_worker_clear_nfc_data(nfc_playlist->emulation_worker);
               } else {
                  // Invalid NFC protocol error
                  nfc_playlist_emulation_worker_clear_nfc_data(nfc_playlist->emulation_worker);
                  if(nfc_playlist->settings.skip_error) {
                     skip_delay = true;
                     continue;
                  }
                  furi_string_printf(
                     tmp_header_str,
                     "ERROR invalid\nNFC protocol:\n%s",
                     furi_string_get_cstr(tmp_file_name));
                  popup_set_header(
                     nfc_playlist->views.popup,
                     furi_string_get_cstr(tmp_header_str),
                     64,
                     5,
                     AlignCenter,
                     AlignTop);
                  nfc_playlist_led_worker_start(
                     nfc_playlist->notification_app, NfcPlaylistLedState_Error);
                  while(time_counter_ms > 0 &&
                        EmulationState == NfcPlaylistEmulationState_Emulating) {
                     furi_string_printf(tmp_counter_str, "%ds", (time_counter_ms / 1000));
                     popup_set_text(
                        nfc_playlist->views.popup,
                        furi_string_get_cstr(tmp_counter_str),
                        64,
                        50,
                        AlignCenter,
                        AlignTop);
                     furi_delay_ms(50);
                     time_counter_ms -= 50;
                  };
               }
            } else {
               // Failed to load NFC data error
               if(nfc_playlist->settings.skip_error) {
                  skip_delay = true;
                  continue;
               }
               furi_string_printf(
                  tmp_header_str,
                  "ERROR failed to\nload NFC data:\n%s",
                  furi_string_get_cstr(tmp_file_name));
               popup_set_header(
                  nfc_playlist->views.popup,
                  furi_string_get_cstr(tmp_header_str),
                  64,
                  5,
                  AlignCenter,
                  AlignTop);
               nfc_playlist_led_worker_start(
                  nfc_playlist->notification_app, NfcPlaylistLedState_Error);
               while(time_counter_ms > 0 &&
                     EmulationState == NfcPlaylistEmulationState_Emulating) {
                  furi_string_printf(tmp_counter_str, "%ds", (time_counter_ms / 1000));
                  popup_set_text(
                     nfc_playlist->views.popup,
                     furi_string_get_cstr(tmp_counter_str),
                     64,
                     50,
                     AlignCenter,
                     AlignTop);
                  furi_delay_ms(50);
                  time_counter_ms -= 50;
               };
            }
         }
      }
      nfc_playlist_led_worker_stop(nfc_playlist->notification_app);

      furi_string_free(line);
      furi_string_free(tmp_header_str);
      furi_string_free(tmp_counter_str);
      furi_string_free(tmp_file_name);
      furi_string_free(tmp_file_ext);
      file_stream_close(stream);

      popup_reset(nfc_playlist->views.popup);
      popup_set_header(
         nfc_playlist->views.popup,
         EmulationState == NfcPlaylistEmulationState_Canceled ? "Emulation stopped" :
                                                                "Emulation finished",
         64,
         5,
         AlignCenter,
         AlignTop);
   } else {
      popup_set_header(
         nfc_playlist->views.popup, "Failed to open playlist", 64, 5, AlignCenter, AlignTop);
   }
   popup_set_text(nfc_playlist->views.popup, "Press back", 64, 50, AlignCenter, AlignTop);

   furi_record_close(RECORD_STORAGE);
   stream_free(stream);

   EmulationState = NfcPlaylistEmulationState_Stopped;

   return 0;
}

static void nfc_playlist_emulation_setup(void* context) {
   NfcPlaylist* nfc_playlist = context;
   nfc_playlist->thread = furi_thread_alloc_ex(
      "NfcPlaylistEmulationWorker", 4096, nfc_playlist_emulation_task, nfc_playlist);
   nfc_playlist->emulation_worker = nfc_playlist_emulation_worker_alloc();
}

static void nfc_playlist_emulation_free(NfcPlaylist* nfc_playlist) {
   furi_assert(nfc_playlist);
   furi_thread_free(nfc_playlist->thread);
   nfc_playlist_emulation_worker_free(nfc_playlist->emulation_worker);
   nfc_playlist->thread = NULL;
   nfc_playlist->emulation_worker = NULL;
}

static void nfc_playlist_emulation_start(NfcPlaylist* nfc_playlist) {
   furi_assert(nfc_playlist);
   furi_thread_start(nfc_playlist->thread);
}

static void nfc_playlist_emulation_stop(NfcPlaylist* nfc_playlist) {
   furi_assert(nfc_playlist);
   furi_thread_join(nfc_playlist->thread);
}

void nfc_playlist_emulation_scene_on_enter(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   nfc_playlist_emulation_setup(nfc_playlist);
   nfc_playlist_emulation_start(nfc_playlist);
}

bool nfc_playlist_emulation_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(context);
   bool consumed = false;
   if(event.type == SceneManagerEventTypeBack &&
      EmulationState == NfcPlaylistEmulationState_Emulating) {
      EmulationState = NfcPlaylistEmulationState_Canceled;
      consumed = true;
   }
   return consumed;
}

void nfc_playlist_emulation_scene_on_exit(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   EmulationState = NfcPlaylistEmulationState_Stopped;
   nfc_playlist_emulation_stop(nfc_playlist);
   nfc_playlist_emulation_free(nfc_playlist);
   popup_reset(nfc_playlist->views.popup);
}
