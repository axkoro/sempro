# Choose a Python base image
FROM --platform=linux/amd64 python:3.12-slim

# Install system dependencies
RUN apt-get update && apt-get install -y \
build-essential \
cmake \
unzip \
&& rm -rf /var/lib/apt/lists/*

RUN pip install \
build \
coverage \
pytest \
matplotlib

# # Install dependencies of evaluation submodule
# COPY ./extlibs/evaluation/requirements_full.txt ./extlibs/evaluation/
# RUN pip install -r ./extlibs/evaluation/requirements_full.txt