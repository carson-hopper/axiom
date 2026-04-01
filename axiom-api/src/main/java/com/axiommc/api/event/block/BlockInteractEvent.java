package com.axiommc.api.event.block;

import com.axiommc.api.event.Cancellable;
import com.axiommc.api.event.Event;
import com.axiommc.api.player.Player;
import com.axiommc.api.world.block.Block;

public class BlockInteractEvent {

    public class MainHand extends Event implements Cancellable {

        private final Player player;
        private final Block block;
        private boolean cancelled = false;

        public MainHand(Player player, Block block) {
            this.player = player;
            this.block = block;
        }

        public Player player() {
            return player;
        }

        public Block block() {
            return block;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

    public class OffHand extends Event implements Cancellable {

        private final Player player;
        private final Block block;
        private boolean cancelled = false;

        public OffHand(Player player, Block block) {
            this.player = player;
            this.block = block;
        }

        public Player player() {
            return player;
        }

        public Block block() {
            return block;
        }

        @Override
        public boolean isCancelled() {
            return cancelled;
        }

        @Override
        public void cancelled(boolean cancelled) {
            this.cancelled = cancelled;
        }
    }

}
