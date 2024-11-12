#define NOMINMAX
#include <windows.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <cmath>
#include <complex>
#include <vector>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

class AudioVisualizer {
private:
    HWND hwnd = nullptr;  // Initialize member variables
    HDC hdc = nullptr;
    HGLRC hrc = nullptr;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioClient* pAudioClient = nullptr;
    IAudioCaptureClient* pCaptureClient = nullptr;
    WAVEFORMATEX* pwfx = nullptr;

    // Buffer for audio processing
    std::vector<float> audioBuffer;
    static constexpr size_t BUFFER_SIZE = 1024;

    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);  // Forward declaration

public:
    bool Initialize(HINSTANCE hInstance);
    void Update();
    void Render();
    void Cleanup();

private:
    bool InitializeWindow(HINSTANCE hInstance);
    bool InitializeOpenGL();
    bool InitializeAudio();
};

// Move WindowProc implementation outside the class
LRESULT CALLBACK AudioVisualizer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// Rest of the method implementations
bool AudioVisualizer::InitializeWindow(HINSTANCE hInstance) {
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"AudioVisualizerClass";

    if (!RegisterClassEx(&wc)) {
        return false;
    }

    hwnd = CreateWindowEx(
        0,
        L"AudioVisualizerClass",
        L"Audio Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd) {
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    return true;
}

bool AudioVisualizer::InitializeOpenGL() {
    hdc = GetDC(hwnd);
    if (!hdc) return false;

    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    int pf = ChoosePixelFormat(hdc, &pfd);
    if (!pf) return false;

    if (!SetPixelFormat(hdc, pf, &pfd)) return false;

    hrc = wglCreateContext(hdc);
    if (!hrc) return false;

    if (!wglMakeCurrent(hdc, hrc)) return false;

    glViewport(0, 0, 800, 600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    return true;
}

bool AudioVisualizer::InitializeAudio() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
    if (FAILED(hr)) return false;

    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&pEnumerator
    );
    if (FAILED(hr)) return false;

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    if (FAILED(hr)) return false;

    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&pAudioClient);
    if (FAILED(hr)) return false;

    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) return false;

    hr = pAudioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK,
        0,
        0,
        pwfx,
        nullptr
    );
    if (FAILED(hr)) return false;

    hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pCaptureClient);
    if (FAILED(hr)) return false;

    hr = pAudioClient->Start();
    if (FAILED(hr)) return false;

    audioBuffer.resize(BUFFER_SIZE);
    return true;
}

bool AudioVisualizer::Initialize(HINSTANCE hInstance) {
    if (!InitializeWindow(hInstance)) {
        MessageBox(nullptr, L"Failed to initialize window.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    if (!InitializeOpenGL()) {
        MessageBox(nullptr, L"Failed to initialize OpenGL.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    if (!InitializeAudio()) {
        MessageBox(nullptr, L"Failed to initialize Audio.", L"Error", MB_OK | MB_ICONERROR);
        return false;
    }
    return true;
}

void AudioVisualizer::Update() {
    UINT32 packetLength = 0;
    BYTE* pData;
    DWORD flags;
    float currentVolume = 0.0f;
    float spectralCentroid = 0.0f;

    HRESULT hr = pCaptureClient->GetNextPacketSize(&packetLength);
    if (FAILED(hr)) return;

    if (packetLength > 0) {
        hr = pCaptureClient->GetBuffer(&pData, &packetLength, &flags, nullptr, nullptr);
        if (SUCCEEDED(hr)) {
            float* samples = (float*)pData;
            size_t sampleCount = packetLength / (pwfx->wBitsPerSample / 8);

            // Calculate volume (RMS)
            float sum = 0.0f;
            for (size_t i = 0; i < sampleCount; i++) {
                sum += samples[i] * samples[i];
            }
            currentVolume = std::sqrt(sum / sampleCount);

            // Simple spectral centroid calculation
            spectralCentroid = 0.0f;
            float weightedSum = 0.0f;
            float magnitudeSum = 0.0f;

            for (size_t i = 0; i < sampleCount; i++) {
                float magnitude = std::abs(samples[i]);
                float frequency = (float)i * pwfx->nSamplesPerSec / sampleCount;
                weightedSum += magnitude * frequency;
                magnitudeSum += magnitude;
            }

            if (magnitudeSum > 0) {
                spectralCentroid = weightedSum / magnitudeSum;
            }

            pCaptureClient->ReleaseBuffer(packetLength);
        }
    }

    // Convert centroid to color
    float hue = std::min(spectralCentroid / 5000.0f, 1.0f);
    float r, g, b;
    float x = 1.0f - std::abs(std::fmod(hue * 6.0f, 2.0f) - 1.0f);

    if (hue < 1.0f / 6.0f) { r = 1.0f; g = x;    b = 0.0f; }
    else if (hue < 2.0f / 6.0f) { r = x;    g = 1.0f; b = 0.0f; }
    else if (hue < 3.0f / 6.0f) { r = 0.0f; g = 1.0f; b = x; }
    else if (hue < 4.0f / 6.0f) { r = 0.0f; g = x;    b = 1.0f; }
    else if (hue < 5.0f / 6.0f) { r = x;    g = 0.0f; b = 1.0f; }
    else { r = 1.0f; g = 0.0f; b = x; }

    float intensity = currentVolume * 5.0f;
    intensity = std::min(1.0f, std::max(0.0f, intensity));

    glClearColor(r * intensity, g * intensity, b * intensity, 1.0f);
}

void AudioVisualizer::Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    SwapBuffers(hdc);
}

void AudioVisualizer::Cleanup() {
    if (pAudioClient) pAudioClient->Stop();
    if (pwfx) CoTaskMemFree(pwfx);
    if (pCaptureClient) pCaptureClient->Release();
    if (pAudioClient) pAudioClient->Release();
    if (pDevice) pDevice->Release();
    if (pEnumerator) pEnumerator->Release();

    wglMakeCurrent(nullptr, nullptr);
    if (hrc) wglDeleteContext(hrc);
    if (hdc && hwnd) ReleaseDC(hwnd, hdc);
    if (hwnd) DestroyWindow(hwnd);
    CoUninitialize();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    AudioVisualizer visualizer;
    if (!visualizer.Initialize(hInstance)) {
        return 1;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        visualizer.Update();
        visualizer.Render();
    }

    visualizer.Cleanup();
    return 0;
}