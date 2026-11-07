#!/bin/sh
# Builds the 3DS port's acww.3dsx (the enhanced port, --direct) in devkitPro's Docker image, so that nothing but
# Docker has to be installed: tools/build_3ds_docker.sh -> build/host3ds_direct_usa/acww.3dsx
# The image is pinned (devkitARM 16.1.0, the toolchain the port is built with); a newer one may need code changes.
set -e
cd "$(dirname "$0")/.."
IMAGE=devkitpro/devkitarm@sha256:116afba8df8453961de2936ffab20dd441edf4d682856c1ec8b0e53d7ed0bbf5
# as the user's own uid, so that the build's files don't belong to root
docker run --rm --user "$(id -u):$(id -g)" -v "$PWD":/src -w /src "$IMAGE" \
    sh -c 'python3 tools/configure_host.py --os 3ds --direct && ninja -f build_host_3ds_direct.ninja'
echo "Built: build/host3ds_direct_usa/acww.3dsx"
