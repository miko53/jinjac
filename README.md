# jinjac

[![Language](https://img.shields.io/badge/language-c99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![License](https://img.shields.io/github/license/miko53/jinjac)](https://raw.githubusercontent.com/miko53/jinjac/master/LICENSE.txt)

Jinjac is a library which adapt the jinja templating language for C.
its aim is to be very minimalistic and with few dependancies in order to be able to use it in a embedded software.
Nevertheless it tries to implement a large set feature.

It comes from with several unit tests (see test folders)

## list of implemented features

- basic template replacing (with all types of data e.g. string, integer, double)
- filtering (see list of build-in function)
- comment management 
- function execution
- for statement
- if statement

## build-in function

Here we are the list of build-in function which are implemented

|function|description|status|
|--------|-----------|------|
|abs()|| not implemented
|attr()|| not implemented
|batch()|| not implemented
|capitalize()| first char in upper case another in lower | [OK]
|center()| center the value center(s, width=80) | [OK]
|default()| default value if undefined | low prio
|dictsort()|| not implemented
|escape()|| low prio
|filesizeformat()|| not implemented
|first()|| not implemented
|float()|| not implemented
|forceescape()|| not implemented
|format()| format string according to parameter (equivalent to printf) | [OK]
|groupby()|| not implemented
|indent()|| low prio
|int()|| not implemented
|join()|| low prio
|last()|| low prio
|length()|| low prio
|list()|| not implemented
|lower()|put string in lower case| [OK]
|map()|| not implemented
|max()|| not implemented
|min()|| not implemented
|pprint()|| not implemented
|random()|| not implemented
|reject()|| not implemented
|rejectattr()|| not implemented
|replace()|| low prio
|reverse()|| low prio
|round()|| not implemented
|safe()|| not implemented
|select()|| not implemented
|selectattr()|| not implemented
|slice()|| low prio
|sort()|| not implemented
|string()|| not implemented
|striptags()|| not implemented
|sum()|| not implemented
|title()|set first char of word in upper case, othersiwe in lower case| [OK]
|tojson()|| not implemented
|trim()|remove space at begin and end of the string| [OK]
|truncate()| truncate a string at a specified offset plus extras options| [OK]
|unique()|| not implemented
|upper()|set string to upper case | [OK]
|urlencode()|| not implemented
|urlize()|| not implemented
|wordcount()|| not implemented
|wordwrap()|| low prio
|xmlattr()|| not implemented

## TODO

### To implement / not implemented

Here we are the list of limitation.
(or the aim of this library some of thems are not really necessary)

- missing function of previous table
- in if statement
  * logical are not completed (not, and, or, is, ...)
- whitespace control (with '-' characters)
- raw statement
- set statement
- template inheritance
- child template
- super block
- extends
- html espacing
- in for the loop variables doesn't exist
- in for the else is not managed
- macros / call
- filters as statement
- assignment in function argument
- include
- autoescape

### To improve

- parameter management
It should be preferable to improve search or use a index when multiple searches are performed.
