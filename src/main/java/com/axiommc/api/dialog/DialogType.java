package com.axiommc.api.dialog;

import com.axiommc.api.chat.ChatComponent;

import java.util.Arrays;
import java.util.List;

public sealed interface DialogType permits DialogType.Notice, DialogType.Confirmation, DialogType.MultiAction {

    record Notice(DialogButton button) implements DialogType {}

    record Confirmation(DialogButton yes, DialogButton no) implements DialogType {}

    record MultiAction(List<DialogButton> buttons) implements DialogType {}

    // ── Factory methods ───────────────────────────────────────────────────────

    /** Creates a notice dialog with a default "OK" button that has no callback. */
    static DialogType notice() {
        return new Notice(DialogButton.close(ChatComponent.text("OK")));
    }

    static DialogType notice(DialogButton button) {
        return new Notice(button);
    }

    static DialogType confirmation(DialogButton yes, DialogButton no) {
        return new Confirmation(yes, no);
    }

    static DialogType multiAction(List<DialogButton> buttons) {
        return new MultiAction(List.copyOf(buttons));
    }

    static DialogType multiAction(DialogButton... buttons) {
        return new MultiAction(List.copyOf(Arrays.asList(buttons)));
    }

}
