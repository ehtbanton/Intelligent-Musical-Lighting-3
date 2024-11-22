#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Team Project");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Blue);
        window.display();
    }

    return 0;
}