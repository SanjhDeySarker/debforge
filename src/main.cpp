#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <sys/utsname.h>
#include <vector>
#include <termios.h>
#include <unistd.h>

using namespace std;
namespace fs = std::filesystem;

// --- Read single key for interactive menu ---
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

// --- Arrow key menu selection ---
int selectOption(const vector<string>& options, const string& prompt) {
    int selected = 0;
    char key;
    while (true) {
        system("clear");
        cout << prompt << " (Use Up/Down arrows, Enter to select)\n\n";
        for (size_t i = 0; i < options.size(); ++i) {
            if ((int)i == selected)
                cout << " 👉 " << options[i] << "\n";
            else
                cout << "    " << options[i] << "\n";
        }

        key = getKey();
        if (key == '\033') { // arrow key
            getchar(); // skip [
            switch (getchar()) {
                case 'A': selected = (selected - 1 + options.size()) % options.size(); break; // Up
                case 'B': selected = (selected + 1) % options.size(); break; // Down
            }
        } else if (key == '\n') {
            return selected;
        }
    }
}

// --- Detect system architecture ---
string getSystemArch() {
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        string arch = buffer.machine;
        if (arch == "x86_64") return "amd64";
        if (arch == "i686" || arch == "i386") return "i386";
        if (arch.find("arm") != string::npos) return "arm";
    }
    return "amd64";
}

// --- Create DEBIAN/control content ---
string createControlFile(const string& appName, const string& version,
                         const string& arch, const string& description) {
    string control = "Package: " + appName + "\n";
    control += "Version: " + version + "\n";
    control += "Section: base\n";
    control += "Priority: optional\n";
    control += "Architecture: " + arch + "\n";
    control += "Maintainer: Unknown <unknown@example.com>\n";
    control += "Description: " + (description.empty() ? "No description provided" : description) + "\n";
    return control;
}

int main() {
    string filePath, appNameInput, description, version;
    vector<string> archOptions = {"amd64", "arm", "i386"};

    // Step 1: Relative file path
    cout << "Enter relative path to binary (e.g., ../bin/myapp): ";
    getline(cin, filePath);
    if (filePath.empty()) {
        cerr << "❌ File path cannot be empty.\n";
        return 1;
    }

    fs::path srcPath = fs::absolute(filePath);
    if (!fs::exists(srcPath) || !fs::is_regular_file(srcPath)) {
        cerr << "❌ Binary file not found or not a regular file: " << srcPath << "\n";
        return 1;
    }

    // Step 2: App name (optional)
    cout << "Enter app name (leave blank to use binary name): ";
    getline(cin, appNameInput);
    string appName = appNameInput.empty() ? srcPath.filename().string() : appNameInput;

    // Step 3: Description
    cout << "Enter app description (optional): ";
    getline(cin, description);

    // Step 4: Architecture
    cout << "Select architecture (interactive menu, Enter to auto-detect):\n";
    int archIndex = selectOption(archOptions, "Choose architecture:");
    string systemArch = getSystemArch();
    string architecture = archOptions[archIndex].empty() ? systemArch : archOptions[archIndex];

    // Step 5: Version (required)
    do {
        cout << "Enter app version (required, e.g., 1.0.0): ";
        getline(cin, version);
        if (version.empty()) cout << "❌ Version is required.\n";
    } while (version.empty());

    // Step 6: Summary
    cout << "\n✅ Summary:\n";
    cout << "File Path: " << srcPath << "\n";
    cout << "App Name: " << appName << "\n";
    cout << "Description: " << (description.empty() ? "(none)" : description) << "\n";
    cout << "Architecture: " << architecture << "\n";
    cout << "Version: " << version << "\n\n";

    // Step 7: Create package structure
    fs::remove_all("package");
    fs::create_directories("package/usr/bin");
    fs::create_directories("package/DEBIAN");
    fs::create_directories("dist");
    cout << "📁 Created package directory structure.\n";

    // Step 8: Copy binary safely (fixed logic)
    fs::path destBinary = fs::path("package/usr/bin") / appName;

    // Ensure parent directories exist
    fs::create_directories(destBinary.parent_path());

    // Debug info
    cout << "DEBUG: srcPath exists? " << fs::exists(srcPath) << "\n";
    cout << "DEBUG: srcPath is regular file? " << fs::is_regular_file(srcPath) << "\n";
    cout << "DEBUG: destBinary parent exists? " << fs::exists(destBinary.parent_path()) << "\n";
    cout << "DEBUG: destBinary: " << destBinary << "\n";

    // Copy
    fs::copy_file(srcPath, destBinary, fs::copy_options::overwrite_existing);
    cout << "✅ Copied binary to " << destBinary << "\n";

    // Step 9: Create control file
    ofstream controlFile("package/DEBIAN/control");
    controlFile << createControlFile(appName, version, architecture, description);
    controlFile.close();
    cout << "✅ Created DEBIAN/control file.\n";

    // Step 10: Fix permissions
    system("chmod -R 755 package/DEBIAN");

    // Step 11: Build .deb
    string debPath = "dist/" + appName + "_" + version + "_" + architecture + ".deb";
    string cmd = "dpkg-deb --build package " + debPath;
    cout << "📦 Building .deb package...\n";
    int result = system(cmd.c_str());
    if (result == 0)
        cout << "✅ Successfully created " << debPath << "\n";
    else
        cerr << "❌ Failed to build .deb package.\n";

    return 0;
}
