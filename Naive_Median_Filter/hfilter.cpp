#include "hfilter.h"
// Functions

void updateWindow(int n, pixel_v_t Input_Pixel, pixel_v_t Window[3][3],
		pixel_v_t Copy_Window_1[3][3], pixel_v_t Copy_Window_2[3][3],
		pixel_v_t *Center) {
	int a, b;
	a = getWindow_dim_0_lut[n];
	b = getWindow_dim_1_lut[n];
	if (n == 0) {
		Window[1][1] = Input_Pixel;
		Copy_Window_1[1][1] = Input_Pixel;
		Copy_Window_2[1][1] = Input_Pixel;
	} else if (n == 1) {

		Window[1][0] = Input_Pixel;
		Window[1][2] = Input_Pixel;
		Copy_Window_1[1][0] = Input_Pixel;
		Copy_Window_1[1][2] = Input_Pixel;
		Copy_Window_2[1][0] = Input_Pixel;
		Copy_Window_2[1][2] = Input_Pixel;
	} else if (n == 2) {
		Window[0][2] = Input_Pixel;
		Window[2][0] = Input_Pixel;
		Copy_Window_1[0][2] = Input_Pixel;
		Copy_Window_1[2][0] = Input_Pixel;
		Copy_Window_2[0][2] = Input_Pixel;
		Copy_Window_2[2][0] = Input_Pixel;
	} else if (n == 3) {
		Window[0][1] = Input_Pixel;
		Window[2][1] = Input_Pixel;
		Copy_Window_1[0][1] = Input_Pixel;
		Copy_Window_1[2][1] = Input_Pixel;
		Copy_Window_2[0][1] = Input_Pixel;
		Copy_Window_2[2][1] = Input_Pixel;

	} else if (n == 4) {
		Window[0][0] = Input_Pixel;
		Copy_Window_1[0][0] = Input_Pixel;
		Copy_Window_2[0][0] = Input_Pixel;
		Window[2][2] = Input_Pixel;
		Copy_Window_1[2][2] = Input_Pixel;
		Copy_Window_2[2][2] = Input_Pixel;
		*Center = Window[1][1];

	} else {

		Window[a][b] = Input_Pixel;
		Copy_Window_1[a][b] = Input_Pixel;
		Copy_Window_2[a][b] = Input_Pixel;
		*Center = Window[getCenter_dim_0_lut[n]][getCenter_dim_1_lut[n]];
	}

}

void sortWindow(pixel_v_t Input_Window[3][3], pixel_v_t *Median) {

	pixel_v_t t1, t2, t3, t4, sort_1;
	pixel_v_t temp_1[9], temp_2[9];
#pragma HLS ARRAY_PARTITION variable=temp_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=temp_2 complete dim=1
	int i, j, k = 0;
	for (int i = 0; i < 3; i++) {
#pragma HLS UNROLL
		for (int j = 0; j < 3; j++) {
#pragma HLS UNROLL
			temp_1[k++] = Input_Window[i][j];
		}
	}
	for (int j = 1; j < 9; j++) {
#pragma HLS UNROLL
		sort_1 = temp_1[j];
		k = j;
		sortWindow_label0: while (k > 0 && temp_1[k - 1] > sort_1) {
			temp_2[k] = temp_1[k - 1];
			k--;
		}
		temp_2[k] = sort_1;
	}
	*Median = temp_2[4];
}

void findDiffWindow(pixel_v_t Window[3][3], pixel_v_t Median,
		pixel_v_t Diff_Window[3][3]) {
	int i, j;
	for (i = 0; i < 3; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 3; j++) {
#pragma HLS UNROLL
			if (Window[i][j] > Median) {
				Diff_Window[i][j] = Window[i][j] - Median;
			} else {
				Diff_Window[i][j] = Median - Window[i][j];
			}
		}
	}
}

void determineOutlier(pixel_v_t Center, pixel_v_t Median_1, pixel_v_t Median_2,
		pixel_v_t *Output) {

	pixel_v_t K, T, diff_center;
	K = 3;
	T = 1.5;

	if (Center >= Median_1) {
		diff_center = Center - Median_1;
	} else {
		diff_center = Median_1 - Center;
	}

	if (diff_center <= T * K * Median_2) {
		*Output = Center;
	} else {
		*Output = Median_1;
	}
}

void assignOutput(pixel_v_t Input, pixel_stream *Output) {
	(*Output).v = Input;
}

void runFilter(int m, pixel_v_t Window[3][3], pixel_v_t Copy_Window_1[3][3],
		pixel_v_t Copy_Window_2[3][3], pixel_v_t Center, pixel_v_t *Temp_Out) {
#pragma HLS PIPELINE

	pixel_v_t diff_window[3][3];
#pragma HLS ARRAY_PARTITION variable=diff_window complete dim=1
	pixel_v_t median_1, diff_center, median_2;

	sortWindow(Copy_Window_1, &median_1);
	findDiffWindow(Copy_Window_2, median_1, diff_window);
	sortWindow(diff_window, &median_2);
	determineOutlier(Center, median_1, median_2, Temp_Out);

}

void hFilter(pixel_stream *In_Pixel, pixel_stream *Out_Image) {
#pragma HLS PIPELINE
	static int n = 0;
	static pixel_v_t Window[3][3], copy_window_1[3][3], copy_window_2[3][3];
	int x, z;
	x = n;
	z = n;
	pixel_v_t center, temp_out;
#pragma HLS ARRAY_PARTITION variable=Window complete dim=1
#pragma HLS ARRAY_PARTITION variable=copy_window_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=copy_window_2 complete dim=1
	updateWindow(x, (*In_Pixel).v, Window, copy_window_1, copy_window_2,
			&center);
	runFilter(z, Window, copy_window_1, copy_window_2, center, &temp_out);
	assignOutput(temp_out, Out_Image);
	if ((*In_Pixel).last) {
		n = 0;
	} else {
		n++;
	}

}
