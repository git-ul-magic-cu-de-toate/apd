#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int P;
int **result_julia;
int **result_mandelbrot;

static pthread_barrier_t barrier;

// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

typedef struct function_arg {
	struct arguments {
		int height;
		int width;
		params par;
	} arguments;

	struct arguments julia;
	struct arguments mandelbrot;
	int thread_id;
} function_arg;

// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 5) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

static inline int min(int a, int b) {
	return a < b ? a : b;
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

// Ruleaza algoritmul julia.
static inline void run_julia(void *arg) {
	function_arg *args = (function_arg *) arg;
	int start_julia = args->thread_id * ceil((double) args->julia.width / P);
	int end_julia = min((args->thread_id + 1) * ceil((double)args->julia.width / P), args->julia.width);
	int w, h, i;

	for (w = start_julia; w < end_julia; ++w) {
		for (h = 0; h < args->julia.height; ++h) {
			int step = 0;
			complex z = { .a = w * args->julia.par.resolution + args->julia.par.x_min,
							.b = h * args->julia.par.resolution + args->julia.par.y_min };

			while ((z.a * z.a) + (z.b * z.b) < 4.0 && step < args->julia.par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };
				z.a = z_aux.a * z_aux.a - z_aux.b * z_aux.b + args->julia.par.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + args->julia.par.c_julia.b;
				step++;
			}

			result_julia[h][w] = step & 255;
		}
	}

	pthread_barrier_wait(&barrier);
	start_julia = args->thread_id * ceil((double) (args->julia.height / 2) / P);
	end_julia = min((args->thread_id + 1) * ceil((double)(args->julia.height / 2) / P), (args->julia.height / 2));
	
	// transforma rezultatul din coordonate matematice in coordonate ecran
	for (i = start_julia; i < end_julia; i++) {
		int *aux = result_julia[i];
		result_julia[i] = result_julia[args->julia.height - i - 1];
		result_julia[args->julia.height - i - 1] = aux;
	}

	pthread_barrier_wait(&barrier);

	if (args->thread_id == P - 1) {
		write_output_file(out_filename_julia, result_julia, args->julia.width, args->julia.height);
	}
}

// Ruleaza algoritmul mandelbrot
static inline void run_mandelbrot(void *arg) {
	function_arg *args = (function_arg *) arg;
	int start_mandel = args->thread_id * ceil((double) args->mandelbrot.width / P);
	int end_mandel = min((args->thread_id + 1) * ceil((double) args->mandelbrot.width / P), args->mandelbrot.width);
	int w, h, i;

	for (w = start_mandel; w < end_mandel; ++w) {
		for (h = 0; h < args->mandelbrot.height; ++h) {
			complex c = { .a = w * args->mandelbrot.par.resolution + args->mandelbrot.par.x_min,
							.b = h * args->mandelbrot.par.resolution + args->mandelbrot.par.y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while ((z.a * z.a) + (z.b * z.b) < 4.0 && step < args->mandelbrot.par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };
				z.a = z_aux.a * z_aux.a - z_aux.b * z_aux.b + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result_mandelbrot[h][w] = step & 255;
		}
	}

	pthread_barrier_wait(&barrier);
	start_mandel = args->thread_id * ceil((double) (args->mandelbrot.height / 2) / P);
	end_mandel = min((args->thread_id + 1) * ceil((double)(args->mandelbrot.height / 2) / P), (args->mandelbrot.height / 2));

	// transforma rezultatul din coordonate matematice in coordonate ecran	
	for (i = start_mandel; i < end_mandel; i++) {
		int *aux = result_mandelbrot[i];
		result_mandelbrot[i] = result_mandelbrot[args->mandelbrot.height - i - 1];
		result_mandelbrot[args->mandelbrot.height - i - 1] = aux;
	}

	pthread_barrier_wait(&barrier);

	if (args->thread_id == P - 1) {
		write_output_file(out_filename_mandelbrot, result_mandelbrot, args->mandelbrot.width, args->mandelbrot.height);
	}

	free(args);
}


void *thread_function(void *arg) {
	run_julia(arg);
	run_mandelbrot(arg);
	pthread_exit(NULL);
}

/*
	Aloca memorie pentru structura ce va fi pasata functiei threadurilor
	si seteaza campurile corespunzator.
*/
static inline function_arg* set_arg(const int width_julia, const int height_julia,
									const int width_mandelbrot, const int height_mandelbrot,
									const params par_julia, const params par_mandelbrot,
									const int id) {
	function_arg *arg = malloc(sizeof(*arg));
	
	if (arg == NULL) {
		fprintf(stderr, "Eroare la malloc!\n");
		exit(EXIT_FAILURE);
	}

	arg->thread_id = id;
	arg->julia.height = height_julia;
	arg->julia.width = width_julia;
	arg->julia.par = par_julia;
	arg->mandelbrot.height = height_mandelbrot;
	arg->mandelbrot.width = width_mandelbrot;
	arg->mandelbrot.par = par_mandelbrot;

	return arg;
}

int main(int argc, char *argv[])
{
	params par_julia, par_mandelbrot;
	int width_julia, height_julia;
	int width_mandelbrot, height_mandelbrot;
	int r;
	get_args(argc, argv);
	
	read_input_file(in_filename_julia, &par_julia);
	read_input_file(in_filename_mandelbrot, &par_mandelbrot);

	width_julia = (par_julia.x_max - par_julia.x_min) / par_julia.resolution;
	height_julia = (par_julia.y_max - par_julia.y_min) / par_julia.resolution;

	width_mandelbrot = (par_mandelbrot.x_max - par_mandelbrot.x_min) / par_mandelbrot.resolution;
	height_mandelbrot = (par_mandelbrot.y_max - par_mandelbrot.y_min) / par_mandelbrot.resolution;

	result_julia = allocate_memory(width_julia, height_julia);
	result_mandelbrot = allocate_memory(width_mandelbrot, height_mandelbrot);
	
	pthread_t threads[P];
	pthread_barrier_init(&barrier, NULL, P);

	for (int i = 0 ; i < P ; ++i) {
		function_arg *arg = set_arg(width_julia, height_julia, width_mandelbrot, height_mandelbrot,
									par_julia, par_mandelbrot, i);
		r = pthread_create(&threads[i], NULL, thread_function, (void *) arg);

		if (r) {
			exit(EXIT_FAILURE);
		}
	}

	void *status;
	
	for (int i = 0 ; i < P ; ++i) {
		r = pthread_join(threads[i], &status);
		if (r) {
			exit(EXIT_FAILURE);
		}
	}

	free_memory(result_julia, height_julia);
	free_memory(result_mandelbrot, height_mandelbrot);
	r = pthread_barrier_destroy(&barrier);
	
	if (r) {
		exit(EXIT_FAILURE);
	}

	return 0;
}