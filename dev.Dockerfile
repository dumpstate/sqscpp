FROM gcc:13.2.0-bookworm

EXPOSE 8080
ENV VCPKG_FORCE_SYSTEM_BINARIES=1

RUN apt update && apt install -y cmake build-essential curl zip unzip tar clang-format ninja-build
RUN git clone https://github.com/microsoft/vcpkg /opt/vcpkg
RUN /opt/vcpkg/bootstrap-vcpkg.sh
RUN /opt/vcpkg/vcpkg install restinio
RUN /opt/vcpkg/vcpkg install nlohmann-json
RUN /opt/vcpkg/vcpkg install boost-program-options
