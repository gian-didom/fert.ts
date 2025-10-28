# Multi-stage Dockerfile for cross-platform builds

# Base image with Node.js and build tools
FROM node:18-bullseye-slim as base

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    python3 \
    python3-pip \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the entire project structure  
COPY . .

# Install cmake-js globally and Node.js dependencies
RUN npm install -g cmake-js && cd nodejs && npm ci

# Linux x64 build
FROM base as linux-x64

# Build the main FERT library first
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=0 .. && \
    make -j$(nproc)

# Build the Node.js addon
RUN cd nodejs && npm run build

# Create prebuild structure
RUN cd nodejs && node scripts/prebuild.js --platform=linux --arch=x64

# Linux ARM64 build  
FROM base as linux-arm64

# Install cross-compilation tools
RUN apt-get update && apt-get install -y \
    gcc-aarch64-linux-gnu \
    g++-aarch64-linux-gnu \
    && rm -rf /var/lib/apt/lists/*

# Set cross-compilation environment
ENV CC=aarch64-linux-gnu-gcc
ENV CXX=aarch64-linux-gnu-g++
ENV AR=aarch64-linux-gnu-ar
ENV STRIP=aarch64-linux-gnu-strip

# Build the main FERT library for ARM64
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=0 \
          -DCMAKE_SYSTEM_NAME=Linux \
          -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
          -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
          -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ \
          .. && \
    make -j$(nproc)

# Build the Node.js addon for ARM64  
RUN cd nodejs && npm run build -- --arch=arm64

# Create prebuild structure
RUN cd nodejs && node scripts/prebuild.js --platform=linux --arch=arm64

# Final stage to collect prebuilds
FROM base as collector
COPY --from=linux-x64 /app/nodejs/prebuilds/ ./prebuilds/
COPY --from=linux-arm64 /app/nodejs/prebuilds/ ./prebuilds/

CMD ["ls", "-la", "prebuilds/"]
