package com.axiommc.fabric.util;

import java.util.ArrayList;
import java.util.List;

public class TaskScheduler {
    private static final TaskScheduler GLOBAL = new TaskScheduler();

    private static class ScheduledTask {
        int ticksRemaining;
        Runnable task;

        ScheduledTask(int ticksRemaining, Runnable task) {
            this.ticksRemaining = ticksRemaining;
            this.task = task;
        }
    }

    private final List<ScheduledTask> tasks = new ArrayList<>();

    public static TaskScheduler global() {
        return GLOBAL;
    }

    public void scheduleTask(int ticks, Runnable task) {
        synchronized (tasks) {
            tasks.add(new ScheduledTask(ticks, task));
        }
    }

    public void tick() {
        synchronized (tasks) {
            for (int i = tasks.size() - 1; i >= 0; i--) {
                ScheduledTask scheduled = tasks.get(i);
                scheduled.ticksRemaining--;

                if (scheduled.ticksRemaining <= 0) {
                    try {
                        scheduled.task.run();
                    } catch (Exception e) {
                        // Don't fail on task execution errors - continue processing other tasks
                    }
                    tasks.remove(i);
                }
            }
        }
    }
}
