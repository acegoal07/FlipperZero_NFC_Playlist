#include "nfc_playlist_led_worker.h"

static NotificationMessage blink_message_normal = {
   .type = NotificationMessageTypeLedBlinkStart,
   .data.led_blink.color = LightBlue | LightGreen,
   .data.led_blink.on_time = 10,
   .data.led_blink.period = 100,
};
static const NotificationSequence blink_sequence_normal = {
   &blink_message_normal,
   &message_do_not_reset,
   NULL,
};

static NotificationMessage blink_message_error = {
   .type = NotificationMessageTypeLedBlinkStart,
   .data.led_blink.color = LightRed,
};
static const NotificationSequence blink_sequence_error = {
   &blink_message_error,
   &message_do_not_reset,
   NULL,
};

void nfc_playlist_led_worker_start(NotificationApp* notification_app, NfcPlaylistLedState state) {
   switch(state) {
   case NfcPlaylistLedState_Normal:
      notification_message_block(notification_app, &blink_sequence_normal);
      break;
   case NfcPlaylistLedState_Error:
      notification_message_block(notification_app, &blink_sequence_error);
      break;
   default:
      break;
   }
}

void nfc_playlist_led_worker_stop(NotificationApp* notification_app) {
   notification_message_block(notification_app, &sequence_blink_stop);
}
