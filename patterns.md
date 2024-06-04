## Common patterns for defining containers
Apt

```dockerfile
RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get --yes update && \
    apt-get --yes upgrade && \
    apt-get --yes --no-install-recommends install \
        ca-certificates \
        curl \
        && \
    apt-get --yes clean && \
    apt-get --yes autoremove && \
    rm -rf /var/lib/apt/lists/*
```


Zypper

```dockerfile
RUN zypper --non-interactive install \
        ca-certificates \
        curl \
        && \
    zypper --non-interactive clean --all && \
```


Autotools

```dockerfile
# TODO
```


Cmake

```dockerfile
# TODO
```

Installing custom version of Python with Apt, setting it as default, upgrading Pip and installing a package.

```dockerfile
RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get --yes update && \
    apt-get --yes upgrade && \
    apt-get --yes --no-install-recommends install \
        python3.11 \
        python3.11-dev \
        python3-pip \
        && \
    apt-get --yes clean && \
    apt-get --yes autoremove && \
    rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.11 1 && \
    python3 -m pip install --no-cache-dir --upgrade pip

RUN python3 -m pip install --no-cache-dir numpy
```
