# Choose a Python base image
FROM --platform=linux/amd64 python:3.12-slim

# Set working directory
WORKDIR /app

# Install system dependencies
RUN apt-get update && apt-get install -y \
build-essential \
cmake \
unzip \
&& rm -rf /var/lib/apt/lists/*

# Install dependencies of evaluation submodule
COPY ./extlibs/evaluation/requirements.txt ./extlibs/evaluation/
RUN pip install -r ./extlibs/evaluation/requirements.txt
