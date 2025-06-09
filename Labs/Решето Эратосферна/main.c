#include <stdio.h>
#include <stdbool.h>
#include <locale.h>


int main() {
    int n; printf("Vvedite vernuy granicy: "); scanf("%d", &n);

    setlocale(LC_ALL, "Rus");

    bool is_prime[n + 1]; for (int i = 2; i <= n; i++) is_prime[i] = true;
    for (int p = 2; p * p <= n; p++) if (is_prime[p]) for (int j = p * p; j <= n; j += p) is_prime[j] = false;
    printf("Prime numbers: "); for (int i = 2; i <= n; i++) if (is_prime[i]) printf("%d ", i);
    printf("\n"); return 0;
}
