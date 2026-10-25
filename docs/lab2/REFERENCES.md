# Карта материалов InsideACOM для лабораторной 2

Подготовлено 2026-10-25. Целевой объём 9–10 баллов выбран пользователем.
Это результаты изучения источников и ориентиры для проектирования,
а не утверждённая архитектура реализации.

## Источники

- Задание: /home/mikhail/HSE/3_year/Arc_and_os/Materials/Задание_2.pdf, 4 страницы.
- Лекция: /home/mikhail/HSE/3_year/Arc_and_os/Materials/Presentations/Проектирование - Лекция 04.pdf.
- Учебный репозиторий: /home/mikhail/HSE/3_year/Arc_and_os/lab_2/Eco.Education.
- origin: git@github.com:peerf-eco/Eco.Education.git.
- Проверенный локальный commit: 7f1664b, ветка main.
- Корень курса в репозитории: Eco.Pro.Training/001.InsideACOM.
- Книга: Books/001.Inside ACOM - Pro Training Series Draft.pdf, 25 страниц.

Версия зафиксирована для воспроизводимости анализа; репозиторий не изменён.

## Подходящие уроки

Пути ниже отсчитываются от Eco.Pro.Training/001.InsideACOM/Lessons.

| Материал | Роль |
|---|---|
| Lesson01/A…E | Готовые демонстрации; не основной источник C-кода |
| Lesson02/Eco.CalculatorA | Обычный компонент с IEcoCalculatorX |
| Lesson03/Eco.CalculatorB | IEcoCalculatorX с поддержкой агрегирования внешним объектом |
| Lesson04/Eco.DemoCalculator1 | Клиент первого калькулятора |
| Lesson05/Eco.CalculatorC | Собственная реализация X и Y в одном объекте |
| Lesson06/Eco.CalculatorD | Реализует Y и повторно использует X через включение B или A |
| Lesson07/Eco.CalculatorE | Реализует Y и получает X через агрегирование B |
| Lesson08/Eco.DemoCalculator2 | Клиент второго калькулятора |

Книга, PDF-страница 7, описывает роли A–E. Для D в книге описано включение A;
в проверенном исходнике Lesson06 сначала запрашивается B, затем при неудаче A.
Это различие нужно учитывать при построении демонстраций всех компонентов.

## Публичные интерфейсы

IEcoCalculatorX:

- Addition: int16_t, int16_t → int32_t;
- Subtraction: int16_t, int16_t → int16_t.

IEcoCalculatorY:

- Multiplication: int16_t, int16_t → int32_t;
- Division: int16_t, int16_t → int16_t.

Сигнатуры взяты из SharedFiles Lesson02 и Lesson05.
Не переносить на эти интерфейсы числовые типы и статус/out-контракт интегратора.
Не изменять публичные IID и сигнатуры примеров ради унификации с интегрированием.

## Особенно полезные места кода

- Lesson03/Eco.CalculatorB/SourceFiles/CEcoCalculatorB.c:
  делегирующие и неделегирующие методы IEcoUnknown, настройка внешнего объекта.
- Lesson03/Eco.CalculatorB/SourceFiles/CEcoCalculatorBFactory.c:
  условие допуска агрегирования только при исходном запросе IID_IEcoUnknown.
- Lesson06/Eco.CalculatorD/SourceFiles/CEcoCalculatorD.c:
  initCEcoCalculatorD, запрос включаемого X без внешнего IEcoUnknown.
- Lesson07/Eco.CalculatorE/SourceFiles/CEcoCalculatorE.c:
  initCEcoCalculatorE, создание B с внешним IEcoUnknown и запросом IID_IEcoUnknown;
  перенаправление QueryInterface для X во внутренний объект.

Чтение реализаций необходимо для понимания шаблона.
Собственный внешний компонент должен взаимодействовать с калькуляторами
через SharedFiles, без включения чужих внутренних заголовков.

## База на момент первоначального анализа

Репозиторий первой работы:
/home/mikhail/HSE/3_year/Arc_and_os/lab_1/EcoLab_1/EcoIntegralSimpson.

- Ветка main, локальный commit 371aa7f.
- Анализ начат с чистого main. Впоследствии пользователь разрешил
  создание тега lab1-final и worktree lab_2/project в ветке lab2-aggregation.
- Текущее состояние подготовки — в PREPARATION.md.
- Фабрика при outer != NULL возвращает ERR_ECO_NOAGGREGATION.
- QueryInterface поддерживает IEcoAdvancedMath и IEcoUnknown.
- В установленном /home/mikhail/Dev/Eco.Framework заголовки
  IEcoCalculatorX/IEcoCalculatorY не обнаружены.
- 2026-10-25 повторно выполнен AssemblyFiles/Linux/gcc_v132/build.sh verify:
  exit 0, 135 проверок UnitTest PASS, 3 теста компаратора OK,
  30/30 сравнений с mpmath PASS, 3 собственных объекта без внешних вызовов libc.

Документация и промпты первой лабораторной находятся выше корня её Git-репозитория.
Поэтому в новом worktree созданы собственные AGENTS.md и документы docs/lab2;
общие положения лекций перенесены в docs/course, нужные промпты — в Eco.AI.Promts.

## Требования выбранного уровня

Из Задание_2.pdf, страницы 1–4:

1. Расширить ранее созданный компонент, используя включение/агрегирование
   калькуляторов со сложением, вычитанием, умножением и делением.
2. Продемонстрировать применение всех A–E.
3. В UnitTest показать переходы между всеми поддерживаемыми интерфейсами
   посредством QueryInterface.
4. Добавить нашему компоненту поддержку агрегирования внешним компонентом.
5. Реализовать новый Eco.Lab2 и показать применение механизмов к A–E и Eco.Lab1.
6. Предоставить код, сборку, UnitTest с примерами и полный отчёт с анализом и выводами.

Наша реализация EcoIntegralSimpson/IEcoAdvancedMath может рассматриваться
как роль Eco.Lab1/IEcoLab1 в схемах. Это предлагаемое сопоставление имён,
а не прямое уточнение преподавателя. Его следует записать в DESIGN перед кодом.

Требование использовать калькуляторы во второй работе заменяет прежнее решение
«только интегрирование» в пределах новой лабораторной. Старое решение не означает,
что для лабораторной 2 запрещены новые интерфейсы X/Y или новый внешний компонент.

В PDF отчёт указан в формате Word. Прежний пользовательский выбор LaTeX
подтверждён пользователем и учитывается как его выбор, а не формулировка условия.

## Проверка совместимости исходников

Предварительная строгая C89-компиляция не прошла: встречаются ERR_ECO_SUCCESES
и комментарии //; подробности — в compatibility-check.json и PREPARATION.md.
Это результат проверки неизменённых учебных исходников, а не готовые калькуляторы.
