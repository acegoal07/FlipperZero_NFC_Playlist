#pragma once
#include <furi.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/file_browser.h>

void nfc_playlist_playlist_select_scene_on_enter(void* context);
bool nfc_playlist_playlist_select_scene_on_event(void* context, SceneManagerEvent event);
void nfc_playlist_playlist_select_scene_on_exit(void* context);