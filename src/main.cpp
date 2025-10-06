#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib> // for system()
#include <string>

namespace fs = std::filesystem;

int main() {
    // --- Adjust binary path relative to current working directory
    // If you run from 'build/', use ../bin/myapp
    std::string userBinaryPath = "../bin/myapp"; 
    std::string appName = "myapp";                      
    std::string version = "1.0.0";                      
    std::string maintainer = "Ankan <ankan@example.com>";
    std::string description = "Sample app built with DebForge"; 
    std::string arch = "amd64";                         
    std::string dependencies = "libc6";                 
    std::string preinstPath = "../scripts/preinst";     
    std::string postinstPath = "../scripts/postinst";  

    try {
        // --- Create folder structure
        fs::create_directories("package/DEBIAN");
        fs::create_directories("package/usr/local/bin");
        std::cout << "📁 Created package structure.\n";

        // --- Verify binary exists
        if (!fs::exists(userBinaryPath)) {
            std::cerr << "❌ Binary not found at " << userBinaryPath << "\n";
            return 1;
        }

        // --- Copy binary into package
        fs::copy_file(userBinaryPath, "package/usr/local/bin/" + appName,
                      fs::copy_options::overwrite_existing);
        std::cout << "✅ Copied binary to package/usr/local/bin/" << appName << "\n";

        // --- Create DEBIAN/control file
        std::ofstream control("package/DEBIAN/control");
