# Guidelines for containerizing scientific applications for HPC clusters
## Introduction
<!-- establish context and prerequisities -->
These guidelines provide general instructions and concrete examples to containerize scientific applications and consistently manage the containers.
We assume basic knowledge about the Linux operating system and shell scripting, and how to build and install software on Linux.

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

<!-- containerization activities -->
We break down containerization into three acticities:

1. Defining and building containers of scientific applications for HPC clusters.
2. Running scientific applications from containers on HPC clusters.
3. Managing container definitions, images and build processes.


## General principles
<!-- general principles for defining and building containers -->
General principles defining containers.

**Software location:**
Install software into `/opt` or `/usr/local` and make it world-readable.
Avoid creating files to the home directories, `/root` and `/home`, or temporary directory `/tmp`.
If your build creates temporary files to these directories, remove them after the build.

**Path:**
We can add executables to path (the `$PATH` environment variable) in few different ways.
We can create a symbolic link to `/usr/local/bin` which is on the path by default.
Alternatively, you can prepend the directory of the executable to path manually in the container definition.

Shell commands for building containers are executed with `/bin/sh`.

<!-- installing with package manager steps: upgrade, install packages, clean cache -->
<!-- installing from source steps:: make temp, download, build, install, clean temp -->

<!--
We explain how to create container definitions using the Apptainer definition file and build them using Apptainer.
Furthermore, we show how to define containers using the Dockerfile format and build them using Docker and Podman.
-->

<!--
We cover version controlling container definitions, versioning containers, storing containers into container registry and automatically building containers.
-->


## Defining containers with Apptainer
We can define containers using a definition file.
For example, we could have Apptainer definition file named `app.def` as follows:

```text
# Header
Bootstrap: docker
From: ubuntu:22.04

# Sections

%arguments
    # define build arguments and default values for them

%files
    # copy files from host machine to the container

%post
    # run shell commands to build the container

%environment
    # define enviroment variables that are available at runtime
```

We recommend to primarily use `From`, `Bootstrap`, `%arguments`, `%files`, `%post` and `%environment` keywords.
It is best to avoid other keywords to keep containers simple and easier to convert to OCI container definitions which we discuss later.

For complete reference to Apptainer, we recommend the [official documentation](https://apptainer.org/docs/user/main/index.html).
We can use Apptainer via the `apptainer` command.


## Building containers with Apptainer
We can build containers from the definition file into container image with `build` subcommand.
For example, we can build `app.sif` container from the `app.def` definition file as follows:

```sh
apptainer build <flags> app.sif app.def
```

We can use the `--build-arg` flag to supply build arguments.
It will overwrite the default values defined in `%arguments` section.

It is important that Apptainer creates cache and temporary files to sane locations in HPC environments.
We can modify them using the `APPTAINER_CACHEDIR` and `APPTAINER_TMPDIR` environment variables.


## Running containers with Apptainer
We can run the commands within the container using the `exec` subcommand as follows:

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


## Example with Apptainer
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


## Docker and OCI containers
In this section, we demonstrate how to use Docker and Podman with Apptainer.
We use Docker to create containers in Docker format and Podman to create container in OCI format.
We show how to them convert these containers into the Apptainer containers.
We also show how to store containers into GitHub container registry.


## Defining containers in Dockerfile format
We can define containers for Docker and Podman using the dockerfile format.
For complete reference to dockerfile format, we recommend the [official documentation](https://docs.docker.com/reference/dockerfile/).

We recommend to name containers files with name and extension in lowercase, such as `app.dockefiler`, rather than simply `Dockerfile` because complex scientific software may require multiple container definition files.

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

The containers should adhere to the best practices for Apptainer compatibility.
We recommend primarily using `FROM`, `ARG`, `COPY`, `RUN`, and `ENV` instructions.
We should not use the `USER` intructions.


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

Pushing to container registry such as GitHub Container Registry.

```sh
docker login --username <username> ghcr.io  # supply an access token
docker localhost/app:0.1.0 ghcr.io/<username>/app:0.1.0
docker push ghcr.io/<username>/app:0.1.0
```


## Building with Podman
Building container with Podman

```sh
podman build --tag localhost/app:0.1.0 --file app.dockerfile .
```

```sh
podman save --output app.tar localhost/app:0.1.0
```

```sh
apptainer build app.sif docker-archive://app.tar
```

```sh
podman login --username <username> ghcr.io  # supply an access token
podman tag localhost/app:0.1.0 ghcr.io/<username>/app:0.1.0
podman push ghcr.io/<username>/app:0.1.0
```


## Pulling containers from container registry with Apptainer
We can pull Docker and OCI containers with Apptainer as follows:

```sh
apptainer pull app.sif docker://ghcr.io/<username>/app:0.1.0
```
