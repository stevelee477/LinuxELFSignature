#pragma once

#if 1
#undef SIGRTMIN
#define SIGRTMIN 34
#endif

void send_signal(int sig_num);