#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <filesystem>

// Utility: Read one key press (without pressing Enter)
char getKey() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// Display a selection menu using arrow keys
int selectOption(const std::vector<std::string>& options, const std::string& prompt) {
    int selected = 0;
    char key;
    while (true) {
        system("clear");
        std::cout << prompt << "\n\n";
        for (size_t i = 0; i < options.size(); ++i) {
            if ((int)i == selected)
                std::cout << " 👉 " << options[i] << "\n";
            else
                std::cout << "    " << options[i] << "\n";
        }

        key = getKey();
        if (key == '\033') { // if the first value is esc
            getchar();       // skip the [
            switch (getchar()) {
                case 'A': selected = (selected - 1 + options.size()) % options.size(); break; // Up
                case 'B': selected = (selected + 1) % options.size(); break; // Down
            }
        } else if (key == '\n') {
            return selected;
        }
    }
}

int main() {
    std::string filePath, appName, description, version;
    std::vector<std::string> archOptions = {"amd64", "arm", "i386"};

    // 1. Get file path
    std::cout << "Enter relative path to binary (e.g., ./bin/myapp): ";
    std::getline(std::cin, filePath);
    if (filePath.empty()) {
        std::cerr << "❌ File path cannot be empty.\n";
        return 1;
    }
    if (std::filesystem::path(filePath).is_absolute()) {
        std::cerr << "❌ Please provide a relative path, not absolute.\n";
        return 1;
    }

    // 2. Get app name
    std::cout << "Enter app name: ";
    std::getline(std::cin, appName);

    // 3. Get description
    std::cout << "Enter app description: ";
    std::getline(std::cin, description);

    // 4. Select architecture
    int archIndex = selectOption(archOptions, "Select architecture:");
    std::string architecture = archOptions[archIndex];

    // 5. Get version
    std::cout << "\nEnter app version (e.g., 1.0.0): ";
    std::getline(std::cin, version);

    // Display summary
    std::cout << "\n✅ Summary:\n";
    std::cout << "File Path: " << filePath << "\n";
    std::cout << "App Name: " << appName << "\n";
    std::cout << "Description: " << description << "\n";
    std::cout << "Architecture: " << architecture << "\n";
    std::cout << "Version: " << version << "\n\n";

    std::cout << "🎉 All inputs collected successfully!\n";

    return 0;
}
