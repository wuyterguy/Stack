#include <stdio.h>  // FIXME README
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#define DEBUG  // TODO all func v gubug

#ifdef DEBUG
    #define ONDEB(...) __VA_ARGS__
#else
    #define ONDEB(...)
#endif

ONDEB(static FILE* log_file = NULL;) // FIXME log
#define STACK_LOG_FILE_NAME "stack_log.txt"

/*#define Print_error_message(message, object)                                         \
    fprintf(stderr, "%s(): %s - %s, line %d\n", __func__, message, object, __LINE__);*/
#undef OVERFLOW

typedef enum {STACK_NORMAL = 0b00000, STACK_DATA_ERROR = 0b00001,
              STACK_CAPACITY_ERROR = 0b00010, STACK_SIZE_ERROR = 0b00100,
              STACK_LEFT_CANARY_ERROR =0b01000, STACK_RIGHT_CANARY_ERROR = 0b10000} Stack_err_t;
typedef enum {SUCCESS = 0, FATAL_ERROR = 1, OVERFLOW = -1, VACUUM = -2}             Stack_report_t;
#define FATAL true
#define NOT_FATAL false
/*typedef enum {PUSH_NORMAL = 0} Push_err_t; // return of StackPush()
typedef enum {POP_NORMAL = 0}  Pop_err_t;  // return of StackPop()*/

#define INT 1
#define FLOAT 2  // REVIEW set type
#define DOUBLE 3
#define POINTER 4

#define STACK_TYPE DOUBLE  // REVIEW add poison to descriptor and pop and init

#if STACK_TYPE == INT
    #define Stack_type_t int
    #define STACK_TYPE_SPECIFIER "%d"
    #define POISON 1666667
#elif STACK_TYPE == FLOAT
    #define Stack_type_t float
    #define STACK_TYPE_SPECIFIER "%f"
    #define POISON NAN
#elif STACK_TYPE == DOUBLE
    #define Stack_type_t double
    #define STACK_TYPE_SPECIFIER "%lf"
    #define POISON NAN
#elif STACK_TYPE == POINTER
    #define Stack_type_t void*
    #define STACK_TYPE_SPECIFIER "%p"
    #define POISON NULL
#else
    #define Stack_type_t long long int
    #define STACK_TYPE_SPECIFIER "%lld"
    #define POISON 166666666667
#endif


typedef char Byte_t;
typedef unsigned long long int Canary_t;

#define CANARY_SPECIFIER "0x%llX"
#define CANARY_BYTE_SIZE sizeof(Canary_t)
#define LEFT_CANARY         0xCA14A2E7CE111A27ull
#define RIGHT_CANARY        0xB12D1412E17B12D5ull
#define LEFT_HANDLE_CANARY  0xEA1AE2EABEE11A98ull
#define RIGHT_HANDLE_CANARY 0xAE4D1DC2F65B134Bull

#define DEFAULT_CAPACITY 10ull
#define CAPACITY_FACTOR 1.5

#define WRAP_IN_STR(not_str) #not_str
#define IN_STR(not_str) WRAP_IN_STR(not_str) // REVIEW STACK_INIT using __line__ inside itself

