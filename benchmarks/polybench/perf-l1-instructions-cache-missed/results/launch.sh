#!/bin/bash

docker run -u $(id -u ${USER}):$(id -g ${USER}) -p 8888:8888 -v "$PWD":/home/jovyan jupyter/scipy-notebook:latest 
