FROM gcc:13.2.0-bookworm

RUN apt update && \
    apt install -y cmake build-essential && \
    rm -rf /var/lib/apt/lists/*
