package com.axiommc.fabric.chat;

import com.axiommc.api.chat.ChatComponent;
import com.axiommc.api.chat.ChatComponentSerializer;
import net.minecraft.network.chat.Component;
import net.minecraft.network.chat.MutableComponent;
import net.minecraft.network.chat.Style;

public class FabricComponentSerializer implements ChatComponentSerializer<Component> {

    @Override
    public Component serialize(ChatComponent component) {
        if (component == null) {
            return Component.empty();
        }

        MutableComponent minecraftComponent = Component.literal(component.content());

        // Click and hover events are skipped for now (complex Minecraft event API)
        // TODO: Implement ClickEvent and HoverEvent support

        minecraftComponent.setStyle(style(component));
        for (ChatComponent child : component.children()) {
            minecraftComponent.append(serialize(child));
        }

        return minecraftComponent;
    }

    private static Style style(ChatComponent component) {
        Style style = Style.EMPTY;

        if (component.color() != null) {
            int rgb = component.color().rgb();
            style = style.withColor(rgb);
        }

        if (component.bold())
            style = style.withBold(true);
        if (component.italic())
            style = style.withItalic(true);
        if (component.underlined())
            style = style.withUnderlined(true);
        if (component.strikethrough())
            style = style.withStrikethrough(true);
        if (component.obfuscated())
            style = style.withObfuscated(true);
        return style;
    }
}
