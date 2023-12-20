#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <locale.h>

// Структура для представления графа
struct Graph {
    int V, E;
    int** Matrix;
};

//Список всех функций
void saveResultsToFile(struct Graph* graph, int* dist, int* path, int src);
void saveMatrixToFile(struct Graph* graph);
void FordBellman(struct Graph* graph, int src);
void manualInput(struct Graph* graph);
void readGraphFromFile(struct Graph* graph);
void generateRandomGraph(struct Graph* graph);

// Функция для сохранения результатов в файл
void saveResultsToFile(struct Graph* graph, int* dist, int* path, int src) {

    FILE* file;
    errno_t err;

    if ((err = fopen_s(&file, "output.txt", "a")) != 0) {
        perror("Ошибка при открытии файла");
        exit(0);
    }

    fprintf(file, "Кратчайшие пути от вершины %d:\n", src + 1);
    for (int i = 0; i < graph->V; i++) {
        if (dist[i] == INT_MAX)
            fprintf(file, "До вершины %d: INFINITY\n", i + 1);
        else {
            fprintf(file, "До вершины %d: %d (%d", i + 1, dist[i], i + 1);
            int vertex = i;
            while (path[vertex] != -1) {
                fprintf(file, " <- %d", path[vertex] + 1);
                vertex = path[vertex];
            }
            fprintf(file, ")\n");
        }
    }
    fprintf(file, "\n");
    fclose(file);
}

