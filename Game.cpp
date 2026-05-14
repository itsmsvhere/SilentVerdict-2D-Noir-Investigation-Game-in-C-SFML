#include "Game.h"
#include <algorithm>
#include <cmath>
#include <sstream>

// ─── Palette ─────────────────────────────────────────────────
static const sf::Color C_BG     {10,  12,  20};
static const sf::Color C_PANEL  {20,  22,  40, 218};
static const sf::Color C_PANEL2 {14,  16,  28, 225};
static const sf::Color C_BTN    {48,  53,  82};
static const sf::Color C_BHOV   {92, 112, 165};
static const sf::Color C_ACCENT {188, 148,  52};
static const sf::Color C_TEXT   {210, 210, 222};
static const sf::Color C_DIM    {112, 112, 132};
static const sf::Color C_GREEN  { 55, 188, 100};
static const sf::Color C_RED    {208,  58,  58};
static const sf::Color C_ARRED  { 64,  28,  28};
static const sf::Color C_ARRHOV {128,  50,  50};
static const sf::Color C_AGONY  {200,  60,  60};

static const sf::Color SUSP_COL[4] = {
    {190,  55,  55},
    { 55, 140, 190},
    { 55, 180, 100},
    {160, 110, 200}
};

static const std::string BACKSTORY =
    "MOTIVE REVEALED\n\n"
    "The Husband had grown increasingly controlling.\n"
    "He resented Elena's long hours with her colleague\n"
    "and accused her of prioritising work over him.\n"
    "That night, a heated argument turned physical.\n\n"
    "Elena fled into the forest to escape.\n"
    "He followed. The confrontation escalated.\n"
    "What began as a fight ended in tragedy.\n\n"
    "The colleague was innocent. It was never an affair.\n"
    "It was a marriage that broke under its own rage.";

// ─── Constructor ─────────────────────────────────────────────
Game::Game()
    : window(sf::VideoMode(WIN_W, WIN_H), "Silent Verdict",
             sf::Style::Titlebar | sf::Style::Close)
{
    window.setFramerateLimit(60);

    bool ok = ui.loadFont("arial.ttf");
    if (!ok) ok = ui.loadFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    if (!ok) ok = ui.loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    if (!ok) ok = ui.loadFont("/System/Library/Fonts/Helvetica.ttc");
    if (!ok) ok = ui.loadFont("C:/Windows/Fonts/arial.ttf");
    if (!ok)      ui.loadFont("font.ttf");

    audio.loadAll();

    hasTexMenu          = loadTex(texMenu,         sprMenu,          "assets/bg_menu.png");
    hasTexIntro         = loadTex(texIntro,         sprIntro,         "assets/bg_intro.png");
    hasTexForest        = loadTex(texForest,        sprForest,        "assets/bg_forest.png");
    hasTexInterrogation = loadTex(texInterrogation, sprInterrogation, "assets/bg_interrogation.png");
    hasTexVerdict       = loadTex(texVerdict,       sprVerdict,       "assets/bg_verdict.png");

    loadSuspectTextures();

    fadeRect.setSize({(float)WIN_W, (float)WIN_H});
    fadeRect.setPosition(0, 0);

    btnMute.init(WIN_W-70, 8, 60, 28, "MUTE", ui.font(), 13);

    resetGame();
    setupMenu();
}

bool Game::loadTex(sf::Texture& tex, sf::Sprite& spr, const std::string& path) {
    if (!tex.loadFromFile(path)) return false;
    tex.setSmooth(true);
    spr.setTexture(tex);
    scaleSprToFill(spr, tex);
    return true;
}

void Game::scaleSprToFill(sf::Sprite& spr, sf::Texture& tex) {
    float sx = (float)WIN_W / tex.getSize().x;
    float sy = (float)WIN_H / tex.getSize().y;
    spr.setScale(sx, sy);
}

void Game::scaleSprToFit(sf::Sprite& spr, sf::Texture& tex, float maxW, float maxH) {
    float sx = maxW / tex.getSize().x;
    float sy = maxH / tex.getSize().y;
    spr.setScale(std::min(sx, sy), std::min(sx, sy));
}

