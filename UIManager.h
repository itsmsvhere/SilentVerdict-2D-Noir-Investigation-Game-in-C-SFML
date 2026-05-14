#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// ─────────────────────────────────────────────
//  Button — a clickable rectangle with a label
// ─────────────────────────────────────────────
struct Button {
    sf::RectangleShape shape;
    sf::Text           label;
    sf::Color          normalColor {48, 53, 82};
    sf::Color          hoverColor  {92, 112, 165};

    void init(float x, float y, float w, float h,
              const std::string& text, sf::Font& font,
              unsigned charSize = 18);

    void recolor(sf::Color n, sf::Color h) {
        normalColor = n; hoverColor = h;
        shape.setFillColor(n);
    }

    void  update(sf::Vector2f mp);
    bool  clicked(sf::Vector2f mp, bool pressed) const;
    void  draw(sf::RenderWindow& window);

private:
    void centerLabel();
};

// ─────────────────────────────────────────────
//  UIManager — font loader + helper drawers
// ─────────────────────────────────────────────
class UIManager {
public:
    bool loadFont(const std::string& path);
    sf::Font& font();

    sf::Text makeText(const std::string& s, unsigned sz,
                      sf::Color c, float x, float y);

    // Returns one sf::Text per '\n'-delimited line
    std::vector<sf::Text> makeBlock(const std::string& s, unsigned sz,
                                     sf::Color c, float x, float y,
                                     float lineSpacing = 27.f);

    // Draw a filled + outlined panel rectangle
    void drawPanel(sf::RenderWindow& w,
                   float x, float y, float wd, float ht,
                   sf::Color fill = sf::Color(26, 28, 46, 238));

private:
    sf::Font f;
};
