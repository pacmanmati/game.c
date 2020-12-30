#pragma once
#include <stdbool.h>
#include <stdio.h> // DEBUG

/* some general purpose functions for dealing with strings in c 
 * if something isn't likely to be re-used it probably won't make its
 * way in here.
*/

// returns the first occurence of c in a string.
// if none, returns '\0'.
static const char * match_char(const char *str, char c)
{
  while (!(*str == '\0' || *str == c)) str++;
  return str;
}

static const char * match_char_up_to(const char *str, char c, const char *end)
{
  while (!(*str == '\0' || *str == c || str == end)) str++;
  return str;
}

// returns the first occurence of any of c's chars in a string.
// if none, returns '\0'.
static const char * match_any_char(const char *str, const char *c)
{
  while (*str != '\0') {
    const char *start = c;
    while (*c != '\0') {
      if (*str == *(c++)) return str;
    }
    c = start;
    str++;
  }
  return str;
}

static const char * match_any_char_up_to(const char *str, const char *c, const char* end)
{
  while (!(*str == '\0' || str == end)) {
    const char *start = c;
    while (*c != '\0') {
      if (*str == *(c++)) return str;
    }
    c = start;
    str++;
  }
  return str;
}

// returns the first occurence of sub in the string.
// if none, returns '\0'.
static const char * match_substring(const char *str, const char *sub)
{
  while (*str != '\0') {
    const char *str_save = str;
    const char *sub_start = sub;
    while (*sub != '\0') {
      if (*sub != *str) break;
      sub++;
      str++;
    }
    if (*sub == '\0') return str_save; // a match was made
    sub = sub_start;
    str = str_save;
    str++;
  }
  return str;
}

static const char * match_substring_up_to(const char *str, const char *sub, const char *end)
{  
  while (!(*str == '\0' || str == end)) {
    const char *str_save = str;
    const char *sub_start = sub;
    while (*sub != '\0') {
      if (*sub != *str) break;
      sub++;
      str++;
    }
    if (*sub == '\0') return str_save; // a match was made
    sub = sub_start;
    str = str_save;
    str++;
  }
  return str;
}
