# Changelog

## 0.1.0
- Fix: добавлен отсутствующий `#include <vector>` в `include/gzip/compress.hpp` (исправляет сборку под MSVC).
- CMake: задана версия проекта (`project(... VERSION 0.1.0 ...)`).
- CMake: безопасное подключение как сабмодуль — по умолчанию не используются mason-зависимости и не собираются тесты/бенчи при `add_subdirectory()`.
- CMake: добавлена header-only цель `gzip-hpp::gzip_hpp` для удобного линкования в потребителях.