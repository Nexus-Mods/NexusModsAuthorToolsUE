# Building From Source

> [!IMPORTANT]
> Building from source is only required if you want to contribute to the project, customise the plugin, or test unreleased changes.

## Requirements

Before building the plugin, make sure you have:

- A supported Unreal Engine version.
- Visual Studio with the C++ workload installed.
- Git installed and available on your PATH.
- A local clone of this repository.

## Clone the Repository

```bash
git clone https://github.com/Nexus-Mods/NexusModsAuthorToolsUE.git
```

## Building the Plugin

1. Place or clone the repository into your Unreal project's `Plugins` directory.
2. Generate project files if required.
3. Open the project in Visual Studio or your preferred Unreal Engine development environment.
4. Build the project.
5. Launch Unreal Engine and confirm the plugin loads successfully.

## Verifying the Build

After compiling successfully:

- Launch Unreal Engine.
- Open your project.
- Go to **Edit → Plugins**.
- Search for **Nexus Mods Author Tools**.
- Confirm the plugin is enabled and loads without errors.

## Common Issues

If the plugin fails to compile:

- Confirm your Unreal Engine version is supported.
- Regenerate project files.
- Check that the required Visual Studio components are installed.
- Review the build output for the first relevant error.

For more help, see [Troubleshooting](Troubleshooting.md).

## Next Steps

- 🧭 [Using the Plugin](Using.md)
- 🚀 [Installing](Installing.md)
- 🩹 [Troubleshooting](Troubleshooting.md)
- 🤝 [Contributing](Contributing.md)
