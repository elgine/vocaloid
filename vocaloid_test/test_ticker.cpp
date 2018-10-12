#include <stdio.h>
#include <vocaloid/common/ticker.hpp>
#include <afxres.h>

int main(int argc, char** argv) {
    auto *ticker = new Ticker();
    ticker->Start();
    Sleep(300);
    ticker->Update();
    printf("%ld\n", ticker->GetCurTimestamp());
    Sleep(300);
    ticker->Update();
    printf("%ld\n", ticker->GetCurTimestamp());
    ticker->Pause();
    Sleep(600);
    ticker->Resume();
    ticker->Update();
    printf("%ld\n", ticker->GetCurTimestamp());
    return 0;
}