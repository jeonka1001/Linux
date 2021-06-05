/*
To compile this file wth gcc , use the following command: 
$ gcc 1d_conv.c -o 1d_conv

*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <immintrin.h>
#include <xmmintrin.h>


#define KERNEL_LENGTH 16


int convolve_naive(float* in, float* out, int length,
        float* kernel, int kernel_length)
{
	length -= 2;
	for(int i = 0 ; i < length ; i ++){
		float tmp_sum = 0.0;
		for(int j = 0 ; j < kernel_length-1 ; j ++){
			float g = *(kernel+(j+1));
			float f = *(in + (i - j + (kernel_length/2)));	
			tmp_sum += g * f ;
			printf("%f, ",g*f);
		}
		printf("\n");
		*(out+i) = tmp_sum;
	}
	return 0;	
}

int convolve_sse_simple(float* in, float* out, int length,
        float* kernel, int kernel_length)
{
	__m128 f1, f2, g, tmp, f_load;
	float result[8], sum[4], g_arr[4];
	// kernel 의 값을 역순으로 읽기 위해 _mm_loadr_ps() 를 사용했다.
	// 이 경우 g 에 18.750559 -30.940628, -23.380100, 0.0f 순으로 저장된다.
	g = _mm_loadr_ps(kernel);
	_mm_store_ps(g_arr,g);
	for(int i = 0 ; i < length ; i +=4){
		for( int j = i ; j < i + 4 ; j ++){
			//__m128 에서 연산 할 값을 매번 바꿔야 하며 위 표의 방식대로 kernel 과 연산을 해야하기 때문에 _mm_set_ps()를 사용 했다.
	     	// _mm_set_ps() 함수의 경우 전달인자를 역순으로 저장하기 때문에 역순으로 전달.
			f_load = _mm_set_ps(*(in+(j+3)), *(in+(j+2)), *(in+(j+1)), *(in+(j)) );
			tmp = _mm_mul_ps(g,f_load);
			_mm_store_ps(sum,tmp);
			// 실질적으로 저장할 값은 3개이므로 3개만 저장.
			// sum[0] = input[0] * kernel[3] (2)
			// sum[1] = input[1] * kernel[2] (1)
			// sum[2] = input[2] * kernel[1] (0)
			// sum[3] = input[3] * kernel[0] --> 이 경우 kernel = 0.0f
			*(out+j) = sum[0] + sum[1] + sum[2];
		}
	}
    return 0;
}


float float_rand( float min, float max )
{
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * ( max - min );      /* [min, max] */
}

int main(int argc, char** argv)
{

	int verbose;
	if (argc > 1)
		verbose = atoi(argv[1]);
	else
		verbose = 0;

	int input_length = 128;
	int kernel_length = 4;
	float kernel[4] = {0.0f, -23.380100, -30.940628, 18.750559};

	float *data_in = NULL;
	float *naive_data_out = NULL;
	float *sse_data_out = NULL;

	//1d convolution output size calculation formula: output_size=(input_size-kernel_size+2*padding)+stride
	int output_size = (input_length - kernel_length + 2 * 0) + 1; 

	data_in = (float*)malloc(input_length * sizeof(float));
	if (!data_in) {
		printf("malloc error\n");
		return 1;
	}

	naive_data_out = (float*)malloc(output_size * sizeof(float));
	if (!naive_data_out) {
		printf("malloc error\n");
		return 1;
	}

	sse_data_out = (float*)malloc(output_size * sizeof(float));
	if (!sse_data_out) {
		printf("malloc error\n");
		return 1;
	}


	// Let's generate some random data !
	srand(0);
	for (int i = 0; i < input_length; i++) {
		data_in[i] = float_rand(-10.0, 10.0);
	}

	printf("=> Use naive convolution.\n");
	clock_t t = clock();
	convolve_naive(data_in, naive_data_out, input_length, kernel, kernel_length);
	printf("The convolution-loop took %f seconds to execute.\n", ((double)clock() - t)/CLOCKS_PER_SEC);
	printf("======================\n");

	t = clock();
	printf("=> Use SSE convolution.\n");
	convolve_sse_simple(data_in, sse_data_out, input_length, kernel, kernel_length);
	printf("The convolution-loop took %f seconds to execute.\n", ((double)clock() - t)/CLOCKS_PER_SEC);
	printf("\n");

	int matched = 1;
	for (int i = 0; i < output_size; i++){
		printf("naive : %f, sse : %f \n", naive_data_out[i] , sse_data_out[i]);
		if (naive_data_out[i] != sse_data_out[i]) {
			printf("not matched[%d] : naive : %f, sse : %f \n", i, naive_data_out[i], sse_data_out[i]);
			matched = 0;
			break;		
		}
	}
	if (matched == 1)
		printf("The outputs of naive and SSE implementation are MATCHED !\n");
	else
		printf("The outputs of naive and SSE implementation are DIFFERENT. Please check again !\n");

    return 0;
}
