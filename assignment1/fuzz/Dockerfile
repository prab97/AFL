FROM ubuntu:24.04

# TODO: Modify me if needed
ENV  AFL_SKIP_CPUFREQ=0


RUN mkdir /target
COPY . /target


# Update and install dependencies
RUN apt-get update && apt-get install -y \ 
    git \
    build-essential \
    gcc \
    clang \
    llvm \
    automake \
    cmake \
    gdb \
    ninja-build \
    libtool \
    binutils-dev \
    curl \
    wget \
    nano \
    vim \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Clone AFLplusplus from GitHub
RUN git clone https://github.com/AFLplusplus/AFLplusplus.git /opt/AFLplusplus

# Build AFL++
WORKDIR /opt/AFLplusplus
RUN make all

# Set environment variables for AFL++
ENV PATH="/opt/AFLplusplus:${PATH}"

RUN /opt/AFLplusplus/afl-clang-fast -g -O0 /target/js.c -o /target/test.o

WORKDIR /

#copying target files into workspace
#COPY /target /workspace
# Default command
CMD ["afl-fuzz", "-i", "/target/in", "-o", "/target/observations", "--", "/target/test.o", "@@"]


