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

#define LOG_CLASS "LG"
#define LOG_UNIT_EXPR_LEN 4

#ifndef MAX_LOG_OPTIONS_RULES
#define MAX_LOG_OPTIONS_RULES 8
#endif // MAX_LOG_OPTIONS_RULES

char logLevel = LOG_LEVEL;
char logOptions[(MAX_LOG_OPTIONS_RULES * LOG_UNIT_EXPR_LEN) + 1];

void disableLogOptions() {
  strcpy(logOptions, " ");
}

bool hasToLog(LogLevel l, const char* clz) {

  if (getLogOptions() == NULL) {
    return (l >= logLevel);
  }

  for (int p = 0; p < strlen(getLogOptions()); p += LOG_UNIT_EXPR_LEN) {
    char optClz0 = logOptions[p+0];
    char optClz1 = logOptions[p+1];
    LogLevel lg = (LogLevel)(logOptions[p+2] - '0');
    if(clz[0] == optClz0 && clz[1] == optClz1) { // direct match
      return (l >= lg);
    } else if (clz[0] == '?' && clz[1] == optClz1) { // one char match
      return (l >= lg);
    } else if (clz[0] == optClz0 && clz[1] == '?') { // one char match
      return (l >= lg);
    } else if ('?' == optClz0 && '?' == optClz1) { // no char match
      return (l >= lg);
    }
  }
  return (l >= logLevel);
}

void setLogOptions(const char *opts) {
  if (opts == NULL) {
    disableLogOptions();
  } else {
    if (strlen(opts) % 4 == 0) {
      strncpy(logOptions, opts, MAX_LOG_OPTIONS_RULES * LOG_UNIT_EXPR_LEN);
      logOptions[MAX_LOG_OPTIONS_RULES * LOG_UNIT_EXPR_LEN] = 0;
    } else {
      log("LG", User, "Invalid log options");
      disableLogOptions();
    }
  }
}

const char* getLogOptions() {
  if (logOptions[0] == ' ' || logOptions[0] == 0 || strlen(logOptions) < 1) {
    return NULL;
  } else {
    return logOptions;
  }
}

#ifdef YES_DEBUG

const char *logLevelStr[5] = {"D", "I", "W", "E", "U"};

#ifdef X86_64
#include <log4ino/Colors.h>
const char *logLevelStrRich[5] = {KYEL "DEBUG" KNRM, KBLU "INFO " KNRM, KMAG "WARN " KNRM, KRED "ERROR" KNRM, KBLU "USER" KNRM};
#endif // X86_64


void (*prntFunc)(const char *msg, const char *clz, LogLevel l) = NULL;
#endif // YES_DEBUG

void setLogLevel(char level) { 
#ifdef YES_DEBUG
  logLevel = level;
#endif // YES_DEBUG
}
char getLogLevel(){
#ifdef YES_DEBUG
  return logLevel;
#endif // YES_DEBUG
}

void setupLog(void (*prnt)(const char *msg, const char *clz, LogLevel l)) {
#ifdef YES_DEBUG
  prnt("-U-", LOG_CLASS, Debug);
  prntFunc = prnt;
  disableLogOptions();
#endif // YES_DEBUG
}

void log(const char *clz, LogLevel l, const char *format, ...) {
#ifdef YES_DEBUG
  if (hasToLog(l, clz)) {

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
    bufferTotal[MAX_LOG_MSG_LENGTH - 2] = '\n';
#ifdef X86_64 // print anyway
    printf("[%8.8s] [%s]: %s\n", clz, logLevelStrRich[l], bufferTotal);
#endif // X86_64
    if (prntFunc != NULL) {
      prntFunc(bufferTotal, clz, l);
    }
  }
#endif // YES_DEBUG
}


void logHex(const char *clz, LogLevel l, const unsigned char *buf, int bytes) {
#ifdef YES_DEBUG
  char buffer[MAX_LOG_MSG_LENGTH];
  char val[4];
  buffer[0] = 0;
  for (int i = 0; i < bytes; i++) {
    sprintf(val, "%.2x\n", buf[i]);
    strncat(buffer, val, MAX_LOG_MSG_LENGTH);
  }
  buffer[MAX_LOG_MSG_LENGTH - 1] = 0;
  log(clz, l, buffer);
#endif // YES_DEBUG
}

void logRaw(const char *clz, LogLevel l, const char *raw) {
#ifdef YES_DEBUG
  if (hasToLog(l, clz)) {
  	if (prntFunc != NULL) {
      prntFunc(clz, clz, l);
      prntFunc(" ", clz, l);
      prntFunc(logLevelStr[l], clz, l);
      prntFunc(" ", clz, l);
      prntFunc(raw, clz, l);
      prntFunc("\n", clz, l);
  	}
  }
#endif // YES_DEBUG
}
