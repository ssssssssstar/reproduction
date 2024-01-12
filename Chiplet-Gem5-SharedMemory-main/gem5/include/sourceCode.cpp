#include "m5ops.h"
int main(){
m5_gadia_call(0, 1, 2, 0);      //chiplet0 send data2 to target chiplet1
uint64_t result = m5_gadia_receive(0);
return 0;
}