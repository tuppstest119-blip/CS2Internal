#pragma once

namespace esp {
    // Hauptfunktion zum Zeichnen des ESPs.
    // Diese rufst du in deinem ImGui-Render-Loop auf (z.B. nach ImGui::NewFrame()).
    void render();
}  // namespace esp