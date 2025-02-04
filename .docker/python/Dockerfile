# Choose a Python base image
FROM --platform=linux/amd64 python:3.12-slim

# Set working directory
WORKDIR /app

# Copy your setup.py
COPY extlibs/evaluation/requirements.txt ./extlibs/evaluation/

# Install system dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    unzip \
    && rm -rf /var/lib/apt/lists/*

RUN pip install -r extlibs/evaluation/requirements.txt
