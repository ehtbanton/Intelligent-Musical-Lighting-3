// MessageBox.cpp
#include "MessageBox.hpp"

MessageBox::MessageBox(const std::string& messageText)
    : isOpen(true)
{
    // Create window
    window.create(sf::VideoMode(400, 200), "Message", sf::Style::Titlebar | sf::Style::Close);
    window.setPosition(sf::Vector2i(
        sf::VideoMode::getDesktopMode().width / 2 - 200,
        sf::VideoMode::getDesktopMode().height / 2 - 100
    ));

    // Initialize components
    init();

    // Set message text
    message.setString(messageText);
}

void MessageBox::init() {
    // Load font
    if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        // Handle font loading error
        // For now, we'll just continue - SFML will use a default font
    }

    // Setup message text
    message.setFont(font);
    message.setCharacterSize(16);
    message.setFillColor(sf::Color::Black);
    message.setPosition(20, 20);

    // Setup background
    background.setSize(sf::Vector2f(400, 200));
    background.setFillColor(sf::Color(240, 240, 240));

    // Setup OK button
    button.setSize(sf::Vector2f(80, 30));
    button.setPosition(160, 140);
    button.setFillColor(sf::Color(200, 200, 200));

    // Setup button text
    buttonText.setFont(font);
    buttonText.setString("OK");
    buttonText.setCharacterSize(16);
    buttonText.setFillColor(sf::Color::Black);
    buttonText.setPosition(190, 145);
}

void MessageBox::show() {
    while (window.isOpen() && isOpen) {
        handleEvents();
        render();
    }
}

void MessageBox::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            isOpen = false;
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (mousePos.x >= 160 && mousePos.x <= 240 &&
                    mousePos.y >= 140 && mousePos.y <= 170) {
                    window.close();
                    isOpen = false;
                }
            }
        }
    }
}

void MessageBox::render() {
    window.clear(sf::Color::White);
    window.draw(background);
    window.draw(message);
    window.draw(button);
    window.draw(buttonText);
    window.display();
}