#define STACK_INIT(stack_name, capacity)                                  \
    StackInit(capacity ONDEB(, #stack_name, __FILE__, __func__, __LINE__))
#define STACK_VERIFY(stack)               \
    StackVerify(stack, __func__, __LINE__)

#define ON 1
#define OFF 0

#define COLOR_SWITCH OFF           // switch color output

#if COLOR_SWITCH == ON
    #define RED "\x1b[31m"
    #define GRE "\x1b[32m"
    #define YEL "\x1b[33m"
    #define BLU "\x1b[34m"
    #define MAG "\x1b[35m"
    #define END "\x1b[0m"

    /* Color: is replaced by s colored in color */
    #define SetColor(color) fprintf(log_file, color);
    #define EndColor fprintf(log_file, END);
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
    ONDEB(Canary_t left_handle_canary;)

    ONDEB(
        const char* stack_name;
        const char* file_name;
        const char* function_name;
        int line_number;
    )

    Stack_type_t* data;
    ssize_t capacity; // capasity unsigned chech prisvaemoe znachenie na polojitelnost
    size_t size;
    Stack_err_t error;
    bool error_fatality;
    Stack_report_t last_function_success;

    ONDEB(Canary_t right_handle_canary;)
};  // REVIEW error status

Stack_t* StackInit(ssize_t capacity ONDEB(, const char* stack_name, const char* file_name,
                                          const char* function_name, int line_number));

void StackDestroy(Stack_t* stack);

void StackFillPoison(Stack_t* stack, int start_offset);

Stack_err_t StackVerify(Stack_t* stack, const char* calling_function_name, int from_line);

Stack_err_t DiagnoseFatalError(Stack_t* stack, const char* calling_function_name, int from_line);

void StackDump(const Stack_t* stack);

void StackPrintError(const Stack_t* stack);

Stack_report_t StackPush(Stack_t* stack, Stack_type_t new_item);

Stack_type_t StackPop(Stack_t* stack);  // REVIEW obratniy spusk

Stack_report_t StackIncreaseCapacity(Stack_t* stack);

Stack_report_t StackReduceCapacity(Stack_t* stack);

Stack_type_t* StackReallocWithCanary(Stack_type_t* old_data, ssize_t new_capacity);
Canary_t* LeftCanaryPtr(Stack_t* stack);
Canary_t* RightCanaryPtr(Stack_t* stack);
void StackSetCanary(Stack_t* stack);


int main() {
    ONDEB(
        log_file = fopen(STACK_LOG_FILE_NAME, "w");
        if (log_file == NULL) {
            SetColor(YEL);
            printf("Cant open %s with \"w\" permission\n", STACK_LOG_FILE_NAME);
            EndColor;
            return 1;
        }
    )

    Stack_t* stk1 = STACK_INIT(stk1, 2);
    //Stack_t* stk1 = STACK_INIT(stk1, -20000000000);
    if (stk1 == NULL) {
        ONDEB(fprintf(log_file, "Bad initialization!\n");)
        return 1;
    }


    //*RightCanaryPtr(&stk1) = 67;

    StackPush(stk1, 67);
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    StackPush(stk1, 67);
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    StackPush(stk1, 67);
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    StackPush(stk1, 67);
    *LeftCanaryPtr(stk1) = 67;
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    StackPush(stk1, 67);
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    //stk1->left_handle_canary = 2;

    ONDEB(fprintf(log_file, "<%lf>\n", StackPop(stk1));)
    if (stk1->last_function_success == VACUUM)
        ONDEB(fprintf(log_file, "GGG\n");)
    ONDEB(fprintf(log_file, "-------------------------------------\n");
    ONDEB(fprintf(log_file, "<%lf>\n", StackPop(stk1));)
    if (stk1->last_function_success == VACUUM))
        ONDEB(fprintf(log_file, "GGG\n");)
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    ONDEB(fprintf(log_file, "<%lf>\n", StackPop(stk1));)
    if (stk1->last_function_success == VACUUM)
        ONDEB(fprintf(log_file, "GGG\n");)
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    ONDEB(fprintf(log_file, "<%lf>\n", StackPop(stk1));)
    if (stk1->last_function_success == VACUUM)
        ONDEB(fprintf(log_file, "GGG\n");)
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    ONDEB(fprintf(log_file, "<%lf>\n", StackPop(stk1));)
    if (stk1->last_function_success == VACUUM)
        ONDEB(fprintf(log_file, "GGG\n");)
    ONDEB(fprintf(log_file, "-------------------------------------\n");)
    ONDEB(fprintf(log_file, "<%lf>\n", StackPop(stk1));)
    if (stk1->last_function_success == VACUUM)
        ONDEB(fprintf(log_file, "GGG\n");)

    //(void)STACK_VERIFY(&stk1);
    StackDestroy(stk1);
}


Stack_t* StackInit(ssize_t capacity ONDEB(, const char* stack_name, const char* file_name,
                                          const char* function_name, int line_number)) {
    ONDEB(assert(stack_name != NULL); assert(file_name != NULL); assert(function_name != NULL);)

    Stack_t* stack = (Stack_t*)malloc(sizeof(Stack_t));

    if (stack == NULL)
        return NULL;

    if (capacity < 1)
        capacity = DEFAULT_CAPACITY;

    ONDEB(stack->left_handle_canary = LEFT_HANDLE_CANARY;)

    ONDEB(
        stack->stack_name = stack_name;
        stack->file_name = file_name;
        stack->function_name = function_name;
        stack->line_number = line_number;
    )

    stack->data = NULL;
    stack->capacity = capacity;
    stack->size = 0;
    stack->error = STACK_NORMAL;
    stack->error_fatality = NOT_FATAL;
    stack->last_function_success = SUCCESS;
    stack->data = StackReallocWithCanary(NULL, stack->capacity); // REVIEW func
    if (stack->data != NULL)
        StackSetCanary(stack);

    ONDEB(StackFillPoison(stack, 0);) // REVIEW make function for fill poison

    ONDEB(stack->right_handle_canary = RIGHT_HANDLE_CANARY;)

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL))    // STUB
        return NULL;

    return stack;
}


