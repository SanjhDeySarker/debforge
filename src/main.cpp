#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

int main() {
    // Input configuration
    std::string appName = "myapp";
    std::string version = "1.0.0";
    std::string maintainer = "Your Name <you@example.com>";
    std::string description = "A simple example Debian package created from C++";
    std::string arch = "amd64";
    std::string binaryPath = "../bin/myapp";  // path to your compiled binary

    try {
        // Create required directories
        fs::create_directories("package/DEBIAN");
        fs::create_directories("package/usr/bin");
        std::cout << "📁 Created package directory structure.\n";

        // Copy the binary into /usr/bin
        if (!fs::exists(binaryPath)) {
            std::cerr << "❌ Binary not found at: " << binaryPath << "\n";
            return 1;
        }
        fs::copy_file(binaryPath, "package/usr/bin/" + appName,
                      fs::copy_options::overwrite_existing);
        std::cout << "✅ Copied binary into package/usr/bin/" << appName << "\n";

        // Create the control file
        std::ofstream control("package/DEBIAN/control");
        control << "Package: " << appName << "\n"
                << "Version: " << version << "\n"
                << "Section: base\n"
                << "Priority: optional\n"
                << "Architecture: " << arch << "\n"
                << "Maintainer: " << maintainer << "\n"
                << "Description: " << description << "\n";
        control.close();
        std::cout << "✅ Created DEBIAN/control file.\n";

        // Build the .deb package
        fs::create_directories("dist");
        std::string debFile = "dist/" + appName + "_" + version + "_" + arch + ".deb";
        std::string cmd = "dpkg-deb --build package " + debFile;

        std::cout << "📦 Building .deb package...\n";
        int result = system(cmd.c_str());
        if (result == 0)
            std::cout << "✅ Successfully created " << debFile << "\n";
        else
            std::cerr << "❌ Failed to build .deb package.\n";

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
