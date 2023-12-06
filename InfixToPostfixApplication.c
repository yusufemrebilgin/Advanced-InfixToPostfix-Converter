#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

#ifdef _WIN32
    #define clearScreen() system("cls")
#elif defined(__linux__)
    #define clearScreen() system("clear")
#endif

#define MAX_EXP_SIZE 200 // maksimum ifade boyutu

// Generic Stack Yapısı
typedef struct {
    int top;
    size_t size;
    size_t elementSize;
    void *data;
} Stack;

// Generic stack işlemleri için gerekli olan fonksiyonların prototipleri
Stack *createStack(size_t size, size_t elementSize);
void push(Stack *stack, void *var);
void *pop(Stack *stack);
void *peek(Stack *stack);
bool isEmpty(Stack *stack);
bool isFull(Stack *stack);
void freeStack(Stack *stack);

// Dönüşüm için gerekli olan fonksiyonların prototipleri 
void infixToPostfix(const char *infix, char *postfix);
int precedence(char operator);
bool isOperator(char c);

// Postfix ifadesinin değerlendirilmesi için gerekli olan fonksiyonun prototipi
double postfixEvaluation(const char *exp);

// Girilen infix ifadenin kurallı olup olmadığını kontrol eden fonksiyonların prototipleri
bool isOpenBracket(char c);
bool isCloseBracket(char c);
bool areBracketsMatching(char c1, char c2);
bool isExpressionValid(const char *text);

void printPostfixResult(const double result);
void clearInputBuffer(void);

int main(void){

    char infix[MAX_EXP_SIZE];       // Kullanıcıdan alınacak infix ifadeyi tutan dizi
    char postfix[MAX_EXP_SIZE * 2]; // İfade dönüşümü sırasında olası taşmalara karşı daha büyük boyut ayrılır

    clearScreen();

    printf("\t\t______ ADVANCED INFIX TO POSTFIX CONVERTER ______\n\n"
           "\t- This program converts infix expression into a postfix expression\n"
           "\t and evaluates the converted postfix expression.\n\n"
           "\t- This program works with floating-point numbers.\n\n"
           "\t- This program does not evaluate negative numbers.\n\n\n\n"
           );
    
    while (true) {

        printf("Please enter infix expression: ");
        // fgets fonksiyonu ile kullanıcının girdiği ifadeyi okuma
        if ((fgets(infix, MAX_EXP_SIZE, stdin)) == NULL) {
            fprintf(stderr, "Failed to read input");
            exit(EXIT_FAILURE);
        }

        // Eğer kullanıcı tarafından girilen ifadenin sonunda yeni satır karakteri yoksa ifade istenilen boyutu aşmıştır
        if (infix[strlen(infix) - 1] != '\n') {
            fprintf(stderr, "The expression has exceeded the desired size. Please restart the program.");
            exit(EXIT_FAILURE);
        }
        else {
            // fgets ile kullanıcıdan alınan ifadenin sonuna eklenen '\n' karakterini kaldır
            infix[strcspn(infix,"\n")] = '\0';
        }

        // Girilen infix ifadesi geçerli değilse hata mesajı yazdır ve programı sonlandır
        if (!isExpressionValid(infix)) {
            fprintf(stderr, "The entered expression is not valid. Please restart the program.");
            exit(EXIT_FAILURE);
        }
        // Infix ifadeyi postfix ifadeye dönüştürme 
        infixToPostfix(infix,postfix);

        // Dönüştürülmüş postfix ifadesini ekrana yazdırma
        printf("\n\nPostfix Expression: %s\n", postfix);

        // Postfix ifadesinin değerlendirme ve sonucu ekrana yazdırma
        printPostfixResult(postfixEvaluation(postfix));

        while (true) {
            // Kullanıcının yeni bir ifade girip girmeme isteğini sorma
            printf("Would you like to enter a new expression?\nYes(y)\nNo(n)\n.."); 
            char ch = getchar();
            clearInputBuffer(); 

            if (ch == 'y' || toupper(ch) == 'Y') {
                clearScreen();
                break;
            }
            else if (ch == 'n' || toupper(ch) == 'N') {
                printf("\n\nThe program has been terminated.");
                return EXIT_SUCCESS;
            }
            else
                clearScreen();
        }
    }

    return 0;
}

// Postfix sonucunu ekrana yazdırır
void printPostfixResult(const double result){
    // Sonuç, double tipinin üst sınırından büyükse hata mesajı yazdır ve programı sonlandır
    if (result > DBL_MAX) {
        fprintf(stderr, "Result too large");
        exit(EXIT_FAILURE);
    }
    // Sonuç, double tipinin alt sınırından küçükse hata mesajı yazdır ve programı sonlandır
    else if (result < -DBL_MAX) {
        fprintf(stderr, "Result too small");
        exit(EXIT_FAILURE);
    }
    // Sonuç, 1e15 sayısından büyükse sonucu bilimsel gösterimle yazdır
    else if (result > 1e15) {
        printf("Postfix Evaluation: %.*e\n\n\n", DBL_DIG, result);
    }
    // Diğer durumlarda sonucu ondalık gösterimle yazdır
    else {
        printf("Postfix Evaluation: %.*lf\n\n\n", DBL_DIG, result);
    }
}