void Game::loadSuspectTextures() {
    const std::string paths[4][3] = {
        {"assets/images/husband_neutral.png",
         "assets/images/husband_agony.png",
         "assets/images/husband_cuffed.png"},
        {"assets/images/chef_neutral.png",
         "assets/images/chef_agony.png",
         "assets/images/chef_agony.png"},
        {"assets/images/colleague_neutral.png",
         "assets/images/colleague_agony.png",
         "assets/images/colleague_agony.png"},
        {"assets/images/suicide_neutral.png",
         "assets/images/suicide_agony.png",
         "assets/images/suicide_agony.png"}
    };
    for (int s = 0; s < 4; s++) {
        for (int e = 0; e < 3; e++) {
            suspectTexLoaded[s][e] = suspectTex[s][e].loadFromFile(paths[s][e]);
            if (suspectTexLoaded[s][e]) {
                suspectTex[s][e].setSmooth(true);
                suspectSpr[s][e].setTexture(suspectTex[s][e]);
            }
        }
    }
}

void Game::buildEvidenceIcons() {
    evIcons.clear();
    revealedIcon = -1;

    struct IconDef {
        std::string imgPath, sym;
        float x, y;
        sf::Color col;
    };
    std::vector<IconDef> defs = {
        {"assets/images/evidence_knife.png",      u8"\U0001F52A", 200, 270, {220,80,80}},
        {"assets/images/evidence_footprints.png", u8"\U0001F463", 450, 270, {180,140,80}},
        {"assets/images/evidence_phone.png",      u8"\U0001F4F1", 700, 270, {80,160,220}},
    };

    const auto& evList = ev.getList();
    for (int i = 0; i < (int)defs.size() && i < (int)evList.size(); i++) {
        EvidenceIcon icon;
        icon.cx = defs[i].x;
        icon.cy = defs[i].y;
        icon.title  = evList[i].name;
        icon.detail = evList[i].description;

        icon.texLoaded = icon.texture.loadFromFile(defs[i].imgPath);
        if (icon.texLoaded) {
            icon.texture.setSmooth(true);
            icon.sprite.setTexture(icon.texture);
            float sc = 90.f / std::max(icon.texture.getSize().x, icon.texture.getSize().y);
            icon.sprite.setScale(sc, sc);
            float sw = icon.texture.getSize().x * sc;
            float sh = icon.texture.getSize().y * sc;
            icon.sprite.setPosition(icon.cx - sw/2.f, icon.cy - sh/2.f);
        } else {
            icon.circle.setRadius(52.f);
            icon.circle.setOrigin(52.f, 52.f);
            icon.circle.setPosition(icon.cx, icon.cy);
            icon.circle.setFillColor(sf::Color(defs[i].col.r,defs[i].col.g,defs[i].col.b,55));
            icon.circle.setOutlineColor(defs[i].col);
            icon.circle.setOutlineThickness(2.f);
            icon.emoji = ui.makeText(defs[i].sym, 38, sf::Color::White, 0, 0);
            auto eb = icon.emoji.getLocalBounds();
            icon.emoji.setOrigin(eb.left+eb.width/2.f, eb.top+eb.height/2.f);
            icon.emoji.setPosition(icon.cx, icon.cy);
        }
        evIcons.push_back(std::move(icon));
    }
}

void Game::drawBg(sf::Sprite& spr, bool has, sf::Color fallback) {
    if (has) {
        window.draw(spr);
        sf::RectangleShape ov({(float)WIN_W,(float)WIN_H});
        ov.setFillColor(sf::Color(0,0,0,148));
        window.draw(ov);
    } else {
        fillBg(fallback);
    }
}

void Game::startFade(Scene target) {
    if (fadeState != FadeState::NONE) return;
    fadeTarget = target;
    fadeState  = FadeState::FADE_OUT;
    fadeAlpha  = 0.f;
    fadeClock.restart();
    audio.play("transition");
}

void Game::updateFade() {
    if (fadeState == FadeState::NONE) return;
    float dt = fadeClock.restart().asSeconds();
    if (fadeState == FadeState::FADE_OUT) {
        fadeAlpha += fadeSpeed * dt;
        if (fadeAlpha >= 255.f) {
            fadeAlpha = 255.f;
            scenes.set(fadeTarget);
            if (fadeTarget == Scene::INTERROGATION) setupInterrogation();
            if (fadeTarget == Scene::VERDICT)       setupVerdict();
            if (fadeTarget == Scene::MENU)          setupMenu();
            if (fadeTarget == Scene::FOREST)        buildEvidenceIcons();
            fadeState = FadeState::FADE_IN;
        }
    } else if (fadeState == FadeState::FADE_IN) {
        fadeAlpha -= fadeSpeed * dt;
        if (fadeAlpha <= 0.f) { fadeAlpha = 0.f; fadeState = FadeState::NONE; }
    }
}

