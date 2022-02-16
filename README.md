# Basic Javascript Core Development Environment

## Introduction
This repository contains a basic environment to develop a modified Javascript 
Core (jsc) runtime by extending values or functions to the jsc context. 

## Build & Run The Docker Image 
```
$ docker build . -t jsc
$ docker run -it --name jsc -v $(pwd):/opt/jsc jsc bash
```

## Build 
```
# inside the docker container
$ cd /opt/jsc
$ cmake -H. -Bbuild
$ make 
```