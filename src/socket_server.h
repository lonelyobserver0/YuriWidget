#pragma once
#include <pthread.h>
#include "window.h"

#define SOCKET_PATH "/tmp/yuriwidget.sock"

void *socket_server_thread(void *arg);
