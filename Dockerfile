FROM quay.io/invisionag/iwfm-build-base-docker:stable

RUN apt-get update \
  && apt-get upgrade -y \
  && apt-get install -y --no-install-recommends \
       gcc libc6-dev make cmake libcmocka-dev cppcheck clang-tidy iwyu \
  && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /code/BD
WORKDIR /code/BD

COPY . /code

ARG VERBOSE

RUN cmake \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_LINK_WHAT_YOU_USE=ON \
  -DCMAKE_BUILD_TYPE=Release \
  ..
RUN make -j"$(grep -c ^processor /proc/cpuinfo)" all test clang-tidy cppcheck iwyu
