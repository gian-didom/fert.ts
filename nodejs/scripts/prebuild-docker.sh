#!/bin/bash

# Cross-platform prebuild script using Docker
# This script builds prebuilds for Linux platforms using Docker

set -e

echo "🐳 Building FERT Node.js prebuilds using Docker"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"  # Go up to the main project root

echo "📁 Project root: $PROJECT_ROOT"

cd "$PROJECT_ROOT"

# Build the Docker image from the main project directory
echo "📦 Building Docker image..."
docker build -f nodejs/Dockerfile -t fert-prebuild .

# Create output directory in nodejs folder
mkdir -p nodejs/prebuilds

# Extract prebuilds from Docker container
echo "📤 Extracting prebuilds..."
docker run --rm -v "$(pwd)/nodejs/prebuilds:/output" fert-prebuild sh -c "
    cp -r prebuilds/* /output/ 2>/dev/null || true
    echo 'Contents copied to /output:'
    ls -la /output/
"

echo "✅ Docker-based prebuild complete!"
echo "📁 Prebuilds available in: nodejs/prebuilds/"

# List created prebuilds
if [ -d "prebuilds" ] && [ "$(ls -A prebuilds)" ]; then
    echo ""
    echo "🎯 Created prebuilds:"
    find prebuilds -name "*.node" -exec ls -lh {} \;
else
    echo "⚠️  No prebuilds were created"
fi
