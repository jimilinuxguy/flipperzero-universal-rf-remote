#include "../storage_settings.h"

static const NotificationMessage message_green_165 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 165,
};

static const NotificationSequence sequence_set_formatting_leds = {
    &message_red_255,
    &message_green_165,
    &message_blue_0,
    &message_do_not_reset,
    NULL,
};

static const NotificationSequence sequence_reset_formatting_leds = {
    &message_red_0,
    &message_green_0,
    NULL,
};

static void
    storage_settings_scene_formatting_dialog_callback(DialogExResult result, void* context) {
    StorageSettings* app = context;

    view_dispatcher_send_custom_event(app->view_dispatcher, result);
}

void storage_settings_scene_formatting_on_enter(void* context) {
    StorageSettings* app = context;
    FS_Error error;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_set_header(dialog_ex, "Formatting...", 64, 32, AlignCenter, AlignCenter);
    view_dispatcher_switch_to_view(app->view_dispatcher, StorageSettingsViewDialogEx);

    notification_message_block(app->notification, &sequence_set_formatting_leds);
    error = storage_sd_format(app->fs_api);
    notification_message(app->notification, &sequence_reset_formatting_leds);
    notification_message(app->notification, &sequence_blink_green_100);

    if(error != FSE_OK) {
        dialog_ex_set_header(dialog_ex, "Cannot format SD Card", 64, 10, AlignCenter, AlignCenter);
        dialog_ex_set_text(
            dialog_ex, storage_error_get_desc(error), 64, 32, AlignCenter, AlignCenter);
    } else {
        dialog_ex_set_header(dialog_ex, "SD card formatted", 64, 10, AlignCenter, AlignCenter);
        dialog_ex_set_text(dialog_ex, "Press back to return", 64, 32, AlignCenter, AlignCenter);
    }

    dialog_ex_set_context(dialog_ex, app);
    dialog_ex_set_result_callback(dialog_ex, storage_settings_scene_formatting_dialog_callback);
    dialog_ex_set_left_button_text(dialog_ex, "Back");
}

bool storage_settings_scene_formatting_on_event(void* context, SceneManagerEvent event) {
    StorageSettings* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case DialogExResultLeft:
            consumed = scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, StorageSettingsStart);
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, StorageSettingsStart);
    }

    return consumed;
}

void storage_settings_scene_formatting_on_exit(void* context) {
    StorageSettings* app = context;
    DialogEx* dialog_ex = app->dialog_ex;

    dialog_ex_set_header(dialog_ex, NULL, 0, 0, AlignCenter, AlignCenter);
    dialog_ex_set_text(dialog_ex, NULL, 0, 0, AlignCenter, AlignTop);
    dialog_ex_set_icon(dialog_ex, 0, 0, NULL);
    dialog_ex_set_left_button_text(dialog_ex, NULL);
    dialog_ex_set_right_button_text(dialog_ex, NULL);
    dialog_ex_set_result_callback(dialog_ex, NULL);
    dialog_ex_set_context(dialog_ex, NULL);
}
