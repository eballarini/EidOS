#ifndef SCRIPTS_MJSRESOLV
#define SCRIPTS_MJSRESOLV

void foo(int x);

char* filetext(char* file_name);

void mjs_pwm_set_dc (int a , int b);
void mjs_pwm_set_period (int a, int b);
void mjs_pwm_on (int a);
void mjs_pwm_off (int a);

void mjs_gpio_set(int a);
void mjs_gpio_unset(int a);
void mjs_gpio_flip(int a);


int mjs_temp_less_than(int a );
int mjs_temp_greater_than(int a );

int mjs_humidity_less_than(int a );
int mjs_humidity_greater_than(int a );

int mjs_light_less_than(int a );
int mjs_light_greater_than(int a );

int mjs_pressure_less_than(int a );
int mjs_pressure_greater_than(int a );
void mjs_wait(unsigned int wait_time);
void mjs_flip_gpio(int num);
void mjs_res_prints(char* string);
char* filetext(char* file_name);

void mjs_delay(int a);

#endif