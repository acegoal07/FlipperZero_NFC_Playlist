#include "nfc_playlist_worker.h"

int32_t nfc_playlist_worker_task(void* context) {
   NfcPlaylistWorker* worker = context;
   furi_assert(worker);

   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);

   int playlist_position = 0;

   if(file_stream_open(
         stream,
         furi_string_get_cstr(worker->settings->playlist_path),
         FSAM_READ,
         FSOM_OPEN_EXISTING)) {
      FuriString* line = furi_string_alloc();

      while(stream_read_line(stream, line) && worker->state != NfcPlaylistWorkerState_Stopped) {
         furi_string_trim(line);
         if(furi_string_empty(line)) {
            continue;
         }

         furi_string_set(worker->nfc_card_path, furi_string_get_cstr(line));
         playlist_position++;

         // Check if the file has a valid NFC extension
         FuriString* file_ext = furi_string_alloc();
         path_extract_ext_str(worker->nfc_card_path, file_ext);
         if(furi_string_cmpi(file_ext, ".nfc") != 0) {
            furi_string_free(file_ext);
            if(worker->settings->skip_error) {
               continue;
            }
            worker->state = NfcPlaylistWorkerState_InvalidFileType;
            worker->ms_counter =
               (options_emulate_timeout[worker->settings->emulate_timeout] * 1000);
            while(worker->state == NfcPlaylistWorkerState_InvalidFileType &&
                  worker->ms_counter > 0 && worker->state != NfcPlaylistWorkerState_Stopped) {
               furi_delay_ms(10);
               worker->ms_counter -= 10;
            }

            worker->ms_counter = (options_emulate_delay[worker->settings->emulate_delay] * 1000);
            if(worker->settings->emulate_delay &&
               playlist_position < worker->settings->playlist_length && worker->ms_counter > 0 &&
               worker->state != NfcPlaylistWorkerState_Stopped) {
               worker->state = NfcPlaylistWorkerState_Delaying;
               while(worker->ms_counter > 0 && worker->state == NfcPlaylistWorkerState_Delaying &&
                     worker->state != NfcPlaylistWorkerState_Stopped) {
                  furi_delay_ms(10);
                  worker->ms_counter -= 10;
               }
            }
            continue;
         }
         furi_string_free(file_ext);

         // Check if the file exists
         if(!storage_file_exists(storage, furi_string_get_cstr(worker->nfc_card_path))) {
            if(worker->settings->skip_error) {
               continue;
            }
            worker->state = NfcPlaylistWorkerState_FileDoesNotExist;
            worker->ms_counter =
               (options_emulate_timeout[worker->settings->emulate_timeout] * 1000);
            while(worker->state == NfcPlaylistWorkerState_FileDoesNotExist &&
                  worker->ms_counter > 0 && worker->state != NfcPlaylistWorkerState_Stopped) {
               furi_delay_ms(10);
               worker->ms_counter -= 10;
            }

            worker->ms_counter = (options_emulate_delay[worker->settings->emulate_delay] * 1000);
            if(worker->settings->emulate_delay &&
               playlist_position < worker->settings->playlist_length && worker->ms_counter > 0 &&
               worker->state != NfcPlaylistWorkerState_Stopped) {
               worker->state = NfcPlaylistWorkerState_Delaying;
               while(worker->ms_counter > 0 && worker->state == NfcPlaylistWorkerState_Delaying &&
                     worker->state != NfcPlaylistWorkerState_Stopped) {
                  furi_delay_ms(10);
                  worker->ms_counter -= 10;
               }
            }
            continue;
         }

         // Load the NFC card and emulate it
         if(nfc_device_load(worker->nfc_device, furi_string_get_cstr(line))) {
            worker->nfc_protocol = nfc_device_get_protocol(worker->nfc_device);

            worker->nfc_listener = nfc_listener_alloc(
               worker->nfc,
               worker->nfc_protocol,
               nfc_device_get_data(worker->nfc_device, worker->nfc_protocol));
            nfc_listener_start(worker->nfc_listener, NULL, NULL);

            worker->state = NfcPlaylistWorkerState_Emulating;

            worker->ms_counter =
               (options_emulate_timeout[worker->settings->emulate_timeout] * 1000);
            while(worker->state == NfcPlaylistWorkerState_Emulating && worker->ms_counter > 0 &&
                  worker->state != NfcPlaylistWorkerState_Stopped) {
               furi_delay_ms(10);
               worker->ms_counter -= 10;
            }

            nfc_listener_stop(worker->nfc_listener);
            nfc_listener_free(worker->nfc_listener);

            worker->ms_counter = (options_emulate_delay[worker->settings->emulate_delay] * 1000);
            if(worker->settings->emulate_delay &&
               playlist_position < worker->settings->playlist_length && worker->ms_counter > 0 &&
               worker->state != NfcPlaylistWorkerState_Stopped) {
               worker->state = NfcPlaylistWorkerState_Delaying;
               while(worker->ms_counter > 0 && worker->state == NfcPlaylistWorkerState_Delaying &&
                     worker->state != NfcPlaylistWorkerState_Stopped) {
                  furi_delay_ms(10);
                  worker->ms_counter -= 10;
               }
            }
         } else {
            // Failed to load NFC card
            if(worker->settings->skip_error) {
               continue;
            }
            worker->state = NfcPlaylistWorkerState_FailedToLoadNfcCard;
            worker->ms_counter =
               (options_emulate_timeout[worker->settings->emulate_timeout] * 1000);
            while(worker->state == NfcPlaylistWorkerState_FailedToLoadNfcCard &&
                  worker->ms_counter > 0 && worker->state != NfcPlaylistWorkerState_Stopped) {
               furi_delay_ms(10);
               worker->ms_counter -= 10;
            }

            worker->ms_counter = (options_emulate_delay[worker->settings->emulate_delay] * 1000);
            if(worker->settings->emulate_delay &&
               playlist_position < worker->settings->playlist_length && worker->ms_counter > 0 &&
               worker->state != NfcPlaylistWorkerState_Stopped) {
               worker->state = NfcPlaylistWorkerState_Delaying;
               while(worker->ms_counter > 0 && worker->state == NfcPlaylistWorkerState_Delaying &&
                     worker->state != NfcPlaylistWorkerState_Stopped) {
                  furi_delay_ms(10);
                  worker->ms_counter -= 10;
               }
            }
         }
      }

      file_stream_close(stream);
      furi_string_free(line);
      worker->state = NfcPlaylistWorkerState_Stopped;
   } else {
      // Failed to load playlist
      worker->state = NfcPlaylistWorkerState_FailedToLoadPlaylist;
   }

   furi_record_close(RECORD_STORAGE);

   return 0;
}

