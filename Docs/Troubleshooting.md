
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
