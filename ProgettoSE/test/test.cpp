#include "../commons/src/list.h"
#include "../commons/src/stack.h"

#if 1
int main() {
#if 0 
    List<int> *l = new List<int>();

    for(int i = 0; i < 15; i++){
        l->addElement(i);
    }

    for(int i = 0; i < l->getSize(); i++){
        printf("Element in position %d: %d\n", i, l->getElement(i));
    }
    printf("\n");

    l->deleteElement(3);
    l->deleteElement(10);
    l->deleteElement(100);

    for(int i = 0; i < l->getSize(); i++){
        printf("Element in position %d: %d\n", i, l->getElement(i));
    }

    delete l;
#else
    Stack<int> *s = new Stack<int>();

    // pushing three elements
    s->push(10);
    s->push(21);
    s->push(31);

    // popping three elements. Should be popped in reverse order
    printf("Element popped: %d\n", s->pop());
    printf("Element popped: %d\n", s->pop());
    printf("Element popped: %d\n", s->pop());
    // printf("Element popped: %d\n", s->pop()); // calling it again results in an error

    s->push(15);
    s->push(1223);
    s->push(479);

    // using it with a while loop
    while(!s->isEmpty()){
        printf("Element %d popped\n", s->pop());
    }

    delete s;
#endif

    return 0;
}
#endif