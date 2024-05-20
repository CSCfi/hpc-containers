FROM ubuntu:22.04

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get --yes update && \
    apt-get --yes upgrade && \
    apt-get --yes --no-install-recommends install \
        ca-certificates \
        curl \
        tar \
        gzip \
        bash \
        gcc \
        make \
        libc6-dev \
        && \
    apt-get --yes clean && \
    apt-get --yes autoremove && \
    rm -rf /var/lib/apt/lists/*

# Install appdemo v0.1.0
RUN APP_VERSION=0.1.0 && \
    mkdir -p /tmp/build && \
    cd /tmp/build && \
    curl --location --output appdemo.tar.gz https://github.com/jaantollander/appdemo/archive/refs/tags/v${APP_VERSION}.tar.gz && \
    tar -xf appdemo.tar.gz && \
    cd appdemo-${APP_VERSION} && \
    make && \
    mv build/main /usr/local/bin/app && \
    rm -rf /tmp/build
