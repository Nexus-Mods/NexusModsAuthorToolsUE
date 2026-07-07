# Release Tools

> [!IMPORTANT]
> This guide is intended for project maintainers responsible for building and publishing releases.

## Requirements

Before creating a release, make sure you have:

- Git installed and available on your PATH.
- GitHub CLI installed and authenticated, if used by the release tooling.
- A clean working tree.
- A supported Unreal Engine installation.
- The plugin version updated as required.

## Typical Release Workflow

1. Update the plugin version and version name.
2. Update documentation or changelog entries as required.
3. Commit and push your changes.
4. Run the release tooling.
5. Verify the generated packages.
6. Publish or update the GitHub release.
7. Download and test the published archive where possible.

## Versioning

Before publishing a release, confirm that the plugin version metadata is correct.

For Unreal Engine plugins, this usually means checking the `.uplugin` file and ensuring both the numeric version and display version are appropriate for the release.

## Release Output

The release tooling should produce packaged plugin archives suitable for distribution through GitHub Releases.

Generated archives should be checked before publishing to confirm that:

- Required plugin files are included.
- Temporary build files are excluded.
- The archive name clearly identifies the plugin version.
- The plugin can be installed from the archive.

## Troubleshooting

If the release process fails:

- Confirm GitHub authentication is valid.
- Confirm required tags exist or can be created.
- Ensure version numbers have been updated.
- Check script output for the first meaningful error.
- Confirm the repository and release permissions are correct.

## Next Steps

- 🧭 [Using the Plugin](Using.md)
- 🛠️ [Building From Source](Building.md)
- 🤝 [Contributing](Contributing.md)
- 🩹 [Troubleshooting](Troubleshooting.md)
