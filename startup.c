#include "startup.h"

void startup() {

int bufferSize = 905;

sys_req(WRITE,COM1,"\033[1;34m░░░░░░░░░░░░░░░▄▄▄▄▄▄░░░░░░░░░░░░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░░░░░░▄▀█▀█▄██████████▄▄░░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░░░░░▐██████████████████▌░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░░░░░███████████████████▌░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░░░░▐███████████████████▌░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░░░░█████████████████████▄░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░▄█▐█▄█▀█████████████▀█▄█▐█▄░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░▄██▌██████▄█▄█▄█▄█▄█▄█████▌██▌░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░▐████▄▀▀▀▀████████████▀▀▀▀▄███░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░▐█████████▄▄▄▄▄▄▄▄▄▄▄▄██████▀░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░▀▀████████████████████▀░░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░▌░░▌█▀▒▒▒▒▒▄▀█▄▒▒▒▒▒▒▒█▒▐░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░▐░░░▒▒▒▒▒▒▒▒▌██▀▒▒░░░▒▒▒▀▄▌░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░▌░▒▄██▄▒▒▒▒▒▒▒▒▒░░░░░░▒▒▒▒▌░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░▀▒▀▐▄█▄█▌▄░▀▒▒░░░░░░░░░░▒▒▒▐░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░▐▒▒▐▀▐▀▒░▄▄▒▄▒▒▒▒▒▒░▒░▒░▒▒▒▒▌░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░▐▒▒▒▀▀▄▄▒▒▒▄▒▒▒▒▒▒▒▒░▒░▒░▒▒▐░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░▌▒▒▒▒▒▒▀▀▀▒▒▒▒▒▒░▒░▒░▒░▒▒▒▌░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▒▄▒▒▐░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░▀▄▒▒▒▒▒▒▒▒▒▒▒░▒░▒░▒▄▒▒▒▒▌░░░░░░░░░\n", &bufferSize);
sys_req(WRITE,COM1,"░░░░░░░░░▀▄▒▒▒▒▒▒▒▒▒▒▄▄▄▀▒▒▒▒▄▀░░░░░░░░░░\033[0m\n", &bufferSize);

//(void)bufferSize;
}
