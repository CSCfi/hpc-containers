# Guidelines for containerizing scientific applications for HPC clusters
## Introduction
<!-- establish context and prerequisities -->
These guidelines provide general principles and concrete examples to containerize scientific applications and consistently manage the containers.
We assume basic knowledge about the Linux operating system and shell scripting, and how to build and install software on Linux.

<!-- define the scope of these guidelines -->

<!-- scientific application -->
A scientific application consists of the application software and various software dependencies.
We assume that the application has a command line interface and it can be configured via command line options, environment variables, configuration files, or some mix of them.
We focus on scientific applications that run as batch processes on HPC clusters, reading input data from input files and writing output data into output files.
We assume that the application is developed using a version control system such as Git so that we can install a specific version of the software.
We assume that the source code is hosted and the software releases on the web, for example using a platform like GitHub and GitLab, such that we can download it.

<!-- container technologies -->
Apptainer is the primary technology used to run and build containers for HPC clusters.
Apptainer was formerly known as Singularity, but it was renamed to Apptainer when it moved under the Linux foundation.
Sylabs maintains another fork of Singularity named SingularityCE, which has minor implementation differences compared to Apptainer.
The command line interface between Apptainer and Singularity is similar, and we can use them interchangeably.
Furthermore, we can use Docker to build Docker containers and Podman to build OCI containers that we can run on HPC clusters using Apptainer.
Internally, Podman uses Buildah to build OCI containers and it is possible to use Buildah directly to build container is necessary.

