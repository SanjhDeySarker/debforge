DebForge

DebForge is a lightweight C++17 CLI tool that converts compiled binaries into Linux .deb installer packages.
It supports both interactive prompts (for beginners) and CI/CD-friendly command-line flags (for automation).

With support for optional preinst and postinst maintainer scripts, DebForge makes it easy to package production-ready software.

Future versions will add support for .rpm, Windows installers, and macOS .pkg.

✨ Features

Convert a single binary into a valid .deb installer.

Interactive mode → step-by-step prompts for beginners.

Non-interactive mode → CLI flags for automation/CI pipelines.

Auto-detects system architecture (default), with override options (amd64, arm64, i386).

Optional preinst and postinst script support.

Output ready-to-install .deb file (dpkg -i myapp.deb).

🚀 Installation (Build from Source)
git clone https://github.com/yourusername/debforge.git
cd debforge
mkdir build && cd build
cmake ..
make
sudo make install


This will install debforge as a global CLI command.

🛠 Usage
Interactive Mode
debforge


Sample flow:

Enter path to binary file: ./bin/myapp
Enter application name: MyApp
Enter description: Sample application
Enter version: 1.0.0
Enter maintainer (Name <email>): Ankan <ankan@example.com>
Enter license: MIT
Select architecture [amd64/arm64/i386] (default amd64): amd64
Enter dependencies (comma-separated, optional): libc6, libssl1.1
Provide preinst script path (optional): ./scripts/preinst.sh
Provide postinst script path (optional): ./scripts/postinst.sh

✅ Successfully generated: ./dist/myapp_1.0.0_amd64.deb

Non-Interactive Mode (CI/CD Friendly)
debforge \
  --binpath ./bin/myapp \
  --name MyApp \
  --desc "Sample App" \
  --version 1.0.0 \
  --maintainer "Ankan <ankan@example.com>" \
  --arch amd64 \
  --deps "libc6, libssl1.1" \
  --preinst ./scripts/preinst.sh \
  --postinst ./scripts/postinst.sh \
  --output ./dist

📂 Example Package Structure
myapp_1.0.0_amd64.deb
└── DEBIAN/
    ├── control
    ├── preinst     (optional, if provided)
    ├── postinst    (optional, if provided)
└── usr/
    └── local/
        └── bin/
            └── myapp

🧪 Roadmap

 .deb packaging support

 Interactive + Non-interactive modes

 Preinst & Postinst script support

 .rpm support (Fedora/RedHat)

 Windows installer generator (.exe/.msi)

 macOS .pkg support

 Config file input (.yaml / .json)

 GUI wrapper
