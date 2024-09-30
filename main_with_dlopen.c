// main_with_dlopen.c
#include <stdio.h>
#include <dlfcn.h>

int main() {
    
    // 共有ライブラリをロード
#ifdef __aarch64__
    void *handle = dlopen("./libadd_aarch64.so", RTLD_LAZY);
#else
    void *handle = dlopen("./libadd.so", RTLD_LAZY);
#endif
    if (!handle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        return 1;
    }

    // 関数ポインタを取得
    int (*add)(int, int) = (int (*)(int, int)) dlsym(handle, "add");
    const char *error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "Failed to load function: %s\n", error);
        dlclose(handle);
        return 1;
    }

    // 関数を呼び出し
    int a = 5;
    int b = 3;
    int result = add(a, b);
    printf("main_with_dlopen.c: Result of %d + %d = %d\n", a, b, result);

    // ライブラリを閉じる
    dlclose(handle);

    return 0;
}