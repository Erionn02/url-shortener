#include <iostream>
#include <string>
#include <memory>
#include <zmq.hpp>

int f(int x){
    return x;
}


std::string f(std::string x){
    return x;
}

int main() {
    int some_data{1234567};
    void* data = &some_data;

    zmq::message_t m{data,sizeof(some_data)};

    auto xd = std::make_unique<uint8_t[]>(sizeof(some_data));
    std::memcpy(xd.get(),m.data(),m.size());
    std::cout<<std::memcmp(&some_data,xd.get(),m.size());

    return 0;
}
