# jinjac

[![Language](https://img.shields.io/badge/language-c99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![License](https://img.shields.io/github/license/miko53/jinjac)](https://raw.githubusercontent.com/miko53/jinjac/master/LICENSE.txt)

Jinjac is a library which adapt the jinja templating language for C.
its aim is to be very minimalistic and with few dependancies in order to be able to use it in a embedded software.
Nevertheless it tries to implement a large set feature.

It comes from with several unit tests (see test folders)

## usage example

Here we are an example with the buffer API. It rules similar with file API.

```c
  char* template =
  "Hello from jinjac {{ user }} !\n"
  "{% for x in data -%}\n"
  " -> data {{ x }}\n"
  "{% endfor %}\n"
  "end template\n"
  ;

  jinjac_init();

  jinjac_parameter john;
  john.type = TYPE_STRING;
  john.value.type_string = "John";

  jinjac_parameter_insert("user", &john);
  jinjac_parameter_array_insert("data", TYPE_INT, 10, 5, 8, 987, 16, 1, 5, 9, 8, 58, 6);

  char* pResult = NULL;
  int sizeResult = 0;

  jinjac_render_with_buffer(template, strlen(template), &pResult, &sizeResult);
  fprintf(stdout, "%s\n", pResult);

  free(pResult);
  jinjac_destroy();
```

It will return 

```
Hello from jinjac John !
-> data 5
-> data 8
-> data 987
-> data 16
-> data 1
-> data 5
-> data 9
-> data 8
-> data 58
-> data 6

end template
```
It can be possible to use an other variable management, by implementing the parameter search and get callbacks as indicated in this example
```c
// map search and get callback 
jinjac_parameter_callback jinjac_specific_search_cb =
{
  .search = p_search,
  .get = p_get,
  .array_getProperties = p_array_getProperties,
  .array_getValue = p_array_getValue
};

//after initialisation register it
jinjac_parameter_register(&jinjac_specific_search_cb);

//callback implementation
int p_search(char* key, int32_t* privKey, int* isArray)
{
  //simulate another array of data to test
  int founded = 0;
  int bIsArray;
  bIsArray = 0;

  if (strcmp(key, "@name") == 0)
  {
    *privKey = 0;
    founded = 1;
    if (isArray != NULL)
      *isArray = 0;
  }
  
  return founded;
}

J_STATUS p_get(int32_t privKey, jinjac_parameter* param)
{
  J_STATUS s;
  s = J_OK;

  switch (privKey)
  {
    case 0: //@name
      param->type = TYPE_STRING;
      param->value.type_string = "Tyrion";
      break;
      
    default:
      s = J_ERROR; //not found
    break;
  }
  
  return s;
}

```
See example in file jinjac_test_app.c
Then after, the library will use the indicated functions to search and retrieve parameters.


## list of implemented features

- basic template replacing (with all types of data e.g. string, integer, double)
- filtering (see list of build-in function)
- comment management 
- function execution
- for statement
- if statement
- strip whitespace (with minus characters in statement)
- possibly to manage variables without store them into jinjac library


## TODO

### To implement / not implemented

Here we are the list of limitation.
(or the aim of this library some of thems are not really necessary)

- missing build-in function (see table at the end)
- in if statement 'is' is not managed
- negative value (-X) not managed in expression
- whitespace control (with '+' characters)
  * in fact it has the default behavior and you can control whitespace strip with '-'
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

Search of parameter could be improved by using a hash table on variable name

- object management

Separate parameter and ast object and add a object form for parameter (design improvement)

## Dependancies

The library uses standard glibc library and need flex and bison to build.
cmake is used to construct make system.
To perform coverage analysis, you need to install lcov package.
It uses dynamic memory allocation (`malloc()` and `free()`).

## Build

It exists 3 scripts which can be used to configure the build of the library:
  - `build_debug.sh`
 This one configures to build library in debug mode (with trace and assertion)
 
 - `build_release.sh`
 This one configure in release mode.
 Example:
 ```
 $ ./build_release.sh
 $ cd build_release
 $ make all 
 $ ctest 
 ```
 
 - `build_coverage.sh`
 this one configures and builds the library, performs unit tests and retrieves the coverage.

 
ruby and valgrind are used to perform tests.

## build-in function

Here we are the list of build-in functions to implement and current status

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
|join()| concatenate the sequence into a string| [OK]
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

