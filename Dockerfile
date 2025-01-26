FROM ubuntu:24.10

RUN apt update
RUN apt install -y cmake pip gcc-13
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100
RUN pip install -U conan --break-system-packages
RUN conan profile detect

WORKDIR /url-shortener
COPY . .
RUN mkdir -p build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -- -j $(nproc --all)

CMD ["/url-shortener/build/bin/url-shortener"]