// Функция для сохранения матрицы в файл
void saveMatrixToFile(struct Graph* graph) {

    FILE* file;
    errno_t err;

    if ((err = fopen_s(&file, "matrix2.txt", "w")) != 0) {
        perror("Ошибка при открытии файла");
        exit(0);
    }

    for (int i = 0; i < graph->V; i++) {
        for (int j = 0; j < graph->V; j++) {
            fprintf(file, "%d ", graph->Matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

// Функция для выполнения алгоритма Форда-Беллмана
void FordBellman(struct Graph* graph, int src) {

    src -= 1;
    int V = graph->V;
    int* dist = (int*)malloc(V * sizeof(int));
    int* path = (int*)malloc(V * sizeof(int));

    // Инициализация расстояний и пути
    for (int i = 0; i < V; i++) {
        dist[i] = INT_MAX;
        path[i] = -1;
    }
    dist[src] = 0;

    // Релаксация ребер
    for (int i = 1; i < V; i++) {
        for (int j = 0; j < V; j++) {
            for (int k = 0; k < V; k++) {
                if (graph->Matrix[j][k] != 0 && dist[j] != INT_MAX && dist[j] + graph->Matrix[j][k] < dist[k]) {
                    dist[k] = dist[j] + graph->Matrix[j][k];
                    path[k] = j;
                }
            }
        }
    }

    // Проверка наличия отрицательных циклов
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (graph->Matrix[i][j] != 0 && dist[i] != INT_MAX && dist[i] + graph->Matrix[i][j] < dist[j]) {
                printf("В графе обнаружен отрицательный цикл.\n");
                free(dist);
                free(path);
                return;

            }
        }
    }

    // Вывод результатов
    printf("\nКратчайшие пути от вершины %d:\n", src + 1);
    for (int i = 0; i < V; i++) {
        if (dist[i] == INT_MAX)
            printf("До вершины %d: INFINITY\n", i + 1);
        else {
            printf("До вершины %d: %d (%d", i + 1, dist[i], i + 1);
            int vertex = i;
            while (path[vertex] != -1) {
                printf(" <- %d", path[vertex] + 1);
                vertex = path[vertex];
            }
            printf(")\n");
        }
    }

    // Запрос пользователя о сохранении результатов
    int choice;
    printf("Хотите ли сохранить результат в файл? (1, если да, и 0, если нет): ");
    scanf_s(" %d", &choice);

    switch (choice) {
    case 0:
        printf("Результат не сохранён.\n");
        break;
    case 1:
        saveResultsToFile(graph, dist, path, src);
        printf("Результат сохранён в файл 'output.txt'.\n");
        break;
    default:
        printf("Недопустимое значение. Попробуйте ещё раз.\n");
    }

    free(dist);
    free(path);
}

// Функция для ручного ввода матрицы смежности
void manualInput(struct Graph* graph) {

    int choice;
    printf("\nВыберете какой граф будете вводить\n");
    printf("1 - неориентированный\n");
    printf("0 - ориентированный\n");
    printf("Выбор: ");
    scanf_s("%d", &choice);
    switch (choice) {
    case 1:
        printf("Вводите положительные веса");
        printf("\nВведите матрицу (значения - через пробел, 0 - ребра нет):\n");

        for (int i = 0; i < graph->V; i++) {
            for (int j = 0; j < graph->V; j++) {
                int value;
                scanf_s("%d", &value);
                while (value < 0){
                    printf("Введено отрицательное значение. Повторите ввод:\n");
                    scanf_s("%d", &value);
                }
                graph->Matrix[i][j] = value;
            }
        }

        for (int i = 0; i < graph->V; i++) {
            for (int j = i + 1; j < graph->V; j++) {
                if (graph->Matrix[i][j] != graph->Matrix[j][i]) {
                    printf("Граф является ориентированным. Повторите ввод\n");
                    manualInput(graph);
                }
            }
        }
        break;
    case 0:
        printf("Введите матрицу (значения - через пробел, 0 - ребра нет):\n");

        for (int i = 0; i < graph->V; i++) {
            for (int j = 0; j < graph->V; j++) {
                scanf_s("%d", &(graph->Matrix[i][j]));
            }
        }
        for (int i = 0; i < graph->V; i++) {
            for (int j = i + 1; j < graph->V; j++) {
                if (graph->Matrix[i][j] == graph->Matrix[j][i] && graph->Matrix[i][j]!=0) {
                    printf("Граф является неориентированным. Повторите ввод\n");
                    manualInput(graph);
                }
            }
        }       
        break;
    default:
        printf("Недопустимое значение. Попробуйте ещё раз.\n");
 
    }
}


void readGraphFromFile(struct Graph* graph) {

    FILE* file;
    errno_t err;

    if ((err = fopen_s(&file, "matrix.txt", "r")) != 0) {
        perror("\nОшибка при открытии файла. Проверьте существует ли файл.\n Для правильного заполнения вводите значения через пробел (0 - ребра нет), количество вершин = количеству вершин");
        exit(0);
    }

    // Подсчет количества вершин (по числу строк в файле)
    int vertexCount = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            vertexCount++;
        }
    }
    vertexCount++;

    // Переходим к началу файла и считаем количество рёбер (ненулевых элементов в матрице)
    fseek(file, 0, SEEK_SET);
    int edgeCount = 0;
    int value;

    // Считываем матрицу смежности
    graph->V = vertexCount;
    graph->E = 0;
    graph->Matrix = (int**)malloc(graph->V * sizeof(int*));

    for (int i = 0; i < graph->V; i++) {
        graph->Matrix[i] = (int*)malloc(graph->V * sizeof(int));
        for (int j = 0; j < graph->V; j++) {
            if (fscanf_s(file, "%d", &value) == 1) {
                graph->Matrix[i][j] = value;
                if (value != 0) {
                    graph->E++;
                }
            }
            // Пропускаем пробельные символы
            while ((c = fgetc(file)) != EOF && (c == ' ' || c == '\t')) {}
            // Возвращаем символ, если это не конец файла
            if (c != EOF) {
                ungetc(c, file);
            }
        }
    }
    // Отладочный вывод матрицы
    printf("\nМатрица:\n");
    for (int i = 0; i < graph->V; i++) {
        for (int j = 0; j < graph->V; j++) {
            printf("%4d ", graph->Matrix[i][j]);
        }
        printf("\n");
    }
    fclose(file);
}

