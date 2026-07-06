# Nexus Mods Author Tools

Nexus Mods Author Tools is an open-source Unreal Engine plugin designed to simplify the process of creating, managing, packaging, and publishing game mods to Nexus Mods.

Rather than requiring authors to switch between multiple external tools, the plugin integrates common mod development workflows directly into the Unreal Editor, helping both new and experienced creators spend less time on project setup and more time building content.

<div align="center">

![Status](https://img.shields.io/badge/Status-Active_Development-2EA043?style=for-the-badge)
[![Version](https://img.shields.io/badge/Version-v0.1-3B82F6?style=for-the-badge)](https://github.com/Nexus-Mods/NexusModsAuthorTools/releases)
[![License](https://img.shields.io/badge/License-Custom-6B7280?style=for-the-badge)](/LICENSE)
![Compatibility](https://img.shields.io/badge/Unreal_Engine-4.26_→_5.8-8B5CF6?style=for-the-badge)

</div>

[![Nexus Mods](https://img.shields.io/badge/Nexus_Mods-Author_Tool-EF7D00?style=for-the-badge)](https://www.nexusmods.com/)

<h1>Header Test</h1>

# Features

Current capabilities include:

* Integrated Nexus Mods Upload API support
* Built-in mod template management
* Mod archive creation before upload
* Unreal Editor toolbar and window integration
* Extensible framework for future authoring tools

The plugin is actively developed and additional functionality will continue to be added over time.

# Coming Soon

Planned areas of development include:

* Markdown rendering for in-editor help, documentation, and release notes
* Additional mod packaging & modding workflows
* Improved validation and diagnostics
* Additional project templates
* Expanded editor utilities
* Improved documentation


# Supported Engine Versions

The plugin currently supports:

* Unreal Engine 4.26
* Unreal Engine 4.27
* Unreal Engine 5.0
* Unreal Engine 5.1
* Unreal Engine 5.2
* Unreal Engine 5.3
* Unreal Engine 5.4
* Unreal Engine 5.5
* Unreal Engine 5.6
* Unreal Engine 5.7
* Unreal Engine 5.8

Support for additional engine versions may be added in future releases.


# Installation

1. Clone or download this repository.
2. Copy the plugin into your project's `Plugins` directory.
3. Regenerate project files if required.
4. Compile the project using Visual Studio or Unreal Build Tool.
5. Open your project in Unreal Engine.
6. Enable the plugin if prompted.

# Building

The repository includes helper scripts for building and releasing the plugin across multiple Unreal Engine versions.

Available build actions include:

1. Build Packages
2. Release Packages
3. Build then Release
4. Exit

Build output is organised into the following directories:

| Folder         | Purpose                          |
| -------------- | -------------------------------- |
| `Output/Build` | Temporary packaged plugin builds |
| `Output/Dist`  | Final distributable ZIP archives |
| `Output/Logs`  | Build and release logs           |

Build configuration is controlled through:

```
Scripts/BuildConfig.ps1
```

This allows configuration of:

* Unreal Engine installation paths
* Target platforms
* Plugin information
* Archive naming
* GitHub release behaviour
* Build output options

# Project Structure

The project is organised into several primary systems:

* **API Services** — communication with Nexus Mods services.
* **Editor UI** — custom Slate widgets and windows.
* **Template System** — creation and management of mod templates.
* **Archive Services** — packaging and distribution.
* **Build Scripts** — automated multi-version packaging and release tooling.
* **Compatibility Layer** — shared Unreal Engine version abstraction supporting UE4 and UE5.

# Contributing

Contributions are welcome.

If you would like to contribute:

* Report bugs through GitHub Issues.
* Suggest new features or improvements.
* Submit Pull Requests for fixes or enhancements.
* Follow the existing project coding style and conventions.

Please keep changes focused, well documented, and consistent with the project's architecture.

# Troubleshooting

### The plugin does not appear inside Unreal Editor

* Verify that the plugin has been compiled successfully.
* Ensure it is located inside the project's `Plugins` directory.
* Confirm that the plugin is enabled in the Plugins window.

### Build scripts cannot find Unreal Engine

Verify the engine paths configured in:

```
Scripts/BuildConfig.ps1
```

### Packaging fails

Check the logs inside:

```
Output/Logs
```

for detailed diagnostics.

# License

This project is licensed under the terms described in the accompanying LICENSE file.

While contributions are welcome, the project is intended to support the Nexus Mods ecosystem and may not be used to develop competing commercial mod distribution services as described in the accompanying license.

# Third-Party Libraries

This project makes use of several third-party libraries:

| Library                  | Purpose                             | License              |
| ------------------------ | ----------------------------------- | -------------------- |
| **miniz**                | ZIP archive creation and extraction | Public Domain / MIT  |
| **Font Awesome v7 Free** | User interface icons                | CC BY 4.0            |

All third-party libraries remain the property of their respective authors.

# Disclaimer

Nexus Mods Author Tools is developed by Nexus Mods to improve the Unreal Engine mod authoring experience.

This project is not affiliated with or endorsed by Epic Games.

Unreal Engine is a trademark or registered trademark of Epic Games, Inc.