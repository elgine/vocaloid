#include <iostream>
#include <string>
#include <vocaloid/common/thread_pool.hpp>
using namespace std;

void Print(const string& s){
    cout<<s<<endl;
}

class A{
private:
    string name;
public:
    A(){
        name = "elgine";
    }
    void Print(const string str){
        cout << name << ": " + str << endl;
    }
};

int main(){
    A *a = new A();
    ThreadPool pool(5);
    pool.Run();
    pool.AddTask([]{cout<<"hello world"<<endl;});
    pool.AddTask([]{cout<<"hello world1"<<endl;});
    pool.AddTask([]{cout<<"hello world2"<<endl;});
    pool.AddTask(Print, string("hello world3"));
    pool.AddTask(&A::Print, a, "hello world");
    pool.Stop();
    return 0;
}