NfcPlaylistWorker* nfc_playlist_worker_alloc(NfcPlaylistWorkerSettings* settings) {
   NfcPlaylistWorker* worker = malloc(sizeof(NfcPlaylistWorker));
   furi_assert(worker);

   worker->thread = furi_thread_alloc();
   furi_thread_set_name(worker->thread, "nfc_playlist_worker");
   furi_thread_set_context(worker->thread, worker);
   furi_thread_set_callback(worker->thread, nfc_playlist_worker_task);
   furi_thread_set_stack_size(worker->thread, 4 * 1024);

   worker->state = NfcPlaylistWorkerState_Ready;
   worker->nfc = nfc_alloc();
   worker->nfc_device = nfc_device_alloc();
   worker->settings = settings;
   worker->nfc_card_path = furi_string_alloc();

   return worker;
}

void nfc_playlist_worker_free(NfcPlaylistWorker* worker) {
   furi_assert(worker);

   if(worker->nfc_device) {
      nfc_device_free(worker->nfc_device);
   }
   if(worker->nfc) {
      nfc_free(worker->nfc);
   }

   furi_thread_free(worker->thread);
   furi_string_free(worker->nfc_card_path);

   free(worker);
}

void nfc_playlist_worker_start(NfcPlaylistWorker* worker) {
   furi_assert(worker);
   furi_thread_start(worker->thread);
   worker->state = NfcPlaylistWorkerState_Emulating;
}

void nfc_playlist_worker_stop(NfcPlaylistWorker* worker) {
   furi_assert(worker);
   worker->state = NfcPlaylistWorkerState_Stopped;
   furi_thread_join(worker->thread);
}
