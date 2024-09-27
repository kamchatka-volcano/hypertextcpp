FROM alpine:3.18.3 AS hypertextcpp-build-container
ARG cmake_preset
RUN apk update && \
    apk add --no-cache \
        git \
        build-base \
        ccache \
        cmake \
        clang \
        clang-dev \
        mold \
        samurai

WORKDIR /hypertextcpp_src
COPY external ./external/
COPY src ./src/
COPY tests ./tests/
COPY CMakeLists.txt .
COPY CMakePresets.json .
RUN cmake --preset "$cmake_preset" -B build -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_EXE_LINKER_FLAGS="-static" -DENABLE_TESTS=ON
RUN cmake --build build
RUN strip --strip-all build/hypertextcpp


FROM scratch AS hypertextcpp-build
COPY --from=hypertextcpp-build-container /hypertextcpp_src/build/hypertextcpp .
COPY --from=hypertextcpp-build-container /hypertextcpp_src/build/tests/test_hypertextcpp ./tests/
