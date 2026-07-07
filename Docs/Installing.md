# Installing

> [!NOTE]
> Nexus Mods Author Tools currently supports Unreal Engine **4.26 → 5.8**.

## Requirements

Before installing the plugin, make sure you have:

- A supported Unreal Engine project.
- A downloaded release of **Nexus Mods Author Tools**.
- Access to your project's `Plugins` directory.

## Installation

1. Download the latest release from the [Releases](https://github.com/Nexus-Mods/NexusModsAuthorToolsUE/releases) page.
2. Extract the plugin archive.
3. Copy the plugin folder into your Unreal project's `Plugins` directory.

   Your project should look similar to this:

   ```text
   YourProject/
   ├── Content/
   ├── Plugins/
   │   └── NexusModsAuthorTools/
   └── YourProject.uproject
   ```

4. Open or restart Unreal Engine.
5. If prompted, allow Unreal Engine to rebuild the plugin.
6. Enable **Nexus Mods Author Tools** from the Unreal Engine plugin browser if it is not already enabled.

## Verifying Installation

After the project opens:

1. Go to **Edit → Plugins**.
2. Search for **Nexus Mods Author Tools**.
3. Confirm that the plugin is enabled.

> [!TIP]
> Installing the plugin per-project is recommended unless you specifically need it available to every Unreal Engine project on your machine.

## Next Steps

- 🧭 [Using the Plugin](Using.md)
- 🛠️ [Building From Source](Building.md)
- 🩹 [Troubleshooting](Troubleshooting.md)
- 🤝 [Contributing](Contributing.md)
