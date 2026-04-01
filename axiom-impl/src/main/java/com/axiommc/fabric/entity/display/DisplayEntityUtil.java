package com.axiommc.fabric.entity.display;

import com.axiommc.api.entity.display.BlockDisplaySpec;
import com.axiommc.api.entity.display.ItemDisplaySpec;
import com.axiommc.api.entity.display.TextDisplaySpec;
import net.minecraft.network.chat.Component;
import net.minecraft.world.entity.Display;
import com.axiommc.fabric.Axiom;

public class DisplayEntityUtil {

    public static void applyTextDisplaySpec(Display.TextDisplay entity, TextDisplaySpec spec) {
        try {
            Axiom.logger().debug("TextDisplay spec application is limited in Minecraft 26.1 - consider using datapacks for full customization");
        } catch (Exception e) {
            Axiom.logger().warn("Failed to apply TextDisplaySpec properties", e);
        }
    }

    public static void applyItemDisplaySpec(Display.ItemDisplay entity, ItemDisplaySpec spec) {
        try {
            Axiom.logger().debug("ItemDisplay spec application is limited in Minecraft 26.1 - consider using datapacks for full customization");
        } catch (Exception e) {
            Axiom.logger().warn("Failed to apply ItemDisplaySpec properties", e);
        }
    }

    public static void applyBlockDisplaySpec(Display.BlockDisplay entity, BlockDisplaySpec spec) {
        try {
            Axiom.logger().debug("BlockDisplay spec application is limited in Minecraft 26.1 - consider using datapacks for full customization");
        } catch (Exception e) {
            Axiom.logger().warn("Failed to apply BlockDisplaySpec properties", e);
        }
    }
}
