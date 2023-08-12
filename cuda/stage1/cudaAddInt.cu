#include <iostream>
#include <cuda.h>

__globa__ void AddIntsCUDA(int *a, int *b)
{
    a[0] += b[0];
}

int main(int argc, char *argv[])
{
    int a = 5, b = 9;
    int *ptr_a, *ptr_b;

    cudaMalloc(&ptr_a, sizeof(int));
    cudaMalloc(&ptr_b, sizeof(int));

    cudaMemcpy(ptr_a, &a, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(ptr_b, &b, sizeof(int), cudaMemcpyHostToDevice);

    AddIntsCUDA<<<1, 1>>>(ptr_a, ptr_b);

    cudaMemcpy(&a, ptr_a, sizeof(int), cudaMemcpyDeviceToHost);

    std::cout << "The answer is" << std::endl;

    cudaFree(ptr_a);
    cudaFree(ptr_b);

    return 0;
}