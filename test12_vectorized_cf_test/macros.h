#define RUN_BOX(name, ina, inb, inc, outa, outb, outc) \
int main()\
{\
  int a = 0;\
  int b = 0;\
  name<int, int>::process_aligned(100, a, b);\
  return 0; \
}

