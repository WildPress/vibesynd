# Syndicate matching-decomp pipeline image.
#
# Reproducible core of the harness: the period-era compiler (Open Watcom,
# targeting DOS/4GW just like the original 1995 build), an assembly-diff oracle,
# and the decomp permuter. The original binary is NEVER baked in -- it is mounted
# read-only at runtime.
#
# Build:  docker build -t synd-decomp reverse/docker
# Run:    reverse/run.sh <cmd>            (mounts the repo + inputs)

FROM debian:bookworm-slim

RUN apt-get update \
 && apt-get install -y --no-install-recommends \
      curl xz-utils ca-certificates git \
      python3 python3-pip \
      binutils \
 && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# Open Watcom v2 (open-source successor to the Watcom C/C++ that built the
# original). We extract the plain snapshot tree -- no interactive installer.
# NOTE: "Current-build" is a rolling tag. Pin to a dated build + checksum later
# if exact toolchain reproducibility becomes necessary for matching.
# ---------------------------------------------------------------------------
ARG OW_URL=https://github.com/open-watcom/open-watcom-v2/releases/download/Current-build/ow-snapshot.tar.xz
RUN mkdir -p /opt/watcom \
 && curl -fsSL "$OW_URL" | tar -xJ -C /opt/watcom \
 && chmod -R a+rX /opt/watcom \
 && /opt/watcom/binl64/wcc386 2>/dev/null | head -1 || true

ENV WATCOM=/opt/watcom \
    PATH=/opt/watcom/binl64:/opt/watcom/binl:${PATH} \
    INCLUDE=/opt/watcom/h \
    EDPATH=/opt/watcom/eddat \
    WIPFC=/opt/watcom/wipfc

# ---------------------------------------------------------------------------
# objdiff -- symbol-level assembly diff oracle (the "am I matching yet" check).
# ---------------------------------------------------------------------------
ARG OBJDIFF_URL=https://github.com/encounter/objdiff/releases/download/v3.7.3/objdiff-cli-linux-x86_64
RUN curl -fsSL "$OBJDIFF_URL" -o /usr/local/bin/objdiff-cli \
 && chmod +x /usr/local/bin/objdiff-cli

# ---------------------------------------------------------------------------
# decomp-permuter -- auto-searches C variants near a match (optional; used once
# a function is close to byte-identical).
# ---------------------------------------------------------------------------
RUN git clone --depth 1 https://github.com/simonlindholm/decomp-permuter /opt/decomp-permuter \
 && (pip3 install --no-cache-dir --break-system-packages \
       -r /opt/decomp-permuter/requirements.txt || true)

# ---------------------------------------------------------------------------
# DOSBox + p7zip: run the PERIOD Watcom 10.0a DOS compiler (tools/wcc_dos.sh)
# headless in-container -- this is the compiler that byte-matches the original
# (Open Watcom v2's register allocation diverges). p7zip unpacks toolchain ISOs.
# ---------------------------------------------------------------------------
RUN apt-get update \
 && apt-get install -y --no-install-recommends dosbox p7zip-full \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /work
CMD ["/bin/bash"]
