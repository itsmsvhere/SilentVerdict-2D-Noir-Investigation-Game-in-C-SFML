#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>

#include "SceneManager.h"
#include "UIManager.h"
#include "DialogueSystem.h"
#include "EvidenceSystem.h"
#include "AudioManager.h"

static const unsigned WIN_W = 900;
static const unsigned WIN_H = 620;

// ─── Emotion states for suspect portraits ─────────────────────
enum class Emotion { NEUTRAL, AGONY, CUFFED };

// ─── Fade transition ──────────────────────────────────────────
enum class FadeState { NONE, FADE_OUT, FADE_IN };

// ─── Clickable evidence icon ──────────────────────────────────
struct EvidenceIcon {
    sf::Sprite   sprite;
    sf::Texture  texture;
    bool         texLoaded = false;

    // Fallback shape (used if image missing)
    sf::CircleShape  circle;
    sf::Text         emoji;

    std::string title;
    std::string detail;
    float       cx, cy;   // centre position

    bool contains(sf::Vector2f p) const {
        if (texLoaded)
            return sprite.getGlobalBounds().contains(p);
        return circle.getGlobalBounds().contains(p);
    }
};

class Game {
public:
    Game();
    void run();

private:
    // ── Core ────────────────────────────────────────────────
    sf::RenderWindow window;
    SceneManager     scenes;
    UIManager        ui;
    DialogueSystem   dlg;
    EvidenceSystem   ev;
    AudioManager     audio;

    // ── Background textures ─────────────────────────────────
    sf::Texture texMenu, texIntro, texForest, texInterrogation, texVerdict;
    sf::Sprite  sprMenu, sprIntro, sprForest, sprInterrogation, sprVerdict;
    bool hasTexMenu=false, hasTexIntro=false, hasTexForest=false;
    bool hasTexInterrogation=false, hasTexVerdict=false;

    // ── Suspect portrait textures [suspect][emotion] ────────
    // 0=Husband 1=Chef 2=Colleague 3=Suicide
    // Emotion: 0=neutral 1=agony 2=cuffed
    sf::Texture suspectTex[4][3];
    sf::Sprite  suspectSpr[4][3];
    bool        suspectTexLoaded[4][3];

    // ── Evidence icon textures ───────────────────────────────
    std::vector<EvidenceIcon> evIcons;
    int revealedIcon = -1;

    // ── Helpers ─────────────────────────────────────────────
    bool loadTex(sf::Texture& tex, sf::Sprite& spr, const std::string& path);
    void scaleSprToFill(sf::Sprite& spr, sf::Texture& tex);
    void scaleSprToFit(sf::Sprite& spr, sf::Texture& tex, float maxW, float maxH);
    void drawBg(sf::Sprite& spr, bool has, sf::Color fallback);
    void loadSuspectTextures();
    void buildEvidenceIcons();

    // ── Fade ────────────────────────────────────────────────
    FadeState         fadeState = FadeState::NONE;
    float             fadeAlpha = 0.f;
    float             fadeSpeed = 300.f;
    Scene             fadeTarget = Scene::MENU;
    sf::Clock         fadeClock;
    sf::RectangleShape fadeRect;

    void startFade(Scene target);
    void updateFade();
    void renderFade();

    // ── Input ───────────────────────────────────────────────
    sf::Vector2f mp;
    bool         mc = false;

    // ── Interrogation state ─────────────────────────────────
    int                        suspIdx  = 0;
    std::vector<bool>          qAsked;
    std::map<std::string, int> suspicion;
    std::string                response;
    bool                       suspDone = false;
    bool                       allDone  = false;

    // Current emotion per suspect (changes as questions asked)
    Emotion suspectEmotion[4] = {
        Emotion::NEUTRAL, Emotion::NEUTRAL,
        Emotion::NEUTRAL, Emotion::NEUTRAL
    };

    // ── Verdict state ───────────────────────────────────────
    std::string vMsg;
    bool        vShown       = false;
    bool        showBackstory = false;   // show husband's motive after solve

    // ── Mute button ─────────────────────────────────────────
    Button btnMute;

    // ── Buttons ─────────────────────────────────────────────
    Button              btnStart, btnExit;
    std::vector<Button> qBtns;
    std::vector<Button> vBtns;

    // ── Loop ────────────────────────────────────────────────
    void handleEvents();
    void update();
    void render();

    // ── Scene setup ─────────────────────────────────────────
    void setupMenu();
    void setupInterrogation();
    void setupVerdict();
    void resetGame();
    void advanceSuspect();

    // ── Scene renderers ─────────────────────────────────────
    void renderMenu();
    void renderIntro();
    void renderForest();
    void renderInterrogation();
    void renderVerdict();

    // ── Portrait drawing ────────────────────────────────────
    void drawSuspectPortrait(int idx, Emotion emo, float cx, float cy);

    // ── Helpers ─────────────────────────────────────────────
    void     fillBg(sf::Color c);
    sf::Text centered(const std::string& s, unsigned sz, sf::Color c, float y);
    void     drawWrapped(const std::string& text, unsigned sz, sf::Color col,
                         float x, float y, float maxWidth);
};