void readGraphFromFileTWO(struct Graph* graph) {

    FILE* file;
    errno_t err;

    if ((err = fopen_s(&file, "matrix2.txt", "r")) != 0) {
        perror("Ошибка при открытии файла. Проверьте существует ли файл.\n Для правильного заполнения вводите значения через пробел (0 - ребра нет), количество вершин = количеству вершин\n");
        exit(0);
    }
    fseek(file, 0, SEEK_SET);
    long filesize = ftell(file);
    if (filesize == EOF) {
        printf("Ошибка. Файл пуст. Необходимо сохранить сгенерированную матрицу.\n");
        exit(0);
    }

    // Подсчет количества вершин (по числу строк в файле)
    int vertexCount = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            vertexCount++;
        }
    }
    // Переходим к началу файла и считаем количество рёбер (ненулевых элементов в матрице)
    fseek(file, 0, SEEK_SET);
    int edgeCount = 0;
    int value;

    // Считываем матрицу смежности
    graph->V = vertexCount;
    graph->E = 0;
    graph->Matrix = (int**)malloc(graph->V * sizeof(int*));

    for (int i = 0; i < graph->V; i++) {
        graph->Matrix[i] = (int*)malloc(graph->V * sizeof(int));
        for (int j = 0; j < graph->V; j++) {
            if (fscanf_s(file, "%d", &value) == 1) {
                graph->Matrix[i][j] = value;
                if (value != 0) {
                    graph->E++;
                }
            }
            // Пропускаем пробельные символы
            while ((c = fgetc(file)) != EOF && (c == ' ' || c == '\t')) {}
            // Возвращаем символ, если это не конец файла
            if (c != EOF) {
                ungetc(c, file);
            }
        }
    }
    // Отладочный вывод матрицы
    printf("\nМатрица:\n");
    for (int i = 0; i < graph->V; i++) {
        for (int j = 0; j < graph->V; j++) {
            printf("%4d ", graph->Matrix[i][j]);
        }
        printf("\n");
    }
    fclose(file);
}

// Функция для генерации случайного графа
void generateRandomGraph(struct Graph* graph) {
    int choice;
    int value;
    int edgeCount = 0;
    printf("\nВыберете какой граф для генерации\n");
    printf("1 - неориентированный\n");
    printf("0 - ориентированный\n");
    printf("Выбор: ");
    scanf_s("%d", &choice);
    switch (choice) {
    case 1:
        printf("\nГраф неориентированный\n ");
        // Запрос пользователя о количестве вершин
        printf("\nВедите количество вершин: ");
        scanf_s("%d", &graph->V);

        // Запрос пользователя о количестве рёбер
        printf("\nВедите количество рёбер: ");
        scanf_s("%d", &value);
        while (value > graph->V * (graph->V - 1) / 2) {
            printf("Количество ребер превышает максимально возможное для данного числа вершин в графе.\nПопробуйте ещё раз:");
            scanf_s("%d", &value);
        }
        graph->E = value;

        
        // Инициализация генератора случайных чисел
        srand(time(NULL));

        // Выделение памяти для матрицы смежности
        graph->Matrix = (int**)malloc(graph->V * sizeof(int*));
        // Инициализация матрицы нулями
        for (int i = 0; i < graph->V; i++) {
            graph->Matrix[i] = (int*)malloc(graph->V * sizeof(int));
            for (int j = 0; j < graph->V; j++) {
                graph->Matrix[i][j] = 0;
            }
        }

        // Заполнение рёбер
        while (edgeCount < graph->E) {
            int i = rand() % graph->V;
            int j = rand() % graph->V;
            if (i != j && graph->Matrix[i][j] == 0) {
                graph->Matrix[i][j] = rand() % 10;
                graph->Matrix[j][i] = graph->Matrix[i][j];
                edgeCount++;
            }
        }

        // Отладочный вывод матрицы
        printf("\nМатрица:\n");
        for (int i = 0; i < graph->V; i++) {
            for (int j = 0; j < graph->V; j++) {
                printf("%4d ", graph->Matrix[i][j]);
            }
            printf("\n");
        }
        break;
    case 0:
        printf("\nГраф ориентированный\n ");
        // Запрос пользователя о количестве вершин
        printf("Ведите количество вершин: ");
        scanf_s("%d", &graph->V);

        // Запрос пользователя о количестве рёбер
        printf("\nВедите количество рёбер: ");
        scanf_s("%d", &value);
        while (value > graph->V * (graph->V - 1) / 2) {
            printf("Количество ребер превышает максимально возможное для данного числа вершин в графе.\nПопробуйте ещё раз:");
            scanf_s("%d", &value);
        }
        graph->E = value;

        // Инициализация генератора случайных чисел
        srand(time(NULL));

        // Выделение памяти для матрицы смежности
        graph->Matrix = (int**)malloc(graph->V * sizeof(int*));
        // Инициализация матрицы нулями
        for (int i = 0; i < graph->V; i++) {
            graph->Matrix[i] = (int*)malloc(graph->V * sizeof(int));
            for (int j = 0; j < graph->V; j++) {
                graph->Matrix[i][j] = 0;
            }
        }

        // Заполнение рёбер
        while (edgeCount < graph->E) {
            int i = rand() % graph->V;
            int j = rand() % graph->V;
            if (i != j && graph->Matrix[i][j] == 0) {
                graph->Matrix[i][j] = rand() % 21 - 10;
                edgeCount++;
            }
        }

        // Отладочный вывод матрицы
        printf("\nМатрица:\n");
        for (int i = 0; i < graph->V; i++) {
            for (int j = 0; j < graph->V; j++) {
                printf("%4d ", graph->Matrix[i][j]);
            }
            printf("\n");
        }
        break;

    default:
        printf("Недопустимое значение. Попробуйте ещё раз.\n");

    }
}

