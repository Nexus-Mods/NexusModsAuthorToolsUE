# Troubleshooting

> [!NOTE]
> This page covers common issues encountered when installing, building, or using Nexus Mods Author Tools.

## Plugin Does Not Appear in Unreal Engine

Check the following:

- The plugin folder is inside your project's `Plugins` directory.
- The folder structure has not been nested incorrectly after extraction.
- Unreal Engine has been restarted after installation.
- The project is using a supported Unreal Engine version.

Expected structure:

```text
YourProject/
├── Plugins/
│   └── NexusModsAuthorTools/
│       ├── NexusModsAuthorTools.uplugin
│       ├── Source/
│       └── Resources/
```

## Unreal Engine Asks to Rebuild the Plugin

This can happen when the plugin was built with a different engine version.

Allow Unreal Engine to rebuild the plugin if prompted. If the rebuild fails, open the project in your development environment and build manually so you can inspect the full compiler output.

## Build Fails

If the plugin fails to build:

- Confirm Visual Studio is installed with C++ support.
- Regenerate project files.
- Confirm your Unreal Engine version is supported.
- Check the first compiler error in the build log rather than the final summary error.

## Packaging or Release Tooling Fails

If release scripts fail:

- Confirm Git is installed and available on your PATH.
- Confirm any required GitHub tooling is authenticated.
- Ensure the release version has been updated correctly.
- Review the script output for the first failure message.

## Still Need Help?

If the issue persists:

- Search the existing [GitHub Issues](https://github.com/Nexus-Mods/NexusModsAuthorToolsUE/issues).
- Open a new issue with reproduction steps, Unreal Engine version, plugin version, and relevant logs.

## Next Steps

- 🧭 [Using the Plugin](Using.md)
- 🚀 [Installing](Installing.md)
- 🛠️ [Building From Source](Building.md)
- 🤝 [Contributing](Contributing.md)
