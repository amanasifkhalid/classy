ARG DISTRO_VERSION=3.13
FROM alpine:${DISTRO_VERSION} AS base

FROM base AS devtools

RUN apk update && apk add boost-dev boost-static build-base cmake make gcc g++ libc-dev sqlite sqlite-dev git

FROM devtools AS build
COPY . /v/classy

WORKDIR /v/classy
RUN git clone https://github.com/CrowCpp/Crow.git && mkdir /v/classy/Crow/build
RUN git clone git://github.com/SOCI/soci.git && mkdir /v/classy/soci/build

WORKDIR /v/classy/Crow/build
RUN cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF && make install

WORKDIR /v/classy/soci/build
RUN cmake -G "Unix Makefiles" -DWITH_BOOST=ON -DWITH_SQLITE3=ON -DSOCI_CXX11=ON .. && make && make install

WORKDIR /v/classy
RUN make clean
RUN g++ -std=c++11 -O3 -L/usr/local/lib64/ -I/usr/local/include/ cpp/*.cpp -o classy.exe -lsoci_core -lsoci_sqlite3 -ldl -lpthread -lsqlite3

RUN strip classy.exe
RUN make db
ENTRYPOINT [ "/v/classy/classy.exe" ]
