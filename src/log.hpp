/**
 * author: brando
 * date:
 */

#ifndef LOG_HPP
#define LOG_HPP

extern "C" {
#include <bflibc/filewriter.h>
}

/**
 * will revisit this when I work on the testing framework in libs
 *
#ifdef TESTING
#define CHAT_LOG_PATH "/tmp/chat-test.log"
#else
#define CHAT_LOG_PATH "/tmp/chat.log"
#endif
*/

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

/**
 * `mode`: 'd' for debug, 'e' for error, or 0 for normal
 */
void _LogWriteEntry(BFFileWriter * filewriter, int mode, ...);

/**
 * LOG_WRITE vs LOG_DEBUG vs LOG_ERROR
 *
 * - each writes a line into the same log file
 * - each log entry will explicitly tell you what type of log entry it is
 */

/**
 * writes ent (line) to log file
 */
#define LOG_WRITE(...) _LogWriteEntry(&gFileWriter, 0, __VA_ARGS__)

#define LOG_ERROR(...) _LogWriteEntry(&gFileWriter, 'e', __VA_ARGS__)

#ifdef DEBUG
#define LOG_DEBUG(...) _LogWriteEntry(&gFileWriter, 'd', __VA_ARGS__)
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