void Game::renderFade() {
    if (fadeAlpha <= 0.f) return;
    fadeRect.setFillColor(sf::Color(0,0,0,(sf::Uint8)std::min(fadeAlpha,255.f)));
    window.draw(fadeRect);
}

void Game::run() {
    while (window.isOpen()) {
        mc = false;
        handleEvents();
        updateFade();
        update();
        render();
    }
}

void Game::handleEvents() {
    sf::Event e;
    while (window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) window.close();
        if (e.type == sf::Event::MouseButtonReleased &&
            e.mouseButton.button == sf::Mouse::Left)
            mc = true;
        if (e.type == sf::Event::KeyPressed &&
            e.key.code == sf::Keyboard::Enter &&
            fadeState == FadeState::NONE) {
            auto s = scenes.get();
            if      (s == Scene::INTRO)  startFade(Scene::FOREST);
            else if (s == Scene::FOREST) startFade(Scene::INTERROGATION);
            else if (s == Scene::INTERROGATION) {
                if      (allDone)  startFade(Scene::VERDICT);
                else if (suspDone) advanceSuspect();
            }
            else if (s == Scene::VERDICT && vShown) {
                resetGame();
                startFade(Scene::MENU);
            }
        }
    }
    mp = window.mapPixelToCoords(sf::Mouse::getPosition(window));
}

void Game::update() {
    if (fadeState != FadeState::NONE) return;
    auto s = scenes.get();

    btnMute.update(mp);
    if (btnMute.clicked(mp, mc)) {
        audio.setMuted(!audio.isMuted());
        btnMute.label.setString(audio.isMuted() ? "UNMUTE" : "MUTE");
        audio.play("click");
    }

    if (s == Scene::MENU) {
        btnStart.update(mp); btnExit.update(mp);
        if (btnStart.clicked(mp, mc)) { audio.play("click"); startFade(Scene::INTRO); }
        if (btnExit.clicked(mp, mc))  { audio.play("click"); window.close(); }
    }

    if (s == Scene::FOREST) {
        for (int i = 0; i < (int)evIcons.size(); i++) {
            if (mc && evIcons[i].contains(mp)) {
                audio.play("click");
                revealedIcon = (revealedIcon == i) ? -1 : i;
            }
        }
    }

    if (s == Scene::INTERROGATION && !allDone) {
        for (int i = 0; i < 5; i++) {
            qBtns[i].update(mp);
            if (!qAsked[i] && qBtns[i].clicked(mp, mc)) {
                audio.play("click");
                response = dlg.getResponse(suspIdx, i);
                dlg.applyQuestion(suspIdx, i, suspicion);
                qAsked[i] = true;
                int asked = 0;
                for (bool b : qAsked) if (b) asked++;
                if (asked >= 3) suspectEmotion[suspIdx] = Emotion::AGONY;
                bool all = true;
                for (bool b : qAsked) if (!b) { all=false; break; }
                if (all) suspDone = true;
            }
        }
    }

    if (s == Scene::VERDICT && !vShown) {
        for (int i = 0; i < 4; i++) {
            vBtns[i].update(mp);
            if (vBtns[i].clicked(mp, mc)) {
                audio.play("click");
                if (i == 0) {
                    vMsg = "CASE SOLVED";
                    suspectEmotion[0] = Emotion::CUFFED;
                    audio.play("husband_grunt");
                } else {
                    vMsg = "CASE FAILED";
                    audio.play("evil_laugh");
                }
                vShown = true;
            }
        }
    }
}

void Game::render() {
    window.clear(C_BG);
    switch (scenes.get()) {
        case Scene::MENU:          renderMenu();          break;
        case Scene::INTRO:         renderIntro();         break;
        case Scene::FOREST:        renderForest();        break;
        case Scene::INTERROGATION: renderInterrogation(); break;
        case Scene::VERDICT:       renderVerdict();       break;
    }
    btnMute.draw(window);
    renderFade();
    window.display();
}

