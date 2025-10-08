#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstdlib>

using namespace std;

// --- Utility: Read one key press ---
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

// --- Menu selection with arrow keys ---
int selectOption(const vector<string>& options, const string& prompt) {
    int selected = 0;
    char key;
    while (true) {
        system("clear");
        cout << prompt << "\n\n";
        for (size_t i = 0; i < options.size(); ++i) {
            if ((int)i == selected)
                cout << " 👉 " << options[i] << "\n";
            else
                cout << "    " << options[i] << "\n";
        }

        key = getKey();
        if (key == '\033') {
            getchar(); // skip [
            switch (getchar()) {
                case 'A': selected = (selected - 1 + options.size()) % options.size(); break;
                case 'B': selected = (selected + 1) % options.size(); break;
            }
        } else if (key == '\n') {
            return selected;
        }
    }
}

// --- Create control file content ---
string createControlFile(const string& appName, const string& version,
                         const string& arch, const string& description) {
    string control = "Package: " + appName + "\n";
    control += "Version: " + version + "\n";
    control += "Section: base\n";
    control += "Priority: optional\n";
    control += "Architecture: " + arch + "\n";
    control += "Maintainer: Unknown <unknown@example.com>\n";
    control += "Description: " + description + "\n";
    return control;
}

int main() {
    string filePath, appName, description, version;
    vector<string> archOptions = {"amd64", "arm", "i386"};

    // --- Get Inputs ---
    cout << "Enter relative path to binary (e.g., ../bin/myapp): ";
    getline(cin, filePath);
    if (filePath.empty()) {
        cerr << "❌ File path cannot be empty.\n";
        return 1;
    }
    if (filesystem::path(filePath).is_absolute()) {
        cerr << "❌ Please provide a relative path, not absolute.\n";
        return 1;
    }
    if (!filesystem::exists(filePath)) {
        cerr << "❌ Binary file not found at: " << filePath << "\n";
        return 1;
    }

    cout << "Enter app name: ";
    getline(cin, appName);

    cout << "Enter app description: ";
    getline(cin, description);

    int archIndex = selectOption(archOptions, "Select architecture:");
    string architecture = archOptions[archIndex];

    cout << "\nEnter app version (e.g., 1.0.0): ";
    getline(cin, version);

    cout << "\n✅ Summary:\n";
    cout << "File Path: " << filePath << "\n";
    cout << "App Name: " << appName << "\n";
    cout << "Description: " << description << "\n";
    cout << "Architecture: " << architecture << "\n";
    cout << "Version: " << version << "\n\n";

    // --- Step 1: Create directory structure ---
    filesystem::remove_all("package");
    filesystem::create_directories("package/usr/bin");
    filesystem::create_directories("package/DEBIAN");
    filesystem::create_directories("dist");
    cout << "📁 Created package directory structure.\n";

    // --- Step 2: Copy binary ---
    filesystem::path destBinary = "package/usr/bin/" + filesystem::path(appName).filename().string();
    filesystem::copy_file(filePath, destBinary, filesystem::copy_options::overwrite_existing);
    cout << "✅ Copied binary into " << destBinary << "\n";

    // --- Step 3: Write control file ---
    ofstream controlFile("package/DEBIAN/control");
    controlFile << createControlFile(appName, version, architecture, description);
    controlFile.close();
    cout << "✅ Created DEBIAN/control file.\n";

    // --- Step 4: Set permissions ---
    system("chmod -R 755 package/DEBIAN");

    // --- Step 5: Build .deb package ---
    string debPath = "dist/" + appName + "_" + version + "_" + architecture + ".deb";
    string cmd = "dpkg-deb --build package " + debPath;

    cout << "📦 Building .deb package...\n";
    int result = system(cmd.c_str());
    if (result == 0) {
        cout << "✅ Successfully created " << debPath << "\n";
    } else {
        cerr << "❌ Failed to build .deb package.\n";
    }

    return 0;
}
