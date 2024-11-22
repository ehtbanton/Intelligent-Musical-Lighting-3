#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "WindowDisplayController.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Team Project");
    WindowDisplayController displayController(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Example: Update color based on some inputs
        // Replace these with your actual input values
        double input1 = 0.5; // Controls hue
        double input2 = 0.8; // Controls brightness

        displayController.updateDisplay(input1, input2);
    }

    return 0;
}