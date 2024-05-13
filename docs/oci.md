# OCI containers for HPC
This is a guide for building OCI containers with [Buildah](https://github.com/containers/buildah), storing the container images to [GitHub Container Registry](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry) and using the containers with [Apptainer](https://github.com/apptainer/apptainer) or Singularity.


## General principles for writing container definitions
The containers should adhere to the best practices for Apptainer compatibility.
We can write container definitions using as scripts or using Dockerfile (aka Containerfile) format.

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