For complete reference to Apptainer, we recommend the [Apptainer documentation](https://apptainer.org/docs/user/main/index.html).


## General principles
<!-- containerization activities -->
We break down containerization into three acticities:

1. Defining and building containers of scientific applications for HPC clusters.
2. Running scientific applications from containers on HPC clusters.
3. Managing container definitions, images and build processes.

We discuss about the general principles and show concrete examples for each of the activities.


### Defining and building containers
We define containers using definition files.

<!-- TODO:
* define build arguments and default values for them
* copy files from host machine to the container
* run shell commands to build the container
* define enviroment variables that are available at runtime
-->

1. Apptainer definition files.
We should name file using the `.def` extension.
We recommend to primarily use `From`, `Bootstrap`, `%arguments`, `%files`, `%post`, `%environment` and `%labels` keywords.
It is best to avoid other keywords to keep containers simple and easier to convert to OCI container definitions which we discuss later.
We can build apptainer containers using `apptainer build`

2. Dockerfile to define containers.
We can define containers for Docker and Podman using the dockerfile format.
We should name Dockerfiles using the `.dockerfile` extension.
Avoid naming files as `Dockerfile` because complex scientific software may require multiple container definition files.
The containers should adhere to the best practices for Apptainer compatibility.
We recommend primarily using `FROM`, `ARG`, `COPY`, `RUN`, `ENV` and `LABEL` instructions.
We should not use the `USER` intruction.
For complete reference to dockerfile format, we recommend the [Dockerfile documentation](https://docs.docker.com/reference/dockerfile/).
We can build containers using `docker build` and `podman build`.

Install software into `/opt` or `/usr/local` and make it world-readable.
Avoid creating files to the home directories, `/root` and `/home`, or temporary directory `/tmp`.
If your build creates temporary files to these directories, remove them after the build.

We can add executables to path (the `$PATH` environment variable) in few different ways.
We can create a symbolic link to `/usr/local/bin` which is on the path by default.
Alternatively, you can prepend the directory of the executable to path manually in the container definition.

Shell commands for building containers are executed with `/bin/sh` by default.

Avoid copying files from host to container with `%files` or `COPY`.
Instead download dependencies via the network in `%post` or `RUN`.
<!-- TODO: explain why -->

Avoid using runscripts such as `%runscript`, `%startscript`, `CMD` or `ENTRYPOINT`.
Instead, use `apptainer exec` to explictly run the application in the container.
<!-- TODO: explain why -->

We can use build arguments to specify software versions when changing the version does not require adding control flow to the build scripts.
We can specify a default value in `%arguments` or `ARG` or using the `--build-arg` flag to supply build arguments which overrides the default values in definition file.

In HPC environments, we should point Apptainer cache and temporary directories to a sane location by setting the `APPTAINER_CACHEDIR` and `APPTAINER_TMPDIR` environment variables.
<!-- TODO: define sane location: tmp to fast local disk, cache to projappl -->


### Running containers
We run commands within the container using the `apptainer exec` command as follows:

```sh
apptainer exec <flags> app.sif <command> <arguments>
```

We use flags to specify bind mounts and environment variables for the runtime.

Environment variables

* `--env` to set environment variables
* `--cleanenv` to avoid passing environment variables from the host

Bind mounts

* `--bind` to bind mount directories
* `--no-home` to disable binding home directory

The container filesystem is read-only at runtime, therefore, make sure that your program does not attempt to write files to the container at runtime.
Instead, write files to the bind mounted directories with write permission.
Bind mounts have read and write permission by default when permissions are not defined explicitly.
Input data is read from and output is written into bind mounted directories.
Apptainer will bind mount certain directories by default such as the home directory (`$HOME`), current working directory and the temporary directory (`/tmp`).


### Managing containers
We should place container definitions into a separate repository instead of placing them to the same repository as the application source code.
The separation makes the separation between the application source code and the container definitions explicit.

<!--
version controlling container definitions
versioning containers
storing containers into container registry
automatically building containers.
-->


## Apptainer example
In this example, we install the [appdemo](https://github.com/jaantollander/appdemo) to container.

We have the following Apptainer definition file named `app.def`:

```text
Bootstrap: docker
From: ubuntu:22.04

%post
    export DEBIAN_FRONTEND=noninteractive && \
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

    # Install appdemo
    APP_VERSION=0.1.0 && \
    mkdir -p /tmp/build && \
    cd /tmp/build && \
    curl --location --output appdemo.tar.gz https://github.com/jaantollander/appdemo/archive/refs/tags/v${APP_VERSION}.tar.gz && \
    tar -xf appdemo.tar.gz && \
    cd appdemo-${APP_VERSION} && \
    make && \
    mv build/main /usr/local/bin/app && \
    rm -rf /tmp/build
```

Next, we build the container

```sh
apptainer build app.sif app.def
```

Input file `input.txt`

```text
1.0
2.0
3.0
```

Now we can run it

```sh
apptainer exec app.sif app input.txt output.txt
```

Output file `output.txt`

```text
Average: 2.00
```


## Docker example
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
```

We can build the container with Docker as follows:

```sh
docker build --tag localhost/app:0.1.0 --file app.dockerfile .
```

Converting Docker container into Apptainer container.

```sh
docker save --output app.tar localhost/app:0.1.0
apptainer build app.sif docker-archive://app.tar
```

Pushing to container registry such as GitHub Container Registry.

```sh
docker login --username <username> ghcr.io  # will prompt for an access token
docker localhost/app:0.1.0 ghcr.io/<username>/app:0.1.0
docker push ghcr.io/<username>/app:0.1.0
```


## Podman example
We use the same `app.dockerfile` as in the Docker example.
Building container with Podman

```sh
podman build --tag localhost/app:0.1.0 --file app.dockerfile .
```

Converting OCI container into Apptainer container.

```sh
podman save --output app.tar localhost/app:0.1.0
apptainer build app.sif docker-archive://app.tar
```

Pushing to container registry such as GitHub Container Registry.

```sh
podman login --username <username> ghcr.io  # will prompt for an access token
podman tag localhost/app:0.1.0 ghcr.io/<username>/app:0.1.0
podman push ghcr.io/<username>/app:0.1.0
```


## Pulling containers from container registry with Apptainer
We can pull Docker and OCI containers with Apptainer as follows:

```sh
apptainer pull app.sif docker://ghcr.io/<username>/app:0.1.0
```
