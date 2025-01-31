# Start with a base image that includes CMake
FROM --platform=linux/amd64 debian:bullseye-slim

# Install essential build tools and CMake
RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update -qy && \
    apt-get install -y \
    cmake \
    g++ \
    make \
    ninja-build \
    gcovr \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app