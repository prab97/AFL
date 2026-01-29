# Simple image that applies patch and verifies the fix inside /app
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /app

# Install required packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential clang patch  nano vim git ca-certificates curl \
    && rm -rf /var/lib/apt/lists/*

# Copy the repo contents into the container
COPY . /app

# Ensure run.sh is executable
RUN chmod +x /app/run.sh

# Default entrypoint runs our run.sh
ENTRYPOINT [ "/app/run.sh" ]
