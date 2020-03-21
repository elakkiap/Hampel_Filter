#include "hfilter.h"
// Functions

void updateWindow(int n, pixel_v_t Input_Pixel, pixel_v_t Window[3][3],
		pixel_v_t Copy_Window_1[3][3], pixel_v_t Copy_Window_2[3][3],
		pixel_v_t *Center) {
	int a, b, c, d;
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
		a = getWindow_dim_0_lut[n];
		b = getWindow_dim_1_lut[n];
		c = getCenter_dim_0_lut[n];
		d = getCenter_dim_1_lut[n];
//		a = ((n - 5) / 3) % 3;
//		b = (n - 5) % 3;
//		c = (n / 3) % 3;
//		d = n % 3;
		Window[a][b] = Input_Pixel;
		Copy_Window_1[a][b] = Input_Pixel;
		Copy_Window_2[a][b] = Input_Pixel;
		*Center = Window[c][d];
	}

}

void sortWindow(pixel_v_t Input_Window[3][3], pixel_v_t Sorted_Window[3][3]) {

	pixel_v_t t1, t2, t3, t4;
	int i, j;
	for (i = 0; i < 3; i++) {
#pragma HLS UNROLL
		if (Input_Window[i][0] > Input_Window[i][1]) {
			t1 = Input_Window[i][0];
			t2 = Input_Window[i][1];
		} else {
			t1 = Input_Window[i][1];
			t2 = Input_Window[i][0];
		}
		if (t2 > Input_Window[i][2]) {
			t3 = t2;
			t4 = Input_Window[i][2];
		} else {
			t3 = Input_Window[i][2];
			t4 = t2;
		}
		if (t1 > t3) {
			Sorted_Window[i][0] = t1;
			Sorted_Window[i][1] = t3;
		} else {
			Sorted_Window[i][0] = t3;
			Sorted_Window[i][1] = t1;
		}
		Sorted_Window[i][2] = t4;
	}
}

void findMin(pixel_v_t Input_1, pixel_v_t Input_2, pixel_v_t Input_3,
		pixel_v_t *Min) {
	if (Input_1 <= Input_2) {
		if (Input_1 <= Input_3) {
			*Min = Input_1;
		} else {
			*Min = Input_3;
		}
	} else {
		if (Input_2 <= Input_3) {
			*Min = Input_2;
		} else {
			*Min = Input_3;
		}
	}
}

void findMed(pixel_v_t Input_1, pixel_v_t Input_2, pixel_v_t Input_3,
		pixel_v_t *Med) {

	if (Input_1 >= Input_2) {
		if (Input_2 >= Input_3) {
			*Med = Input_2;
		}
	} else {
		if (Input_1 >= Input_3) {
			*Med = Input_1;
		}
	}

	if (Input_1 >= Input_3) {
		if (Input_3 >= Input_2) {
			*Med = Input_3;
		}
	} else {
		if (Input_1 >= Input_2) {
			*Med = Input_1;
		}
	}

	if (Input_2 >= Input_3) {
		if (Input_3 >= Input_1) {
			*Med = Input_3;
		}
	} else {
		if (Input_2 >= Input_1) {
			*Med = Input_2;
		}
	}
}

void findMax(pixel_v_t Input_1, pixel_v_t Input_2, pixel_v_t Input_3,
		pixel_v_t *Max) {
	if (Input_1 >= Input_2) {
		if (Input_1 >= Input_3) {
			*Max = Input_1;
		} else {
			*Max = Input_3;
		}
	} else {
		if (Input_2 >= Input_3) {
			*Max = Input_2;
		} else {
			*Max = Input_3;
		}
	}
}

void findMedianWindow(pixel_v_t Sorted_Window[3][3], pixel_v_t *Median) {
	pixel_v_t min, med, max;
	findMin(Sorted_Window[0][0], Sorted_Window[1][0], Sorted_Window[2][0],
			&min);

	findMed(Sorted_Window[0][1], Sorted_Window[1][1], Sorted_Window[2][1],
			&med);
	findMax(Sorted_Window[0][2], Sorted_Window[1][2], Sorted_Window[2][2],
			&max);
	findMed(min, med, max, Median);
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

void runFilter(pixel_v_t Window[3][3], pixel_v_t Copy_Window_1[3][3],
		pixel_v_t Copy_Window_2[3][3], pixel_v_t Center, pixel_v_t *Temp_Out) {
#pragma HLS PIPELINE

	pixel_v_t sorted_window[3][3];
	pixel_v_t diff_window[3][3];
	pixel_v_t sorted_diff_window[3][3];
	pixel_v_t median_1, diff_center, median_2;

#pragma HLS ARRAY_PARTITION variable=sorted_window complete dim=1
#pragma HLS ARRAY_PARTITION variable=sorted_window complete dim=2
#pragma HLS ARRAY_PARTITION variable=diff_window complete dim=1
#pragma HLS ARRAY_PARTITION variable=diff_window complete dim=2
#pragma HLS ARRAY_PARTITION variable=sorted_diff_window complete dim=1
#pragma HLS ARRAY_PARTITION variable=sorted_diff_window complete dim=2

	sortWindow(Copy_Window_1, sorted_window);
	findMedianWindow(sorted_window, &median_1);
	findDiffWindow(Copy_Window_2, median_1, diff_window);
	sortWindow(diff_window, sorted_diff_window);
	findMedianWindow(sorted_diff_window, &median_2);
	determineOutlier(Center, median_1, median_2, Temp_Out);
}

void hFilter(pixel_stream *In_Pixel, pixel_stream *Out_Pixel) {
#pragma HLS PIPELINE
	static int n = 0;
	static pixel_v_t Window[3][3], copy_window_1[3][3], copy_window_2[3][3];

	pixel_v_t center, temp_out;
	pixel_stream temp_pixel;

#pragma HLS ARRAY_PARTITION variable=Window complete dim=1
#pragma HLS ARRAY_PARTITION variable=Window complete dim=2
#pragma HLS ARRAY_PARTITION variable=copy_window_1 complete dim=1
#pragma HLS ARRAY_PARTITION variable=copy_window_1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=copy_window_2 complete dim=1
#pragma HLS ARRAY_PARTITION variable=copy_window_2 complete dim=2

	updateWindow(n, (*In_Pixel).v, Window, copy_window_1, copy_window_2,
			&center);
	runFilter(Window, copy_window_1, copy_window_2, center, &temp_out);
	temp_pixel.v = temp_out;
	if ((*In_Pixel).last) {
		temp_pixel.last = 1;
		n = 0;
	} else {
		temp_pixel.last = 0;
		n++;
	}
	*Out_Pixel = temp_pixel;

}
