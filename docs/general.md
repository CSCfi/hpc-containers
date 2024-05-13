# HPC Containers
Apptainer and Singularity
Commands `apptainer` and `singularity`


## General principles
Install software into `/opt` or `/usr/local` and make it world-readable.
Avoid creating files to the home directories, `/root` and `/home`, or temporary directory `/tmp`.
If your build creates temporary files to these directories, remove them after the build.

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

Command and arguments invoke the application via its command line interface.
