#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <utility>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;


constexpr int N = 100;
using Array = array<int, N>;
using Clock = chrono::steady_clock;

double elapsed(Clock::time_point start) {
    return chrono::duration<double, nano>(Clock::now() - start).count();
}

void print(const Array& a, int count = N) {
    for (int i = 0; i < count; ++i)
        cout << a[i] << ' ';
    cout << '\n';
}

void bubbleSort(Array& a) {
    for (int end = N - 1; end > 0; --end)
        for (int i = 0; i < end; ++i)
            if (a[i] > a[i + 1])
                swap(a[i], a[i + 1]);
}

void insertionSort(Array& a) {
    for (int i = 1; i < N; ++i) {
        int value = a[i];
        int j = i;

        while (j > 0 && a[j - 1] > value) {
            a[j] = a[j - 1];
            --j;
        }

        a[j] = value;
    }
}

void selectionSort(Array& a) {
    for (int i = 0; i < N - 1; ++i) {
        int minIndex = i;

        for (int j = i + 1; j < N; ++j)
            if (a[j] < a[minIndex])
                minIndex = j;

        swap(a[i], a[minIndex]);
    }
}

void gnomeSort(Array& a) {
    int i = 1;

    while (i < N) {
        if (i == 0 || a[i - 1] <= a[i]) {
            ++i;
        } else {
            swap(a[i - 1], a[i]);
            --i;
        }
    }
}

void quickSort(Array& a, int left = 0, int right = N - 1) {
    if (left >= right)
        return;

    int pivot = a[left + (right - left) / 2];
    int i = left;
    int j = right;

    while (i <= j) {
        while (a[i] < pivot)
            ++i;
        while (a[j] > pivot)
            --j;

        if (i <= j) {
            swap(a[i], a[j]);
            ++i;
            --j;
        }
    }

    quickSort(a, left, j);
    quickSort(a, i, right);
}

void compareSorts(const Array& original) {
    const char* names[] = {
        "Пузырьковая", "Вставками", "Выбором", "Гномья", "Быстрая"
    };

    for (int method = 0; method < 5; ++method) {
        Array a = original;
        auto start = Clock::now();

        switch (method) {
            case 0: bubbleSort(a); break;
            case 1: insertionSort(a); break;
            case 2: selectionSort(a); break;
            case 3: gnomeSort(a); break;
            case 4: quickSort(a); break;
        }

        double time = elapsed(start);
        cout << names[method] << ": " << time << " нс\n";
        print(a);
    }
}

void showMinMax(const Array& original, const Array& sorted) {
    auto start = Clock::now();
    int minimum = original[0];
    int maximum = original[0];

    for (int x : original) {
        if (x < minimum)
            minimum = x;
        if (x > maximum)
            maximum = x;
    }

    double scanTime = elapsed(start);

    start = Clock::now();
    int first = sorted[0];
    int last = sorted[N - 1];
    double sortedTime = elapsed(start);

    cout << "Исходный: min = " << minimum << ", max = " << maximum
         << ", время " << scanTime << " нс\n";
    cout << "Отсортированный: min = " << first << ", max = " << last
         << ", время " << sortedTime << " нс\n";
}

int boundary(const Array& a, int x, bool strict) {
    int left = 0;
    int right = N;

    while (left < right) {
        int middle = left + (right - left) / 2;

        if (a[middle] < x || (strict && a[middle] == x))
            left = middle + 1;
        else
            right = middle;
    }

    return left;
}

void waitForEnter() {
    cout << "\nНажмите Enter, чтобы вернуться в меню..." << flush;
    cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    cin.get();
}

void showAverage(const Array& original, const Array& sorted) {
    double average = (static_cast<double>(sorted[0]) + sorted[N - 1]) / 2.0;
    int x = static_cast<int>(round(average));
    cout << "Округлённая полусумма: " << x << '\n';

    Array indices{};
    int count = 0;
    auto start = Clock::now();

    for (int i = 0; i < N; ++i) {
        if (original[i] == x) {
            indices[count] = i;
            ++count;
        }
    }

    double time = elapsed(start);
    cout << "Исходный: найдено " << count << ", время " << time << " нс. Индексы: ";
    print(indices, count);

    start = Clock::now();
    int first = boundary(sorted, x, false);
    int last = boundary(sorted, x, true);
    time = elapsed(start);

    cout << "Отсортированный: найдено " << last - first
         << ", время " << time << " нс. Индексы: ";

    for (int i = first; i < last; ++i)
        cout << i << ' ';

    cout << '\n';
}

