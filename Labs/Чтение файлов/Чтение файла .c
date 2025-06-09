#include <stdio.h>
#include <ctype.h>

int isvowel(int c);

int main() {
    char filename[256];
    FILE *F;
    int c;

    int total = 0, lines = 0, digits = 0, punct = 0;
    int spaces = 0, letters = 0, lowercase = 0, uppercase = 0;
    int vowels = 0, consonants = 0;

    // Ввод имени файла
    printf("Введите имя файла: ");
    scanf("%255s", filename);

    // Открытие файла
    F = fopen(filename, "r");
    if (F == NULL) {
        perror("Ошибка при открытии файла");
        return 1;
    }

    // Посимвольное чтение
    while ((c = fgetc(F)) != EOF) {
        total++;
        if (c == '\n') lines++;
        if (isdigit(c)) digits++;
        if (ispunct(c)) punct++;
        if (isspace(c)) spaces++;
        if (isalpha(c)) {
            letters++;
            if (islower(c)) lowercase++;
            if (isupper(c)) uppercase++;

            if (isvowel(c)) vowels++;
            else consonants++;
        }
    }

    fclose(F);

    // Вывод результатов
    printf("Общее количество символов: %d\n", total);
    printf("Количество строк: %d\n", lines);
    printf("Количество цифр: %d\n", digits);
    printf("Количество знаков препинания: %d\n", punct);
    printf("Количество пробельных символов: %d\n", spaces);
    printf("Количество букв: %d\n", letters);
    printf("  из них строчных: %d\n", lowercase);
    printf("  из них прописных: %d\n", uppercase);
    printf("Количество гласных: %d\n", vowels);
    printf("Количество согласных: %d\n", consonants);

    return 0;
}

// Проверка, является ли символ гласной буквой (латиница)
int isvowel(int c) {
    switch(tolower(c)) {
        case 'a': case 'e': case 'i': case 'o': case 'u': case 'y':
            return 1;
    }
    return 0;
}
