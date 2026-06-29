# Making a release

Releases are created from tags on `main`. The release workflow will only publish a release when all of the following are true:

* the tag has the form `vX.Y.Z`
* the version in `CMakeLists.txt` matches the tag
* the tagged commit is the current `main` HEAD

If any of these checks fail, the workflow fails and no release is published.

## 1. Update the version

Set the release version in `CMakeLists.txt`

```cmake
project(
  daisy
  VERSION X.Y.Z
  ...
)
```

Commit that change on a branch, open a pull request, and merge it to `main`.

## 2. Create the release tag

Update your local `main` to the merged `origin/main` HEAD

    git checkout main
    git pull --ff-only origin main

Then create the tag from the version in `CMakeLists.txt`

    make release-tag

This creates an annotated tag named `vX.Y.Z`. The target refuses to tag unless your local `main` is exactly at the current `origin/main` HEAD.

## 3. Push the tag

Push the new tag to GitHub

    git push origin vX.Y.Z

## 4. Wait for the release workflow

Pushing the tag starts the GitHub release workflow. It will build and publish these release artifacts:

* Linux: `.deb` and `.flatpak`
* MacOS: `.zip`
* Windows: `.exe` installer and `.zip`

The workflow then creates or updates the GitHub release for that tag and uploads the artifacts.
