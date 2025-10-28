#!/bin/bash

# Simple Docker-based Linux prebuild script
# This builds FERT Node.js prebuilds for Linux x64 only

set -e

echo "🐳 Building FERT Node.js prebuilds for Linux x64 using Docker"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "📁 Project root: $PROJECT_ROOT"

cd "$PROJECT_ROOT"

# Create a simple Dockerfile for this specific build
cat > /tmp/fert-linux-build.dockerfile << 'EOF'
FROM node:18-bullseye-slim

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3 \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Install Node.js dependencies
RUN cd nodejs && npm ci

# Build the main FERT library
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=0 .. && \
    make -j$(nproc)

# Build the Node.js addon for Linux x64
RUN cd nodejs && npm run build

# Create prebuild manually
RUN cd nodejs && \
    mkdir -p prebuilds/linux-x64 && \
    cp build/Release/fert-node.node prebuilds/linux-x64/node.napi.node && \
    echo '{"platform":"linux","arch":"x64","buildTime":"'$(date -Iseconds)'"}' > prebuilds/linux-x64/manifest.json

CMD ["find", "nodejs/prebuilds", "-name", "*.node", "-ls"]
EOF

# Build the Docker image
echo "📦 Building Docker image for Linux x64..."
docker build -f /tmp/fert-linux-build.dockerfile -t fert-linux-prebuild .

# Create output directory in nodejs folder
mkdir -p nodejs/prebuilds

# Extract prebuilds from Docker container
echo "📤 Extracting Linux prebuilds..."
docker run --rm -v "$(pwd)/nodejs/prebuilds:/output" fert-linux-prebuild sh -c "
    echo 'Copying prebuilds to /output...'
    cp -r nodejs/prebuilds/* /output/ 2>/dev/null || echo 'No prebuilds found to copy'
    echo 'Contents of output:'
    ls -la /output/
"

# Cleanup
rm -f /tmp/fert-linux-build.dockerfile

echo "✅ Docker-based Linux prebuild complete!"
echo "📁 Prebuilds available in: nodejs/prebuilds/"

# List created prebuilds
if [ -d "nodejs/prebuilds" ] && [ "$(ls -A nodejs/prebuilds)" ]; then
    echo ""
    echo "🎯 Created prebuilds:"
    find nodejs/prebuilds -name "*.node" -exec ls -lh {} \;
else
    echo "⚠️  No prebuilds were created"
fi
