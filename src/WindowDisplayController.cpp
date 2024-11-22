// WindowDisplayController.cpp
#include "WindowDisplayController.hpp"

WindowDisplayController::WindowDisplayController(sf::RenderWindow& win)
    : window(win),
    currentHue(0.0),
    currentBrightness(1.0),
    colorCycleSpeed(0.2f),    // Complete color cycle every 5 seconds
    brightnessCycleSpeed(0.5f) // Complete brightness cycle every 2 seconds
{
    clock.restart();
}

double WindowDisplayController::clamp(double value, double min, double max) {
    return std::max(min, std::min(value, max));
}

sf::Color WindowDisplayController::HSVtoRGB(double hue, double saturation, double value) {
    hue = clamp(hue);
    saturation = clamp(saturation);
    value = clamp(value);

    double h = hue * 6.0;
    int i = static_cast<int>(h);
    double f = h - i;
    double p = value * (1.0 - saturation);
    double q = value * (1.0 - saturation * f);
    double t = value * (1.0 - saturation * (1.0 - f));

    double r, g, b;
    switch (i % 6) {
    case 0: r = value; g = t; b = p; break;
    case 1: r = q; g = value; b = p; break;
    case 2: r = p; g = value; b = t; break;
    case 3: r = p; g = q; b = value; break;
    case 4: r = t; g = p; b = value; break;
    case 5: r = value; g = p; b = q; break;
    default: r = 0; g = 0; b = 0; break;
    }

    return sf::Color(
        static_cast<sf::Uint8>(r * 255),
        static_cast<sf::Uint8>(g * 255),
        static_cast<sf::Uint8>(b * 255)
    );
}

void WindowDisplayController::updateCycleParameters(float deltaTime) {
    // Update hue (continuous cycle)
    currentHue += colorCycleSpeed * deltaTime;
    if (currentHue >= 1.0) {
        currentHue -= 1.0;
    }

    // Update brightness (sine wave oscillation)
    double brightnessPhase = brightnessCycleSpeed * clock.getElapsedTime().asSeconds() * 2.0 * M_PI;
    currentBrightness = (std::sin(brightnessPhase) + 1.0) * 0.5; // Oscillate between 0 and 1
}

void WindowDisplayController::updateDisplay() {
    float deltaTime = clock.restart().asSeconds();

    // Update cycling parameters
    updateCycleParameters(deltaTime);

    // Convert to RGB color (using constant saturation for vibrant colors)
    sf::Color color = HSVtoRGB(currentHue, 1.0, currentBrightness);

    // Clear window with new color
    window.clear(color);
    window.display();
}