#include "UIManager.h"
#include <sstream>

// ─── Button ──────────────────────────────────────────────────

void Button::centerLabel() {
    auto tb = label.getLocalBounds();
    auto sb = shape.getGlobalBounds();
    label.setOrigin(tb.left + tb.width  / 2.f,
                    tb.top  + tb.height / 2.f);
    label.setPosition(sb.left + sb.width  / 2.f,
                      sb.top  + sb.height / 2.f);
}

void Button::init(float x, float y, float w, float h,
                  const std::string& text, sf::Font& font,
                  unsigned charSize) {
    shape.setSize({w, h});
    shape.setPosition(x, y);
    shape.setFillColor(normalColor);
    shape.setOutlineColor(sf::Color(75, 80, 108));
    shape.setOutlineThickness(1.f);

    label.setFont(font);
    label.setString(text);
    label.setCharacterSize(charSize);
    label.setFillColor(sf::Color::White);
    centerLabel();
}

void Button::update(sf::Vector2f mp) {
    bool hov = shape.getGlobalBounds().contains(mp);
    shape.setFillColor(hov ? hoverColor : normalColor);
    centerLabel();
}

bool Button::clicked(sf::Vector2f mp, bool pressed) const {
    return pressed && shape.getGlobalBounds().contains(mp);
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(label);
}

// ─── UIManager ───────────────────────────────────────────────

bool UIManager::loadFont(const std::string& path) {
    return f.loadFromFile(path);
}

sf::Font& UIManager::font() { return f; }

sf::Text UIManager::makeText(const std::string& s, unsigned sz,
                               sf::Color c, float x, float y) {
    sf::Text t;
    t.setFont(f);
    t.setString(s);
    t.setCharacterSize(sz);
    t.setFillColor(c);
    t.setPosition(x, y);
    return t;
}

std::vector<sf::Text> UIManager::makeBlock(const std::string& s, unsigned sz,
                                             sf::Color c, float x, float y,
                                             float ls) {
    std::vector<sf::Text> out;
    std::istringstream ss(s);
    std::string line;
    float cy = y;
    while (std::getline(ss, line)) {
        out.push_back(makeText(line, sz, c, x, cy));
        cy += ls;
    }
    return out;
}

void UIManager::drawPanel(sf::RenderWindow& w,
                           float x, float y, float wd, float ht, sf::Color fill) {
    sf::RectangleShape r({wd, ht});
    r.setPosition(x, y);
    r.setFillColor(fill);
    r.setOutlineColor(sf::Color(68, 70, 95));
    r.setOutlineThickness(1.f);
    w.draw(r);
}
