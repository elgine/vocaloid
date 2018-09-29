#include <thread>
#include <mutex>
#include <set>
#include <random>
#include <functional>
using namespace std;

int main(){
    set<int> int_set;
    mutex mt;
    auto f = [&int_set, &mt](){
        try{
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(1, 1000);
            for(size_t i = 0;i != 100000;++i){
                unique_lock<mutex> lck(mt, defer_lock);
                int_set.insert(dis(gen));
                throw "..";
            }
        }catch(...){}
    };
    thread td1(f), td2(f);
    td1.join();
    td2.join();
    return 0;
}