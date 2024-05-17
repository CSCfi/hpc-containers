# HPC containers
## Scientific application
In this guide, we provide instruction to containerize a scientific application that consists of software and various dependencies.
Furthermore, we assume that the application has a command line interface and it can be configured via command line options, environment variables, configuration files or some mix of them.
Also, we assume that the application runs as a batch processes reading input data from input files and writing output data into output files.


## Defining containers
Apptainer is the primary technology used to run and build HPC containers.
It was formerly known as Singularity.
We can use Apptainer via the `apptainer` command.

We follow general principles defining HPC containers.
Install software into `/opt` or `/usr/local` and make it world-readable.
Avoid creating files to the home directories, `/root` and `/home`, or temporary directory `/tmp`.
If your build creates temporary files to these directories, remove them after the build.

We can add executables to path (the `$PATH` environment variable) in few different ways.
We can create a symbolic link to `/usr/local/bin` which is on the path by default.
Alternatively, you can prepend the directory of the executable to path manually in the environment block.

We can define containers using a definition file.
For example, we could have Apptainer definition file named `app.def` as follows:

```singularity
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

We recommend using `%arguments`, `%files`, `%post` and `%environment` sections and avoiding other sections to keep containers simple and easier to convert to OCI container definitions which we discuss later.


## Building containers
We can build containers from the definition file into container image with `build` subcommand.
For example, we can build `app.sif` container from the `app.def` definition file as follows:

```sh
apptainer build <flags> app.sif app.def
```

We can use the `--build-arg` flag to supply build arguments.
It will overwrite the default values defined in `%arguments` section.

It is important that Apptainer creates cache and temporary files to sane locations in HPC environments.
We can modify them using the `APPTAINER_CACHEDIR` and `APPTAINER_TMPDIR` environment variables.


## Running containers
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


## Example
In this example, we install the [appdemo](https://github.com/jaantollander/appdemo) to container.

We have the following Apptainer definition file named `app.def`:

```singularity
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
    rm -Rf /var/lib/apt/lists/*

    # Install appdemo
    APP_VERSION=0.1.0 && \
    cd /opt && \
    curl --location --output appdemo.tar.gz https://github.com/jaantollander/appdemo/archive/refs/tags/v${APP_VERSION}.tar.gz && \
    tar -xf appdemo.tar.gz && \
    rm appdemo.tar.gz && \
    cd appdemo-${APP_VERSION} && \
    make && \
    ln -s /opt/appdemo-${APP_VERSION}/build/main /usr/local/bin/app

%environment
    export LC_ALL=C.UTF-8
    export LANG=C.UTF-8
    export LANGUAGE=C.UTF-8
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
