#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "WindowDisplayController.hpp"

#include <iostream>
using namespace std;

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Team Project");
    WindowDisplayController displayController(window);

    // Optionally adjust cycle speeds (in Hz)
    // displayController.setColorCycleSpeed(0.1f);      // Slower color cycle
    // displayController.setBrightnessCycleSpeed(0.3f); // Slower brightness cycle

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Update display (will automatically cycle colors and brightness)
        displayController.updateDisplay();
    }

    return 0;
}