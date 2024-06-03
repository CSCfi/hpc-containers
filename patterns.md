## Package managers and build tools
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


Pip

```dockerfile
RUN python3 -m pip install --no-cache-dir --upgrade pip && \
    python3 -m pip install --no-cache-dir numpy
```
