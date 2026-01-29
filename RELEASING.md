# Releasing

This project follows a simple, tag-based release process suitable for an ESPHome external component.

1. Prepare changes
   - Update `CHANGELOG.md` with a new version section.
   - Ensure CI is green (Markdown lint, link check, ESPHome config validation).

2. Version bump
   - Update the version in `CHANGELOG.md` to e.g. `v1.0.0`.

3. Tag and push
   - Create an annotated tag: `git tag -a v1.0.0 -m "First stable release"`
   - Push tags: `git push --tags`

4. GitHub Release (optional)
   - Draft a GitHub Release for the tag and summarize changes.

5. Pinning in ESPHome
   - In your YAML, reference a specific tag for reliable builds:
     ```yaml
     external_components:
       - source: github://<your-org-or-user>/heater@v1.0.0
         components: [diesel_heater_ble]
     ```

6. Post-release checks
   - Validate installation via the example YAML.
   - Triage any user reports and update `doc/COMPATIBILITY.md`.
