// WindowDisplayController.hpp
#ifndef WINDOW_DISPLAY_CONTROLLER_HPP
#define WINDOW_DISPLAY_CONTROLLER_HPP

#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

// Define M_PI if it's not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class WindowDisplayController {
private:
    sf::RenderWindow& window;
    sf::Clock clock;  // Added to track time for color cycling
    double currentHue;
    double currentBrightness;

    // Helper function to clamp values between min and max
    double clamp(double value, double min = 0.0, double max = 1.0);

    // Convert HSV to RGB color
    sf::Color HSVtoRGB(double hue, double saturation, double value);

    // Update cycling parameters
    void updateCycleParameters(float deltaTime);

public:
    // Constructor
    WindowDisplayController(sf::RenderWindow& win);

    // Update window color based on automatic cycling
    void updateDisplay();

    // Setters for cycle speeds (in Hz)
    void setColorCycleSpeed(float speed) { colorCycleSpeed = speed; }
    void setBrightnessCycleSpeed(float speed) { brightnessCycleSpeed = speed; }

private:
    float colorCycleSpeed;      // Color cycles per second
    float brightnessCycleSpeed; // Brightness cycles per second
};

#endif // WINDOW_DISPLAY_CONTROLLER_HPP