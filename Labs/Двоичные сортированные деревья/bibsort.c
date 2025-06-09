#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>
#include <limits.h>

typedef struct {
    char* author;
    char* title;
    char* year;
    char* publisher;
} BibRecord;

typedef struct TreeNode {
    BibRecord record;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Определение кодировки файла
const char* detectFileEncoding(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) return "UNKNOWN";

    unsigned char bom[4] = {0};
    size_t read = fread(bom, 1, 3, file);
    
    // Проверка BOM
    if (read >= 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
        fclose(file);
        return "UTF-8";
    }
    if (read >= 2) {
        if (bom[0] == 0xFF && bom[1] == 0xFE) { fclose(file); return "UTF-16LE"; }
        if (bom[0] == 0xFE && bom[1] == 0xFF) { fclose(file); return "UTF-16BE"; }
    }
    
    fseek(file, 0, SEEK_SET);
    
    // Анализ содержимого
    int highBytes = 0, totalBytes = 0;
    unsigned char ch;
    while (fread(&ch, 1, 1, file) && totalBytes < 1000) {
        if (ch > 0x7F) highBytes++;
        totalBytes++;
    }
    
    if (highBytes == 0) { fclose(file); return "ASCII"; }
    
    fseek(file, 0, SEEK_SET);
    int cp1251Like = 0, koi8rLike = 0, utf8Like = 0;
    
    for (int i = 0; i < 100 && fread(&ch, 1, 1, file); i++) {
        if (ch >= 0xC0 && ch <= 0xDF) {
            if (fread(&ch, 1, 1, file) && ch >= 0x80 && ch <= 0xBF) {
                utf8Like += 2;
                continue;
            }
        }
        
        if (ch >= 0xC0 && ch <= 0xFF) {
            if (ch >= 0xE0 && ch <= 0xFF) koi8rLike++;
            else cp1251Like++;
        }
    }
    fclose(file);
    
    if (utf8Like > cp1251Like && utf8Like > koi8rLike) return "UTF-8";
    if (koi8rLike > cp1251Like * 2) return "KOI8-R";
    if (cp1251Like > koi8rLike * 2) return "WINDOWS-1251";
    
    return "UTF-8";
}

// Конвертация кодировки
char* convertEncoding(const char* input, size_t len, const char* from, const char* to, const char* filename) {
    if (strcmp(from, to) == 0 || strcmp(from, "ASCII") == 0) {
        return strndup(input, len);
    }

    iconv_t cd = iconv_open(to, from);
    if (cd == (iconv_t)-1) {
        fprintf(stderr, "ICONV_OPEN error: %s to %s (%s)\n", from, to, filename);
        return strndup(input, len);
    }

    size_t inbytes = len;
    size_t outbytes = len * 4;
    char* inbuf = (char*)input;
    char* outbuf = malloc(outbytes);
    char* outptr = outbuf;

    if (iconv(cd, &inbuf, &inbytes, &outptr, &outbytes) == (size_t)-1) {
        fprintf(stderr, "ICONV error in %s: ", filename);
        switch (errno) {
            case EILSEQ: fprintf(stderr, "Invalid sequence"); break;
            case E2BIG: fprintf(stderr, "Buffer too small"); break;
            case EINVAL: fprintf(stderr, "Incomplete sequence"); break;
            default: fprintf(stderr, "Error %d", errno);
        }
        fprintf(stderr, " (%s → %s)\n", from, to);
        
        iconv_close(cd);
        free(outbuf);
        return strndup(input, len);
    }

    *outptr = '\0';
    char* result = strdup(outbuf);
    iconv_close(cd);
    free(outbuf);
    return result;
}

// Сравнение строк с поддержкой русского
int russian_strncmp(const char* s1, const char* s2, size_t n) {
    for (size_t i = 0; i < n && *s1 && *s2; ++i, ++s1, ++s2) {
        unsigned char c1 = *s1;
        unsigned char c2 = *s2;

        if (c1 >= 0xC0 && c1 <= 0xDF) c1 += 0x20;
        if (c2 >= 0xC0 && c2 <= 0xDF) c2 += 0x20;
        
        if (c1 == 0xB8) c1 = 0xB5;
        if (c1 == 0xA8) c1 = 0xA5;
        if (c2 == 0xB8) c2 = 0xB5;
        if (c2 == 0xA8) c2 = 0xA5;

        if (c1 != c2) return c1 - c2;
    }
    return 0;
}