void Game::fillBg(sf::Color c) {
    sf::RectangleShape r({(float)WIN_W,(float)WIN_H});
    r.setFillColor(c); window.draw(r);
}

sf::Text Game::centered(const std::string& s, unsigned sz, sf::Color c, float y) {
    auto t = ui.makeText(s,sz,c,0,0);
    auto b = t.getLocalBounds();
    t.setOrigin(b.left+b.width/2.f, b.top+b.height/2.f);
    t.setPosition(WIN_W/2.f, y);
    return t;
}

void Game::drawWrapped(const std::string& text, unsigned sz, sf::Color col,
                        float x, float y, float maxWidth) {
    std::istringstream stream(text);
    std::string line;
    float cy = y;
    while (std::getline(stream, line)) {
        if (line.empty()) { cy += sz * 0.8f; continue; }
        int charsPerLine = (int)(maxWidth / (sz * 0.55f));
        while ((int)line.size() > charsPerLine) {
            int cut = charsPerLine;
            while (cut > 0 && line[cut] != ' ') cut--;
            if (!cut) cut = charsPerLine;
            window.draw(ui.makeText(line.substr(0, cut), sz, col, x, cy));
            line = line.substr(cut+1);
            cy += sz * 1.4f;
        }
        window.draw(ui.makeText(line, sz, col, x, cy));
        cy += sz * 1.4f;
    }
}

void Game::drawSuspectPortrait(int idx, Emotion emo, float cx, float cy) {
    int emoIdx = (emo == Emotion::NEUTRAL) ? 0 :
                 (emo == Emotion::AGONY)   ? 1 : 2;

    if (suspectTexLoaded[idx][emoIdx]) {
        auto& spr = suspectSpr[idx][emoIdx];
        auto& tex = suspectTex[idx][emoIdx];
        float maxW=130.f, maxH=180.f;
        float sc = std::min(maxW/tex.getSize().x, maxH/tex.getSize().y);
        spr.setScale(sc, sc);
        float sw = tex.getSize().x * sc;
        float sh = tex.getSize().y * sc;
        spr.setPosition(cx-sw/2.f, cy-sh/2.f);
        if      (emo == Emotion::AGONY)  spr.setColor(sf::Color(255,180,180,230));
        else if (emo == Emotion::CUFFED) spr.setColor(sf::Color(180,180,255,230));
        else                             spr.setColor(sf::Color::White);
        window.draw(spr);
        if (emo != Emotion::NEUTRAL) {
            std::string badge = (emo==Emotion::AGONY) ? "DISTRESSED" : "ARRESTED";
            sf::Color   bcol  = (emo==Emotion::AGONY) ? C_AGONY : sf::Color(80,80,220);
            sf::RectangleShape bar({130.f,22.f});
            bar.setFillColor(sf::Color(bcol.r,bcol.g,bcol.b,200));
            bar.setPosition(cx-65.f, cy+sh/2.f-10.f);
            window.draw(bar);
            auto bl = ui.makeText(badge,12,sf::Color::White,0,0);
            auto bb = bl.getLocalBounds();
            bl.setOrigin(bb.left+bb.width/2.f, bb.top+bb.height/2.f);
            bl.setPosition(cx, cy+sh/2.f+1.f);
            window.draw(bl);
        }
    } else {
        sf::Color col = SUSP_COL[idx];
        if (emo == Emotion::AGONY)  col = sf::Color(200,60,60);
        if (emo == Emotion::CUFFED) col = sf::Color(80,80,200);

        sf::CircleShape glow(58.f);
        glow.setOrigin(58.f,58.f); glow.setPosition(cx,cy);
        glow.setFillColor(sf::Color(col.r,col.g,col.b,28));
        glow.setOutlineColor(sf::Color(col.r,col.g,col.b,110));
        glow.setOutlineThickness(3.f);
        window.draw(glow);

        sf::CircleShape head(38.f);
        head.setOrigin(38.f,38.f); head.setPosition(cx,cy);
        head.setFillColor(sf::Color(55,58,78));
        head.setOutlineColor(col); head.setOutlineThickness(2.f);
        window.draw(head);

        sf::RectangleShape body({76.f,95.f});
        body.setOrigin(38.f,0.f); body.setPosition(cx,cy+42.f);
        body.setFillColor(sf::Color(42,45,65));
        body.setOutlineColor(col); body.setOutlineThickness(1.5f);
        window.draw(body);

        for (int side : {-1,1}) {
            sf::CircleShape eye(5.f);
            eye.setOrigin(5.f,5.f);
            eye.setPosition(cx+side*14.f, cy-8.f);
            eye.setFillColor(col);
            window.draw(eye);
        }

        sf::RectangleShape mouth({22.f,3.f});
        mouth.setOrigin(11.f,1.5f); mouth.setPosition(cx,cy+14.f);
        mouth.setFillColor(emo==Emotion::AGONY ? C_AGONY :
                           sf::Color(col.r,col.g,col.b,180));
        window.draw(mouth);

        if (emo == Emotion::CUFFED) {
            for (int side : {-1,1}) {
                sf::CircleShape cuff(8.f,20);
                cuff.setOrigin(8.f,8.f);
                cuff.setPosition(cx+side*20.f, cy+100.f);
                cuff.setFillColor(sf::Color::Transparent);
                cuff.setOutlineColor(sf::Color(200,200,220));
                cuff.setOutlineThickness(3.f);
                window.draw(cuff);
            }
            sf::RectangleShape chain({28.f,3.f});
            chain.setOrigin(14.f,1.5f); chain.setPosition(cx,cy+100.f);
            chain.setFillColor(sf::Color(200,200,220));
            window.draw(chain);
        }

        if (emo != Emotion::NEUTRAL) {
            std::string lbl = (emo==Emotion::AGONY) ? "DISTRESSED" : "ARRESTED";
            sf::Color   lc  = (emo==Emotion::AGONY) ? C_AGONY : sf::Color(80,80,220);
            sf::RectangleShape bar({130.f,20.f});
            bar.setFillColor(sf::Color(lc.r,lc.g,lc.b,200));
            bar.setPosition(cx-65.f, cy+140.f);
            window.draw(bar);
            auto lt = ui.makeText(lbl,12,sf::Color::White,0,0);
            auto lb2 = lt.getLocalBounds();
            lt.setOrigin(lb2.left+lb2.width/2.f, lb2.top+lb2.height/2.f);
            lt.setPosition(cx, cy+150.f);
            window.draw(lt);
        }

        sf::CircleShape badge(14.f);
        badge.setOrigin(14.f,14.f); badge.setPosition(cx+46.f,cy-46.f);
        badge.setFillColor(col); window.draw(badge);
        auto num = ui.makeText(std::to_string(idx+1),13,sf::Color::White,0,0);
        auto nb  = num.getLocalBounds();
        num.setOrigin(nb.left+nb.width/2.f,nb.top+nb.height/2.f);
        num.setPosition(cx+46.f,cy-46.f);
        window.draw(num);
    }
}

