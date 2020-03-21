#include "hfilter.h"
// Functions

void updateWindow(int n, pixel_v_t Input_Pixel, pixel_v_t Window[3][3],
		pixel_v_t Copy_Window_1[3][3], pixel_v_t *Center) {
	int a, b;
	a = getWindow_dim_0_lut[n];
	b = getWindow_dim_1_lut[n];
	if (n == 0) {
		Window[1][1] = Input_Pixel;
		Copy_Window_1[1][1] = Input_Pixel;
	} else if (n == 1) {

		Window[1][0] = Input_Pixel;
		Window[1][2] = Input_Pixel;
		Copy_Window_1[1][0] = Input_Pixel;
		Copy_Window_1[1][2] = Input_Pixel;
	} else if (n == 2) {
		Window[0][2] = Input_Pixel;
		Window[2][0] = Input_Pixel;
		Copy_Window_1[0][2] = Input_Pixel;
		Copy_Window_1[2][0] = Input_Pixel;
	} else if (n == 3) {
		Window[0][1] = Input_Pixel;
		Window[2][1] = Input_Pixel;
		Copy_Window_1[0][1] = Input_Pixel;
		Copy_Window_1[2][1] = Input_Pixel;

	} else if (n == 4) {
		Window[0][0] = Input_Pixel;
		Copy_Window_1[0][0] = Input_Pixel;
		Window[2][2] = Input_Pixel;
		Copy_Window_1[2][2] = Input_Pixel;
		*Center = Window[1][1];

	} else {

		Window[a][b] = Input_Pixel;
		Copy_Window_1[a][b] = Input_Pixel;
		*Center = Window[getCenter_dim_0_lut[n]][getCenter_dim_1_lut[n]];
	}

}

void getMean(pixel_v_t Window[3][3], pixel_v_t *Mean, pixel_v_t *Copy_Mean) {
	ap_fixed<32, 24> mean = 0;
	int i, j;
	for (i = 0; i < 3; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 3; j++) {
#pragma HLS UNROLL
			mean = mean + Window[i][j];
		}
	}
	*Mean = mean / 9;
	*Copy_Mean = *Mean;
}

void getStdev(pixel_v_t Window[3][3], pixel_v_t Mean, pixel_v_t *Stdev) {
	pixel_v_t a, b, c, d;
	c = 0;
	int i, j;
	for (i = 0; i < 3; i++) {
#pragma HLS UNROLL
		for (j = 0; j < 3; j++) {
#pragma HLS UNROLL
			a = (Window[i][j] - Mean);
			b = a * a;
			c += b;
		}
	}
	d = c / 9;
	*Stdev = sqrt((float) d);
}

void determineOutlier(pixel_v_t Center, pixel_v_t Mean, pixel_v_t Stdev,
		pixel_v_t *Output) {

	pixel_v_t diff_center;

	if (Center >= Mean) {
		diff_center = Center - Mean;
	} else {
		diff_center = Mean - Center;
	}

	if (diff_center <= 3 * Stdev) {
		*Output = Center;
	} else {
		*Output = Mean;
	}

}

void assignOutput(pixel_v_t Input, pixel_stream *Output) {
	(*Output).v = Input;
}

void hFilter(pixel_stream *In_Pixel, pixel_stream *Out_Image) {

#pragma HLS PIPELINE
	static int n = 0;
	static pixel_v_t Window[3][3], copy_window[3][3];
	pixel_v_t center, mean, copy_mean, stdev, temp_out;

#pragma HLS ARRAY_PARTITION variable=Window complete dim=1
#pragma HLS ARRAY_PARTITION variable=Window complete dim=2
#pragma HLS ARRAY_PARTITION variable=copy_window complete dim=1
#pragma HLS ARRAY_PARTITION variable=copy_window complete dim=2

	updateWindow(n, (*In_Pixel).v, Window, copy_window, &center);
	getMean(Window, &mean, &copy_mean);
	getStdev(copy_window, mean, &stdev);
	determineOutlier(center, copy_mean, stdev, &temp_out);
	assignOutput(temp_out, Out_Image);
	if ((*In_Pixel).last) {
		n = 0;
	} else {
		n++;
	}

}
