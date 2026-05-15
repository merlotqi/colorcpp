# colorcpp Debian Official Submission Guide

This guide describes the path from a local Debian source package to a submission that can be sponsored into the Debian archive.

## 1. Prepare the source package

Ensure the Debian packaging metadata is complete:

- `debian/control`
- `debian/changelog`
- `debian/copyright`
- `debian/rules`
- `debian/source/format`
- `debian/watch`

Build a source package with signatures:

```bash
debuild -S -sa
```

If you only want to validate the source package locally without uploading yet:

```bash
dpkg-buildpackage -S -us -uc
```

## 2. Run quality checks

Run the Debian packaging lint pass:

```bash
lintian ../colorcpp_0.3.0-1_source.changes
```

Prefer a clean build in a Debian chroot before asking for sponsorship:

```bash
sbuild -A -s -v
```

If you use `pbuilder`, run the equivalent `pdebuild` flow instead.

## 3. Upload to mentors

If you are not a Debian Developer, upload the signed source package to mentors.debian.net and request sponsorship.

Typical flow:

```bash
dput mentors ../colorcpp_0.3.0-1_source.changes
```

Then post the package on Debian mentors and ask a Debian sponsor to review and upload it.

## 4. Sponsor review checklist

- The package is source-only and builds cleanly in a chroot.
- The copyright file is complete and accurate.
- The changelog explains the upload reason and target suite.
- The package name is appropriate for Debian archive policy.
- The installed files match the header-only scope.

## 5. After upload

- Watch the Debian NEW queue if the package name or binary package is new.
- Fix any sponsor feedback or lintian issues before re-submitting.
- After acceptance, the package will move through `unstable` and Debian migration as usual.
