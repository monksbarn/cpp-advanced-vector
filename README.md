## cpp-advanced-vector

### Аналог структуры данных из стандартной библиотеки std::vector
***
Полное соответствие парадигмам ООП, RAII, а также обеспечение строгой гарантии безопасности при операциях над контейнером.

Реализованные методы:
* Reserve - выделяет и резервирует память на заданное количетсов элементов указанного типа;
* Resize - изменяет размер контейнера;
* PushBack - всталяет элемент в конец контейнера;
* PopBack - удаляет последний элемент из контейнера;
* EmplaceBack - создает элемент на месте в конце контейнера;
* Emplace - создает элемент на месте в заданной позиции;
* Erase - удаляет элемент из указанной позиции;
* Insert - вставляет элемент в указанную позицию;
* Size - возвращаят задействованный размер контейнера;
* Capacity - возвращает реальный размер контейнера;
* Swap - обменивает данные заданного контейнера с указанным;
