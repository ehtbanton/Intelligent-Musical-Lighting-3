# Project Setup Guide

## Prerequisites
1. Install Visual Studio 2022
   - Download from: https://visualstudio.microsoft.com/vs/
   - During installation, select "Desktop development with C++"

## Project Setup Steps

### 1. Getting the Project
```bash
git clone [your-repository-url]
cd TeamProject
```

### 2. SFML Setup  (you can skip this step because Anton has done it for you!)
1. Download SFML 2.5.1 for Visual C++ 2022 (64-bit):
   - Go to: https://www.sfml-dev.org/download/sfml/2.5.1/
   - Download "Visual C++ 15 (2017) - 64-bit"

2. Extract SFML:
   - Copy the contents of the downloaded SFML folder into `TeamProject/extern/SFML/`
   - Your SFML folder should look like this:
   ```
   extern/SFML/
   ├── bin/
   ├── include/
   ├── lib/
   └── readme.md
   ```

### 3. Visual Studio Configuration
1. Open Visual Studio 2022
2. Create a new project in the TeamProject folder:
   - File → New → Project
   - Select "Empty C++ Project"
   - Set location to your TeamProject folder
   - Name it "TeamProject"

3. Configure Project Properties:
   - Right-click the solution in Solution Explorer
   - Select Configuration Properties
   - Make sure "All Configurations" and "x64" are selected
   
4. Set include directories:
   - C/C++ → General → Additional Include Directories
   - Add: `$(ProjectDir)extern\SFML\include`

5. Set library directories:
   - Linker → General → Additional Library Directories
   - Add: `$(ProjectDir)extern\SFML\lib`

6. Add library dependencies:
   - Linker → Input → Additional Dependencies
   - Add:
     ```
     sfml-graphics-d.lib
     sfml-window-d.lib
     sfml-system-d.lib
     sfml-audio-d.lib
     ```
   - Note: Remove the "-d" for Release configuration

### 4. Testing the Setup
1. Copy this test code into `src/main.cpp`:
```cpp
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
```

2. Copy required DLLs:
   - Copy all DLL files from `extern/SFML/bin/` to your project's output directory
   - Default location: `TeamProject/x64/Debug/`

### 5. Running the Project
1. Set solution configuration to "Debug" and platform to "x64"
2. Build the project (F7)
3. Run (F5) - You should see a blue window

## Common Issues and Solutions

1. **Missing DLL Error**
   - Copy all DLLs from SFML/bin to your exe location
   - Make sure you're using the debug (-d) DLLs for Debug configuration

2. **Linker Errors**
   - Double-check Additional Dependencies
   - Verify you're using correct debug/release libraries

3. **Include Errors**
   - Verify the include path is correct
   - Check that SFML files are in the correct location

## Need Help?
- Check SFML documentation: https://www.sfml-dev.org/documentation/2.5.1/
- Contact [Team Lead contact info]