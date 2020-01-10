# backdoor

A mini tool to verify some TCP socket functionality and learn testing with
[cmocka](https://cmocka.org) ([cmocka documentation](https://api.cmocka.org)).


## Dependencies

- C compiler
- [cmocka](https://cmocka.org)


## Build

```
mkdir BD_release
cd BD_release
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release ..
make all test
```
