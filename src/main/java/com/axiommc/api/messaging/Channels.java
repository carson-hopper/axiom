package com.axiommc.api.messaging;

public final class Channels {

    private Channels() {}

    public static final String MODULE_BEGIN = "helix:module_begin";
    public static final String MODULE_CHUNK = "helix:module_chunk";
    public static final String MODULE_FINALIZE = "helix:module_finalize";
    public static final String MODULE_UNLOAD = "helix:module_unload";
    public static final String MODULE_STATUS = "helix:module_status";
    public static final String MODULE_SYNC_REQUEST = "helix:module_sync_request";
    public static final String MODULE_MESSAGE = "helix:module_message";
    public static final String OPS_SYNC = "helix:ops_sync";
    public static final String OPS_SERVER_SYNC = "helix:ops_server_sync";

    public static final String GUI_OPEN   = "helix:gui_open";
    public static final String GUI_CLICK  = "helix:gui_click";
    public static final String GUI_CLOSE  = "helix:gui_close";
    public static final String GUI_UPDATE = "helix:gui_update";

    public static final String EVENT_FORWARD = "helix:event_forward";

    public static final String SOUND_PLAY      = "helix:sound_play";
    public static final String PLAYER_TELEPORT = "helix:player_teleport";
}
