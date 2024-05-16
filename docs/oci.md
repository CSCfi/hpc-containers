# OCI-compliant HPC containers
This is a guide for building OCI containers with [Buildah](https://github.com/containers/buildah), storing the container images to [GitHub Container Registry](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry) and using the containers with [Apptainer](https://github.com/apptainer/apptainer) or Singularity.


## Container definitions
The containers should adhere to the best practices for Apptainer compatibility.
We can write container definitions using as scripts or using Dockerfile format.
We'll use the Dockerfile format in this guide.

```dockerfile
FROM ubuntu:22.04

# TODO
```


## Building container images
Building container with Buildah

```sh
buildah build --tag ghcr.io/<username>/<name>:<tag> -f <dockerfile>
```


## Pushing to container registry
Pushing to GitHub container registry

```sh
buildah login -u <username> ghcr.io  # supply an access token
```

```sh
buildah push ghcr.io/<username>/<name>:<tag>
```


## Using container images
Usage with Apptainer

```sh
mkdir -p <name>/<tag>
apptainer pull <name>/<tag>.sif docker://ghcr.io/<username>/<name>:<tag>
```
