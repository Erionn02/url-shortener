FROM ubuntu:24.10 as build

RUN apt update
RUN apt install -y cmake pip gcc-13
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
RUN pip install -U conan --break-system-packages
RUN conan profile detect

WORKDIR /url-shortener
COPY . .
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . -- -j $(nproc --all)
RUN mkdir -p bin
RUN mv ./build/apps/url-shortener ./bin
RUN rm -rf ./build
RUN strip ./bin/url-shortener
RUN rm -rf /root/.conan2

FROM ubuntu:24.10
COPY --from=build /url-shortener /url-shortener
RUN apt update && apt install -y ca-certificates
CMD ["/url-shortener/bin/url-shortener"]