// AudioAnalyzer.hpp
#ifndef AUDIO_ANALYZER_HPP
#define AUDIO_ANALYZER_HPP

#include <SFML/Audio.hpp>
#include <vector>
#include <complex>
#include <string>

class AudioAnalyzer {
private:
    static const size_t SAMPLE_RATE = 44100;
    static const size_t BUFFER_SIZE = 1024;  // Reduced for testing

    sf::SoundBufferRecorder recorder;
    std::vector<sf::Int16> samples;
    std::vector<float> spectrum;

    // Audio analysis parameters
    float currentVolume;
    float currentCentroid;

    // Helper functions
    void calculateSpectrum();
    float calculateSpectralCentroid();
    float calculateRMS();
    void normalizeValue(float& value, float minValue = 0.0f, float maxValue = 1.0f);

    // Smoothing parameters
    float volumeSmoothing;
    float centroidSmoothing;
    float smoothValue(float current, float target, float smoothing);

    // Debug functions
    void printAudioDevices();
    bool checkAudioAvailability();
    bool setupAudioDevice(const std::string& deviceName = "");

public:
    AudioAnalyzer();

    bool start();
    void stop();
    void update();

    // Getters for normalized values (0.0 to 1.0)
    float getVolume() const { return currentVolume; }
    float getSpectralCentroid() const { return currentCentroid; }
};

#endif // AUDIO_ANALYZER_HPP