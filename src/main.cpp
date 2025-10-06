#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>

namespace fs = std::filesystem;

int main() {
    // --- User inputs (adjust paths if running from build/)
    std::string userBinaryPath = "../bin/myapp"; // path to your binary
    std::string appName = "myapp";                      
    std::string version = "1.0.0";                      
    std::string maintainer = "Ankan <ankan@example.com>";
    std::string description = "Sample app built with DebForge"; 
    std::string arch = "amd64";                         
    std::string dependencies = "libc6";                 
    std::string preinstPath = "../scripts/preinst";     
    std::string postinstPath = "../scripts/postinst";  

    try {
        // --- Create folder structure under package/usr/bin
        fs::create_directories("package/DEBIAN");
        fs::create_directories("package/usr/bin");
        std::cout << "📁 Created package structure.\n";

        // --- Verify binary exists
        if (!fs::exists(userBinaryPath)) {
            std::cerr << "❌ Binary not found at " << userBinaryPath << "\n";
            return 1;
        }

        // --- Copy binary into package/usr/bin
        fs::copy_file(userBinaryPath, "package/usr/bin/" + appName,
                      fs::copy_options::overwrite_existing);
        std::cout << "✅ Copied binary to package/usr/bin/" << appName << "\n";

        // --- Create DEBIAN/control file
        std::ofstream control("package/DEBIAN/control");
        if (!control) {
            std::cerr << "❌ Failed to create control file!\n";
            return 1;
        }
        control << "Package: " << appName << "\n"
                << "Version: " << version << "\n"
                << "Section: base\n"
                << "Priority: optional\n"
                << "Architecture: " << arch << "\n"
                << "Depends: " << dependencies << "\n"
                << "Maintainer: " << maintainer << "\n"
                << "Description: " << description << "\n";
        control.close();
        std::cout << "✅ Control file created successfully!\n";

        // --- Copy optional preinst/postinst scripts
        if (fs::exists(preinstPath)) {
            fs::copy_file(preinstPath, "package/DEBIAN/preinst",
                          fs::copy_options::overwrite_existing);
            system("chmod +x package/DEBIAN/preinst");
            std::cout << "✅ Added preinst script.\n";
        }

        if (fs::exists(postinstPath)) {
            fs::copy_file(postinstPath, "package/DEBIAN/postinst",
                          fs::copy_options::overwrite_existing);
            system("chmod +x package/DEBIAN/postinst");
            std::cout << "✅ Added postinst script.\n";
        }

        // --- Create output folder
        fs::create_directories("../dist");
        
        // --- Build the .deb package
        std::string debFile = "../dist/" + appName + "_" + version + "_" + arch + ".deb";
        std::string cmd = "dpkg-deb --build package " + debFile;
        std::cout << "📦 Building .deb package...\n";

        int result = system(cmd.c_str());
        if (result == 0)
            std::cout << "✅ Successfully created: " << debFile << "\n";
        else
            std::cerr << "❌ Failed to build .deb package.\n";

    } catch (const fs::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