void Game::setupMenu() {
    auto& f = ui.font();
    btnStart.init(WIN_W/2.f-115,300,230,52,"Start Game",f,21);
    btnExit .init(WIN_W/2.f-115,368,230,52,"Exit",f,21);
}

void Game::setupInterrogation() {
    auto& f = ui.font();
    qBtns.clear(); qBtns.resize(5);
    for (int i=0;i<5;i++)
        qBtns[i].init(470,188+i*58.f,392,46,QUESTIONS[i],f,15);
}

void Game::setupVerdict() {
    auto& f = ui.font();
    const std::vector<std::string> lb = {
        "Arrest Husband","Arrest Chef","Arrest Colleague","Declare Suicide"
    };
    vBtns.clear(); vBtns.resize(4);
    for (int i=0;i<4;i++){
        vBtns[i].init(WIN_W/2.f-130,228+i*70.f,260,54,lb[i],f,19);
        vBtns[i].recolor(C_ARRED,C_ARRHOV);
    }
}

void Game::resetGame() {
    suspIdx=0; qAsked.assign(5,false); suspicion.clear();
    for (auto& s:SUSPECTS) suspicion[s]=0;
    response=""; suspDone=false; allDone=false;
    vMsg=""; vShown=false; showBackstory=false;
    for (int i=0;i<4;i++) suspectEmotion[i]=Emotion::NEUTRAL;
    ev.reset(); dlg.reset();
    buildEvidenceIcons();
}

void Game::advanceSuspect() {
    suspIdx++;
    if (suspIdx>=(int)SUSPECTS.size()) allDone=true;
    else { qAsked.assign(5,false); response=""; suspDone=false; }
}

