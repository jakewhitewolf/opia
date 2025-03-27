#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Объявление константы для файла
#define FILENAME "digital_books.txt"

// Структура книги
typedef struct {
    char title[100];
    char author[100];
    char genre[50];
    int year;
} Book;

// Объявление функций
void add_book();
void book_list();
void find_book();
void delete_book();

int main() {
    int choice;
    do {
        // Вывод меню
        printf("Меню\n");
        printf("1. Добавить книгу в список\n");
        printf("2. Вывести список всех книг\n");
        printf("3. Найти книгу\n");
        printf("4. Удалить книгу\n");
        printf("5. Выйти\n");
        scanf("%d", &choice);
        getchar();
        // Выбор команды
        switch (choice) {
            case 1:
            add_book();
            break;

            case 2:
            book_list();
            break;

            case 3:
            find_book();
            break;

            case 4:
            delete_book();
            break;
        }
    }
    while (choice != 5);
    return 0;
}

// Функция для добавления книг
void add_book() {
    FILE *file = fopen(FILENAME, "a");

    if (file == NULL) {
        printf("Не удалось открыть файл для записи.\n");
        return;
    }

    Book book;
    printf("Введите название книги: ");
    fgets(book.title, sizeof(book.title), stdin);
    strtok(book.title, "\n");

    printf("Введите автора: ");
    fgets(book.author, sizeof(book.author), stdin);
    strtok(book.author, "\n");

    printf("Введите жанр: ");
    fgets(book.genre, sizeof(book.genre), stdin);
    strtok(book.genre, "\n");

    printf("Введите год издания: ");
    scanf("%d", &book.year);
    getchar();

    printf("\nВы ввели:\n");
    printf("Название книги: %s\n", book.title);
    printf("Автор книги: %s\n", book.author);
    printf("Год издания книги: %d\n", book.year);
    printf("Жанр книги: %s\n", book.genre);

    fprintf(file, "%s;%s;%d;%s\n", book.title, book.author, book.year, book.genre);
    fclose(file);
}

// Функция для вывода списка книг
void book_list() {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл для записи.\n");
        return;
    }
    Book book;
    printf("\nСписок книг\n");
    while (fscanf(file, "%[^;];%[^;];%d;%[^\n]\n", book.title, book.author, &book.year, book.genre) != EOF) {
        printf("%s - %s (%d) [%s]\n", book.title, book.author, book.year, book.genre);
    }
    fclose(file);
}

// Функция для поиска книг по автору или названию
void find_book() {
    Book book;
    char query[sizeof(book.title)];
    printf("Введите название или автора: ");
    fgets(query, sizeof(book.title), stdin);
    strtok(query, "\n");
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл для записи.\n");
        return;
    }
    int found = 0;
    while (fscanf(file, "%[^;];%[^;];%d;%[^\n]\n", book.title, book.author, &book.year, book.genre) != EOF) {
        if (strstr(book.title, query) || strstr(book.author, query)) {
            printf("%s - %s (%d) [%s]\n", book.title, book.author, book.year, book.genre);
            found = 1;
        }
    }
    fclose(file);
    if (!found) {
        printf("Книг не найдено.\n");
    }
}

// Функция для удаления книги из списка
void delete_book() {
    Book book;
    char title[sizeof(book.title)];
    printf("Введите название книги для удаления: ");
    fgets(title, sizeof(book.title), stdin);
    strtok(title, "\n");

    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл для записи.\n");
        return;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        printf("Ошибка открытия временного файла!\n");
        fclose(file);
        return;
    }

    int deleted = 0;
    while (fscanf(file, "%[^;];%[^;];%d;%[^\n]\n", book.title, book.author, &book.year, book.genre) != EOF) {
        if (strcmp(book.title, title) != 0) {
            fprintf(temp_file, "%s;%s;%d;%s\n", book.title, book.author, book.year, book.genre);
        } else {
            deleted = 1;
        }
    }
    fclose(file);
    fclose(temp_file);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (deleted) {
        printf("Книга удалена успешно.\n");
    } else {
        printf("Книга не найдена.\n");
    }
    
}
