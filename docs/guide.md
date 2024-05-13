# Apptainer and Singularity
## Defining a container
We have `container.def ` file.

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
apptainer exec [<flag>] container.sif <command>
```