void Game::renderMenu() {
    drawBg(sprMenu,hasTexMenu,C_BG);
    if (!hasTexMenu) {
        for (int i=0;i<8;i++){
            sf::RectangleShape l({(float)WIN_W,1});
            l.setFillColor(sf::Color(34,36,56));
            l.setPosition(0,55+i*80.f); window.draw(l);
        }
    }
    auto sh=centered("SILENT VERDICT",54,sf::Color(0,0,0,160),158);
    sh.move(3,3); window.draw(sh);
    window.draw(centered("SILENT VERDICT",54,C_ACCENT,155));
    window.draw(centered("A 2D Investigation Thriller",17,C_DIM,218));
    sf::RectangleShape div({300.f,2});
    div.setFillColor(C_ACCENT); div.setPosition(WIN_W/2.f-150,256); window.draw(div);
    btnStart.draw(window); btnExit.draw(window);
    window.draw(centered("Mouse to interact  |  ENTER advances scenes",13,C_DIM,WIN_H-20));
}

void Game::renderIntro() {
    drawBg(sprIntro,hasTexIntro,C_BG);
    ui.drawPanel(window,70,90,WIN_W-140,360,sf::Color(8,10,20,215));
    auto sh=centered("CASE FILE  #0047",20,sf::Color(0,0,0,150),148);
    sh.move(2,2); window.draw(sh);
    window.draw(centered("CASE FILE  #0047",20,C_ACCENT,145));
    sf::RectangleShape div({WIN_W-210.f,1});
    div.setFillColor(C_ACCENT); div.setPosition(105,175); window.draw(div);
    std::string story =
        "A young girl was found dead in a remote forest...\n\n"
        "Her name was Elena Voss, 29.\n"
        "Found at dawn by a jogger - no signs of distress\n"
        "reported the previous evening.\n\n"
        "Four persons of interest have been identified.\n"
        "You must question each one and reach a verdict.";
    for (auto& l:ui.makeBlock(story,17,C_TEXT,112,192,27)) window.draw(l);
    window.draw(centered("Press  ENTER  to begin the investigation...",14,C_DIM,WIN_H-38));
}

void Game::renderForest() {
    drawBg(sprForest,hasTexForest,sf::Color(8,18,10));
    if (!hasTexForest) {
        for (int i=0;i<13;i++){
            sf::RectangleShape trunk({10.f,78.f});
            trunk.setFillColor(sf::Color(35,24,8));
            trunk.setPosition(14+i*68.f,WIN_H-90); window.draw(trunk);
            sf::CircleShape top(33.f);
            top.setFillColor(sf::Color(14,44,14));
            top.setPosition(2+i*68.f,WIN_H-175); window.draw(top);
        }
    }
    ui.drawPanel(window,50,22,WIN_W-100,105,sf::Color(8,10,20,215));
    window.draw(centered("EVIDENCE FOUND",22,C_ACCENT,52));
    window.draw(centered("Click each icon to reveal details",14,C_DIM,85));
    sf::RectangleShape div({WIN_W-200.f,1});
    div.setFillColor(C_ACCENT); div.setPosition(100,105); window.draw(div);

    for (int i=0;i<(int)evIcons.size();i++) {
        auto& icon = evIcons[i];
        bool revealed = (revealedIcon == i);
        if (revealed) {
            sf::CircleShape pulse(62.f);
            pulse.setOrigin(62.f,62.f);
            pulse.setPosition(icon.cx, icon.cy);
            pulse.setFillColor(sf::Color::Transparent);
            pulse.setOutlineColor(sf::Color(188,148,52,200));
            pulse.setOutlineThickness(3.f);
            window.draw(pulse);
        }
        if (icon.texLoaded) window.draw(icon.sprite);
        else { window.draw(icon.circle); window.draw(icon.emoji); }

        auto lbl = ui.makeText(icon.title,14,revealed?C_ACCENT:C_DIM,0,0);
        auto lb  = lbl.getLocalBounds();
        lbl.setOrigin(lb.left+lb.width/2.f, lb.top+lb.height/2.f);
        lbl.setPosition(icon.cx, icon.cy+75.f);
        window.draw(lbl);
    }

    if (revealedIcon >= 0 && revealedIcon < (int)evIcons.size()) {
        auto& icon = evIcons[revealedIcon];
        float pw=300.f, ph=90.f;
        float px=std::max(10.f,std::min(icon.cx-pw/2.f,(float)WIN_W-pw-10.f));
        float py=icon.cy+90.f;
        ui.drawPanel(window,px,py,pw,ph,sf::Color(16,18,32,248));
        sf::RectangleShape topBar({pw,3.f});
        topBar.setFillColor(C_ACCENT); topBar.setPosition(px,py); window.draw(topBar);
        window.draw(ui.makeText(icon.title,15,C_ACCENT,px+14,py+10));
        std::string rem=icon.detail; float dy=py+34;
        while (!rem.empty()){
            int cut=42;
            if((int)rem.size()<=cut){window.draw(ui.makeText(rem,13,C_TEXT,px+14,dy));break;}
            while(cut>0&&rem[cut]!=' ')cut--;
            if(!cut)cut=42;
            window.draw(ui.makeText(rem.substr(0,cut),13,C_TEXT,px+14,dy));
            rem=rem.substr(cut+1); dy+=18;
        }
    }
    window.draw(centered("Press  ENTER  to begin interrogations...",14,C_DIM,WIN_H-25));
}

