FROM ubuntu:20.04
MAINTAINER Abdulrahman Semrie <xabush@singularitynet.io>

#Run apt-get in NONINTERACTIVE mode
ENV DEBIAN_FRONTEND noninteractive

ENV DEBIAN_FRONTEND noninteractive 

RUN apt-get update

RUN apt-get install -y git ssh libssl-dev libboost-all-dev cython dh-autoreconf unzip gdb vim

##Install latest cmake - grpc complains for old cmake versions
RUN cd /tmp && wget -O cmake.sh https://github.com/Kitware/CMake/releases/download/v3.18.1/cmake-3.18.1-Linux-x86_64.sh && \
    sh ./cmake.sh --prefix=/usr/local --skip-license

RUN cmake --version

#Install Guile dependecies
RUN apt-get install -y libgmp-dev libltdl-dev libunistring-dev libffi-dev libgc-dev flex texinfo  libreadline-dev pkg-config

ENV HOME /root

ENV GUILE_VERSION 3.0.9
RUN cd /tmp && wget https://ftp.gnu.org/gnu/guile/guile-$GUILE_VERSION.tar.gz  && \
         tar -xvzf guile-$GUILE_VERSION.tar.gz && cd guile-$GUILE_VERSION && \
         autoreconf -vif && \
         ./configure && \
         make -j4 && make install


##Install grpc cpp
#prev version -b v1.31.0
ENV GRPC_VERSION 1.56.0
RUN cd /tmp &&  git clone -b v$GRPC_VERSION https://github.com/grpc/grpc && \
    cd grpc && git submodule update --init && \
    mkdir build && cd build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF \
    -DgRPC_ZLIB_PROVIDER=package \
    -DgRPC_SSL_PROVIDER=package -DCMAKE_BUILD_TYPE=Release .. && \
    make -j8 && \
    make install && \
    ldconfig

RUN cd /tmp && git clone https://github.com/singnet/cogutil.git && \
    cd cogutil && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j8 && \
    make install && \
    ldconfig /usr/local/lib/opencog && \
    make clean && \
    cd ../.. && \
    rm -r cogutil

#Install atomspace
RUN cd /tmp && git clone https://github.com/singnet/atomspace.git && \
    cd atomspace && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j8 && \
    make install && \
    ldconfig /usr/local/lib/opencog && \
    make clean && \
    cd ../.. && \
    rm -r atomspace

#Install agi-bio
RUN cd /tmp && git clone https://github.com/opencog/agi-bio.git && \
    cd agi-bio && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make -j8 && \
    make install && \
    ldconfig /usr/local/lib/opencog && \
    make clean && \
    cd ../.. && \
    rm -r agi-bio

RUN cd /tmp && git clone https://github.com/aconchillo/guile-json && \
    cd guile-json && \
    autoreconf -vif && \
    ./configure  && \
    make && \
    make install

##Install nlohmann json
RUN mkdir -p /usr/local/include/nlohmann && wget -O /usr/local/include/nlohmann/json.hpp https://github.com/nlohmann/json/releases/download/v3.9.1/json.hpp
WORKDIR $HOME

ENV CODE $HOME/atomspace-rpc
RUN mkdir $CODE

WORKDIR $CODE

COPY . $CODE

RUN cd $CODE && mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Debug .. && \
    make -j && \
    make install && ldconfig

WORKDIR $CODE
#RUN ls $CODE
CMD ["atom_server", "--config", "./config.json", "--port", "8005"]

