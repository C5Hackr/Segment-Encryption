#include <Windows.h>
#include <iostream>
#include <signal.h>
#include "SED.h"

#pragma region Math Stuff
int factorial(int n) {
	if (n == 0 || n == 1) {
		return 1;
	}
	else {
		return n * factorial(n - 1);
	}
}
int fibonacci(int n) {
	if (n <= 1) {
		return n;
	}
	else {
		return fibonacci(n - 1) + fibonacci(n - 2);
	}
}
void printArray(int arr[], int size) {
	int i;
	for (i = 0; i < size; i++) {
		printf("%d ", arr[i]);
	}
}
void modifyArray(int arr[], int size) {
	int i;
	for (i = 0; i < size; i++) {
		arr[i] *= 2;
	}
}

#define SQUARE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Structure definition
struct Person {
	char name[50];
	int age;
};

int gcd(int a, int b) {
	if (b == 0)
		return a;
	else
		return gcd(b, a % b);
}
void swap(int* xp, int* yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}
void bubbleSort(int arr[], int n) {
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < n - i - 1; j++) {
			if (arr[j] > arr[j + 1]) {
				swap(&arr[j], &arr[j + 1]);
			}
		}
	}
}
#pragma endregion

__declspec(noinline) int testCCode()
{
	// Arithmetic operations
	int a = 5, b = 3;
	int result = a + b * 2 - 1;

	// Logical operations
	int x = 10, y = 20;
	int logical_result = (x > y) && (x != 0);

	// Control structures
	if (a > b) {
		printf("a is greater than b\n");
	}
	else {
		printf("b is greater than or equal to a\n");
	}

	// Pointers and arrays
	int arr[5] = { 1, 2, 3, 4, 5 };
	int* ptr = arr;
	printf("First element of array: %d\n", *ptr);
	printf("Third element of array: %d\n", *(ptr + 2));

	// Loops
	printf("Array elements: ");
	printArray(arr, 5);
	printf("\n");

	// Function call
	int factorial_result = factorial(5);
	printf("Factorial of 5: %d\n", factorial_result);

	// Fibonacci sequence
	printf("Fibonacci sequence up to 10: ");
	int i;
	for (i = 0; i < 10; i++) {
		printf("%d ", fibonacci(i));
	}
	printf("\n");

	// Dynamic memory allocation
	int* dynamic_array = (int*)malloc(3 * sizeof(int));
	dynamic_array[0] = 10;
	dynamic_array[1] = 20;
	dynamic_array[2] = 30;
	printf("Dynamic array elements: ");
	printArray(dynamic_array, 3);
	free(dynamic_array);

	// Bitwise operations
	int num1 = 10, num2 = 5;
	printf("Bitwise AND: %d\n", num1 & num2);
	printf("Bitwise OR: %d\n", num1 | num2);
	printf("Bitwise XOR: %d\n", num1 ^ num2);

	// Sizeof operator
	printf("Size of int: %lu bytes\n", sizeof(int));


	return endfunction(0, 0);
}

__declspec(noinline) int testCCode2()
{
	// String manipulation
	char str1[20] = "Hello";
	char str2[20] = "World";
	char str3[40];

	// String concatenation
	strcpy(str3, str1);
	strcat(str3, " ");
	strcat(str3, str2);
	printf("Concatenated string: %s\n", str3);

	// String length
	printf("Length of str1: %lu\n", strlen(str1));

	// String comparison
	printf("Comparison of str1 and str2: %d\n", strcmp(str1, str2));

	// Structures
	struct Person person1;
	strcpy(person1.name, "John Doe");
	person1.age = 30;

	struct Person person2 = { "Jane Doe", 25 };

	printf("Person 1: %s, %d\n", person1.name, person1.age);
	printf("Person 2: %s, %d\n", person2.name, person2.age);

	// Dynamic memory
	int* arr = (int*)malloc(5 * sizeof(int));
	if (arr == NULL) {
		printf("Memory allocation failed!\n");
		return 0;
	}
	for (int i = 0; i < 5; i++) {
		arr[i] = i + 1;
	}
	printf("Dynamic array elements: ");
	for (int i = 0; i < 5; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
	free(arr);

	// Macros
	int num = 4;
	int max_val = MAX(10, 20);
	printf("Square of %d: %d\n", num, SQUARE(num));
	printf("Max of 10 and 20: %d\n", max_val);

	// Recursive functions
	int gcd_result = gcd(48, 18);
	printf("GCD of 48 and 18: %d\n", gcd_result);

	// Sorting algorithms
	int sortArr[] = { 64, 34, 25, 12, 22, 11, 90 };
	int n = sizeof(sortArr) / sizeof(sortArr[0]);
	bubbleSort(sortArr, n);
	printf("Sorted array: ");
	for (int i = 0; i < n; i++) {
		printf("%d ", sortArr[i]);
	}
	printf("\n");

	return endfunction(0, 0);
}

/*
__declspec(noinline) int DemoFunction() //Function type must be a INT, and must have the __declspec(noinline) attribute.
{
	printf("[ourfunction] - hello world!\n");
	return endfunction(0, 0); //Function must end with "return endfunction(0, 0)".
}
*/

int main() //Project must be in Release mode, x64 and x86 are both supported.
{
	printf("Encrypting code section (Execution will pause once finish for inspection in x64dbg)...\n");
	EncryptFunction((uintptr_t)testCCode); //Encrypt function "testCCode".
	EncryptFunction((uintptr_t)testCCode2); //Encrypt function "testCCode2".
	system("pause");
	printf("[START!]\n");
	printf("[======================testCCode========================]\n");
	testCCode();
	printf("[======================testCCode2=======================]\n");
	testCCode2();
	printf("[END!]\n");
	system("pause");
	exit(0);
}