#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define FILENAME "digital_books.txt"
#define MAX_LINE_LENGTH 300

typedef struct {
    char title[100];
    char author[100];
    char genre[50];
    int year;
    float rating;
} Book;

// Работа с файлами
Book* load_from_file();
int save_to_file(Book *books, int count);

// Формирование строки для вывода
char* output_book(const Book* book);

// Добавление книги
int input_book(Book *book);
Book* add_book(Book* books, int* count, Book new_book);

// Вывод списка книг
char* get_book_list(Book* books, int count);

// Поиск книги по названию или автору
Book* search_books(Book* books, int count, const char* input, int* found);

// Удаление книги
int delete_book(Book* books, int* count, const char* title);

int main() {
    setlocale(LC_ALL, "RUS");
    int count = 0;
    Book* library = load_from_file(&count);
    int choice;
    char input[100];
    while (1) {
        printf("\nМеню:\n");
        printf("1. Добавить книгу\n");
        printf("2. Просмотр всех книг\n");
        printf("3. Поиск книги по названию или автору\n");
        printf("4. Удалить книгу\n");
        printf("5. Сохранить изменения\n");
        printf("6. Выход\n");
        printf("Выбор: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: {
                Book new_book;
                if (input_book(&new_book)) {
                    library = add_book(library, &count, new_book);
                    printf("Книга успешно добавлена\n");
                } else {
                    printf("Ошибка при добавлении книги\n");
                }
                break;
            }
            case 2: {
                char* list = get_book_list(library, count);
                printf("Список книг:\n");
                printf("%s", list);
                free(list);
                break;
            }
            case 3: {
                printf("Введите запрос: ");
                fgets(input, sizeof(input), stdin); 
                strtok(input, "\n");
                int found = 0;
                Book* result = search_books(library, count, input, &found);
                if (found == 0) printf("Ничего не найдено\n");
                else {
                    printf("Найденные книги:\n");
                    for (int i = 0; i < found; i++) {
                        char* book_output = output_book(&result[i]);               
                        printf("%s", book_output);
                        free(book_output);
                    }
                }
                free(result);
                break;
            }
            case 4: {
                printf("Введите название книги для удаления: ");
                fgets(input, sizeof(input), stdin); 
                strtok(input, "\n");
                if (delete_book(library, &count, input))
                    printf("Книга удалена\n");
                else
                    printf("Книга не найдена\n");
                break;
            }
            case 5: {
                if (save_to_file(library, count))
                    printf("Файл успешно сохранён\n");
                else
                    printf("Ошибка при сохранении файла\n");
                break;
            }
            case 6: {
                free(library);
                return 0;
            }   
        }
    }
}

// Загрузка из файла
Book* load_from_file(int *count) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        *count = 0;
        return NULL;
    }

    Book* books = NULL;
    *count = 0;
    
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        books = realloc(books, (*count + 1) * sizeof(Book));
        sscanf(line, "%99[^;];%99[^;];%d;%49[^;];%f", books[*count].title, books[*count].author, &books[*count].year, books[*count].genre, &books[*count].rating);
        (*count)++;
    }

    fclose(file);
    return books;
}

// Сохранение в файл
int save_to_file(Book *books, int count) {
    FILE *file = fopen(FILENAME, "w");
    if (!file) {
        printf("Ошибка открытия файла для сохранения.\n");
        return 0;
    }

    for (int i = 0; i < count; i++) {
        fprintf(file, "%s;%s;%d;%s;%.1f\n", books[i].title, books[i].author, books[i].year, books[i].genre, books[i].rating);
    }

    fclose(file);
    return 1;
}

// Ввод данных книги для добавления
int input_book(Book *book) {
    printf("Введите название: ");
    if (!fgets(book->title, sizeof(book->title), stdin)) {
        return 0;
    }
    strtok(book->title, "\n");

    // Ввод автора
    printf("Введите автора: ");
    if (!fgets(book->author, sizeof(book->author), stdin)) {
        return 0;
    }
    strtok(book->author, "\n");

    printf("Введите жанр: ");
    if (!fgets(book->genre, sizeof(book->genre), stdin)) {
        return 0;
    }
    strtok(book->genre, "\n");

    printf("Введите год: ");
    if (scanf("%d", &book->year) != 1) {
        return 0;
    }
    getchar();

    printf("Введите рейтинг (0,0 - 5,0): ");
    if (scanf("%f", &book->rating) != 1) {
        return 0;
    }
    getchar();

    return 1;   
}

// Добавление книги
Book* add_book(Book* books, int* count, Book new_book) {
    books = realloc(books, (*count + 1) * sizeof(Book));
    books[*count] = new_book;
    (*count)++;
    return books;
}

// Формирование строки для вывода
char* output_book(const Book* book) {
    char* output = malloc(MAX_LINE_LENGTH);

    if (output != NULL) {
        snprintf(output, MAX_LINE_LENGTH, "%s - %s (%d) [%s] Рейтинг: %.1f\n", book->title, book->author, book->year, book->genre, book->rating);
    }

    return output;
}

// Возвращение списка книг
char* get_book_list(Book* books, int count) {
    char *list = malloc(count * MAX_LINE_LENGTH);
    list[0] = '\0';

    for (int i = 0; i < count; i++) {
        char* book_output = output_book(&books[i]);
        strcat(list, book_output);
        free(book_output);
    }

    return list;
}

// Поиск книг по названию или автору
Book* search_books(Book* books, int count, const char* input, int* found) {
    Book* results = NULL;
    *found = 0;

    for (int i = 0; i < count; i++) {
        if (strstr(books[i].title, input) || strstr(books[i].author, input)) {
            results = realloc(results, (*found + 1) * sizeof(Book));
            results[*found] = books[i];
            (*found)++;
        }
    }

    return results;
}

// Удаление книги
int delete_book(Book* books, int* count, const char* title) {
    for (int i = 0; i < *count; i++) {

        if (strcmp(books[i].title, title) == 0) {

            for (int j = i; j < *count - 1; j++) {
                books[j] = books[j + 1];
            }

            (*count)--;
            return 1;
        }
    }
    return 0;
}
