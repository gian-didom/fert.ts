# FERT Node.js Prebuild System

This document explains how to build and distribute precompiled binaries for the FERT Node.js addon across multiple platforms and architectures.

## Overview

The prebuild system supports:
- **Platforms**: macOS (darwin), Linux, Windows (win32)
- **Architectures**: x64, arm64, ia32 (Windows only)
- **Node.js versions**: 16, 18, 20, 22

## Quick Start

### Local Development Build
```bash
# Build for current platform/architecture
npm run prebuild

# Build for specific architecture on current platform
npm run prebuild:x64
npm run prebuild:arm64

# Build for specific platform (if supported)
npm run prebuild:darwin
npm run prebuild:linux
npm run prebuild:win32

# Build for all supported targets on current platform
npm run prebuild:all
```

### Cross-Platform Builds

#### Using Docker (Linux targets)
```bash
# Build Linux prebuilds using Docker
npm run prebuild:docker
```

#### Using GitHub Actions
The repository includes GitHub Actions workflow (`.github/workflows/prebuild.yml`) that automatically builds prebuilds for all supported platforms when you:
- Push to main/master branch
- Create a tag starting with 'v'
- Manually trigger the workflow

## Manual Build Commands

### Local prebuild script
```bash
# Build for current platform
node scripts/prebuild.js

# Build with specific options
node scripts/prebuild.js --all
node scripts/prebuild.js --platform=linux --arch=x64
node scripts/prebuild.js --arch=arm64
```

### Using cmake-js directly
```bash
# Build for specific architecture
cmake-js build --arch=x64
cmake-js build --arch=arm64
```

## Cross-Compilation Support

### macOS
- ✅ **x64 ↔ arm64**: Full cross-compilation support
- Automatically builds for both architectures when available

### Linux
- ✅ **x64**: Native build supported
- ⚠️ **arm64**: Requires cross-compilation toolchain or Docker
- 🐳 **Docker**: Use `npm run prebuild:docker` for Linux cross-compilation

### Windows
- ✅ **x64**: Native build on Windows
- ✅ **ia32**: Supported on Windows
- ❌ **Cross-platform**: Building Windows binaries from macOS/Linux requires specialized tools

## CI/CD Integration

### GitHub Actions
The included workflow automatically:
1. Builds prebuilds for all supported platforms
2. Uploads artifacts for each build
3. Publishes to npm on version tags
4. Creates GitHub releases with prebuilds

### Prerequisites for CI
Set these secrets in your GitHub repository:
- `NPM_TOKEN`: For publishing to npm
- `GITHUB_TOKEN`: Automatically available

## Distribution

### End User Installation
When users install your package:
```bash
npm install fert-node
```

The installation process:
1. First tries to download a matching prebuild
2. Falls back to building from source if no prebuild available

### Publishing Prebuilds
```bash
# Upload prebuilds to GitHub releases
npm run prebuild-upload

# Or publish package with prebuilds to npm
npm publish
```

## File Structure

```
nodejs/
├── prebuilds/                 # Generated prebuilds
│   ├── darwin-x64/
│   │   ├── node.napi.node    # macOS x64 binary
│   │   └── manifest.json     # Build metadata
│   ├── darwin-arm64/
│   │   ├── node.napi.node    # macOS ARM64 binary
│   │   └── manifest.json
│   ├── linux-x64/
│   └── win32-x64/
├── scripts/
│   ├── prebuild.js           # Main prebuild script
│   └── prebuild-docker.sh    # Docker-based cross-compilation
├── .github/workflows/
│   └── prebuild.yml          # GitHub Actions workflow
└── Dockerfile                # For Linux cross-compilation
```

## Troubleshooting

### Build Failures
1. **Missing dependencies**: Ensure CMake and build tools are installed
2. **Cross-compilation**: Some targets may not be buildable on current platform
3. **Architecture mismatch**: Verify CMake architecture settings

### Testing Prebuilds
```bash
# Test that prebuilds work
npm run test

# Test specific prebuild
node -e "console.log(require('./prebuilds/darwin-x64/node.napi.node'))"
```

### Debug Information
Each prebuild includes a `manifest.json` with build details:
```json
{
  "platform": "darwin",
  "arch": "x64", 
  "nodeArch": "x64",
  "buildTime": "2024-01-01T00:00:00.000Z",
  "nodeVersions": ["16.0.0", "18.0.0", "20.0.0", "22.0.0"]
}
```

## Advanced Configuration

### Custom Node.js Versions
Edit `scripts/prebuild.js` to modify supported Node.js versions:
```javascript
const nodeVersions = ['16.0.0', '18.0.0', '20.0.0', '22.0.0', '24.0.0'];
```

### Platform-Specific Options
Modify the `targetConfigs` array in `prebuild.js` to add/remove target platforms.

### CMake Options
The prebuild script automatically handles CMake architecture settings, but you can customize them in the build commands.