void Game::renderInterrogation() {
    drawBg(sprInterrogation,hasTexInterrogation,sf::Color(11,13,22));

    if (allDone) {
        ui.drawPanel(window,95,110,WIN_W-190,340,sf::Color(8,10,20,215));
        window.draw(centered("All suspects questioned.",24,C_ACCENT,205));
        sf::RectangleShape d({WIN_W-280.f,1});
        d.setFillColor(sf::Color(58,60,82)); d.setPosition(140,245); window.draw(d);
        float sy=262;
        for (auto& kv:suspicion){
            window.draw(ui.makeText(kv.first+"  -  "+
                std::to_string(kv.second)+" suspicion pts",17,C_TEXT,WIN_W/2.f-170,sy));
            sy+=32;
        }
        window.draw(centered("Press  ENTER  to deliver your verdict...",14,C_DIM,WIN_H-38));
        return;
    }

    const std::string& sName = SUSPECTS[suspIdx];
    Emotion emo = suspectEmotion[suspIdx];

    ui.drawPanel(window,14,14,448,WIN_H-28,sf::Color(8,10,20,210));
    window.draw(ui.makeText("SUSPECT",13,C_DIM,36,36));
    window.draw(ui.makeText(sName,26,SUSP_COL[suspIdx],36,58));

    sf::RectangleShape div1({392.f,1});
    div1.setFillColor(sf::Color(52,55,75)); div1.setPosition(36,100); window.draw(div1);

    int sc=suspicion.at(sName);
    window.draw(ui.makeText("Suspicion: "+std::to_string(sc)+" pts",15,C_DIM,36,112));
    sf::RectangleShape scBar({(float)std::min(sc*2,380),5.f});
    scBar.setFillColor(SUSP_COL[suspIdx]); scBar.setPosition(36,132); window.draw(scBar);

    drawSuspectPortrait(suspIdx,emo,210.f,220.f);

    std::string emoStr = (emo==Emotion::NEUTRAL)?"Composed":
                         (emo==Emotion::AGONY)  ?"Distressed":"Arrested";
    sf::Color emoCol = (emo==Emotion::NEUTRAL)?C_DIM:
                       (emo==Emotion::AGONY)  ?C_AGONY:sf::Color(100,100,220);
    window.draw(ui.makeText("Status: "+emoStr,14,emoCol,36,340));

    ui.drawPanel(window,24,358,428,218,sf::Color(10,12,24,228));
    window.draw(ui.makeText("Response:",13,C_DIM,44,372));
    if (!response.empty()){
        std::string rem=response; float ry=394;
        while(!rem.empty()){
            int cut=50;
            if((int)rem.size()<=cut){window.draw(ui.makeText(rem,15,C_TEXT,44,ry));break;}
            while(cut>0&&rem[cut]!=' ')cut--;
            if(!cut)cut=50;
            window.draw(ui.makeText(rem.substr(0,cut),15,C_TEXT,44,ry));
            rem=rem.substr(cut+1); ry+=22;
        }
    } else {
        window.draw(ui.makeText("Click a question to interrogate...",15,C_DIM,44,394));
    }

    int asked=0; for(bool b:qAsked)if(b)asked++;
    window.draw(ui.makeText("Questions: "+std::to_string(asked)+" / 5",13,C_DIM,36,WIN_H-42));

    ui.drawPanel(window,466,14,WIN_W-480,WIN_H-28,sf::Color(12,14,26,215));
    window.draw(ui.makeText("QUESTIONS",15,C_DIM,490,36));
    window.draw(ui.makeText("( ask all 5 to advance )",12,C_DIM,490,58));
    for(int i=0;i<5;i++){
        if(qAsked[i]){
            qBtns[i].shape.setFillColor(sf::Color(24,26,42));
            qBtns[i].label.setFillColor(C_DIM);
        }
        qBtns[i].draw(window);
        if(qAsked[i]){
            sf::CircleShape tick(4.f); tick.setFillColor(C_GREEN);
            tick.setPosition(466+386,188+i*58.f+19); window.draw(tick);
        }
    }
    if(suspDone)
        window.draw(ui.makeText("All asked! Press ENTER for next suspect.",
                                 13,sf::Color(148,200,130),470,WIN_H-40));
    window.draw(ui.makeText("Suspect "+std::to_string(suspIdx+1)+" of "+
                             std::to_string((int)SUSPECTS.size()),13,C_DIM,470,WIN_H-22));
}

