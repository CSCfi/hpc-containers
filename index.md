# Guidelines for containerizing scientific applications for HPC clusters
## Introduction
<!-- establish context and prerequisities -->
These guidelines provide general principles and concrete examples to containerize scientific applications that are used in HPC clusters.
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
Internally, Podman uses Buildah to build [OCI](https://opencontainers.org/) containers and it is possible to use Buildah directly to build container is necessary.

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
Container definitions should aim to explicitly define all software dependencies for reproducibility.

The table below contains recommendations for keeping the container simple, extensible and ensure compatibility between Apptainer, Docker and OCI containers.
We can define containers for Docker and Podman using the Dockerfile format.

| [Apptainer](https://apptainer.org/docs/user/main/definition_files.html) | [Dockerfile](https://docs.docker.com/reference/dockerfile/) | Recommendation |
| - | - | - |
| `.def` | `.dockerfile` | File extension to use for the definition file. Avoid using plain `Dockerfile` because complex applications may require multiple definition files. |
| `From` | `FROM` | Use normally |
| `Bootstrap` | - | Use normally |
| `%post` | `RUN` | Use normally to run shell commands with `/bin/sh` to build the container. |
| `%environment` | `ENV` | Use to define runtime environment variables |
| `%arguments` | `ARG` | We can use build arguments to specify default software versions when changing the version does not require adding control flow to the build scripts. We can override default values using the `--build-arg` flag for the build command. |
| `%labels` | `LABEL` | Add metadata to container as name-value pairs. |
| `%files` | `COPY` | Avoid copying files from host to container. Instead download dependencies via the network in `%post` or `RUN`. |
| `%runscript`, `%startscript` | `CMD`, `ENTRYPOINT` | Avoid using runscripts. Instead, use `apptainer exec` to explictly run commands. |
| - | `USER` | Do not use. It can lead to access permission issues. |
| - | `SHELL` | Do not use. It is not part of OCI specification. |

It is best to avoid other keywords to keep containers simple and easier to convert to OCI container definitions which we discuss later.

<!-- TODO: base images and package managers -->

We can install software into `/opt`, `/usr` or `/usr/local` depending on our needs.
We need to make files world-readable.
Install directories includes `bin` directory for executables and `lib` directory for shared libraries.
[File Hierarchy Standard (FHS)](https://en.wikipedia.org/wiki/Filesystem_Hierarchy_Standard)

We should avoid creating files to the home directories, `/root` and `/home` because Apptainer mounts them at runtime by default.
We can create temporary files to temporary directory `/tmp` during build as long as we remove them in the end of the build.

We can add executables to path (the `$PATH` environment variable) in few different ways.
We can create a symbolic link to `/usr/local/bin` which is on the path by default.
Alternatively, you can prepend the directory of the executable to path manually in the container definition.

In HPC environments, we should point Apptainer cache and temporary directories to a sane location by setting the `APPTAINER_CACHEDIR` and `APPTAINER_TMPDIR` environment variables.
For example, we can set cache directory to a shared location if we are working in a project and temporary directory to fast local disk.

We can build apptainer containers using `apptainer build`.
We can build containers using `docker build` and `podman build`.


### Running containers
We run commands within the container using the `apptainer exec` command as follows:

```sh
apptainer exec <flags> app.sif <command> <arguments>
```

We can use `--env` flag to set environment variables and `--cleanenv` to avoid passing environment variables from the host.
For bind mounts, we can use `--bind` to bind mount directories and `--no-home` to disable binding home directory.

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


## Examples overview
In the following examples, we demonstrate how to create a container for a small scientific application called [sciapp](https://github.com/jaantollander/sciapp).
We install the dependencies to run and build `sciapp` and them we install `sciapp` itself by building it from the source.


## Apptainer example
Let's start by writing the following Apptainer definition to `app.def` file:

```sh
Bootstrap: docker
From: ubuntu:22.04

%post
    # Install dependencies for building and running sciapp
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

    # Install sciapp
    SCIAPP_VERSION=0.1.0 && \
    SCIAPP_URL=https://github.com/jaantollander/sciapp/archive/refs/tags/v${SCIAPP_VERSION}.tar.gz && \
    mkdir -p /tmp/build && \
    cd /tmp/build && \
    curl --location --output sciapp.tar.gz ${SCIAPP_URL} && \
    tar -xf sciapp.tar.gz && \
    cd sciapp-${SCIAPP_VERSION} && \
    make && \
    mv build/main /usr/local/bin/app && \
    rm -rf /tmp/build
```

Next, we build the container as follows:

```sh
apptainer build app.sif app.def
```

Once the container is built, we can test it.
Let's create an input file `input.txt` with the following lines:

```text
1.0
2.0
3.0
```

Let's run the containerized application and supply path to the input and output files as arguments.

```sh
apptainer exec app.sif app input.txt output.txt
```

If everything worked correctly, the application produces an output file `output.txt` with the following output:

```text
Average: 2.00
```

We can store Apptainer images to container registries that support [ORAS](https://oras.land/) such as GitHub Container Registry as follows:

```sh
apptainer registry login --username <username> oras://ghcr.io  # will prompt for an access token
apptainer push app.sif oras://ghcr.io/<username>/app:0.1.0
```

We can pull the container with Apptainer as follows:

```sh
apptainer pull app.sif oras://ghcr.io/<username>/app:0.1.0
```

In the next examples, we build Docker container and OCI container with Podman for the same application and convert it to Apptainer container.


## Docker example
Let's start by writing the following Dockerfile to `app.dockerfile` file:

```dockerfile
FROM ubuntu:22.04

# Install dependencies for building and running sciapp
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

# Install sciapp
RUN SCIAPP_VERSION=0.1.0 && \
    SCIAPP_URL=https://github.com/jaantollander/sciapp/archive/refs/tags/v${SCIAPP_VERSION}.tar.gz && \
    mkdir -p /tmp/build && \
    cd /tmp/build && \
    curl --location --output sciapp.tar.gz ${SCIAPP_URL} && \
    tar -xf sciapp.tar.gz && \
    cd sciapp-${SCIAPP_VERSION} && \
    make && \
    mv build/main /usr/local/bin/app && \
    rm -rf /tmp/build
```

Next, we build the container with Docker as follows:

```sh
docker build --tag localhost/app:0.1.0 --file app.dockerfile .
```

We can converting the Docker image into Apptainer image by saving the Docker image into an archive and building the Apptainer image from the archive as follows:

```sh
docker save --output app.tar localhost/app:0.1.0
apptainer build app.sif docker-archive://app.tar
```

Now, we can test that the container works as expected in the same way as in the Apptainer example.

Finally, we can push the container image to a container registry.
For example, we can use GitHub Container Registry (GHCR) as follows:

```sh
docker login --username <username> ghcr.io  # will prompt for an access token
docker localhost/app:0.1.0 ghcr.io/<username>/app:0.1.0
docker push ghcr.io/<username>/app:0.1.0
```

We can pull the container with Apptainer as follows:

```sh
apptainer pull app.sif docker://ghcr.io/<username>/app:0.1.0
```


## Podman example
We use the Dockerfile that we defined in the Docker example.
We can build the container with Podman as follows:

```sh
podman build --tag localhost/app:0.1.0 --file app.dockerfile .
```

We can converting the OCI image into Apptainer image by saving the OCI image into an archive and building the Apptainer image from the archive as follows:

```sh
podman save --output app.tar localhost/app:0.1.0
apptainer build app.sif docker-archive://app.tar
```

Finally, we can push the container image to a container registry.
For example, we can use GitHub Container Registry (GHCR) as follows:

```sh
podman login --username <username> ghcr.io  # will prompt for an access token
podman tag localhost/app:0.1.0 ghcr.io/<username>/app:0.1.0
podman push ghcr.io/<username>/app:0.1.0
```

We can pull the container with Apptainer as follows:

```sh
apptainer pull app.sif docker://ghcr.io/<username>/app:0.1.0
```
