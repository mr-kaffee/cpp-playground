# 1) Use the latest ubuntu image as the base image
FROM ubuntu:latest

# 2) Install basic tooling
RUN apt-get update && apt-get install -y \
    build-essential git curl wget \
    python3 \
    sudo \
    lsb-release software-properties-common ca-certificates gnupg gpg

# 3) Install clang-17 compiler
RUN wget https://apt.llvm.org/llvm.sh \
    && chmod 755 ./llvm.sh \
    && echo -e "\n" | ./llvm.sh 17 \
    && apt-get update && apt-get install -y clang-17 clang-format-17 clang-tidy-17 clangd-17 \
    && update-alternatives --install /usr/bin/clang        clang        /usr/bin/clang-17        100 \
    && update-alternatives --install /usr/bin/clang++      clang++      /usr/bin/clang++-17      100 \
    && update-alternatives --install /usr/bin/clang-cpp    clang-cpp    /usr/bin/clang-cpp-17    100 \
    && update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-17 100 \
    && update-alternatives --install /usr/bin/clang-tidy   clang-tidy   /usr/bin/clang-tidy-17   100 \
    && update-alternatives --install /usr/bin/clangd       clangd       /usr/bin/clangd-17       100

# 4) Install CMAKE
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null \
    && echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null \
    && apt-get update \
    && rm /usr/share/keyrings/kitware-archive-keyring.gpg \
    && apt-get install -y kitware-archive-keyring cmake

# 5) Install gdb debugger
RUN apt-get update && apt-get install -y gdb

# 6) Create user 'docker' with sudo privileges
RUN useradd -ms /bin/bash docker \
    && echo 'docker ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers
USER docker

# Setup git environment and checker
COPY check-git.sh /home/docker/check-git.sh
RUN echo "$HOME/check-git.sh" >> /home/docker/.bashrc \
    && git config --global core.editor "code --wait" \
    && git config --global safe.directory "/workspace"
