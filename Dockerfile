FROM ubuntu
RUN apt update && apt upgrade -y
RUN apt update && apt install -y gcc-arm-none-eabi libstdc++-arm-none-eabi-newlib cmake build-essential git python3
WORKDIR /work
COPY CMakeLists.txt .
COPY src src
COPY deps deps
CMD cd build && cmake .. && make
