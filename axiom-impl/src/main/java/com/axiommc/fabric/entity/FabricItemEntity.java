package com.axiommc.fabric.entity;

import com.axiommc.api.entity.ItemEntity;
import com.axiommc.api.item.Item;
import com.axiommc.api.item.ItemStack;
import com.axiommc.fabric.mixin.net.minecraft.world.entity.item.ItemEntityAccessor;

/**
 * Wraps a Minecraft {@link net.minecraft.world.entity.item.ItemEntity}
 * as an Axiom {@link ItemEntity}.
 */
public class FabricItemEntity extends FabricEntity implements ItemEntity {

    private final net.minecraft.world.entity.item.ItemEntity itemEntity;

    public FabricItemEntity(net.minecraft.world.entity.item.ItemEntity itemEntity) {
        super(itemEntity);
        this.itemEntity = itemEntity;
    }

    @Override
    public ItemStack itemStack() {
        net.minecraft.world.item.ItemStack mcStack = itemEntity.getItem();
        String key = mcStack.getItem().builtInRegistryHolder().key().identifier().toString();
        return ItemStack.of(Item.of(key), mcStack.getCount());
    }

    @Override
    public int pickupDelay() {
        return ((ItemEntityAccessor) itemEntity).pickupDelay();
    }

    @Override
    public void pickupDelay(int ticks) {
        itemEntity.setPickUpDelay(ticks);
    }
}
