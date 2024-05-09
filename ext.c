#include <sqlite3ext.h>
#include <stdio.h>

#include <stdlib.h>

SQLITE_EXTENSION_INIT1

typedef struct
{
	double *values;
	int count;
} MedianWindowData;

// add values to current window value
static void window_add_value(MedianWindowData *data, double value)
{
	data->values = realloc(data->values, (data->count + 1) * sizeof(double));
	data->values[data->count] = value;
	data->count++;
}

static int window_compare_doubles(const void *a, const void *b)
{
	double arg1 = *(const double *)a;
	double arg2 = *(const double *)b;

	if (arg1 < arg2)
		return -1;
	if (arg1 > arg2)
		return 1;
	return 0;
}

// calculate median
static double window_calculate_median(MedianWindowData *data)
{
	if (data->count == 0)
		return 0.0;

	qsort(data->values, data->count, sizeof(double), window_compare_doubles);

	if (data->count % 2 == 0)
	{
		return (data->values[data->count / 2 - 1] + data->values[data->count / 2]) / 2.0;
	}
	else
	{
		return data->values[data->count / 2];
	}
}

// sqlite window function step
static void window_median_step(sqlite3_context *context, int argc, sqlite3_value **argv)
{
	MedianWindowData *data = sqlite3_aggregate_context(context, sizeof(MedianWindowData));
	double value = sqlite3_value_double(argv[0]);
	window_add_value(data, value);
}

// finalize function
static void window_median_finalize(sqlite3_context *context)
{
	MedianWindowData *data = sqlite3_aggregate_context(context, 0);
	double median = window_calculate_median(data);
	sqlite3_result_double(context, median);
}

int register_median(sqlite3 *db)
{
	return sqlite3_create_window_function(db, "median", 1, SQLITE_UTF8, 0, window_median_step, window_median_finalize, NULL, NULL, NULL);
}

// register ext
int sqlite3_extension_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
	static int window_ext_loaded = 0;

	if (window_ext_loaded == 1)
	{
		return 0;
	}
	printf("%s\n", "median function extension loaded");
	SQLITE_EXTENSION_INIT2(pApi)

	window_ext_loaded = 1;

	return register_median(db);
}