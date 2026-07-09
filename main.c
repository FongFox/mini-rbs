#include <stdio.h>

struct UE {
    int id;
    int state; // 0 = IDE; 1 = CONNECTED; 2 = ATTACHED
};

int main() {
    printf("MiniRBS server starting...\n");

    struct UE ue1;
    ue1.id = 101;
    ue1.state = 0;

    struct UE *p = &ue1;
    printf("UE %d, state: %d\n", p->id, p->state);

    return 0;
}