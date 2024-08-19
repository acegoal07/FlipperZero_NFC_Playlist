#include "../nfc_playlist.h"

typedef enum {
   NfcPlaylistFileEdit_CreatePlaylist,
   NfcPlaylistFileEdit_DeletePlaylist,
   NfcPlaylistFileEdit_RenamePlaylist,
   NfcPlaylistFileEdit_AddNfcItem,
   NfcPlaylistFileEdit_RemoveNfcItem,
   NfcPlaylistFileEdit_ViewPlaylistContent
} NfcPlaylistFileEditMenuSelection;

void nfc_playlist_playlist_edit_menu_callback(void* context, uint32_t index) {
   NfcPlaylist* nfc_playlist = context;
   scene_manager_handle_custom_event(nfc_playlist->scene_manager, index);
}

void nfc_playlist_playlist_edit_scene_on_enter(void* context) {
   NfcPlaylist* nfc_playlist = context;

   submenu_set_header(nfc_playlist->submenu, "Edit Playlist");

   bool playlist_path_empty = furi_string_empty(nfc_playlist->settings.playlist_path);

   submenu_add_item(
      nfc_playlist->submenu,
      "Create Playlist",
      NfcPlaylistFileEdit_CreatePlaylist,
      nfc_playlist_playlist_edit_menu_callback,
      nfc_playlist);

   submenu_add_lockable_item(
      nfc_playlist->submenu,
      "Delete Playlist",
      NfcPlaylistFileEdit_DeletePlaylist,
      nfc_playlist_playlist_edit_menu_callback,
      nfc_playlist,
      playlist_path_empty,
      "No\nplaylist\nselected");

   submenu_add_lockable_item(
      nfc_playlist->submenu,
      "Rename Playlist",
      NfcPlaylistFileEdit_RenamePlaylist,
      nfc_playlist_playlist_edit_menu_callback,
      nfc_playlist,
      playlist_path_empty,
      "No\nplaylist\nselected");

   submenu_add_lockable_item(
      nfc_playlist->submenu,
      "Add NFC Item",
      NfcPlaylistFileEdit_AddNfcItem,
      nfc_playlist_playlist_edit_menu_callback,
      nfc_playlist,
      playlist_path_empty,
      "No\nplaylist\nselected");

   submenu_add_lockable_item(
      nfc_playlist->submenu,
      "Remove NFC Item",
      NfcPlaylistFileEdit_RemoveNfcItem,
      nfc_playlist_playlist_edit_menu_callback,
      nfc_playlist,
      playlist_path_empty,
      "No\nplaylist\nselected");

   submenu_add_lockable_item(
      nfc_playlist->submenu,
      "View Playlist Content",
      NfcPlaylistFileEdit_ViewPlaylistContent,
      nfc_playlist_playlist_edit_menu_callback,
      nfc_playlist,
      playlist_path_empty,
      "No\nplaylist\nselected");

   view_dispatcher_switch_to_view(nfc_playlist->view_dispatcher, NfcPlaylistView_Submenu);
}

bool nfc_playlist_playlist_edit_scene_on_event(void* context, SceneManagerEvent event) {
   NfcPlaylist* nfc_playlist = context;
   bool consumed = false;
   if(event.type == SceneManagerEventTypeCustom) {
      switch(event.event) {
         case NfcPlaylistFileEdit_CreatePlaylist:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_NameNewPlaylist);
            consumed = true;
            break;
         case NfcPlaylistFileEdit_DeletePlaylist:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_ConfirmDelete);
            consumed = true;
            break;
         case NfcPlaylistFileEdit_RenamePlaylist:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_PlaylistRename);
            consumed = true;
            break;
         case NfcPlaylistFileEdit_AddNfcItem:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_NfcAdd);
            consumed = true;
            break;
         case NfcPlaylistFileEdit_RemoveNfcItem:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_NfcRemove);
            consumed = true;
            break;
         case NfcPlaylistFileEdit_ViewPlaylistContent:
            scene_manager_next_scene(nfc_playlist->scene_manager, NfcPlaylistScene_ViewPlaylistContent);
            consumed = true;
            break;
         default:
            break;
      }
   }
   return consumed;
}

void nfc_playlist_playlist_edit_scene_on_exit(void* context) {
   NfcPlaylist* nfc_playlist = context;
   submenu_reset(nfc_playlist->submenu);
}