# OCI containers for HPC
This is a guide for building OCI containers with Buildah, storing the container images to GitHub container registry and using the containers with Apptainer (previously Singularity).


## Writing container definitions
We can write container definitions using as scripts or using Dockerfile (aka Containerfile) format.
The containers should adhere to the best practices for Apptainer compatibility.

* We should install software into `/opt` or `/usr/local` to make them available for all users.
* We should not create files to the home directories, `/root` and `/home`.
* We should clean the temporary directory `/tmp` if we use during the build.
* We should assume read-only file system at runtime.
  The software should not create files inside the container at runtime.

We'll use the Dockerfile format in this guide.

```dockerfile
FROM ubuntu:22.04

# TODO
```


## Building container images
Building container with Buildah

```sh
buildah build --tag <name>:latest <directory>
```


## Pushing to container registry
Pushing to GitHub container registry

```sh
buildah login -u <username> ghcr.io  # supply an access token
```

```sh
buildah tag localhost/<name>:latest ghcr.io/<username>/<name>:latest
buildah push ghcr.io/<username>/<name>:latest
```


## Using container images
Usage with Apptainer

```sh
apptainer pull <name>.sif docker://ghcr.io/<username>/<name>
```

```sh
apptainer exec <name>.sif <command>
```
