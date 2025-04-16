#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>

#define FILENAME "digital_books.txt"
#define MAX_LINE_LENGTH 300 // для хранения строки книги

// Структура книги
typedef struct {
    char title[100];
    char author[100];
    char genre[50];
    int year;
} Book;

// Основные функции
Book input_book();
void print_book(const Book *book);
int save_to_file(const char *book_str);
char* write_book(const Book *book);
char* read_book(FILE *file);
int match_query(const Book *book, const char *query);
int delete_book_by_title(const char *title);
Book* find_books_by_author(const char *author, int *count);

// Функции меню
void add_book();
void book_list();
void find_book();
void delete_book();
void search_by_author();

int main() {
    setlocale(LC_ALL, "RUS");
    int choice;
    while (1) {
        printf("\nМеню:\n");
        printf("1. Добавить книгу\n");
        printf("2. Просмотр всех книг\n");
        printf("3. Поиск книги по названию\n");
        printf("4. Поиск книг по автору\n");
        printf("5. Удалить книгу\n");
        printf("6. Выход\n");
        printf("Выбор: ");
        scanf("%d", &choice);
        getchar(); // удалить \n после scanf

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
                search_by_author();
                break;

            case 5:
                delete_book();
                break;

            case 6:
                return 0;

            default:
                printf("Неверный выбор. Попробуйте снова.\n");
        }
    }
}

// ввод данных книги
Book input_book() {
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

    return book;
}

// печать книги 
void print_book(const Book *book) {
    printf("%s - %s (%d) [%s]\n", book->title, book->author, book->year, book->genre);
}

// сохранение строки в файл
int save_to_file(const char *book_str) {
    FILE *file = fopen(FILENAME, "a");
    if (!file) return 0;
    fprintf(file, "%s\n", book_str);
    fclose(file);
    return 1;
}

// формирование строки для записи в файл
char* write_book(const Book *book) {
    char *book_str = malloc(MAX_LINE_LENGTH);
    snprintf(book_str, MAX_LINE_LENGTH, "%s;%s;%d;%s", 
             book->title, book->author, book->year, book->genre);
    return book_str;
}

// чтение строки из файла
char* read_book(FILE *file) {
    char *line = malloc(MAX_LINE_LENGTH);
    if (!fgets(line, MAX_LINE_LENGTH, file)) {
        free(line);
        return NULL;
    }
    line[strcspn(line, "\n")] = '\0'; // Удаляем \n
    return line;
}

// сравнение книги с запросом
int match_query(const Book *book, const char *query) {
    return strstr(book->title, query) || strstr(book->author, query);
}

// удаление книги по названию
int delete_book_by_title(const char *title) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return 0;

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        fclose(file);
        return 0;
    }

    char *line;
    int deleted = 0;

    while ((line = read_book(file)) != NULL) {
        Book book;
        sscanf(line, "%[^;];%[^;];%d;%[^\n]", 
               book.title, book.author, &book.year, book.genre);
        
        if (strcmp(book.title, title) != 0) {
            fprintf(temp, "%s\n", line);
        } else {
            deleted = 1;
        }
        free(line);
    }

    fclose(file);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);
    return deleted;
}

// поиск книг по автору
Book* find_books_by_author(const char *author, int *count) {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        *count = 0;
        return NULL;
    }

    Book *results = malloc(sizeof(Book) * 100);
    int found = 0;
    char *line;

    while ((line = read_book(file)) != NULL) {
        Book book;
        sscanf(line, "%[^;];%[^;];%d;%[^\n]", 
               book.title, book.author, &book.year, book.genre);
        
        if (strcmp(book.author, author) == 0) {
            results[found++] = book;
        }
        free(line);
    }

    fclose(file);
    *count = found;
    return results;
}

// добавление книги
void add_book() {
    Book book = input_book();
    printf("\nВы ввели:\n");
    print_book(&book);
    
    char *book_str = write_book(&book);
    if (save_to_file(book_str)) {
        printf("Книга добавлена успешно.\n");
    } else {
        printf("Ошибка сохранения книги.\n");
    }
    free(book_str);
}

// вывод списка книг
void book_list() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("Файл не найден.\n");
        return;
    }

    char *line;
    printf("\nСписок книг:\n");
    
    while ((line = read_book(file)) != NULL) {
        Book book;
        sscanf(line, "%[^;];%[^;];%d;%[^\n]", 
               book.title, book.author, &book.year, book.genre);
        print_book(&book);
        free(line);
    }

    fclose(file);
}

// поиск книги
void find_book() {
    char query[100];
    printf("Введите строку для поиска (название или автор): ");
    fgets(query, sizeof(query), stdin);
    strtok(query, "\n");

    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("Файл не найден.\n");
        return;
    }

    char *line;
    int found = 0;
    
    while ((line = read_book(file)) != NULL) {
        Book book;
        sscanf(line, "%[^;];%[^;];%d;%[^\n]", 
               book.title, book.author, &book.year, book.genre);
        
        if (match_query(&book, query)) {
            print_book(&book);
            found = 1;
        }
        free(line);
    }

    fclose(file);
    if (!found) {
        printf("Ничего не найдено по запросу.\n");
    }
}

// удаление книги
void delete_book() {
    char title[100];
    printf("Введите название книги для удаления: ");
    fgets(title, sizeof(title), stdin);
    strtok(title, "\n");

    if (delete_book_by_title(title)) {
        printf("Книга удалена.\n");
    } else {
        printf("Книга не найдена.\n");
    }
}

// поиск по автору
void search_by_author() {
    char author[100];
    printf("Введите имя автора: ");
    fgets(author, sizeof(author), stdin);
    strtok(author, "\n");

    int count = 0;
    Book *books = find_books_by_author(author, &count);

    if (count == 0) {
        printf("Книг автора \"%s\" не найдено.\n", author);
    } else {
        printf("Найдено %d книг:\n", count);
        for (int i = 0; i < count; i++) {
            print_book(&books[i]);
        }
    }

    free(books);
}
