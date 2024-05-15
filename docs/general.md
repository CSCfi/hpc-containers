# HPC containers
## Scientific application
In this guide, we provide instruction to containerize a scientific application that consists of software and various dependencies.
Furthermore, we assume that the application has a command line interface and it can be configured via command line options, environment variables, configuration files or some mix of them.
Also, we assume that the application runs as a batch processes reading input data from input files and writing output data into output files.

For example, running such application without container looks something like this:

```bash
app input.txt output.txt
```


## Defining containers
Apptainer is the primary technology used to run and build HPC containers.
It was formerly known as Singularity.
We can use Apptainer via the `apptainer` command.

We follow general principles defining HPC containers.
Install software into `/opt` or `/usr/local` and make it world-readable.
Avoid creating files to the home directories, `/root` and `/home`, or temporary directory `/tmp`.
If your build creates temporary files to these directories, remove them after the build.

We can add executables to path (the `PATH` environment variable) in few different ways.
We can create a symbolic link to `/usr/local/bin` which is on the path by default.
Alternatively, you can prepend the directory of the executable to path manually in the environment block.

We can define containers using a definition file.

Apptainer definition files use the `.def` file extension.

```singularity
Bootstrap: docker
From: ubuntu:22.04

%arguments
    # supply build arguments (e.g. version numbers)

%files
    # copy files from host machine to the container

%post
    # run shell commands to build the container

%environment
    # define enviroment variables that are available at runtime
```

It is recommended to avoid other directives to keep containers simple and easier to convert to OCI container definitions which we cover later.


## Building containers
We can build containers from the definition file into container image with `build` subcommand.
Images have `.sif` file extension.

```bash
apptainer build <flags> container.sif container.def
```

Flags `--build-arg`

Environment variables `APPTAINER_CACHEDIR` and `APPTAINER_TMPDIR`.


## Running containers
We can run the containerized application with the `exec` subcommand.

```bash
apptainer exec <flags> container.sif app <arguments>
```

We use flags to specify bind mounts and environment variables.

* `--bind` to bind mount directories
* `--no-home` to disable binding home directory

Runtime environment

* `--env` to set environment variables
* `--cleanenv` to avoid passing environment variables from host

Command and arguments invoke the application via its command line interface.

The container filesystem is read-only at runtime, therefore, make sure that your program does not attempt to write files to the container at runtime.
Instead, write files to the bind mounted directories with write permission.
Bind mounts have read and write permission by default when permissions are not defined explicitly.
Input data is read from and output is written into bind mounted directories.

