/**
 * author: brando
 * date:
 */

#ifndef LOG_HPP
#define LOG_HPP

extern "C" {
#include <bflibc/filewriter.h>
}

#define CHAT_LOG_PATH "/tmp/chat.log"

extern BFFileWriter gFileWriter;

/**
 * must define in a source file
 */
#define LOG_INIT BFFileWriter gFileWriter = 0;

/**
 * initializes log
 */
#define LOG_OPEN BFFileWriterCreate(&gFileWriter, CHAT_LOG_PATH)

void _LogWriteEntry(BFFileWriter * filewriter, ...);

/**
 * writes ent (line) to log file
 */
#define LOG_WRITE(...) _LogWriteEntry(&gFileWriter, __VA_ARGS__)

#define LOG_ERROR(...)

#ifdef DEBUG
#define LOG_DEBUG(...) _LogWriteEntry(&gFileWriter, __VA_ARGS__)
#else // DEBUG
#define LOG_DEBUG(...)
#endif // DEBUG

/**
 * flushes write buffers
 */
#define LOG_FLUSH BFFileWriterFlush(&gFileWriter)

/**
 * closes log file
 */
#define LOG_CLOSE BFFileWriterClose(&gFileWriter)


#endif // LOG_HPP