void StackDestroy(Stack_t* stack) { // FIXME proverit videlen li etot adress
    assert(stack != NULL);

    if (stack->data != NULL) {
        ONDEB(StackFillPoison(stack, 0);)
        free((Byte_t*)stack->data - CANARY_BYTE_SIZE); //REVIEW free(stack) with clearing fields by 0 values in case if it was requested bi stackInit
    }

    free(stack);
}


void StackFillPoison(Stack_t* stack, int start_offset) { // REVIEW add parameter - offset that filling starting from
    assert(stack != NULL);

    if (stack->data != NULL) {
        for (Stack_type_t* in_stack_ptr = stack->data + start_offset;
             (in_stack_ptr - stack->data) < stack->capacity; in_stack_ptr++)
            *in_stack_ptr = POISON;
    }
}


Stack_err_t StackVerify(Stack_t* stack, const char* calling_function_name, int from_line) { // REVIEW split on functions
    assert(stack != NULL); assert(calling_function_name != NULL);   // REVIEW get calling function name as parameter
    ONDEB(assert(log_file != NULL);)

    ONDEB(fprintf(log_file, ">>>>>\n");)  // REVIEW canory for data and Stack_t

    (void)DiagnoseFatalError(stack, calling_function_name, from_line);

    ONDEB(
        fprintf(log_file, "\n");
        StackDump(stack);
    )

    ONDEB(fprintf(log_file, "<<<<<\n\n");)

    return stack->error;
}


Stack_err_t DiagnoseFatalError(Stack_t* stack, const char* calling_function_name, int from_line) {
    assert(stack != NULL); assert(calling_function_name != NULL); // FIXME HASH data and srtuct
    ONDEB(assert(log_file != NULL);)

    ONDEB(SetColor(RED);)
                                        // STUB
    ONDEB (
        if ((stack->left_handle_canary != LEFT_HANDLE_CANARY) ||
            (stack->right_handle_canary != RIGHT_HANDLE_CANARY)) {
            ONDEB(
                fprintf(log_file, "Diagnostic from %s(), line %d: Fatal error - handle information has been damaged, program aborted\n",
                       calling_function_name, from_line);
                fprintf(log_file, "Left " CANARY_SPECIFIER ", Right " CANARY_SPECIFIER "\n",
                       stack->left_handle_canary, stack->right_handle_canary);
            )
            ONDEB(EndColor;)
            abort();
        }
    )

    stack->error = STACK_NORMAL;
    stack->error_fatality = NOT_FATAL;

    if (stack->data == NULL) {
        ONDEB(fprintf(log_file, "Diagnostic from %s(), line %d: Fatal error - data is lost, may be it did not allocated or was free\n", calling_function_name, from_line);)
        stack->error = (Stack_err_t)(stack->error | STACK_DATA_ERROR);
        stack->error_fatality = FATAL;
    }
    ONDEB(
        else { // FIXME print canary value
            if (*LeftCanaryPtr(stack) != LEFT_CANARY) {
                ONDEB(
                    fprintf(log_file, "Diagnostic from %s(), line %d: Warning - left canary has been damaged\n",
                           calling_function_name, from_line);
                    fprintf(log_file, "Its value: " CANARY_SPECIFIER "\n", *LeftCanaryPtr(stack));
                )
                stack->error = (Stack_err_t)(stack->error | STACK_LEFT_CANARY_ERROR);
            }
            if (*RightCanaryPtr(stack) != RIGHT_CANARY) {
                ONDEB(
                    fprintf(log_file, "Diagnostic from %s(), line %d: Warning - right canary has been damaged\n",
                           calling_function_name, from_line);
                    fprintf(log_file, "Its value: " CANARY_SPECIFIER "\n", *RightCanaryPtr(stack));
                )
                stack->error = (Stack_err_t)(stack->error | STACK_RIGHT_CANARY_ERROR);
            }
        }
    )

    if (stack->capacity < 1) {
        ONDEB(fprintf(log_file, "Diagnostic from %s(), line %d: Fatal error - invalid stack capacity, it must be positive\n", calling_function_name, from_line);)
        stack->error = (Stack_err_t)(stack->error | STACK_CAPACITY_ERROR);
        stack->error_fatality = FATAL;
    }
    if (stack->size > stack->capacity) {
        ONDEB(fprintf(log_file, "Diagnostic from %s(), line %d: Fatal error - invalid stack size, it must not exceed capacity\n", calling_function_name, from_line);)
        stack->error = (Stack_err_t)(stack->error | STACK_SIZE_ERROR);
        stack->error_fatality = FATAL;
    }
    if (stack->size < 0) {
        ONDEB(fprintf(log_file, "Diagnostic from %s(), line %d: Fatal error - invalid stack size, it can not be negative\n", calling_function_name, from_line);)
        stack->error = (Stack_err_t)(stack->error | STACK_SIZE_ERROR);
        stack->error_fatality = FATAL;
    }

    ONDEB(SetColor(GRE);)
    if (stack->error == STACK_NORMAL) {
        ONDEB(fprintf(log_file, "Diagnostic from %s(), line %d: No fatal damage\n", calling_function_name, from_line);)
    }

    ONDEB(EndColor;)

    return stack->error;
}


