/**
 * author: brando
 * date: 2/12/24
 */

#include "log.hpp"
#include <stdarg.h>
#include <bflibcpp/bflibcpp.hpp>

void _LogWriteEntry(BFFileWriter * filewriter, int mode, ...) {
	va_list arg0, arg1;
	va_start(arg0, mode);
	va_start(arg1, mode);

	const char * format = va_arg(arg0, const char *);
	if (!format) return;

	char * logstr = BFStringCreateFormatArgListString(format, arg0);
	if (!logstr) return;

	BFDateTime dt = {0};
	if (BFTimeGetCurrentDateTime(&dt)) return;

	switch (mode) {
		case 'd': // debug
			format = "[%02d/%02d/%04d, %02d:%02d:%02d] DEBUG - %s";
			break;
		case 'e': // error
			format = "[%02d/%02d/%04d, %02d:%02d:%02d] ERROR - %s";
			break;
		default: // normal
			format = "[%02d/%02d/%04d, %02d:%02d:%02d] - %s";
			break;
	}
	BFFileWriterQueueFormatLine(
		filewriter,
		format,
		dt.month,
		dt.day,
		dt.year,
		dt.hour,
		dt.minute,
		dt.second,
		logstr
	);

	va_end(arg0);
	va_end(arg1);

	BFFree(logstr);
}

