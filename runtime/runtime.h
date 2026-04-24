#ifndef RUNTIME_H
#define RUNTIME_H

#define BUFFER_SIZE 65536

/* Assembly must provide */
extern void frame(void);
extern unsigned char image[BUFFER_SIZE];

/* Framework provides */
void run(void);

#endif