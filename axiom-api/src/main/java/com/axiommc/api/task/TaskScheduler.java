package com.axiommc.api.task;

import java.util.concurrent.TimeUnit;

public interface TaskScheduler {
    /**
     * Schedules {@code task} to run once after {@code delay} has elapsed.
     */
    void runLater(Runnable task, long delay, TimeUnit unit);
}
