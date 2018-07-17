#ifndef __mt19937ar_h
#define __mt19937ar_h 1



void init_genrand(unsigned long s);
void init_by_array(const unsigned long * const init_key, 
                   unsigned long key_length);


/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32();

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31();

/* generates a random number on [0,1]-real-interval */
double genrand_real1();

/* generates a random number on [0,1)-real-interval */
double genrand_real2();

/* generates a random number on (0,1)-real-interval */
double genrand_real3();

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53();

/* generates a random chars in buffer*/
char *genrand_buf(char *buffer, long buf_size);



#endif // __mt19937ar_h