// Infix ifadeyi postfix ifadeye dönüştürür
void infixToPostfix(const char *infix, char *postfix){
    
    int len = strlen(infix);
    // İfadeyi dönüştürmek için kullanılacak olan stack'in bellek ayırma işlemi
    Stack *stack = createStack(len, sizeof(char));
    // Bellek ayırma işlemi başarısız olursa hata mesajı yazdır ve programı sonlandır
    if (stack == NULL) {
        fprintf(stderr, "Memory allocation failed for the stack!\n");
        exit(EXIT_FAILURE);
    }
    
    // Döngü içinde kullanılacak değişkenlerin tanımlamaları
    int i, j = 0;
    char token, current;

    for (i = 0; i < len; i++) {
        token = infix[i];
        // Eğer karakter, sayı veya ondalık belirten nokta ise girilen tam sayıyı veya ondalık sayıyı postfix ifadesinde tut
        if (isdigit(token) || token == '.') {
            while (isdigit(infix[i]) || infix[i] == '.')
                postfix[j++] = infix[i++];
            postfix[j++] = ' '; // Bir sonraki indise boşluk ekler
            i--; // Döngü içinde bir kez daha arttırıldığından döngü işleyişini düzeltmek için bir azaltılır
        }
        // Eğer karakter açık parantez ise
        else if (token == '(') {
            push(stack, &token); // stack'e ekle (push işlemi)
        }
        // Eğer karakter kapalı parantez ise stack'te bulunan operatörleri çıkarıp postfix ifadesine ekle
        else if (token == ')') {
            while ((current = *(char *)pop(stack)) != '(') {
                postfix[j++] = current;
                postfix[j++] = ' ';
            }
        }
        // Eğer karakter operatör ise stack'e ekle
        else if (isOperator(token)) {
            /* Stack boş değil ve stack'in en üstünde bulunan operatörün önceliği, değişkende tutulan operatörün
               önceliğinden büyük veya eşit ise operatörleri stack'ten çıkarıp postfix ifadesine ekle */
            while (!isEmpty(stack) && precedence(*(char *)peek(stack)) >= precedence(token)) {
                postfix[j++] = *(char *)pop(stack);
                postfix[j++] = ' ';
            }
            push(stack, &token);
        }
    }
    // Stack'te kalan ifadeleri postfix ifadesine ekle
    while (!isEmpty(stack)) {
        postfix[j++] = *(char *)pop(stack);
        postfix[j++] = ' ';
    }
    postfix[j] = '\0';
    freeStack(stack);
}

// Postfix ifadesini değerlendirir ve elde edilen postfix sonucunu döndürür
double postfixEvaluation(const char *exp){
    // İfadede bulunan operandların değerlendirilmesinde kullanılacak olan stack'in oluşturma işlemi
    Stack *stack = createStack(strlen(exp), sizeof(double));
    // Stack oluşturma işlemi başarıyla gerçekleşmezse hata mesajı yazdır ve programı sonlandır
    if (stack == NULL) {
        fprintf(stderr, "Memory allocation failed for the stack!");
        exit(EXIT_FAILURE);
    }

    // Gönderilen ifadenin sınırlayıcı parametreye (boşluk karakterine) kadar olan kısmını alır
    char *token = strtok((char *)exp, " ");

    while (token != NULL) {
        // Eğer karakter sayı ise stack'e ekle
        if (isdigit(*token)) {
            // İfadeden okunan postfix parçası double değere dönüştürülür
            double operand = atof(token);
            push(stack, &operand);
        }
        else {
            // Karakter operatör ise stack'ten çıkarılan iki operand, operatör ile işleme sokulup stack'e eklenir
            double operand1 = *(double *)pop(stack);
            double operand2 = *(double *)pop(stack);
            switch (*token) {
            case '+': {
                double result = operand2 + operand1;
                push(stack, &result);
                break;
            }
            case '-': {
                double result = operand2 - operand1;
                push(stack, &result);
                break;
            }
            case '*': {
                double result = operand2 * operand1;
                push(stack, &result);
                break;
            }
            case '/': {
                double result = operand2 / operand1;
                push(stack, &result);
                break;
            }
            case '^': {
                double result = pow(operand2, operand1);
                push(stack, &result);
                break;
            }
            }
        }
        token = strtok(NULL, " "); // Boşluk karakterine kadar olan bir sonraki parça alınır
    }
    double result = *(double *)pop(stack); // Stack'te kalan değer postfix ifadesinin sonucu olur
    freeStack(stack);                  
    return result;
}


// Döndürülen sayı ne kadar büyükse operatörün öncelik seviyesi o kadar fazladır
int precedence(char operator){
    if (operator == '+' || operator == '-')
        return 1;
    else if (operator == '*' || operator == '/')
        return 2;
    else if (operator == '^')
        return 3;
    else
        return 0; // Geçersiz operatör olma durumunda
}

