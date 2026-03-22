# Reproducible build + unit tests (header-only library; image documents a verified toolchain).
FROM ubuntu:24.04 AS verify

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -G Ninja -B /build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCOLORCPP_BUILD_TESTS=ON \
    -DCOLORCPP_BUILD_EXAMPLES=ON \
  && cmake --build /build \
  && ctest --test-dir /build --output-on-failure

# Optional: retain build tree for interactive debugging
FROM verify AS dev
WORKDIR /src
CMD ["/bin/bash"]
