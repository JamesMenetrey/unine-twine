#!/bin/bash

docker run -p 8888:8888 -v "$PWD":/home/jovyan jupyter/scipy-notebook:latest 
