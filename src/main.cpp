#include <iostream>
#include <string>
#include <unordered_map>

void print_help() {
    std::cout << "DebForge - Binary to .deb packager\n\n"
              << "Usage:\n"
              << "  debforge [options]\n\n"
              << "Options:\n"
              << "  --binpath <path>     Path to binary file (required)\n"
              << "  --name <appname>     Application name (required)\n"
              << "  --desc <description> Short description\n"
              << "  --version <version>  Version number\n"
              << "  --maintainer <info>  Maintainer name/email\n"
              << "  --arch <arch>        Target architecture (default: host)\n"
              << "  --deps <list>        Dependencies (comma-separated)\n"
              << "  --preinst <path>     Path to pre-install script (optional)\n"
              << "  --postinst <path>    Path to post-install script (optional)\n"
              << "  --output <dir>       Output directory (default: ./dist)\n"
              << "  -h, --help           Show this help\n";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_help();
        return 0;
    }

    std::unordered_map<std::string, std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string key = argv[i];
        if (key == "-h" || key == "--help") {
            print_help();
            return 0;
        }
        if (key.rfind("--", 0) == 0 && i + 1 < argc) {
            args[key] = argv[++i];
        }
    }

    if (args.find("--binpath") == args.end() || args.find("--name") == args.end()) {
        std::cerr << "Error: --binpath and --name are required.\n";
        return 1;
    }

    std::cout << "Packaging " << args["--name"]
              << " from binary: " << args["--binpath"] << std::endl;

    // Next step: Implement .deb structure generation here

    return 0;
}