void Game::renderVerdict() {
    drawBg(sprVerdict,hasTexVerdict,sf::Color(7,7,14));

    auto sh=centered("THE  VERDICT",38,sf::Color(0,0,0,160),78);
    sh.move(3,3); window.draw(sh);
    window.draw(centered("THE  VERDICT",38,C_ACCENT,75));

    ui.drawPanel(window,WIN_W-222,110,204,225,sf::Color(8,10,20,215));
    window.draw(ui.makeText("SUSPICION LOG",13,C_ACCENT,WIN_W-212,124));
    float sy=150;
    for(auto& kv:suspicion){
        window.draw(ui.makeText(kv.first+": "+std::to_string(kv.second),
                                 13,C_TEXT,WIN_W-212,sy));
        sy+=24;
    }

    if (!vShown) {
        window.draw(centered("Choose the guilty party:",17,C_DIM,162));
        for(int i=0;i<4;i++)
            drawSuspectPortrait(i,suspectEmotion[i],120.f+i*160.f,230.f);
        for(auto& b:vBtns) b.draw(window);
    } else {
        ui.drawPanel(window,80,155,WIN_W-290,360,sf::Color(8,10,20,220));
        sf::Color mc=(vMsg=="CASE SOLVED")?C_GREEN:C_RED;
        auto rs=centered(vMsg,40,sf::Color(0,0,0,160),205);
        rs.move(3,3); window.draw(rs);
        window.draw(centered(vMsg,40,mc,202));
        sf::RectangleShape div({WIN_W-380.f,1});
        div.setFillColor(mc); div.setPosition(140,238); window.draw(div);

        if (vMsg=="CASE SOLVED") {
            drawSuspectPortrait(0,Emotion::CUFFED,175.f,355.f);
            float bx=230.f, by=258.f;
            std::string lines[] = {
                "MOTIVE: Escalating Domestic Conflict",
                "",
                "The Husband grew controlling over time.",
                "He resented Elena's work and her colleague.",
                "That night their fight turned violent.",
                "She fled into the forest. He followed.",
                "",
                "It was never an affair. Just a marriage",
                "that shattered under its own rage."
            };
            for (auto& l:lines){
                if(l.empty()){by+=10;continue;}
                sf::Color lc=(l.find("MOTIVE")!=std::string::npos)?C_ACCENT:C_TEXT;
                unsigned  lsz=(l.find("MOTIVE")!=std::string::npos)?15:14;
                window.draw(ui.makeText(l,lsz,lc,bx,by));
                by+=22;
            }
        } else {
            window.draw(centered("Wrong conclusion.",20,C_RED,290));
            window.draw(centered("The real killer walks free...",16,C_DIM,325));
            window.draw(centered("Their fight that night went too far.",14,
                                  sf::Color(160,80,80),362));
            window.draw(centered("The Husband was never held accountable.",14,
                                  sf::Color(160,80,80),385));
        }
        window.draw(centered("Press  ENTER  to return to Main Menu",14,C_DIM,WIN_H-30));
    }
}