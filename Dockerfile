FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y \
    ca-certificates \
    curl \
    gnupg \
    build-essential \
    cmake \
    git \
    ninja-build \
    zip \
    unzip \
    tar \
    pkg-config \
    bison \
    autoconf \
    flex \
    linux-libc-dev \
    perl \
    libpq-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp/build2-install
RUN curl -fsSLO https://download.build2.org/0.17.0/build2-install-0.17.0.sh && \
    sh build2-install-0.17.0.sh --yes --trust yes --jobs $(nproc) /usr/local && \
    rm -rf /tmp/build2-install

WORKDIR /tmp/odb-build
RUN set -e; \
    curl -fsSLO https://www.codesynthesis.com/download/odb/2.5.0/libcutl-1.11.0.tar.gz && \
    tar -xzf libcutl-1.11.0.tar.gz && \
    cd libcutl-1.11.0 && \
    JOBS=$(nproc) && \
    b configure: config.cxx=g++ config.install.root=/opt/odb-2.5 config.bin.rpath=/opt/odb-2.5/lib && \
    b install -j ${JOBS}
RUN set -e; \
    curl -fsSLO https://www.codesynthesis.com/download/odb/2.5.0/libodb-2.5.0.tar.gz && \
    tar -xzf libodb-2.5.0.tar.gz && \
    cd libodb-2.5.0 && \
    JOBS=$(nproc) && \
    b configure: config.cxx=g++ config.install.root=/opt/odb-2.5 config.bin.rpath=/opt/odb-2.5/lib config.cc.poptions="-I/opt/odb-2.5/include" config.cc.loptions="-L/opt/odb-2.5/lib" && \
    b install -j ${JOBS}
RUN set -e; \
    curl -fsSLO https://www.codesynthesis.com/download/odb/2.5.0/libodb-pgsql-2.5.0.tar.gz && \
    tar -xzf libodb-pgsql-2.5.0.tar.gz && \
    cd libodb-pgsql-2.5.0 && \
    JOBS=$(nproc) && \
    b configure: config.cxx=g++ config.install.root=/opt/odb-2.5 config.bin.rpath=/opt/odb-2.5/lib config.cc.poptions="-I/opt/odb-2.5/include" config.cc.loptions="-L/opt/odb-2.5/lib" && \
    b install -j ${JOBS}
ENV ODB_ROOT=/opt/odb-2.5
ENV ODB_PGSQL_ROOT=/opt/odb-2.5
RUN echo /opt/odb-2.5/lib > /etc/ld.so.conf.d/odb.conf && ldconfig && rm -rf /tmp/odb-build

WORKDIR /vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git . && ./bootstrap-vcpkg.sh

WORKDIR /build
COPY vcpkg.json CMakeLists.txt ./
COPY src ./src

RUN /vcpkg/vcpkg install && \
    cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake -DODB_ROOT=/opt/odb-2.5 -DODB_PGSQL_ROOT=/opt/odb-2.5 && \
    cmake --build build

FROM ubuntu:22.04
RUN apt-get update && apt-get install -y libpq5 libssl3 zlib1g && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /opt/odb-2.5 /opt/odb-2.5
COPY --from=builder /build/build/vcpkg_installed/x64-linux /vcpkg/installed/x64-linux
COPY --from=builder /build/build/cpp-collaborate-platform .
ENV LD_LIBRARY_PATH=/opt/odb-2.5/lib:/vcpkg/installed/x64-linux/lib

EXPOSE 8081
CMD [ "./cpp-collaborate-platform" ]