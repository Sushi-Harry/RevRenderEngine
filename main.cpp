#include "include/Engine.hpp"

int main(){
    Engine newEngine(900, 1600, "main");
    while (newEngine.isRunning()) {
        newEngine.Update();
    }
    return 0;
}