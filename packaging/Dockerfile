FROM debian:bookworm-slim

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    debhelper \
    devscripts \
    dpkg-dev \
    git \
    libx11-dev \
    libxinerama-dev \
    libxft-dev \
    && rm -rf /var/lib/apt/lists/*

# Create build directory
WORKDIR /build

# Copy source code
COPY . /build/

# Build the package
RUN cd /build && \
    make clean && \
    make && \
    mkdir -p /build/packaging/build && \
    cd /build/packaging && \
    bash -c 'dpkg-buildpackage -us -uc -b || true' && \
    cp -v /build/*.deb /build/packaging/build/ 2>/dev/null || true && \
    cp -v /build/../*.deb /build/packaging/build/ 2>/dev/null || true && \
    if [ ! -f /build/packaging/build/*.deb ]; then \
      echo "Falling back to manual package build..." && \
      cd /build && \
      bash /build/packaging/build-package.sh; \
    fi

# Output directory
VOLUME /output
CMD ["bash", "-c", "cp -v /build/packaging/build/*.deb /output/ 2>/dev/null || echo 'No packages found to copy'"]