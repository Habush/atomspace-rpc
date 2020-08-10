FROM ubuntu:20.04
MAINTAINER Abdulrahman Semrie<hsamireh@gmail.com>

#Run apt-get in NONINTERACTIVE mode
ENV DEBIAN_FRONTEND noninteractive

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update

RUN apt-get install -y git libboost-all-dev cython dh-autoreconf unzip gdb vim wget curl libssl-dev 

#Install Guile dependecies
RUN apt-get install -y libgmp-dev libltdl-dev libunistring-dev libffi-dev libgc-dev flex texinfo  libreadline-dev pkg-config

##Install latest cmake
RUN cd /tmp && wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.18.1/cmake-3.18.1-Linux-x86_64.sh && \
    sh ./cmake.sh --prefix=/usr/local --skip-license

RUN cmake --version

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


##Install grpc cpp
RUN cd /tmp &&  git clone -b v1.31.0 https://github.com/grpc/grpc && \
    cd grpc && git submodule update --init && \
    mkdir build && cd build && \
    cmake .. && make && make install && \
    ldconfig

##Install nlohmann json
RUN mkdir -p /usr/local/include/nlohmann && wget -O /usr/local/include/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp

##Build atomspace-rpc
COPY . /opt/atomspace-rpc
WORKDIR /opt/atomspace-rpc

RUN mkdir build && cd build && \
    cmake .. && make -j2 && make install && \
    ldconfig

WORKDIR /opt/atomspace-rpc
ENTRYPOINT ["/usr/local/bin/atom_server --config /opt/settings.json --host 0.0.0.0"]
