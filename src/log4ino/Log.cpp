/*
 * LOG4INO
 *
 * Copyright 2016 Mauricio Jost
 * Contact: mauriciojostx@gmail.com
 *
 * This file is part of LOG4INO.
 *
 * LOG4INO is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * LOG4INO is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LOG4INO. If not, see <http://www.gnu.org/licenses/>.
 */

#include <log4ino/Log.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#ifndef MAX_LOG_MSG_LENGTH
#define MAX_LOG_MSG_LENGTH 16
#endif // MAX_LOG_MSG_LENGTH

#ifndef UNIT_TEST

// !UNIT_TEST, SO ON-BOARD EXECUTION
#ifdef YES_DEBUG

char logLevel = LOG_LEVEL;
const char *logLevelStr[4] = {"D", "I", "W", "E"};
void (*prntFunc)(const char *) = NULL;

void setLogLevel(char level) { logLevel = level; }

void setupLog(void (*prnt)(const char *)) {
  prnt("-U-");
  prntFunc = prnt;
}

void log(const char *clz, LogLevel l, const char *format, ...) {
  if (logLevel <= l) {

    char buffer[MAX_LOG_MSG_LENGTH];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, MAX_LOG_MSG_LENGTH, format, args);
    va_end(args);
    buffer[MAX_LOG_MSG_LENGTH - 1] = 0;

    char bufferTotal[MAX_LOG_MSG_LENGTH];
    snprintf(bufferTotal, MAX_LOG_MSG_LENGTH, "%s %s %s\n", clz, logLevelStr[l],
             buffer);
    bufferTotal[MAX_LOG_MSG_LENGTH - 1] = 0;

    if (prntFunc != NULL) {
      prntFunc(bufferTotal);
    }
  }
}

void logHex(const char *clz, LogLevel l, const unsigned char *buf, int bytes) {
  char buffer[MAX_LOG_MSG_LENGTH];
  char val[3];
  buffer[0] = 0;
  for (int i = 0; i < bytes; i++) {
    sprintf(val, "%.2x\n", buf[i]);
    strncat(buffer, val, MAX_LOG_MSG_LENGTH);
  }
  buffer[MAX_LOG_MSG_LENGTH - 1] = 0;
  log(clz, l, buffer);
}

void logRaw(const char *clz, LogLevel l, const char *raw) {
  if (logLevel <= l) {
  	if (prntFunc != NULL) {
      prntFunc(clz);
      prntFunc(" ");
      prntFunc(logLevelStr[l]);
      prntFunc(" ");
      prntFunc(raw);
      prntFunc("\n");
  	}
  }
}

#else // !YES_DEBUG

// Do not generate logs
void setupLog(void (*prnt)(const char *)) {}

void setLogLevel(char level) {}

void log(const char *clz, LogLevel l, const char *format, ...) {}

void logHex(const char *clz, LogLevel l, const unsigned char *buf, int bytes) {}

void logRaw(const char *clz, LogLevel l, const char *raw) {}

#endif // YES_DEBUG

#else // UNIT_TEST, SO ON-PC EXECUTION

#include <log4ino/Colors.h>

char logLevel = LOG_LEVEL;
const char *logLevelStr[4] = {KYEL "DEBUG" KNRM, KBLU "INFO " KNRM,
                              KMAG "WARN " KNRM, KRED "ERROR" KNRM};

void setupLog(void (*prnt)(const char *)) {}

void setLogLevel(char level) {
  logLevel = level;
}

void log(const char *clz, LogLevel l, const char *format, ...) {
  if (logLevel <= l) {
    char buffer[MAX_LOG_MSG_LENGTH];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, MAX_LOG_MSG_LENGTH, format, args);
    buffer[MAX_LOG_MSG_LENGTH - 1] = 0;
    printf("[%8.8s] [%s]: %s\n", clz, logLevelStr[l], buffer);
    va_end(args);
  }
}

void logHex(const char *clz, LogLevel l, const unsigned char *buf, int bytes) {
  char buffer[MAX_LOG_MSG_LENGTH];
  char val[3];
  buffer[0] = 0;
  for (int i = 0; i < bytes; i++) {
    sprintf(val, "%.2x", buf[i]);
    strncat(buffer, val, MAX_LOG_MSG_LENGTH);
  }
  buffer[MAX_LOG_MSG_LENGTH - 1] = 0;
  log(clz, l, buffer);
}

void logRaw(const char *clz, LogLevel l, const char *raw) {
  if (logLevel <= l) {
    printf("[%8.8s] [%s]: %s\n", clz, logLevelStr[l], raw);
  }
}

#endif // UNIT_TEST
