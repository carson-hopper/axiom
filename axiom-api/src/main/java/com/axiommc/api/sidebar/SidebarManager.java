package com.axiommc.api.sidebar;

import com.axiommc.api.chat.ChatComponent;

/** Factory for creating {@link Sidebar} instances. */
public interface SidebarManager {

    /** Creates a new sidebar with the given title. The sidebar is not shown to anyone until {@link Sidebar#show} is called. */
    Sidebar create(ChatComponent title);
}
