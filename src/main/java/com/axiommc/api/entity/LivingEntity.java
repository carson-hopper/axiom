package com.axiommc.api.entity;

public interface LivingEntity extends Entity {

    double health();
    void health(double health);
    double maxHealth();

    void damage(double amount);
}
