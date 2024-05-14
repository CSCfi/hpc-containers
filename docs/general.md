# HPC containers
## Scientific application
In this guide, we provide instruction to containerize a scientific application that consists of software and various dependencies.
Furthermore, we assume that the application has a command line interface and it can be configured via command line options, environment variables, configuration files or some mix of them.
Also, we assume that the application runs as a batch processes reading input data from input files and writing output data into output files.


## Apptainer
Apptainer is the primary technology used to run and build HPC containers.
It was formerly known as Singularity.
We can use Apptainer via the `apptainer` command.


## General principles for containerizing scientific applications
Install software into `/opt` or `/usr/local` and make it world-readable.
Avoid creating files to the home directories, `/root` and `/home`, or temporary directory `/tmp`.
If your build creates temporary files to these directories, remove them after the build.

We can add executables to path (the `PATH` environment variable) in few different ways.
We can create a symbolic link to `/usr/local/bin` which is on the path by default.
Alternatively, you can prepend the directory of the executable to path manually in the environment block.

The container filesystem is read-only at runtime, therefore, make sure that your program does not attempt to write files to the container at runtime.
Instead, write files to the bind mounted directories with write permission.
Bind mounts have read and write permission by default when permissions are not defined explicitly.

Input data is read from and output is written into bind mounted directories.


## Container definition file
We can define containers using a definition file.
Definition files use the `.def` file extension.

```singularity
Bootstrap: docker
From: ubuntu/22.04

%files
    # include files to the container

%post
    # build the container with shell commands

%environment
    # define enviroment variables that are available at runtime
```


## Building container
We can build containers from the defintion file into container image with `build` subcommand.
Images have `.sif` file extension.

```bash
apptainer build container.sif container.def
```


## Running container
We can run the containerized application with the `exec` subcommand.

```bash
apptainer exec <flags> container.sif <command> <arguments>
```

Flags specify bind mounts and environment variables.

* `--bind`
* `--env`
* `--cleanenv`

Command and arguments invoke the application via its command line interface.
