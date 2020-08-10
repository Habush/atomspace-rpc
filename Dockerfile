FROM ubuntu:latest
MAINTAINER Abdulrahman Semrie<hsamireh@gmail.com>

#Run apt-get in NONINTERACTIVE mode
ENV DEBIAN_FRONTEND noninteractive

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update

RUN apt-get install -y git cmake libboost-all-dev cython dh-autoreconf unzip gdb vim wget

#Install Guile dependecies
RUN apt-get install -y libgmp-dev libltdl-dev libunistring-dev libffi-dev libgc-dev flex texinfo  libreadline-dev pkg-config

#Install guile-3.x
RUN cd /tmp && wget https://ftp.gnu.org/gnu/guile/guile-3.0.2.tar.gz  && \
         tar -xvzf guile-3.0.2.tar.gz && cd guile-3.0.2 && \
         autoreconf -vif && \
         ./configure && \
         make -j4 && make install

RUN cd /tmp && git clone https://github.com/opencog/cogutil.git && \
    cd cogutil && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j4 && \
    make install && \
    ldconfig /usr/local/lib/opencog

#Install atomspace
RUN cd /tmp && git clone https://github.com/opencog/atomspace.git && \
    cd atomspace && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j4 && \
    make install && \
    ldconfig /usr/local/lib/opencog

#Install agi-bio
RUN cd /tmp && git clone https://github.com/opencog/agi-bio.git && \
    cd agi-bio && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j4 && \
    make install && \
    ldconfig /usr/local/lib/opencog

##Install protobuf
RUN cd /tmp && \
    wget -O protobuf-4.0.tar.gz https://github.com/protocolbuffers/protobuf/releases/download/v4.0.0-rc2/protobuf-all-4.0.0-rc-2.tar.gz && \
    tar -xvzf protobuf-4.0.tar.gz && cd protobuf-4.0.0-rc-2 && \
    ./configure && make && make install && \
    ldconfig

##Install grpc cpp

RUN cd /tmp &&  git clone -b v1.31.0 https://github.com/grpc/grpc && \
    cd grpc && git submodule update --init && \
    mkdir build && cd build && \
    cmake .. && make -j4 && make install && \
    ldconfig

##Install nlohmann json
RUN mkdir -p /usr/local/lib/nlohmann && wget -O /usr/local/lib/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp

##Build atomspace-rpc
COPY . /opt/atomspace-rpc

RUN mkdir -p /opt/atomspace-rpc/build && cd build && \
    cmake .. && make -j2 && make install && \
    ldconfig

WORKDIR /opt/atomspace-rpc


ENTRYPOINT ["/usr/local/bin/atom_server --config /opt/settings.json --host 0.0.0.0"]