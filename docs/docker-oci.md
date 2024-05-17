# Docker and OCI containers for HPC
In this section, we demonstrate how to use Docker and Podman with Apptainer.
We use Docker to create containers in Docker format and Podman to create container in OCI format.
We show how to them convert these containers into the Apptainer containers.
We also show how to store containers into GitHub container registry.


## Container definition
We can define container for Docker and Podman using the Dockerfile format.
The containers should adhere to the best practices for Apptainer compatibility.

We have the following Docker definition file named `app.dockerfile`:

```dockerfile
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
        libc6-dev && \
    apt-get --yes clean && \
    apt-get --yes autoremove && \
    rm -Rf /var/lib/apt/lists/*

# Install appdemo v0.1.0
RUN APP_VERSION=0.1.0 && \
    cd /opt && \
    curl --location --output appdemo.tar.gz https://github.com/jaantollander/appdemo/archive/refs/tags/v${APP_VERSION}.tar.gz && \
    tar -xf appdemo.tar.gz && \
    rm appdemo.tar.gz && \
    cd appdemo-${APP_VERSION} && \
    make && \
    ln -s /opt/appdemo-${APP_VERSION}/build/main /usr/local/bin/app

ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    LANGUAGE=C.UTF-8
```


## Building with Docker
We can build the container with Docker as follows:

```sh
docker build --tag localhost/app:0.1.0 --file app.dockerfile .
```

```sh
docker save --output app.tar localhost/app:0.1.0
```

```sh
apptainer build app.sif docker-archive://app.tar
```


## Building with Podman
Building container with Docker or Podman

```sh
podman build --tag localhost/app:0.1.0 --file app.dockerfile .
```

```sh
podman save --output app.tar localhost/app:0.1.0
```

```sh
apptainer build app.sif docker-archive://app.tar
```

Pushing to GitHub container registry

```sh
podman login -u <username> ghcr.io  # supply an access token
```

```sh
podman tag localhost/app:0.1.0 ghcr.io/<username>/app:0.1.0
```

```sh
podman push ghcr.io/<username>/app:0.1.0
```

Usage with Apptainer

```sh
apptainer pull app.sif docker://ghcr.io/<username>/app:0.1.0
```
