/*
 Testbench for Hampel Filter to Eliminate Profile-Isolated Outliers in Laser Vision Measurement.
 INPUT: ap_fixed<16,11> [1 x 1920]
 OUTPUT: Array of length 1920, each element in the array is ap_fixed<16,11>

 Jeremy Ford and Elakkia Kanaka Parthipan
 CSE237c - Final Project
 March 9, 2020
 */

#include <stdio.h>
#include "hfilter.h"

struct Rmse {
	int num_sq;
	float sum_sq;
	float error;
	Rmse() {
		num_sq = 0;
		sum_sq = 0;
		error = 0;
	}
	float add_value(float d_n) {
		num_sq++;
		sum_sq += (d_n * d_n);
		error = sqrtf(sum_sq / num_sq);
		return error;
	}
};

Rmse rmse;

pixel_stream IN_IMAGE[SIZE + P_L], OUT_IMAGE[SIZE];

void run_test(const char * In_File, const char * Out_File,
		const char * Comp_File) {

	int i;

	// Open Input, Output, and Comparison Files
	FILE *fin, *fout, *fcomp;
	fin = fopen(In_File, "r");
	fout = fopen(Out_File, "w");
	fcomp = fopen(Comp_File, "r");

	// Get input data
	float input[SIZE], input_fixed[SIZE + P_L];
	for (i = 0; i < SIZE; i++) {
		fscanf(fin, "%f", &input[i]);
	}
	int a = 1;
	for (i = 0; i < SIZE + P_L; i++) {
		if (i == SIZE + P_L - 1) {
			IN_IMAGE[i].last = 1;
		} else {
			IN_IMAGE[i].last = 0;
		}
		if (i < SIZE) {
			IN_IMAGE[i].v = input[i];
		} else {
			IN_IMAGE[i].v = input[i - 2 * a];
			a++;
		}
		input_fixed[i] = (float) IN_IMAGE[i].v;
	}

	// Reset Output Data
	for (i = 0; i < SIZE; i++) {
		OUT_IMAGE[i].v = 0;
	}

	// Run Hampel Filter
	pixel_stream in_pixel, out_pixel;
	for (i = 0; i < SIZE + P_L; i++) {
		in_pixel = IN_IMAGE[i];
		hFilter(&in_pixel, &out_pixel);
		if (i >= P_L) {
			OUT_IMAGE[i - P_L] = out_pixel;
		}
	}

	// Save to Output File
	for (i = 0; i < SIZE; i++) {
		fprintf(fout, "%f\n", (float) OUT_IMAGE[i].v);
	}

	float comp;
	// Compare Output w/ Comparison File
	for (i = 0; i < SIZE; i++) {
		fscanf(fcomp, "%f", &comp);
		rmse.add_value((float) OUT_IMAGE[i].v - comp);
		printf("i = %d, in = %f, out = %f, comp = %f\n", i, input[i],
				(float) OUT_IMAGE[i].v, comp);
	}

	// Close Input, Output, and Comparison Files
	fclose(fin);
	fclose(fcomp);
	fclose(fout);

}

int main() {

	run_test("dhc_outlier_2.dat",
			"D:/UCSD/Wi20/CSE237C/Final Project/Naive_Median_Filter/op_dhc_outlier_2.dat",
			"dhc_ideal.dat");
//	printf("--------------- Real Data ---------------\n");
//	run_test("J:/jford/project/Final/data/input/real/profile_0.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_0.dat",
//		 "J:/jford/project/Final/data/input/real/profile_0.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_1.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_1.dat",
//		 "J:/jford/project/Final/data/input/real/profile_1.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_2.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_2.dat",
//		 "J:/jford/project/Final/data/input/real/profile_2.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_3.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_3.dat",
//		 "J:/jford/project/Final/data/input/real/profile_3.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_4.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_4.dat",
//		 "J:/jford/project/Final/data/input/real/profile_4.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_5.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_5.dat",
//		 "J:/jford/project/Final/data/input/real/profile_5.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_0.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_6.dat",
//		 "J:/jford/project/Final/data/input/real/profile_6.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_7.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_7.dat",
//		 "J:/jford/project/Final/data/input/real/profile_7.dat");
//
//	run_test("J:/jford/project/Final/data/input/real/profile_8.dat",
//		 "J:/jford/project/Final/data/output_median_optimized_1/real/profile_8.dat",
//		 "J:/jford/project/Final/data/input/real/profile_8.dat");
//
//
//	 printf("--------------- LINE ---------------\n");
//	 printf("-------------- ideal ---------------\n");
//	 run_test("J:/jford/project/Final/data/input/line/ideal.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/ideal.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 printf("-------------- noise ---------------\n");
//	 run_test("J:/jford/project/Final/data/input/line/noise.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/noise.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/line/outlier_1.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/outlier_1.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/line/outlier_2.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/outlier_2.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/line/outlier_3.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/outlier_3.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/line/outlier_4.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/outlier_4.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/line/outlier_5.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/outlier_5.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/line/outlier_6.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/outlier_6.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/line/outlier_7.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/line/outlier_7.dat",
//	 "J:/jford/project/Final/data/input/line/ideal.dat");
//
//	 printf("--------------- RAMP ---------------\n");
//	 run_test("J:/jford/project/Final/data/input/ramp/ideal.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/ideal.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/noise.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/noise.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/outlier_1.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/outlier_1.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/outlier_2.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/outlier_2.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/outlier_3.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/outlier_3.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/outlier_4.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/outlier_4.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/outlier_5.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/outlier_5.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/outlier_6.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/outlier_6.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/ramp/outlier_7.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/ramp/outlier_7.dat",
//	 "J:/jford/project/Final/data/input/ramp/ideal.dat");
//
//	 printf("--------------- Half-Circle ---------------\n");
//	 run_test("J:/jford/project/Final/data/input/hc/ideal.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/ideal.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/noise.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/noise.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/outlier_1.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/outlier_1.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/outlier_2.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/outlier_2.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/outlier_3.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/outlier_3.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/outlier_4.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/outlier_4.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/outlier_5.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/outlier_5.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/outlier_6.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/outlier_6.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/hc/outlier_7.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/hc/outlier_7.dat",
//	 "J:/jford/project/Final/data/input/hc/ideal.dat");
//
//	 printf("--------------- Double Half-Circle ---------------\n");
//	 run_test("J:/jford/project/Final/data/input/dhc/ideal.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/ideal.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/noise.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/noise.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/outlier_1.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/outlier_1.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/outlier_2.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/outlier_2.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/outlier_3.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/outlier_3.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/outlier_4.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/outlier_4.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/outlier_5.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/outlier_5.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/outlier_6.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/outlier_6.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");
//	 run_test("J:/jford/project/Final/data/input/dhc/outlier_7.dat",
//	 "J:/jford/project/Final/data/output_median_optimized_1/dhc/outlier_7.dat",
//	 "J:/jford/project/Final/data/input/dhc/ideal.dat");

	printf("---RMS error----------------------------------\n");
	printf("----------------------------------------------\n");
	printf("   RMSE        ");
	printf("%0.15f\n", rmse.error);
	printf("----------------------------------------------\n");

	float error_threshold = 100;

	int success = (rmse.error < error_threshold);

	if (success)
		return 0;
	else
		return 1;
}

