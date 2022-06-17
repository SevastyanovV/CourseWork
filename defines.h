#ifndef defines_h
#define defines_h

//__attribute__((always_inline)) для unix-систем, __forceinline для windows
#ifdef _WIN32
    #define INLINE __forceinline
#else 
    #define INLINE __attribute__((always_inline))
#endif

#define SWAP(a, b, width)                                                      \
    do {                                                                       \
        char *_a = (char *) a, *_b = (char *) b, _tmp;                         \
        for (size_t _k = 0; _k < width; ++_k, ++_a, ++_b) {                    \
            _tmp = *_a;                                                        \
            *_a = *_b;                                                         \
            *_b = _tmp;                                                        \
        }                                                                      \
    } while (0)

// Копирует байты справа налево, что позволяет сдвигать элементы массива вправо,
// однако медленнее memcpy / memmove
#define ASSIGN(a, b, width) {                                                  \
    char *_a = (char *) a + width - 1, *_b = (char *) b + width - 1;           \
    for (size_t _k = 0; _k < width; ++_k, --_a, --_b)                          \
        *_a = *_b;                                                             \
}

typedef struct {
    char *L;
    char *R;
} stack_node;

#ifndef CHAR_BIT
    #define CHAR_BIT 8
#endif

#define STACK_SIZE (CHAR_BIT * sizeof(size_t)) // size_t обеспечит в массиве
    // 2^(кол-во бит в size_t) элементов (однако в зависимости от разрядности
    // системы sizeof(size_t) различен), в стеке же нам достаточно хранить
    // (кол-во бит в size_t) = CHAR_BIT * sizeof(size_t) элементов, чтобы, при
    // условиях сохранения только большего из двух подмассивов и несохранения
    // посторонних данных, развернуть рекурсию деления массива на подмассивы
#define PUSH(_L, _R) (top->L = _L, top->R = _R, ++top)
#define	POP(_L, _R)	 (--top, _L = top->L, _R = top->R)
#define	STACK_NOT_EMPTY	(stack < top)

#endif