// Вставка в дерево
TreeNode* insert(TreeNode* root, BibRecord record) {
    if (!root) {
        TreeNode* node = malloc(sizeof(TreeNode));
        node->record = record;
        node->left = node->right = NULL;
        return node;
    }

    int cmp = russian_strncmp(record.author, root->record.author, 5);
    if (cmp == 0) {
        cmp = russian_strncmp(record.title, root->record.title, 5);
    }

    if (cmp < 0) {
        root->left = insert(root->left, record);
    } else if (cmp > 0) {
        root->right = insert(root->right, record);
    }

    return root;
}

// Парсинг .bib файла
BibRecord parseBibFile(const char* filename) {
    BibRecord record = {0};
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Cannot open %s\n", filename);
        return record;
    }

    const char* encoding = detectFileEncoding(filename);
    printf("Processing: %s\n", filename);

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char* braceOpen = strchr(line, '{');
        char* braceClose = strrchr(line, '}');
        char* equalsPos = strchr(line, '=');
        
        if (!braceOpen || !braceClose || !equalsPos) continue;

        *equalsPos = '\0';
        char* field = line;
        while (*field && isspace(*field)) field++;
        
        char* value = braceOpen + 1;
        *braceClose = '\0';

        if (strstr(field, "author")) {
            record.author = convertEncoding(value, braceClose - value, encoding, "UTF-8", filename);
        } else if (strstr(field, "title")) {
            record.title = convertEncoding(value, braceClose - value, encoding, "UTF-8", filename);
        } else if (strstr(field, "year")) {
            record.year = strndup(value, braceClose - value);
        } else if (strstr(field, "publisher")) {
            record.publisher = convertEncoding(value, braceClose - value, encoding, "UTF-8", filename);
        }
    }
    fclose(file);
    return record;
}

// Получение списка .bib файлов
char** getBibFiles(const char* dir, int* count) {
    DIR* dp = opendir(dir);
    if (!dp) {
        fprintf(stderr, "Cannot open directory %s\n", dir);
        *count = 0;
        return NULL;
    }

    char** files = NULL;
    int capacity = 10;
    files = malloc(capacity * sizeof(char*));
    *count = 0;

    struct dirent* entry;
    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {
            const char* ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".bib") == 0) {
                if (*count >= capacity) {
                    capacity *= 2;
                    files = realloc(files, capacity * sizeof(char*));
                }
                
                files[*count] = malloc(strlen(dir) + strlen(entry->d_name) + 2);
                sprintf(files[*count], "%s/%s", dir, entry->d_name);
                (*count)++;
            }
        }
    }
    closedir(dp);
    return files;
}

// Запись отсортированных данных
void writeSorted(TreeNode* root, FILE* out) {
    if (!root) return;
    writeSorted(root->left, out);
    
    fprintf(out, "Author: %s\nTitle: %s\nYear: %s\nPublisher: %s\n\n",
            root->record.author,
            root->record.title,
            root->record.year,
            root->record.publisher);
    
    writeSorted(root->right, out);
}

// Освобождение памяти дерева
void freeTree(TreeNode* root) {
    if (!root) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root->record.author);
    free(root->record.title);
    free(root->record.year);
    free(root->record.publisher);
    free(root);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: %s <output> <dir1> [dir2 ...]\n", argv[0]);
        return 1;
    }

    TreeNode* root = NULL;
    
    // Обработка всех директорий
    for (int i = 2; i < argc; i++) {
        int count = 0;
        char** files = getBibFiles(argv[i], &count);
        
        for (int j = 0; j < count; j++) {
            BibRecord record = parseBibFile(files[j]);
            if (record.author) {
                root = insert(root, record);
            }
            free(files[j]);
        }
        free(files);
    }

    // Запись результата
    FILE* out = fopen(argv[1], "w");
    if (!out) {
        perror("Cannot open output file");
        return 1;
    }
    
    writeSorted(root, out);
    fclose(out);
    freeTree(root);

    printf("Done. Results saved to %s\n", argv[1]);
    return 0;
}
