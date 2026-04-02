package com.axiommc.fabric.mixin.net.minecraft.world.entity.item;

import net.minecraft.world.entity.item.ItemEntity;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.gen.Accessor;

@Mixin(value = ItemEntity.class, remap = false)
public interface ItemEntityAccessor {

    @Accessor("pickupDelay")
    int pickupDelay();
}
