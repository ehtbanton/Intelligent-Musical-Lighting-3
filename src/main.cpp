#include "WindowDisplayController.hpp"
#include "AudioAnalyzer.hpp"
#include "MessageBox.hpp"

int main() {
    try {
        // Create window
        sf::RenderWindow window(sf::VideoMode(800, 600), "Audio Reactive Display");
        WindowDisplayController displayController(window);
        AudioAnalyzer audioAnalyzer;

        // Show startup message
        MessageBox startMsg("Starting Audio Analyzer...\nPlease wait.");
        startMsg.show();

        // Start audio analysis
        if (!audioAnalyzer.start()) {
            MessageBox errorMsg("Failed to start audio analyzer.\nThe application will now exit.");
            errorMsg.show();
            return -1;
        }

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            audioAnalyzer.update();
            float spectralCentroid = audioAnalyzer.getSpectralCentroid();
            float volume = audioAnalyzer.getVolume();
            displayController.updateDisplay(spectralCentroid, volume);
        }

    }
    catch (const std::exception& e) {
        MessageBox errorMsg(std::string("Error: ") + e.what());
        errorMsg.show();
        return -1;
    }

    return 0;
}