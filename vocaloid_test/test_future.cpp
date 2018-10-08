#include <future>
#include <iostream>
using namespace std;

bool BiggerThanZero(int n){
    return n > 0;
}

int main(){
    future<bool> fut = async(BiggerThanZero, 313222313);
    std::chrono::milliseconds span(100);
    while (fut.wait_for(span) != std::future_status::ready);
    bool ret = fut.get();
    std::cout << "final result: " << ret << std::endl;
    return 0;
}