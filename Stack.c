#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEBUG

#ifdef DEBUG
    #define ONDEB(...) __VA_ARGS__
#else
    #define ONDEB(...)
#endif

/*#define Print_error_message(message, object)                                         \
    fprintf(stderr, "%s(): %s - %s, line %d\n", __func__, message, object, __LINE__);*/

enum {STACK_NORMAL = 0b000, STACK_DATA_ERROR = 0b100,
      STACK_CAPACITY_ERROR = 0b010, STACK_SIZE_ERROR = 0b001};
/*typedef enum {PUSH_NORMAL = 0} Push_err_t; // return of StackPush()
typedef enum {POP_NORMAL = 0}  Pop_err_t;  // return of StackPop()*/

#define DEFAULT_CAPACITY 10
#define POISON 1666667   // REVIEW add poison to descriptor and pop and init

#define STACK_TYPE double

#define WRAP_IN_STR(not_str) #not_str
#define IN_STR(not_str) WRAP_IN_STR(not_str) // REVIEW STACK_INIT using __line__ inside itself

#define STACK_INIT(stack_name, capacity)                                                \
    StackInit(&stack_name, capacity ONDEB(, #stack_name, __FILE__, __func__, __LINE__))
#define STACK_VERIFY(stack)               \
    StackVerify(stack, __func__, __LINE__)

#define COLOR_SWITCH ON           // switch color output

#if COLOR_SWITCH == ON
    #define RED "\x1b[31m"
    #define GRE "\x1b[32m"
    #define YEL "\x1b[33m"
    #define BLU "\x1b[34m"
    #define MAG "\x1b[35m"
    #define END "\x1b[0m"

    /* Color: is replaced by s colored in color */
    #define SetColor(color) printf(color);
    #define EndColor printf(END);
    #define Color(s, color) color s END
#else
    #define RED ""
    #define BLU ""
    #define GRE ""
    #define YEL ""
    #define MAG ""
    #define END ""

    #define SetColor(color)
    #define EndColor
    #define Color(s, color) s
#endif

struct Stack_t {
    ONDEB(
        const char* stack_name;
        const char* file_name;
        const char* function_name;
        int line_number;
    )

    STACK_TYPE* data;
    ssize_t capacity; // capasity unsigned chech prisvaemoe znachenie na polojitelnost
    size_t size;
    int error;
};

int StackInit(Stack_t* stack, ssize_t capacity
              ONDEB(, const char* stack_name, const char* file_name,
                    const char* function_name, int line_number));

void StackDestroy(Stack_t* stack);

void StackPoisonFill(Stack_t* stack);

int StackVerify(Stack_t* stack, const char* calling_function_name, int from_line);

int DiagnoseFatalError(Stack_t* stack, const char* calling_function_name, int from_line);

void StackDump(const Stack_t* stack);

void StackPrintError(const Stack_t* stack);

STACK_TYPE StackPop();  // TODO obratniy spusk


int main() {
    Stack_t stk1 = {};
    /*if (StackInit(&stk1, 2) == STACK_NORMAL)
        printf("Nice initialization!\n");*/
    if (STACK_INIT(stk1, 2) == STACK_NORMAL)
        printf("Nice initialization!\n");
    //(void)STACK_VERIFY(&stk1);
}


int StackInit(Stack_t* stack, ssize_t capacity
              ONDEB(, const char* stack_name, const char* file_name,
                    const char* function_name, int line_number)) {
    assert(stack != NULL); // TODO anyway make stack but return pointer
    ONDEB(
        assert(stack_name != NULL); assert(file_name != NULL); assert(function_name != NULL);
    )

    if (capacity < 1)
        capacity = DEFAULT_CAPACITY;

    ONDEB(
        stack->stack_name = stack_name;
        stack->file_name = file_name;
        stack->function_name = function_name;
        stack->line_number = line_number;
    )

    stack->data = (STACK_TYPE*)calloc(sizeof(STACK_TYPE), capacity);
    stack->capacity = capacity;
    stack->size = 0;
    stack->error = STACK_NORMAL;

    ONDEB(StackPoisonFill(stack);) // REVIEW make function for fill poison

    return STACK_VERIFY(stack);  // REVIEW add verify here
}


void StackDestroy(Stack_t* stack) {
    assert(stack != NULL);

    if (stack->data != NULL) {
        ONDEB(StackPoisonFill(stack);)
        free(stack->data); //TODO free(stack) with clearing fields by 0 values in case if it was requested bi stackInit
    }
}


void StackPoisonFill(Stack_t* stack) {
    assert(stack != NULL);

    if (stack->data != NULL) {
        for (STACK_TYPE* in_stack_ptr = stack->data;
             (in_stack_ptr - stack->data) < stack->capacity; in_stack_ptr++)
            *in_stack_ptr = POISON;
    }
}


int StackVerify(Stack_t* stack, const char* calling_function_name, int from_line) { // REVIEW split on functions
    assert(stack != NULL); assert(calling_function_name != NULL);   // REVIEW get calling function name as parameter

    ONDEB(printf(">>>>>\n");)

    (void)DiagnoseFatalError(stack, calling_function_name, from_line);

    ONDEB(
        printf("\n");
        StackDump(stack);
    )

    ONDEB(printf("<<<<<\n\n");)

    return stack->error;
}


int DiagnoseFatalError(Stack_t* stack, const char* calling_function_name, int from_line) {
    assert(stack != NULL); assert(calling_function_name != NULL);

    stack->error = STACK_NORMAL;

    SetColor(RED);
    if (stack->data == NULL) {
        ONDEB(printf("Diagnostic from %s(), line %d: Fatal error - data is lost, may be it did not allocated or was free\n", calling_function_name, from_line);)
        stack->error |= STACK_DATA_ERROR;
    }
    if (stack->capacity < 1) {
        ONDEB(printf("Diagnostic from %s(), line %d: Fatal error - invalid stack capacity, it must be positive\n", calling_function_name, from_line);)
        stack->error |= STACK_CAPACITY_ERROR;
    }
    if (stack->size > stack->capacity) {
        ONDEB(printf("Diagnostic from %s(), line %d: Fatal error - invalid stack size, it must not exceed capacity\n", calling_function_name, from_line);)
        stack->error |= STACK_SIZE_ERROR;
    }
    if (stack->size < 0) {
        ONDEB(printf("Diagnostic from %s(), line %d: Fatal error - invalid stack size, it can not be negative\n", calling_function_name, from_line);)
        stack->error |= STACK_SIZE_ERROR;
    }

    SetColor(GRE);
    if (stack->error == STACK_NORMAL) {
        ONDEB(printf("Diagnostic from %s(), line %d: No fatal damage\n", calling_function_name, from_line);)
    }

    EndColor;

    return stack->error;
}


void StackDump(const Stack_t* stack) { // TODO color
    assert(stack != NULL);

    ONDEB(
        printf("%s[%p] with type %s from file %s, func %s(), line %d:\n\n",
               stack->stack_name, stack->data, IN_STR(STACK_TYPE),
               stack->file_name, stack->function_name, stack->line_number);
    )

    printf("capacity = %d\nsize = %d\n\n", stack->capacity, stack->size); // REVIEW print stack->error
    StackPrintError(stack);

    printf("data = %p:\n", stack->data);
    if (stack->data != NULL) {
        for (STACK_TYPE* in_stack_ptr = stack->data;
             (in_stack_ptr - stack->data) < stack->capacity; in_stack_ptr++) {
            if ((in_stack_ptr - stack->data) < stack->size) printf(" * ");
            else printf("   ");

            printf("%lf", *in_stack_ptr);          // TODO def %lf

            if (*in_stack_ptr == POISON) printf(" (POISON)");
            printf("\n");
        }
    }
    else
        printf("   Impossible to print data\n");
}


#define STACK_CHECK_ERROR(errors_code, checking_error)\
    if ((errors_code & checking_error) != 0) {        \
        printf("   " #checking_error "\n");           \
    }

void StackPrintError(const Stack_t* stack) {
    assert(stack != NULL);

    printf("error:\n");

    if (stack->error == STACK_NORMAL)
        printf("   no error\n");
    else {
        SetColor(RED);

        STACK_CHECK_ERROR(stack->error, STACK_DATA_ERROR);
        STACK_CHECK_ERROR(stack->error, STACK_CAPACITY_ERROR);
        STACK_CHECK_ERROR(stack->error, STACK_SIZE_ERROR);

        EndColor;
    }

    printf("\n");
}


STACK_TYPE StackPop(Stack_t* stack) {
    assert(stack != NULL);

    if (STACK_VERIFY(stack) != STACK_NORMAL)
        return POISON;

    STACK_TYPE result = stack->data[stack->size];
    ONDEB(stack->data[stack->size] = POISON);
    stack->size--;

    /*if (STACK_VERIFY(stack) != STACK_NORMAL) // MENTOR
        return POISON;*/
    STACK_VERIFY(stack);

    return result;
}
