Dockerhub, GitHub container registry


# Building container with Apptainer or Singularity
We have `container.def ` file.

```singularity
Bootstrap: docker
From: ghcr.io/gnu-octave/octave:9.1.0

%files
    # include files

%post
    # run commands, install packages, compile code, etc
```

Building container create `container.sif` file.

```bash
apptainer build container.sif container.def
```


# Running container with Apptainer or Singularity
Running commands inside the container

```bash
apptainer exec container.sif <command>
```
