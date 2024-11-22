// MessageBox.hpp
#ifndef MESSAGE_BOX_HPP
#define MESSAGE_BOX_HPP

#include <SFML/Graphics.hpp>
#include <string>

class MessageBox {
private:
    sf::RenderWindow window;
    sf::Text message;
    sf::RectangleShape background;
    sf::RectangleShape button;
    sf::Text buttonText;
    bool isOpen;
    sf::Font font;  // Keep font as member variable

public:
    // Explicit constructor declaration
    explicit MessageBox(const std::string& messageText);

    // Destructor
    ~MessageBox() = default;

    // Delete copy constructor and assignment operator
    MessageBox(const MessageBox&) = delete;
    MessageBox& operator=(const MessageBox&) = delete;

    // Member functions
    void show();

private:
    void init();
    void handleEvents();
    void render();
};

#endif // MESSAGE_BOX_HPP