void StackDump(const Stack_t* stack) { // TODO color
    assert(stack != NULL);
    assert(log_file != NULL);

    ONDEB(
        fprintf(log_file, "%s[%p] with type %s from file %s, func %s(), line %d:\n\n",
               stack->stack_name, stack->data, IN_STR(Stack_type_t),
               stack->file_name, stack->function_name, stack->line_number);
    )

    fprintf(log_file, "capacity = %d\nsize = %d\n\n", stack->capacity, stack->size); // REVIEW print stack->error
    StackPrintError(stack);

    fprintf(log_file, "data = %p:\n", stack->data);
    if (stack->data != NULL) {
        for (Stack_type_t* in_stack_ptr = stack->data;
             (in_stack_ptr - stack->data) < stack->capacity; in_stack_ptr++) {
            if ((in_stack_ptr - stack->data) < stack->size) fprintf(log_file, " * ");
            else fprintf(log_file, "   ");

            fprintf(log_file, STACK_TYPE_SPECIFIER, *in_stack_ptr);

            if (*in_stack_ptr == POISON) fprintf(log_file, " (POISON)");
            fprintf(log_file, "\n");
        }
    }
    else
        fprintf(log_file, "   Impossible to print data\n");
}


#define STACK_CHECK_ERROR(errors_code, checking_error)\
    if ((errors_code & checking_error) != 0) {        \
        fprintf(log_file, "   " #checking_error "\n");           \
    }

void StackPrintError(const Stack_t* stack) {
    assert(stack != NULL);
    assert(log_file != NULL);

    fprintf(log_file, "error:\n");

    if (stack->error == STACK_NORMAL)
        fprintf(log_file, "   no error\n");
    else {
        SetColor(RED);

        STACK_CHECK_ERROR(stack->error, STACK_DATA_ERROR);
        STACK_CHECK_ERROR(stack->error, STACK_CAPACITY_ERROR);
        STACK_CHECK_ERROR(stack->error, STACK_SIZE_ERROR);
        STACK_CHECK_ERROR(stack->error, STACK_LEFT_CANARY_ERROR);
        STACK_CHECK_ERROR(stack->error, STACK_RIGHT_CANARY_ERROR);

        EndColor;
    }

    fprintf(log_file, "\n");
}


Stack_report_t StackPush(Stack_t* stack, Stack_type_t new_item) {
    assert(stack != NULL);

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL))
        return stack->last_function_success = FATAL_ERROR; // REVIEW reduce adn increase func

    if ((stack->size + 1) > stack->capacity) {
        Stack_report_t increase_res = SUCCESS;
        if ((increase_res = StackIncreaseCapacity(stack)) != SUCCESS)
            return stack->last_function_success = increase_res;
    }

    stack->data[stack->size] = new_item;
    stack->size++;

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL))
        return stack->last_function_success = FATAL_ERROR;

    return stack->last_function_success = SUCCESS;
}


