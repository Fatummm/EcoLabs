# EcoLab2

## Реализация включения и агрегирования компонентов по стандарту ACOM

---

1. [Задача](#1-задача)
2. [Архитектура компонентов](#2-архитектура-компонентов)
3. [Паттерны ACOM](#3-паттерны-acom)
4. [Описание компонентов](#4-описание-компонентов)
5. [Навигация по интерфейсам](#5-навигация-по-интерфейсам)
6. [Результаты тестирования](#6-результаты-тестирования)

Работу подготовил Лепа Дмитрий

---

## 1. Задача

Требуется реализовать пять вариантов компонентов-калькуляторов (A, B, C, D, E), предоставляющих интерфейсы `IEcoCalculatorX` (сложение, вычитание) и `IEcoCalculatorY` (умножение, деление), а также два составных компонента — Lab1 и Lab2, — демонстрирующих механизмы *включения* (containment) и *агрегирования* (aggregation) по стандарту ACOM.

Каждый компонент наследует от `IEcoUnknown` и обязан выполнять правила идентичности, симметрии и транзитивности интерфейсов при вызове `QueryInterface`.

### Интерфейсы

**IEcoCalculatorX** (IID `{3B92D548-1E54-4D90-9C94-FB9C1E2A77CE}`):

```c
int32_t Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b);
int32_t Subtraction(IEcoCalculatorXPtr_t me, int32_t a, int32_t b);
```

**IEcoCalculatorY** (IID `{5C3B5422-7218-4B29-94E3-8A8EAB9C7E40}`):

```c
int32_t Multiplication(IEcoCalculatorYPtr_t me, int32_t a, int32_t b);
int32_t Division(IEcoCalculatorYPtr_t me, int32_t a, int32_t b);
```

Оба интерфейса наследуют от `IEcoUnknown` — методы `QueryInterface`, `AddRef` и `Release` расположены на позициях 0, 1, 2 виртуальной таблицы.

## 2. Архитектура компонентов

Иерархия компонентов спроектирована так, чтобы продемонстрировать все основные механизмы повторного использования ACOM:

| Компонент | Реализует | Механизм |
|---|---|---|
| **A** | IEcoCalculatorX | Простой компонент, без поддержки агрегирования |
| **B** | IEcoCalculatorX | Поддержка агрегирования (NonDelegating паттерн) |
| **C** | IEcoCalculatorX + IEcoCalculatorY | Два интерфейса в одном компоненте |
| **D** | IEcoCalculatorY | Включение компонента A для IEcoCalculatorX |
| **E** | IEcoCalculatorY | Агрегирование компонента B для IEcoCalculatorX |
| **Lab1** | IKD_Tree + IEcoCalculatorX + IEcoCalculatorY | Включение A и D, поддержка агрегирования |
| **Lab2** | IKD_Tree + IEcoCalculatorX + IEcoCalculatorY | Агрегирование Lab1, включение B и E |

### Схема зависимостей

```
Lab2
 ├── [агрегирование] Lab1 (KD-Tree)
 │    ├── [включение] A (IEcoCalculatorX)
 │    └── [включение] D (IEcoCalculatorY)
 │         └── [включение] A (IEcoCalculatorX)
 ├── [включение] B (IEcoCalculatorX)
 └── [включение] E (IEcoCalculatorY)
      └── [агрегирование] B (IEcoCalculatorX)
```

## 3. Паттерны ACOM

### 3.1. Включение (Containment)

При включении внешний компонент создаёт экземпляр внутреннего компонента и хранит указатель на его интерфейс. Внешний компонент предоставляет собственную виртуальную таблицу, которая делегирует вызовы методов данных внутреннему компоненту, но управляет подсчётом ссылок самостоятельно. Внутренний компонент не знает о существовании внешнего.

Пример — компонент D включает A:

```c
typedef struct CEcoCalculatorD {
    IEcoCalculatorYVTbl* m_pVTblIEcoCalculatorY;   /* основной интерфейс */
    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;   /* делегирующая vtable */
    uint32_t m_cRef;
    ...
    IEcoCalculatorX* m_pInnerCalculatorA;           /* включённый A */
} CEcoCalculatorD;
```

Делегирующая vtable для IEcoCalculatorX:
- `QueryInterface`, `AddRef`, `Release` — перенаправляются к основному интерфейсу D через макрос `offsetof`, обеспечивая единый подсчёт ссылок.
- `Addition`, `Subtraction` — делегируются включённому компоненту A:

```c
static int32_t ECOCALLMETHOD CEcoCalculatorD_X_Addition(IEcoCalculatorXPtr_t me, int32_t a, int32_t b) {
    CEcoCalculatorD* pCMe = (CEcoCalculatorD*)((char_t*)me - offsetof(CEcoCalculatorD, m_pVTblIEcoCalculatorX));
    return pCMe->m_pInnerCalculatorA->pVTbl->Addition(pCMe->m_pInnerCalculatorA, a, b);
}
```

При уничтожении D освобождает включённый A вызовом `Release`.

### 3.2. Агрегирование (Aggregation)

При агрегировании внешний компонент создаёт внутренний, передавая ему указатель `pIUnkOuter` на свой `IEcoUnknown`. Внутренний компонент реализует две версии `IEcoUnknown`:

- **Не-делегирующий** (NonDelegating) — управляет собственным счётчиком ссылок, используется внешним компонентом для управления временем жизни внутреннего.
- **Делегирующий** — перенаправляет `QueryInterface`, `AddRef` и `Release` к `pIUnkOuter`, обеспечивая единую идентичность составного объекта.

Пример — компонент B (поддерживает агрегирование):

```c
typedef struct CEcoCalculatorB {
    IEcoCalculatorXVTbl* m_pVTblIEcoCalculatorX;    /* делегирующий */
    IEcoUnknownVTbl*     m_pVTblINondelegatingUnk;  /* не-делегирующий */
    uint32_t m_cRef;
    ...
    IEcoUnknown* m_pIUnkOuter;
} CEcoCalculatorB;
```

Не-делегирующий `QueryInterface`:
- Для `IID_IEcoUnknown` — возвращает не-делегирующий указатель, вызывает не-делегирующий `AddRef` (++m_cRef).
- Для `IID_IEcoCalculatorX` — возвращает делегирующий указатель, вызывает делегирующий `AddRef` (→ pIUnkOuter→AddRef).

Делегирующий `QueryInterface`, `AddRef`, `Release` безусловно перенаправляются к `m_pIUnkOuter`.

При создании в автономном режиме (pIUnkOuter == 0) поле `m_pIUnkOuter` устанавливается на собственный не-делегирующий `IEcoUnknown`, и делегирующие вызовы замыкаются на самого себя.

### 3.3. Восстановление указателя через offsetof

Для компонентов с несколькими интерфейсами указатель `me` может указывать не на начало структуры, а на поле вторичной vtable. Для восстановления указателя на компонент используется макрос `offsetof`:

```c
CEcoCalculatorD* pCMe = (CEcoCalculatorD*)((char_t*)me - offsetof(CEcoCalculatorD, m_pVTblIEcoCalculatorX));
```

Этот приём применяется в компонентах C, D, Lab1 и Lab2 для всех вторичных интерфейсов.

### 3.4. Правило идентичности

Вызов `QueryInterface(riid = IID_IEcoUnknown)` на любом интерфейсе компонента должен возвращать один и тот же указатель. Это обеспечивается тем, что все вторичные vtable перенаправляют `QueryInterface` к основному интерфейсу (через `offsetof` или через делегирование к `pIUnkOuter`), а основной `QueryInterface` всегда возвращает фиксированный указатель для `IID_IEcoUnknown`.

Например, для Lab2:
- `QI` на `IKD_Tree` — напрямую: возвращает `&m_pVTblIKD_Tree`.
- `QI` на `IEcoCalculatorX` — через `offsetof` → основной `QI` → `&m_pVTblIKD_Tree`.
- `QI` на `IEcoCalculatorY` — через `offsetof` → основной `QI` → `&m_pVTblIKD_Tree`.

Все три пути возвращают один и тот же указатель.

## 4. Описание компонентов

### Компонент A — CEcoCalculatorA

Простейший компонент, реализующий `IEcoCalculatorX`. Не поддерживает агрегирование — при передаче ненулевого `pIUnkOuter` возвращает `ERR_ECO_NOAGGREGATION`.

Структура содержит единственную vtable, счётчик ссылок, указатели на системный интерфейс и аллокатор памяти. `QueryInterface` обрабатывает `IID_IEcoCalculatorX` и `IID_IEcoUnknown`, возвращая один и тот же указатель.

### Компонент B — CEcoCalculatorB

Реализует `IEcoCalculatorX` с поддержкой агрегирования по паттерну NonDelegating (описан в разделе 3.2). Функция `CreateInstance` возвращает указатель на не-делегирующий `IEcoUnknown` (`IEcoUnknown**`), а не на интерфейс калькулятора. Вызывающий код должен получить `IEcoCalculatorX` через `QueryInterface` на не-делегирующем `IEcoUnknown`.

### Компонент C — CEcoCalculatorC

Реализует оба интерфейса — `IEcoCalculatorX` и `IEcoCalculatorY` — в одном компоненте. Содержит две vtable. Вторичная vtable (`IEcoCalculatorY`) использует `offsetof` для восстановления указателя на компонент и перенаправляет `QueryInterface` к основной vtable. Не поддерживает агрегирование.

### Компонент D — CEcoCalculatorD

Реализует `IEcoCalculatorY` собственными методами и использует *включение* компонента A для предоставления `IEcoCalculatorX`. При создании экземпляра вызывает `CEcoCalculatorA_CreateInstance` с `pIUnkOuter = 0`. Делегирующая vtable для `IEcoCalculatorX` перенаправляет `Addition` и `Subtraction` к включённому A, а `QI`/`AddRef`/`Release` — к основному интерфейсу D. Не поддерживает агрегирование.

### Компонент E — CEcoCalculatorE

Реализует `IEcoCalculatorY` собственными методами и использует *агрегирование* компонента B для предоставления `IEcoCalculatorX`. При создании вызывает `CEcoCalculatorB_CreateInstance` с `pIUnkOuter = (IEcoUnknown*)pCMe`, передавая свой `IEcoUnknown` как внешний. Собственной vtable для `IEcoCalculatorX` не содержит — при запросе `IID_IEcoCalculatorX` делегирует вызов не-делегирующему `QueryInterface` компонента B, который возвращает делегирующий указатель B.

Делегирующий `IEcoCalculatorX` компонента B перенаправляет `QI`/`AddRef`/`Release` к E (через `pIUnkOuter`), тем самым подсчёт ссылок и идентичность управляются E.

### Lab1 — CKD_Tree (модифицированный)

Компонент KD-дерева из Задания 1, расширенный для поддержки агрегирования и включения калькуляторных компонентов.

**Добавленные элементы структуры:**
- `m_pVTblINondelegatingUnk` — не-делегирующий `IEcoUnknown` для поддержки агрегирования.
- `m_pVTblIEcoCalculatorX` — делегирующая vtable, делегирует `Addition`/`Subtraction` включённому компоненту A.
- `m_pVTblIEcoCalculatorY` — делегирующая vtable, делегирует `Multiplication`/`Division` включённому компоненту D.
- `m_pIUnkOuter` — указатель на внешний `IEcoUnknown` (при агрегировании) или на собственный не-делегирующий (при автономной работе).
- `m_pInnerCalculatorX` — включённый компонент A.
- `m_pInnerCalculatorY` — включённый компонент D (D, в свою очередь, включает собственный A).

**Две функции создания:**
- `CreateInstance` — для автономного использования, возвращает `IKD_Tree**`. Отклоняет агрегирование.
- `CreateInstanceAggr` — для агрегирования, принимает `pIUnkOuter`, возвращает не-делегирующий `IEcoUnknown**`.

Не-делегирующий `QueryInterface` обрабатывает `IID_IKD_Tree`, `IID_IEcoUnknown`, `IID_IEcoCalculatorX` и `IID_IEcoCalculatorY`, возвращая соответствующие vtable компонента и инкрементируя собственный счётчик ссылок.

Делегирующие `QI`/`AddRef`/`Release` для `IKD_Tree`, `IEcoCalculatorX` и `IEcoCalculatorY` перенаправляются к `m_pIUnkOuter`.

### Lab2 — CEcoLab2

Составной компонент, объединяющий все механизмы:

- **Агрегирование Lab1** — Lab2 создаёт Lab1 через `CreateInstanceAggr`, передавая себя как `pIUnkOuter`. Lab2 хранит не-делегирующий `IEcoUnknown` Lab1 (`m_pInnerUnkLab1`) для управления временем жизни и делегирующий `IKD_Tree` (`m_pInnerLab1`) для вызова методов KD-дерева. После получения `IKD_Tree` через `QueryInterface` на не-делегирующем `IEcoUnknown` выполняется балансирующий `Release`, чтобы время жизни Lab1 управлялось единственной ссылкой `m_pInnerUnkLab1`.

- **Включение B** — Lab2 создаёт B в автономном режиме (`pIUnkOuter = 0`), получает `IEcoCalculatorX` и хранит его для делегирования `Addition`/`Subtraction`.

- **Включение E** — Lab2 создаёт E в автономном режиме, хранит `IEcoCalculatorY` для делегирования `Multiplication`/`Division`. При этом E внутри себя агрегирует собственный экземпляр B.

Lab2 предоставляет три интерфейса через собственные vtable:
- `IKD_Tree` — основной; `QI` обрабатывает все три IID и `IEcoUnknown`. Методы KD-дерева делегируются агрегированному Lab1.
- `IEcoCalculatorX` — через `offsetof` перенаправляет `QI`/`AddRef`/`Release` к основному, а `Addition`/`Subtraction` — к включённому B.
- `IEcoCalculatorY` — аналогично, `Multiplication`/`Division` — к включённому E.

Lab2 не поддерживает собственное агрегирование — при передаче ненулевого `pIUnkOuter` возвращает `ERR_ECO_NOAGGREGATION`.

## 5. Навигация по интерфейсам

Реализация гарантирует выполнение всех правил `QueryInterface` стандарта ACOM:

**Правило идентичности.** `QI(любой_интерфейс, IID_IEcoUnknown)` всегда возвращает один и тот же указатель для данного компонента. Для автономных компонентов это указатель на основную vtable; при агрегировании — указатель, возвращённый `QI` внешнего компонента.

**Правило симметрии.** Если `QI(pA, IID_B)` успешен, то `QI(pB, IID_A)` также успешен. Это обеспечивается тем, что все вторичные vtable перенаправляют `QI` к единому основному обработчику, который знает обо всех поддерживаемых интерфейсах.

**Правило транзитивности.** Если `QI(pA, IID_B)` и `QI(pB, IID_C)` успешны, то `QI(pA, IID_C)` также успешен. Следует из того, что все `QI` сводятся к одному обработчику.

Демонстрация в тестовом коде (`Lab1.c`, секция 2):

```
IEcoCalculatorX -> IEcoCalculatorY: OK
IEcoCalculatorX -> IKD_Tree:        OK
IEcoCalculatorY -> IEcoCalculatorX: OK
IEcoCalculatorY -> IKD_Tree:        OK
```

Из любого интерфейса Lab1 можно перейти к любому другому через `QueryInterface`.

## 6. Результаты тестирования

Тестирование реализовано в функции `EcoMain` файла `Lab1.c` и разделено на четыре секции.

### Секция 1: Автономные компоненты A–E

Для каждого компонента проверяется:
- Успешное создание экземпляра.
- Корректность арифметических операций через основной интерфейс.
- Навигация к вторичному интерфейсу через `QueryInterface` (для C, D, E).
- Корректность операций через вторичный интерфейс.
- Корректное освобождение (все `Release` сбалансированы).

| Компонент | Основной интерфейс | Вторичный (через QI) | Механизм |
|---|---|---|---|
| A | IEcoCalculatorX: Addition, Subtraction | — | — |
| B | IEcoCalculatorX: Addition, Subtraction | — | NonDelegating |
| C | IEcoCalculatorX: Addition, Subtraction | IEcoCalculatorY: Multiplication, Division | Dual-interface |
| D | IEcoCalculatorY: Multiplication, Division | IEcoCalculatorX (via A): Addition, Subtraction | Включение |
| E | IEcoCalculatorY: Multiplication, Division | IEcoCalculatorX (via B): Addition, Subtraction | Агрегирование |

### Секция 2: Lab1 с включением

- Создание Lab1 с включёнными компонентами A и D.
- Получение `IEcoCalculatorX` и `IEcoCalculatorY` через `QueryInterface` на `IKD_Tree`.
- Проверка арифметических операций через оба калькуляторных интерфейса.
- Полная навигация между интерфейсами: ICalcX ↔ ICalcY ↔ IKD_Tree.
- Построение KD-дерева (`BuildTreeLongLong`) и поиск 4 ближайших соседей.

### Секция 3: Lab2 с агрегированием и включением

- Создание Lab2, которое внутри:
  - Агрегирует Lab1 для `IKD_Tree`.
  - Включает B для `IEcoCalculatorX`.
  - Включает E (агрегирующий B) для `IEcoCalculatorY`.
- Проверка `IEcoCalculatorX` (через содержащийся B): `Addition(50, 25) = 75`, `Subtraction(50, 25) = 25`.
- Проверка `IEcoCalculatorY` (через содержащийся E): `Multiplication(11, 11) = 121`, `Division(100, 7) = 14`.
- Построение и поиск в KD-дереве через агрегированный Lab1.

### Секция 4: Модульные тесты KD-дерева

12 тестов из Задания 1, проверяющих корректность KD-дерева для всех типов координат и при различных объёмах данных. Подробное описание тестов приведено в `report.md`.
