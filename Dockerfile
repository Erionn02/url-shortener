FROM rikorose/gcc-cmake:latest

RUN apt update
RUN apt install -y pip
RUN pip install -U conan==1.60.0
RUN conan profile new default --detect --force
RUN conan profile update settings.compiler.libcxx=libstdc++11 default
RUN conan profile update conf.tools.system.package_manager:mode=install default
RUN conan profile update conf.tools.system.package_manager:sudo=True default

WORKDIR /url-shortener
COPY . .

RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -- -j $(nproc --all)

CMD ["./build/bin/url-shortener"]