# Project Progress

## Current Sprint/Phase: Automate Releases with GitHub Actions
- [ ] Create a new GitHub Actions workflow file for releases.
- [ ] Configure the workflow to trigger on new tag creation (e.g., `v*`).
- [ ] Set up a build matrix for multiple operating systems (Linux, macOS, Windows).
- [ ] Add steps to compile the project for each OS in the matrix.
- [ ] Add a step to package the build artifacts (e.g., into a zip or tar.gz file).
- [ ] Add a step to create a new GitHub Release corresponding to the tag.
- [ ] Add a step to upload the packaged build artifacts to the GitHub Release.

## Backlog
- [ ] Investigate creating installers for different platforms.
- [ ] Add automated testing to the release workflow.

## Notes
- This plan addresses issue #43 by automating the build and release process.
- The workflow will need the specific build commands for the Daisy model.
- The initial focus will be on creating compiled binaries for major platforms.
