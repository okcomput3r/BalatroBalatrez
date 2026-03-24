#pragma once
#define __DEBUG

#ifndef __DEBUG
    #define TRACE(fmt,...) ((void)0)
#else
    #include <unistd.h>
    #define TRACE(fmt,...) printf("%s: " fmt "\n", __PRETTY_FUNCTION__, ## __VA_ARGS__)
#endif

int innitLogs();

void updateLogs();

void terminateLogs();
