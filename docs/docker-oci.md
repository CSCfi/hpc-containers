# Docker and OCI containers for HPC
<!--
This is a guide for building OCI containers with [Buildah](https://github.com/containers/buildah), storing the container images to [GitHub Container Registry](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry) and using the containers with [Apptainer](https://github.com/apptainer/apptainer) or Singularity.
-->

The containers should adhere to the best practices for Apptainer compatibility.
We can write container definitions using as scripts or using Dockerfile format.
We'll use the Dockerfile format in this guide.

We use Docker for Docker containers.
We use Podman for OCI containers.


## Container definition
We have the following Docker definition file named `app.docker`:

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
RUN curl --location --output /opt/appdemo-0.1.0.tar.gz https://github.com/jaantollander/appdemo/archive/refs/tags/v0.1.0.tar.gz && \
    tar xf /opt/appdemo-0.1.0.tar.gz --directory /opt && \
    rm /opt/appdemo-0.1.0.tar.gz && \
    cd /opt/appdemo-0.1.0 && \
    make && \
    ln -s /opt/appdemo-0.1.0/build/main /usr/local/bin/app

ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    LANGUAGE=C.UTF-8
```


## Building with Docker
Building with Docker

```sh
docker build --tag localhost/app:0.1.0 --file app.docker .
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
podman build --tag app:0.1.0 --file app.docker --format oci
```

```sh
podman save --output app.tar localhost/app:0.1.0
```

```sh
apptainer build app.sif docker-archive://app.tar
```

---

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
