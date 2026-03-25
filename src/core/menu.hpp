#pragma once

namespace menu {
    // ============================================
    // PUBLIC API
    // ============================================

    /// Initialize ImGui and menu systems
    void create();

    /// Shutdown ImGui and menu systems
    void destroy();

    /// Render the ImGui menu
    /// Call this every frame during the main render pass
    void render();

}  // namespace menu