// Verilen karakterin operatör olup olmadığını kontrol eder
bool isOperator(char c){
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^') ? true : false;
}

// Gönderilen ifadenin geçerli olup olmadığını kontrol eder
bool isExpressionValid(const char *text){

    // Stack oluşturma işlemi
    Stack *stack = createStack(strlen(text), sizeof(char));
    // Stack oluşturma işlemi başarıyla gerçekleşmezse hata mesajı yazdır ve programı sonlandır
    if (stack == NULL) {
        fprintf(stderr, "Memory allocation failed for the stack!\n");
        exit(EXIT_FAILURE);
    }

    int i, operator = 0, operand = 0;
    for (i = 0; text[i] != '\0'; i++) {
        char character = text[i];
        /* ------------------------- Parantezlerin Kontrolü ------------------------- */
        // Eğer karakter açık parantez ise stack'e ekle (push işlemi)
        if (isOpenBracket(character)) {
            push(stack, &character);
        }
        // Eğer karakter kapalı parantez ise
        else if (isCloseBracket(character)) {
            // Stack'te kapalı parantez ile karşılaştırılacak herhangi bir karakter yoksa 'false' döndürülür
            if (isEmpty(stack))
                return false;
            // Karakterler eşleşmiyorsa ve stack boş değilse 'false' döndürülür
            if (!isEmpty(stack) && !areBracketsMatching(*(char *)pop(stack),character))
                return false;
        }
        /* -------------------------------------------------------------------------- */
        // Eğer karakter rakam ise
        else if (isdigit(character)) {
            operand++;
            // Sayının tamamı okunana kadar indis arttırılır
            while (isdigit(text[i]) || text[i] == '.')
                i++;
            i--; // Döngü içinde tekrar arttırıldığından dolayı bir azaltılır
        }
        else if (isOperator(character)) {
            operator++;
        }
        else if (character == '.') // Operand . ile başlıyorsa false döndürülür (Örneğin: .56)
            return false;
        else
            continue;    
    }

    // Operator sayısı operand sayısının bir eksiğine eşit değilse ifade geçerli değildir
    if (operator != operand - 1)
        return false;

    // Stack boşsa bellek serbest bırakılır ve 'true' değeri döndürülür
    if (isEmpty(stack)) {
        freeStack(stack);
        return true;
    }
    else {
        freeStack(stack);
        return false;
    }
}

// Verilen karakterin açık parantez olup olmadığını kontrol eder
bool isOpenBracket(char c){
    return (c == '(' || c == '{' || c == '[') ? true : false;
}

// Verilen karakterin kapalı parantez olup olmadığını kontrol eder
bool isCloseBracket(char c){
    return (c == ')' || c == '}' || c == ']') ? true : false;
}

// Verilen parentezlerin eşleşip eşleşmediğini kontrol eder
bool areBracketsMatching(char c1, char c2){
    return ((c1 == '(' && c2 == ')') || (c1 == '{' && c2 == '}') || (c1 == '[' && c2 == ']')) ? true : false;
}

// Geçici olarak bufferda tutulan karakterleri temizler
void clearInputBuffer(void){
    while (getchar() != '\n')
        continue;
}

// Verilen boyutta stack oluşturur
Stack *createStack(size_t size, size_t elementSize){
    Stack *stack = malloc(sizeof(Stack));
    if (stack != NULL) {
        stack->data = malloc(size * elementSize);
        if (stack->data != NULL) {
            stack->top = -1;
            stack->size = size;
            stack->elementSize = elementSize;
        }
        // 'stack->data' için bellek ayırma işlemi başarısız olursa 'stack' için ayrılan bellek serbest bırakılır
        else {
            free(stack);
            return NULL;
        }
    }
    return stack;
}

// Stack'e eleman ekler
void push(Stack *stack, void *var){
    if (isFull(stack)) {
        fprintf(stderr, "Stack is full!\n");
        return;
    }
    memcpy((char *)stack->data + (++stack->top) * stack->elementSize, var, stack->elementSize);
}

// Stack'ten eleman çıkarır
void *pop(Stack *stack){
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack is empty!\n");
        exit(EXIT_FAILURE);
    }
    return ((char *)stack->data + (stack->top--) * stack->elementSize);
}

// Stack'in en üstündeki elemanı döndürür
void *peek(Stack *stack){
    if (isEmpty(stack)) {
        fprintf(stderr, "Stack is empty!\n");
        exit(EXIT_FAILURE);
    }
    return ((char *)stack->data + (stack->top) * stack->elementSize);
}

// Stack'in boş olup olmadığını kontrol eder
bool isEmpty(Stack *stack){
    return stack->top == -1;
}

// Stack'in dolu olup olmadığını kontrol eder
bool isFull(Stack *stack){
    return stack->top == stack->size - 1;
}

// Stack için bellekten ayrılan alanı serbest bırakır
void freeStack(Stack *stack){
    if (stack != NULL) {
        free(stack->data);
        free(stack);
    }
}