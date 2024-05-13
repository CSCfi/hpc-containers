# HPC Containers
Apptainer and Singularity
Commands `apptainer` and `singularity`


## General principles
Install software into `/opt` or `/usr/local` and make it world-readable.

Avoid creating files to the home directories, `/root` and `/home`, or temporary directory `/tmp`.
If your build creates temporary files to these directories, remove them after the build.

The container filesystem is read-only at runtime, therefore, make sure that your program does not attempt to write files to the container at runtime.
Instead, write files to the bind mounted directories with write permission which is the default.


## Container definition file
We have `container.def` file.

```singularity
Bootstrap: docker
From: ubuntu/22.04

%files
    # include files

%post
    # run commands, install packages, compile code, etc
```


## Building container
Building container create `container.sif` file.

```bash
apptainer build container.sif container.def
```


## Running container
Running commands inside the container

```bash
apptainer exec <flag> container.sif <command>
```
