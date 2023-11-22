# OCI containers for HPC
This is a guide for building OCI containers with Buildah, storing the container images to GitHub container registry and using the containers with Apptainer (previosly Singularity).

The containers adhere to the best practices for Apptainer compatibility.

* We should install software into `/opt` or `/usr/local` to make them available for all users.
* We should not create files to the user directories, `/root` and `/home`.
* We should clean the temporary directory `/tmp` if we use during the build.
* We should assume read-only file system such that the software does not attempt to create files inside the container at runtime.


We can write container definitions using as scripts or using Dockerfile (aka Containerfile) format.
We'll use the Dockerfile format in this guide.


Building with Buildah

```sh
buildah build --tag <name>:latest <directory>
```


Pushing to GitHub container registry

```sh
buildah login -u <username> ghcr.io  # supply an access token
```

```sh
buildah tag localhost/<name>:latest ghcr.io/<username>/<name>:latest
buildah push ghcr.io/<username>/<name>:latest
```


Usage with Apptainer

```sh
apptainer pull <name>.sif docker://ghcr.io/<username>/<name>
```

```sh
apptainer exec <name>.sif <command>
```
