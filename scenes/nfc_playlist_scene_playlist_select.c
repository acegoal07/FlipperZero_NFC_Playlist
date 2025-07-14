#include "../nfc_playlist.h"

static void nfc_playlist_playlist_select_menu_callback(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   furi_string_swap(
      nfc_playlist->worker_info.settings->playlist_path, nfc_playlist->views.file_browser.output);
   furi_string_reset(nfc_playlist->views.file_browser.output);

   Storage* storage = furi_record_open(RECORD_STORAGE);
   Stream* stream = file_stream_alloc(storage);

   if(file_stream_open(
         stream,
         furi_string_get_cstr(nfc_playlist->worker_info.settings->playlist_path),
         FSAM_READ,
         FSOM_OPEN_EXISTING)) {
      nfc_playlist->worker_info.settings->playlist_length = 0;
      FuriString* line = furi_string_alloc();
      while(stream_read_line(stream, line)) {
         nfc_playlist->worker_info.settings->playlist_length++;
      }
      furi_string_free(line);
      file_stream_close(stream);
   }

   stream_free(stream);
   furi_record_close(RECORD_STORAGE);

   scene_manager_previous_scene(nfc_playlist->scene_manager);
}

void nfc_playlist_playlist_select_scene_on_enter(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   file_browser_configure(
      nfc_playlist->views.file_browser.view,
      ".txt",
      PLAYLIST_LOCATION,
      true,
      true,
      &I_Playlist_10px,
      true);
   file_browser_set_callback(
      nfc_playlist->views.file_browser.view,
      nfc_playlist_playlist_select_menu_callback,
      nfc_playlist);
   FuriString* tmp_str = furi_string_alloc_set_str(PLAYLIST_LOCATION);
   file_browser_start(nfc_playlist->views.file_browser.view, tmp_str);
   furi_string_free(tmp_str);

   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_FileBrowser);
}

bool nfc_playlist_playlist_select_scene_on_event(void* context, SceneManagerEvent event) {
   UNUSED(event);
   UNUSED(context);
   return false;
}

void nfc_playlist_playlist_select_scene_on_exit(void* context) {
   furi_assert(context);
   NfcPlaylist* nfc_playlist = context;
   file_browser_stop(nfc_playlist->views.file_browser.view);
}
