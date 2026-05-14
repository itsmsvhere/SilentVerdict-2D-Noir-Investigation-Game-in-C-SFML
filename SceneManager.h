#pragma once

enum class Scene {
    MENU,
    INTRO,
    FOREST,
    INTERROGATION,
    VERDICT
};

class SceneManager {
public:
    SceneManager() : current(Scene::MENU) {}
    void  set(Scene s) { current = s; }
    Scene get() const  { return current; }
private:
    Scene current;
};