Stack_type_t StackPop(Stack_t* stack) {
    assert(stack != NULL);
    ONDEB(assert(log_file != NULL);)

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL)) {
        stack->last_function_success = FATAL_ERROR;
        return POISON;
    }

    if (stack->size < 1) {
        ONDEB(
            SetColor(YEL);
            fprintf(log_file, "note from %s(), line %d: can not do pop because stack is empty, pop was cancelled\n",
                   __func__, __LINE__, stack->stack_name);
            EndColor;
        )
        stack->last_function_success = VACUUM;
        return POISON;
    }

    stack->size--;
    Stack_type_t result = stack->data[stack->size];
    ONDEB(stack->data[stack->size] = POISON);

    if (((ssize_t)(stack->size * pow(CAPACITY_FACTOR, 2)) < stack->capacity) && (stack->size > 0)) {
        Stack_report_t reduce_res = SUCCESS;
        if ((reduce_res = StackReduceCapacity(stack)) != SUCCESS) {
            stack->last_function_success = reduce_res;
            return POISON;
        }
    }

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL)) {
        stack->last_function_success = FATAL_ERROR;
        return POISON;
    }

    stack->last_function_success = SUCCESS;
    return result;
}


Stack_report_t StackIncreaseCapacity(Stack_t* stack) {
    assert(stack != NULL);
    ONDEB(assert(log_file != NULL);)

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL))
        return FATAL_ERROR;

    ssize_t new_capacity = (ssize_t)(stack->capacity * CAPACITY_FACTOR) + 1;
        Stack_type_t* new_data = StackReallocWithCanary(stack->data, new_capacity);

        if (new_data == NULL) {
            ONDEB(
                SetColor(YEL);
                fprintf(log_file, "note from %s(), line %d: can not grow up capacity for %s, push was cancelled\n",
                       __func__, __LINE__, stack->stack_name);
                EndColor;
            )
            return OVERFLOW;
        }

        stack->capacity = new_capacity; // REVIEW add canary
        stack->data = new_data;
        StackSetCanary(stack);

        ONDEB(StackFillPoison(stack, stack->size);)   // REVIEW doinitializirovat poisonami

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL))
        return FATAL_ERROR;

    return SUCCESS;
}


Stack_report_t StackReduceCapacity(Stack_t* stack) {
    assert(stack != NULL);

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL))
        return FATAL_ERROR;

    ssize_t new_capacity = (ssize_t)(stack->capacity / CAPACITY_FACTOR);
        Stack_type_t* new_data = StackReallocWithCanary(stack->data, new_capacity);

        if (new_data != NULL) {
            stack->capacity = new_capacity; // REVIEW add canary
            stack->data = new_data;
            StackSetCanary(stack);

            ONDEB(StackFillPoison(stack, stack->size);)   // REVIEW doinitializirovat poisonami
        }

    if ((STACK_VERIFY(stack) != STACK_NORMAL) && (stack->error_fatality == FATAL))
        return FATAL_ERROR;

    return SUCCESS;
}


Stack_type_t* StackReallocWithCanary(Stack_type_t* old_data, ssize_t new_capacity) {
    Byte_t* byte_old_data = (Byte_t*)old_data;
    if (old_data != NULL)
        byte_old_data -= CANARY_BYTE_SIZE;
    void* buffer_start = realloc(byte_old_data, (new_capacity * sizeof(Stack_type_t)) + (2 * CANARY_BYTE_SIZE));

    if (buffer_start == NULL)
        return NULL;

    return (Stack_type_t*)((Byte_t*)buffer_start + CANARY_BYTE_SIZE);
}

Canary_t* LeftCanaryPtr(Stack_t* stack) {
    assert(stack != NULL);

    return (Canary_t*)(stack->data) - 1;
}

Canary_t* RightCanaryPtr(Stack_t* stack) {
    assert(stack != NULL);

    return (Canary_t*)(stack->data + stack->capacity);
}

void StackSetCanary(Stack_t* stack) {
    *LeftCanaryPtr(stack) = LEFT_CANARY;
    *RightCanaryPtr(stack) = RIGHT_CANARY;
}
