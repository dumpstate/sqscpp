FROM gcc:13.2.0-bookworm

EXPOSE 8080

RUN apt update && \
    apt install -y cmake build-essential curl zip unzip tar && \
    rm -rf /var/lib/apt/lists/* && \
    git clone https://github.com/microsoft/vcpkg /opt/vcpkg && \
    /opt/vcpkg/bootstrap-vcpkg.sh && \
    /opt/vcpkg/vcpkg install restinio nlohmann-json

RUN /opt/vcpkg/vcpkg install boost-program-options

RUN apt update && apt install -y clang-format && rm -rf /var/lib/apt/lists/*
