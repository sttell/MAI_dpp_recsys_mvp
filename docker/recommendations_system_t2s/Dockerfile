FROM ubuntu
ENV TZ=Europe/Moscow

ARG BUILD_CORES_COUNT=4
ARG POCO_BRACH='poco-1.12.3-release'

RUN apt-get clean && apt-get update -y
RUN DEBIAN_FRONTEND=noninteractive \
    TZ=Etc/UTC \
    apt-get -y install \
      tzdata git sudo build-essential autoconf \
      libtool libssl-dev zlib1g-dev librdkafka-dev \
      pkg-config cmake gcc git g++ \
      mysql-client libmysqlclient-dev \
      libboost-all-dev libssl-dev && \
    apt-get clean

RUN git clone https://github.com/pocoproject/poco.git -b ${POCO_BRACH} --depth 1 &&  \
    cd poco && \
    mkdir cmake-build && \
    cd cmake-build &&  \
    cmake .. && \
    make -j ${BUILD_CORES_COUNT} &&  \
    sudo make install &&  \
    cd .. && cd ..


RUN git clone https://github.com/sttell/MAI_dpp_recsys_mvp.git -b main && \
    sudo ldconfig && \
    cd MAI_dpp_recsys_mvp/sources && \
    mkdir build && cd build && \
    cmake .. && \
    make -j ${BUILD_CORES_COUNT}

ENTRYPOINT [ "/MAI_dpp_recsys_mvp/sources/build/recommendations_system_t2s/recommendation_system_t2s_service", "/MAI_dpp_recsys_mvp/sources/build/recommendations_system_t2s/recommendations_system_t2s_data/server_config.json" ]
