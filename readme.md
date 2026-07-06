# Nexus Mods: Unreal Engine Author Tools

<div align="center">
    <img src=".github/Images/AuthorModToolsUE.png" style="margin-top: 28px;">
</div>
<div align="center">

[![Status](https://img.shields.io/badge/Status-Active_Development-2EA043?style=for-the-badge)](https://github.com/Nexus-Mods/NexusModsAuthorToolsUE)
[![Version](https://img.shields.io/github/v/release/Nexus-Mods/NexusModsAuthorToolsUE?label=Version&style=for-the-badge)](https://github.com/Nexus-Mods/NexusModsAuthorToolsUE/releases)
[![License](https://img.shields.io/badge/License-Custom-6B7280?style=for-the-badge)](/LICENSE)
[![Compatibility](https://img.shields.io/badge/Unreal_Engine-4.26_→_5.8-8B5CF6?style=for-the-badge)](#supported-engine-versions)

</div>

---

# Project Goals

Nexus Mods Author Tools is an open-source Unreal Engine plugin designed to simplify the process of creating, managing, packaging, and publishing game mods to Nexus Mods. The aim is to reduce the technical barriers involved in creating Unreal Engine game mods by providing a consistent, integrated workflow directly within the Unreal Editor. 

Rather than requiring authors to switch between multiple external tools, the plugin integrates common mod development workflows directly into the Unreal Editor, helping both new and experienced creators spend less time on project setup and more time building content. 

---

# Features

Current capabilities include:

- Native Unreal Editor integration
- Integrated Nexus Mods upload support
- Built-in mod template management
- Mod archive creation prior to upload
- Cross-version support for Unreal Engine 4 and 5
- Extensible framework for future authoring tools

---

# Roadmap

Planned areas of development include:

- Markdown rendering for in-editor help, documentation and release notes
- Additional modding workflows and project templates
- Improved validation and diagnostics
- Expanded editor utilities
- Improved documentation

---

# Supported Engine Versions

The plugin currently supports the following Unreal Engine versions:

- Unreal Engine 4.26
- Unreal Engine 4.27
- Unreal Engine 5.0
- Unreal Engine 5.1
- Unreal Engine 5.2
- Unreal Engine 5.3
- Unreal Engine 5.4
- Unreal Engine 5.5
- Unreal Engine 5.6
- Unreal Engine 5.7
- Unreal Engine 5.8

Support for additional engine versions may be added in future releases.

---

# Installation

Pre-built plugin packages are available from the project's [**GitHub Releases**](https://github.com/Nexus-Mods/NexusModsAuthorToolsUE/releases) page.

1. Download the package matching your Unreal Engine version.
2. Extract the archive.
3. Create a `Plugins` directory in your Unreal Engine project folder (if it doesnt already exist).
4. Copy the `NexusModsAuthorTools` folder into your project's `Plugins` folder.

Example:

```text
MyAwesomeProject/
└── Plugins/
    └── NexusModsAuthorTools/
```

4. Launch Unreal Engine.
5. Enable the plugin if prompted.
6. Restart the editor if required.

---

# Building From Source

Building from source is only required if you wish to contribute to the project or make your own modifications.

## Requirements

- Unreal Engine 4.26–5.8
- Visual Studio 2019 or newer
- Git

## Building

1. Clone this repository inside your project's `Plugins` directory.
2. Regenerate project files if required.
3. Build the project using Visual Studio or Unreal Build Tool.
4. Launch Unreal Engine.

---

# Release Tooling

The repository includes optional PowerShell scripts used to automate building and releasing the plugin across multiple Unreal Engine versions.

## Available Actions

1. Build Packages
2. Release Packages
3. Build then Release
4. Exit

## Output Directories

| Folder | Purpose |
|---------|---------|
| `Output/Build` | Temporary packaged plugin builds |
| `Output/Dist` | Final distributable ZIP archives |
| `Output/Logs` | Build and release logs |

## Configuration

Build behaviour can be configured through:

```text
Scripts/BuildConfig.ps1
```

This includes:

- Unreal Engine installation paths
- Target platforms
- Plugin information
- Archive naming
- GitHub release behaviour
- Build output options


---

# Contributing

Contributions are welcome.

If you would like to contribute:

- Report bugs through GitHub Issues.
- Suggest new features or improvements.
- Submit Pull Requests.
- Follow the existing project architecture and coding style.

Please keep contributions focused, well documented, and consistent with the project's overall design.

---

# Troubleshooting

## The plugin does not appear inside Unreal Editor

- Verify the plugin is located inside your project's `Plugins` directory.
- Ensure the plugin has been enabled.
- Confirm you downloaded the package matching your Unreal Engine version.

## Build scripts cannot find Unreal Engine

Verify the engine installation paths configured in:

```text
Scripts/BuildConfig.ps1
```

## Packaging fails

Check the logs located in:

```text
Output/Logs
```

for detailed diagnostics.

---

# License

This project is licensed under the terms described in the accompanying [LICENSE](LICENSE) file.

While contributions are welcome, the project is intended to support the Nexus Mods ecosystem and may not be used to develop competing commercial mod distribution services as described in the license.

---

# Third-Party Libraries / Resources

This project includes the following third-party libraries and assets:

| Library/Resource | Purpose | License |
|---------|---------|---------|
| [**FontAwesome**](https://fontawesome.com) | User interface icons | CC BY 4.0 |
| [**miniz**](https://github.com/richgel999/miniz) | ZIP archive creation and extraction | MIT / Public Domain |

---

# Disclaimer

Nexus Mods Author Tools is developed by Nexus Mods to improve the Unreal Engine mod authoring experience.

This project is not affiliated with or endorsed by Epic Games.

Unreal Engine is a trademark or registered trademark of Epic Games, Inc.