// Главная функция
int main() {
    setlocale(LC_ALL, "Rus");

    struct Graph graph;
    int choice, src;

    printf("\t\t\t\t Курсовая работа студента группы 22ВВС1  Макеевой Д.Д. \n ");
    printf("\t\t\t\t           Алгоритм Форда-Беллмана            \n ");


    while (1) {
        printf("\n");
        printf("-----------------------------------------------------");
        printf("\nМеню:\n");
        printf("1. Взять данные из файла для реализации алгоритма\n");
        printf("2. Ввести вручную данные для реализации алгоритма\n");
        printf("3. Сгенерировать данные для реализации алгоритма\n");
        printf("4. Выйти из программы\n\n");
        printf("Выбор: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            printf("\nПредупреждение! Если в файл введён неориентированный граф, то проверьте, чтобы в нём не было отрицательных весов. \n");
            printf("В противном случае всегда будет отрицательный цикл. \n");

            printf("\nКаким файлом воспользоваться?\n ");
            printf("1 - если данные в файл введены вручную (matrix.txt)\n ");
            printf("0 - если данные в файл сохранены при генерации (matrix2.txt)\n ");
            printf("Выбор: ");
            scanf_s("%d", &choice);
            switch (choice) {
            case 1:
                readGraphFromFile(&graph);
                break;
            case 0:
                readGraphFromFileTWO(&graph);
                break;
            default:
                printf("Недопустимое значение. Попробуйте ещё раз.\n");
            }

            printf("Введите исходную вершину (от 1 до %d): ", graph.V);
            scanf_s("%d", &src);
            if (src < 1 || src > graph.V) {
                printf("Недопустимая исходная вершина\n");
                break;
            }

            FordBellman(&graph, src);

            for (int i = 0; i < graph.V; i++)
                free(graph.Matrix[i]);
            free(graph.Matrix);
            break;

        case 2:
            printf("Введите количество вершин: ");
            scanf_s("%d", &graph.V);

            graph.E = graph.V * graph.V;
            graph.Matrix = (int**)malloc(graph.V * sizeof(int*));
            for (int i = 0; i < graph.V; i++)
                graph.Matrix[i] = (int*)malloc(graph.V * sizeof(int));

            manualInput(&graph);

            printf("Введите исходную вершину (от 1 до %d): ", graph.V);
            scanf_s("%d", &src);
            if (src < 1 || src > graph.V) {
                printf("Недопустимая исходная вершина\n");
                break;
            }

            FordBellman(&graph, src);

            for (int i = 0; i < graph.V; i++)
            free(graph.Matrix[i]);
            free(graph.Matrix);
            break;

        case 3:

            generateRandomGraph(&graph);

            printf("Сохранить ли матрицу в файл? (1 - да/ 0 - продолжить без сохранения): ");
            scanf_s("%d", &choice);
            switch (choice) {
            case 1:
                saveMatrixToFile(&graph);
                printf("Матрица сохранена в файл 'matrix2.txt'.\n");
                break;
            case 0:
                break;
            default:
                printf("Недопустимое значение. Попробуйте ещё раз.\n");
            }

            printf("\nВведите исходную вершину (от 1 до %d): ", graph.V);
            scanf_s("%d", &src);
            if (src < 1 || src > graph.V) {
                printf("Недопустимая исходная вершина\n");
                break;
            }
            
            FordBellman(&graph, src);

            for (int i = 0; i < graph.V; i++)
            free(graph.Matrix[i]);
            free(graph.Matrix);
            break;

        case 4:
            printf("\nВыход из программы.\n");
            exit(0);

        default:
            printf("Недопустимое значение. Попробуйте ещё раз.\n");
        }
    }

    return 0;
}
