// AudioAnalyzer.cpp
#define _USE_MATH_DEFINES

#include "AudioAnalyzer.hpp"
#include "MessageBox.hpp"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cassert>

AudioAnalyzer::AudioAnalyzer()
    : currentVolume(0.0f)
    , currentCentroid(0.0f)
    , volumeSmoothing(0.2f)
    , centroidSmoothing(0.2f)
{
    samples.reserve(BUFFER_SIZE);
    spectrum.resize(BUFFER_SIZE / 2, 0.0f);
}

bool AudioAnalyzer::start() {
    // Get available devices
    std::vector<std::string> availableDevices = sf::SoundRecorder::getAvailableDevices();

    // Check if audio recording is available at all
    if (!sf::SoundRecorder::isAvailable()) {
        MessageBox errorBox("Sound recording is not available on this system.\n"
            "Please check:\n"
            "1. Your microphone is plugged in and enabled\n"
            "2. Windows microphone permissions are enabled\n"
            "3. SFML has permission to access audio devices");
        errorBox.show();
        return false;
    }

    // No devices found
    if (availableDevices.empty()) {
        MessageBox errorBox("No audio input devices detected.\n"
            "Please check:\n"
            "1. Open Windows Sound Settings\n"
            "2. Under Input, ensure a microphone is selected\n"
            "3. Make sure apps have permission to access microphone");
        errorBox.show();
        return false;
    }

    // Show available devices
    std::stringstream deviceInfo;
    deviceInfo << "Available audio devices:\n\n";
    for (const auto& device : availableDevices) {
        deviceInfo << "- " << device << "\n";
    }
    deviceInfo << "\nDefault device: " << sf::SoundRecorder::getDefaultDevice();

    MessageBox deviceBox(deviceInfo.str());
    deviceBox.show();

    // Try to set up the audio device
    if (!recorder.setDevice(sf::SoundRecorder::getDefaultDevice())) {
        MessageBox errorBox("Failed to set up the audio device.\n"
            "Please check your audio settings.");
        errorBox.show();
        return false;
    }

    // Start the recorder
    if (!recorder.start()) {
        MessageBox errorBox("Failed to start audio recorder.\n"
            "Please check your microphone permissions.");
        errorBox.show();
        return false;
    }

    return true;
}

void AudioAnalyzer::stop() {
    recorder.stop();
}

void AudioAnalyzer::update() {
    const sf::SoundBuffer& buffer = recorder.getBuffer();
    const sf::Int16* bufferSamples = buffer.getSamples();
    size_t sampleCount = buffer.getSampleCount();

    // Handle no samples case
    if (sampleCount == 0) {
        static bool noSamplesWarningShown = false;
        if (!noSamplesWarningShown) {
            MessageBox warningBox("No audio samples being received.\nPlease check if audio is playing and system permissions are correct.");
            warningBox.show();
            noSamplesWarningShown = true;
        }
        samples.clear();
        samples.resize(BUFFER_SIZE, 0);  // Fill with silence
        return;
    }

    // Safely copy samples
    samples.clear();
    samples.resize(BUFFER_SIZE, 0);  // Initialize with zeros

    size_t copyCount = std::min(sampleCount, BUFFER_SIZE);
    size_t startPos = (sampleCount > BUFFER_SIZE) ? (sampleCount - BUFFER_SIZE) : 0;

    for (size_t i = 0; i < copyCount; ++i) {
        if (startPos + i < sampleCount) {  // Extra safety check
            samples[i] = bufferSamples[startPos + i];
        }
    }

    // Calculate audio features
    float targetVolume = calculateRMS();
    calculateSpectrum();
    float targetCentroid = calculateSpectralCentroid();

    // Smooth and normalize values
    currentVolume = smoothValue(currentVolume, targetVolume, volumeSmoothing);
    currentCentroid = smoothValue(currentCentroid, targetCentroid, centroidSmoothing);

    normalizeValue(currentVolume);
    normalizeValue(currentCentroid);
}

void AudioAnalyzer::calculateSpectrum() {
    // Reset spectrum
    spectrum.clear();
    spectrum.resize(BUFFER_SIZE / 2, 0.0f);

    // Create and fill complex input array
    std::vector<std::complex<float>> complex_input(BUFFER_SIZE);
    for (size_t i = 0; i < BUFFER_SIZE && i < samples.size(); ++i) {
        // Apply Hanning window while converting to complex
        float multiplier = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (BUFFER_SIZE - 1)));
        complex_input[i] = std::complex<float>((samples[i] / 32768.0f) * multiplier, 0.0f);
    }

    // Simple spectral magnitude calculation (placeholder for FFT)
    // In a real application, you'd want to use a proper FFT library
    for (size_t i = 0; i < BUFFER_SIZE / 2 && i < spectrum.size(); ++i) {
        float frequency = static_cast<float>(i) * SAMPLE_RATE / BUFFER_SIZE;
        float magnitude = std::abs(complex_input[i]);
        spectrum[i] = magnitude;
    }
}

float AudioAnalyzer::calculateSpectralCentroid() {
    if (spectrum.empty()) {
        return 0.0f;
    }

    float numerator = 0.0f;
    float denominator = 0.0f;

    for (size_t i = 0; i < spectrum.size(); ++i) {
        float frequency = static_cast<float>(i) * SAMPLE_RATE / (2.0f * spectrum.size());
        numerator += frequency * spectrum[i];
        denominator += spectrum[i];
    }

    if (denominator > 0.0f) {
        return numerator / denominator;
    }
    return 0.0f;
}

float AudioAnalyzer::calculateRMS() {
    if (samples.empty()) {
        return 0.0f;
    }

    double sum = 0.0;  // Using double for better precision in accumulation
    for (const auto& sample : samples) {
        float normalized = sample / 32768.0f;  // Normalize 16-bit audio
        sum += normalized * normalized;
    }

    return static_cast<float>(std::sqrt(sum / samples.size()));
}

void AudioAnalyzer::normalizeValue(float& value, float minValue, float maxValue) {
    value = std::clamp(value, minValue, maxValue);
    value = (value - minValue) / (maxValue - minValue);
}

float AudioAnalyzer::smoothValue(float current, float target, float smoothing) {
    smoothing = std::clamp(smoothing, 0.0f, 1.0f);
    return current + smoothing * (target - current);
}