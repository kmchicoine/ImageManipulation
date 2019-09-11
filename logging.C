#include <logging.h>
#include <stdio.h>
#include <string.h>

FILE * Logger::logger = fopen("logger", "w");

void Logger::LogEvent(const char *event) {
    fwrite(event, strlen(event), 1, logger);
}

void Logger::Finalize() {
    fclose(logger);
}

DataFlowException::DataFlowException(const char *type, const char *error) {
    sprintf(msg, "Throwing exception: (%s): %s\n", type, error);
    Logger::LogEvent(msg);
}
