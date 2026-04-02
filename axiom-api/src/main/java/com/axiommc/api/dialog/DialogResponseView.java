package com.axiommc.api.dialog;

import java.util.Optional;

public interface DialogResponseView {

    <T> Optional<T> get(Class<T> type, String key);

    <T> Optional<T> get(Class<T> type, String key, T defaultValue);
}
