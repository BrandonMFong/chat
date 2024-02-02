/**
 * author: brando
 * date: 1/31/24
 */

#ifndef LOG_H
#define LOG_H

#define ELog(...) printf("error: "); printf(__VA_ARGS__)

#ifdef DEBUG
#define DLog(...) printf("debug: "); printf(__VA_ARGS__)
#else // DEBUG
#define DLog(...)
#endif // DEBUG

#endif // LOG_H