void compareCounts(const Array& sorted, int a, int b) {
    auto start = Clock::now();
    int less = boundary(sorted, a, false);
    int greater = N - boundary(sorted, b, true);
    double binaryTime = elapsed(start);

    int linearLess = 0;
    int linearGreater = 0;
    start = Clock::now();

    for (int x : sorted) {
        if (x < a)
            ++linearLess;
        if (x > b)
            ++linearGreater;
    }

    double linearTime = elapsed(start);

    cout << "Границы: меньше a = " << less << ", больше b = " << greater
         << ", время " << binaryTime << " нс\n";
    cout << "Перебор: меньше a = " << linearLess << ", больше b = " << linearGreater
         << ", время " << linearTime << " нс\n";
}

int linearSearch(const Array& a, int x) {
    for (int i = 0; i < N; ++i)
        if (a[i] == x)
            return i;

    return -1;
}

int binarySearch(const Array& a, int x) {
    int i = boundary(a, x, false);

    if (i < N && a[i] == x)
        return i;

    return -1;
}

void compareSearches(const Array& sorted, int x) {
    constexpr int repeats = 10000;
    volatile int index = -1;

    auto start = Clock::now();
    for (int i = 0; i < repeats; ++i)
        index = linearSearch(sorted, x);

    double linearTime = elapsed(start) / repeats;
    cout << "Линейный: " << linearTime << " нс, индекс " << index << '\n';

    start = Clock::now();
    for (int i = 0; i < repeats; ++i)
        index = binarySearch(sorted, x);

    double binaryTime = elapsed(start) / repeats;
    cout << "Бинарный: " << binaryTime << " нс, индекс " << index << '\n';
}

void exchange(Array& a, int i, int j) {
    if (i < 0 || i >= N || j < 0 || j >= N) {
        cout << "Индексы должны быть от 0 до " << N - 1 << ".\n";
        return;
    }

    constexpr int repeats = 100001;
    volatile int& first = a[i];
    volatile int& second = a[j];
    auto start = Clock::now();

    for (int k = 0; k < repeats; ++k) {
        int saved = first;
        first = second;
        second = saved;
    }

    double time = elapsed(start) / repeats;
    cout << "Среднее время обмена: " << time << " нс\n";
    print(a);
}

void checkOrder(const Array& a) {
    for (int i = 1; i < N; ++i) {
        if (a[i] < a[i - 1]) {
            cout << "Первое нарушение на индексе " << i
                 << ": " << a[i - 1] << " > " << a[i] << '\n';
            return;
        }
    }

    cout << "Массив отсортирован по неубыванию.\n";
}

int main() {

    #ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    #endif

    Array original{};
    mt19937 generator{random_device{}()};
    uniform_int_distribution<int> randomNumber(-99, 99);

    for (int& x : original)
        x = randomNumber(generator);

    while (true) {
        cout << "\n1. Заполнить массив\n"
                "2. Сравнить сортировки\n"
                "3. Минимум и максимум\n"
                "4. Полусумма и совпадения\n"
                "5. Подсчитать числа меньше a и больше b\n"
                "6. Сравнить поиски\n"
                "7. Обменять элементы\n"
                "8. Проверить порядок (вариант 26)\n"
                "9. Показать массив\n"
                "0. Выход\n"
                "Выбор: ";

        int choice;
        if (!(cin >> choice) || choice == 0)
            break;

        Array sorted = original;
        if (choice >= 3 && choice <= 6)
            quickSort(sorted);

        int a, b;

        switch (choice) {
            case 1:
                cout << "1 — случайно, 2 — ввести вручную: ";
                if (!(cin >> a))
                    return 0;

                if (a == 1) {
                    for (int& x : original)
                        x = randomNumber(generator);
                } else if (a == 2) {
                    cout << "Введите " << N << " целых чисел: ";
                    for (int& x : original)
                        if (!(cin >> x))
                            return 0;
                } else {
                    cout << "Выберите 1 или 2.\n";
                    break;
                }

                print(original);
                break;

            case 2:
                compareSorts(original);
                break;

            case 3:
                showMinMax(original, sorted);
                break;

            case 4:
                showAverage(original, sorted);
                break;

            case 5:
                cout << "Введите a и b: ";
                if (!(cin >> a >> b))
                    return 0;
                compareCounts(sorted, a, b);
                break;

            case 6:
                cout << "Введите число для поиска. Индекс -1 означает, что числа нет: ";
                if (!(cin >> a))
                    return 0;
                compareSearches(sorted, a);
                break;

            case 7:
                cout << "Введите два индекса: ";
                if (!(cin >> a >> b))
                    return 0;
                exchange(original, a, b);
                break;

            case 8:
                checkOrder(original);
                break;

            case 9:
                print(original);
                break;

            default:
                cout << "Выберите пункт от 0 до 9.\n";
        }
    }

    return 0;
}
