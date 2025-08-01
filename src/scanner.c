//> Scanning on Demand scanner-c
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "scanner.h"

typedef struct {
  const char* start;
  const char* current;
  int line;
} Scanner;

Scanner scanner;
//> init-scanner
void initScanner(const char* source) {
  scanner.start = source;
  scanner.current = source;
  scanner.line = 1;
}
//< init-scanner
//> is-alpha
static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
          c == '_';
}
//< is-alpha
//> is-digit
static bool isDigit(char c) {
  return c >= '0' && c <= '9';
}
//< is-digit
//> is-at-end
static bool isAtEnd() {
  return *scanner.current == '\0';
}
//< is-at-end
//> advance
static char advance() {
  scanner.current++;
  return scanner.current[-1];
}
//< advance
//> peek
static char peek() {
  return *scanner.current;
}
//< peek
//> peek-next
static char peekNext() {
  if (isAtEnd()) return '\0';
  return scanner.current[1];
}
//< peek-next
//> match
static bool match(char expected) {
  if (isAtEnd()) return false;
  if (*scanner.current != expected) return false;
  scanner.current++;
  return true;
}
//< match
//> make-token
static Token makeToken(TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.line;
  return token;
}
//< make-token
//> error-token
static Token errorToken(const char* message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.line;
  return token;
}
//< error-token
//> skip-whitespace
static void skipWhitespace() {
  for (;;) {
    char c = peek();
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
        advance();
        break;
//> newline
      case '\n':
        scanner.line++;
        advance();
        break;
//< newline
//> comment
     case '#':
  // A comment goes until the end of the line.
  while (peek() != '\n' && !isAtEnd()) advance();
  break;
//< comment
      default:
        return;
    }
  }
}
//< skip-whitespace
//> check-keyword
static TokenType checkKeyword(int start, int length,
    const char* rest, TokenType type) {
  if (scanner.current - scanner.start == start + length &&
      memcmp(scanner.start + start, rest, length) == 0) {
    return type;
  }

  return TOKEN_IDENTIFIER;
}

static TokenType identifierType() {
  switch (scanner.start[0]) {
    case 'd':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'h': return checkKeyword(2, 1, "e", TOKEN_AND);     // dhe
          case 'e': return checkKeyword(2, 4, "risa", TOKEN_WHILE);  // derisa
        }
      }
      break;
    case 'f': return checkKeyword(1, 7, "unksion", TOKEN_FUN);     // funksion
    case 'g': return checkKeyword(1, 5, "abuar", TOKEN_FALSE);     // gabuar
     case 'k': return checkKeyword(1, 3, "the", TOKEN_RETURN);
      case 't': return checkKeyword(1, 2, "ip", TOKEN_CLASS);
    case 'n': return checkKeyword(1, 3, "ese", TOKEN_IF);         // nese
    case 'o': return checkKeyword(1, 2, "se", TOKEN_OR);          // ose
    case 'p':
      if (scanner.current - scanner.start > 1) {
        switch (scanner.start[1]) {
          case 'e': return checkKeyword(2, 1, "r", TOKEN_FOR);       // per
          case 'r':
              if (scanner.current - scanner.start > 2) {
                  switch (scanner.start[2]) {
                      case 'i': return checkKeyword(3, 3, "nto", TOKEN_PRINT); // printo
                      case 'n': return checkKeyword(3, 6, "dryshe", TOKEN_ELSE);// perndryshe
                  }
              }
              break;
        }
      }
      break;
    case 's': return checkKeyword(1, 5, "hpall", TOKEN_VAR);       // shpall
    case 'v': return checkKeyword(1, 5, "ertet", TOKEN_TRUE);      // vertet
  }

  return TOKEN_IDENTIFIER;
}
static Token identifier() {
  while (isAlpha(peek()) || isDigit(peek())) advance();
  return makeToken(identifierType());
}
//< identifier
//> number
static Token number() {
  while (isDigit(peek())) advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the ".".
    advance();

    while (isDigit(peek())) advance();
  }

  return makeToken(TOKEN_NUMBER);
}
//< number
//> string
static Token string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') scanner.line++;
    advance();
  }

  if (isAtEnd()) return errorToken("Unterminated string.");

  // The closing quote.
  advance();
  return makeToken(TOKEN_STRING);
}
//< string
//> scan-token
Token scanToken() {
//> call-skip-whitespace
  skipWhitespace();
//< call-skip-whitespace
  scanner.start = scanner.current;

  if (isAtEnd()) return makeToken(TOKEN_EOF);
//> scan-char
  
  char c = advance();
//> scan-identifier
  if (isAlpha(c)) return identifier();
//< scan-identifier
//> scan-number
  if (isDigit(c)) return number();
//< scan-number

  switch (c) {
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '[': return makeToken(TOKEN_LEFT_BRACKET); // <-- ADD THIS
    case ']': return makeToken(TOKEN_RIGHT_BRACKET); // <-- ADD THIS
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case '-': return makeToken(TOKEN_MINUS);
    case '+': return makeToken(TOKEN_PLUS);
    case '/': return makeToken(TOKEN_SLASH);
    case '*': return makeToken(TOKEN_STAR);
//> two-char
    case '!':
      return makeToken(
          match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
    case '=':
      return makeToken(
          match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
    case '<':
      return makeToken(
          match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
    case '>':
      return makeToken(
          match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
//< two-char
//> scan-string
    case '"': return string();
//< scan-string
  }
//< scan-char

  return errorToken("Unexpected character.");
}
//< scan-token