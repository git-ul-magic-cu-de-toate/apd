// Author: APD team, except where source was noted

#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#define CONTOUR_CONFIG_COUNT    16
#define FILENAME_MAX_SIZE       50
#define STEP                    8
#define SIGMA                   200
#define RESCALE_X               2048
#define RESCALE_Y               2048

#define CLAMP(v, min, max) if(v < min) { v = min; } else if(v > max) { v = max; }

pthread_barrier_t barrier;

typedef struct {
    int nr_th;
    int thread_id;
    int step_x;
    int step_y;
    unsigned char **grid;
    int rescale_flag;
    ppm_image *scaled_image;
    ppm_image *image;
    ppm_image **contour_map;
} pthread_args;

// Creates a map between the binary configuration (e.g. 0110_2) and the corresponding pixels
// that need to be set on the output image. An array is used for this map since the keys are
// binary numbers in 0-15. Contour images are located in the './contours' directory.
ppm_image **init_contour_map() {
    ppm_image **map = (ppm_image **)malloc(CONTOUR_CONFIG_COUNT * sizeof(ppm_image *));
    if (!map) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
        char filename[FILENAME_MAX_SIZE];
        sprintf(filename, "./contours/%d.ppm", i);
        map[i] = read_ppm(filename);
    }

    return map;
}

// Updates a particular section of an image with the corresponding contour pixels.
// Used to create the complete contour image.
void update_image(ppm_image *image, ppm_image *contour, int x, int y) {
    for (int i = 0; i < contour->x; i++) {
        for (int j = 0; j < contour->y; j++) {
            int contour_pixel_index = contour->x * i + j;
            int image_pixel_index = (x + i) * image->y + y + j;

            image->data[image_pixel_index].red = contour->data[contour_pixel_index].red;
            image->data[image_pixel_index].green = contour->data[contour_pixel_index].green;
            image->data[image_pixel_index].blue = contour->data[contour_pixel_index].blue;
        }
    }
}

// Corresponds to step 1 of the marching squares algorithm, which focuses on sampling the image.
// Builds a p x q grid of points with values which can be either 0 or 1, depending on how the
// pixel values compare to the `sigma` reference value. The points are taken at equal distances
// in the original image, based on the `step_x` and `step_y` arguments.
unsigned char **sample_grid(ppm_image *image, int step_x, int step_y, unsigned char sigma) {
    int p = image->x / step_x;
    int q = image->y / step_y;

    unsigned char **grid = (unsigned char **)malloc((p + 1) * sizeof(unsigned char*));
    if (!grid) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    for (int i = 0; i <= p; i++) {
        grid[i] = (unsigned char *)malloc((q + 1) * sizeof(unsigned char));
        if (!grid[i]) {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }

    for (int i = 0; i < p; i++) {
        for (int j = 0; j < q; j++) {
            ppm_pixel curr_pixel = image->data[i * step_x * image->y + j * step_y];

            unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

            if (curr_color > sigma) {
                grid[i][j] = 0;
            } else {
                grid[i][j] = 1;
            }
        }
    }
    grid[p][q] = 0;

    // last sample points have no neighbors below / to the right, so we use pixels on the
    // last row / column of the input image for them
    for (int i = 0; i < p; i++) {
        ppm_pixel curr_pixel = image->data[i * step_x * image->y + image->x - 1];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[i][q] = 0;
        } else {
            grid[i][q] = 1;
        }
    }
    for (int j = 0; j < q; j++) {
        ppm_pixel curr_pixel = image->data[(image->x - 1) * image->y + j * step_y];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > sigma) {
            grid[p][j] = 0;
        } else {
            grid[p][j] = 1;
        }
    }

    return grid;
}

// Calls `free` method on the utilized resources.
void free_resources(ppm_image *image, ppm_image **contour_map, unsigned char **grid, int step_x, pthread_args *pth_args) {
    for (int i = 0; i < CONTOUR_CONFIG_COUNT; i++) {
        free(contour_map[i]->data);
        free(contour_map[i]);
    }
    free(contour_map);

    for (int i = 0; i <= image->x / step_x; i++) {
        free(grid[i]);
    }
    free(grid);

    free(image->data);
    free(image);

    free(pth_args);
}

ppm_image *rescale_image(ppm_image *image) {
    uint8_t sample[3];

    // we only rescale downwards
    if (image->x <= RESCALE_X && image->y <= RESCALE_Y) {
        return image;
    }

    // alloc memory for image
    ppm_image *new_image = (ppm_image *)malloc(sizeof(ppm_image));
    if (!new_image) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    new_image->x = RESCALE_X;
    new_image->y = RESCALE_Y;

    new_image->data = (ppm_pixel*)malloc(new_image->x * new_image->y * sizeof(ppm_pixel));
    if (!new_image) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    // use bicubic interpolation for scaling
    for (int i = 0; i < new_image->x; i++) {
        for (int j = 0; j < new_image->y; j++) {
            float u = (float)i / (float)(new_image->x - 1);
            float v = (float)j / (float)(new_image->y - 1);
            sample_bicubic(image, u, v, sample);

            new_image->data[i * new_image->y + j].red = sample[0];
            new_image->data[i * new_image->y + j].green = sample[1];
            new_image->data[i * new_image->y + j].blue = sample[2];
        }
    }

    free(image->data);
    free(image);

    return new_image;
}

// The thread function that implements the marching squares algorithm
// for a specific range of rows in the sampled grid. It processes the
// grid points, identifies the corresponding contour configuration, and
// updates the scaled image
void* threadFunction(void *args)
{
    uint8_t sample[3];
    pthread_args* pth_arg=(pthread_args*)args;

    int p = pth_arg->scaled_image->x / pth_arg->step_x;
    int q = pth_arg->scaled_image->y / pth_arg->step_y;
    // Calculate the start and end indices for rows assigned to this thread

    if(pth_arg->rescale_flag==1)
    {
        int start_r=pth_arg->thread_id * ceil((double)pth_arg->scaled_image->x/ pth_arg->nr_th);
        int end_r=fmin(pth_arg->scaled_image->x, (pth_arg->thread_id + 1) * ceil((double)pth_arg->scaled_image->x / pth_arg->nr_th));

        for (int i = start_r; i < end_r; i++) {
        for (int j = 0; j < pth_arg->scaled_image->y; j++) {
            float u = (float)i / (float)(pth_arg->scaled_image->x - 1);
            float v = (float)j / (float)(pth_arg->scaled_image->y - 1);
            sample_bicubic(pth_arg->image, u, v, sample);

            pth_arg->scaled_image->data[i * pth_arg->scaled_image->y + j].red = sample[0];
            pth_arg->scaled_image->data[i * pth_arg->scaled_image->y + j].green = sample[1];
            pth_arg->scaled_image->data[i * pth_arg->scaled_image->y + j].blue = sample[2];
        }
        pthread_barrier_wait(&barrier);
    }
    }


    int start=pth_arg->thread_id * ceil((double)p/ pth_arg->nr_th);
    int end=fmin(p, (pth_arg->thread_id + 1) * ceil((double)p / pth_arg->nr_th));

    for (int i = start; i < end; i++) {
        for (int j = 0; j < q; j++) {

            ppm_pixel curr_pixel = pth_arg->scaled_image->data[i * pth_arg->step_x * pth_arg->scaled_image->y + j * pth_arg->step_y];

            unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

            if (curr_color > SIGMA) {
                pth_arg->grid[i][j] = 0;
            } else {
                pth_arg->grid[i][j] = 1;
            }

        }
        ppm_pixel curr_pixel = pth_arg->scaled_image->data[i * pth_arg->step_x * pth_arg->scaled_image->y + pth_arg->scaled_image->x - 1];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > SIGMA) {
            pth_arg->grid[i][q] = 0;
        } else {
            pth_arg->grid[i][q] = 1;
        }

    }
    // pthread_barrier_wait(pth_arg->barrier);
    if(end==p){
        for (int j = 0; j < q; j++) {
        ppm_pixel curr_pixel = pth_arg->scaled_image->data[(pth_arg->scaled_image->x - 1) * pth_arg->scaled_image->y + j * pth_arg->step_y];

        unsigned char curr_color = (curr_pixel.red + curr_pixel.green + curr_pixel.blue) / 3;

        if (curr_color > SIGMA) {
            pth_arg->grid[p][j] = 0;
        } else {
            pth_arg->grid[p][j] = 1;
        }
        }
        pth_arg->grid[p][q] = 0;
    }



    for (int i = start; i < end; i++) {
        for (int j = 0; j < q; j++) {
            unsigned char k = 8 * pth_arg->grid[i][j] + 4 * pth_arg->grid[i][j + 1] + 2 * pth_arg->grid[i + 1][j + 1] + 1 * pth_arg->grid[i + 1][j];
            update_image(pth_arg->scaled_image, pth_arg->contour_map[k], i * pth_arg->step_x, j * pth_arg->step_y);
        }
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: ./tema1 <in_file> <out_file> <nr_threaduri>\n");
        return 1;
    }

    ppm_image *image = read_ppm(argv[1]);
    int nr_th=atoi(argv[3]);
    int step_x = STEP;
    int step_y = STEP;
    int r;

    pthread_args* pth_args=(pthread_args*)malloc(sizeof(pthread_args)*nr_th);

    // 0. Initialize contour map
    ppm_image **contour_map = init_contour_map();
    ppm_image *scaled_image = (ppm_image *)malloc(sizeof(ppm_image));
    if (!scaled_image) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }
    scaled_image->x = RESCALE_X;
    scaled_image->y = RESCALE_Y;

    scaled_image->data = (ppm_pixel*)malloc(scaled_image->x * scaled_image->y * sizeof(ppm_pixel));
    if (!scaled_image->data) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    int flg=1;

    if (image->x <= RESCALE_X && image->y <= RESCALE_Y) {
        scaled_image = image;
        flg = 0;
    }

    // 1. Rescale the image
    // ppm_image *scaled_image = rescale_image(image);

    // 2. Sample the grid
    // unsigned char **grid = sample_grid(scaled_image, step_x, step_y, SIGMA);

    int p = scaled_image->x / step_x;
    int q = scaled_image->y / step_y;

    pthread_barrier_init(&barrier, NULL, nr_th);

    unsigned char **grid = (unsigned char **)malloc((p + 1) * sizeof(unsigned char*));
    if (!grid) {
        fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
    }

    for (int i = 0; i <= p; i++) {
        grid[i] = (unsigned char *)malloc((q + 1) * sizeof(unsigned char));
        if (!grid[i]) {
            fprintf(stderr, "Unable to allocate memory\n");
            exit(1);
        }
    }

    pthread_t threads[nr_th];
    // 3. March the squares
    for (int id = 0; id < nr_th; id++)
    {
        pth_args[id].nr_th=nr_th;
        pth_args[id].thread_id = id;
        pth_args[id].step_x=step_x;
        pth_args[id].step_y=step_y;
        pth_args[id].scaled_image=scaled_image;
        pth_args[id].grid=grid;
        pth_args[id].image=image;
        pth_args[id].contour_map=contour_map;
        pth_args[id].rescale_flag = flg;
        r = pthread_create(&(threads[id]), NULL, threadFunction, &(pth_args[id]));
        if (r) {
            printf("Eroare la crearea thread-ului %d\n", id);
            exit(-1);
        }
    }

    for (int id = 0; id < nr_th; id++)
    {
        r=pthread_join(threads[id], NULL);
        if (r) {
            printf("Eroare la asteptarea thread-ului %d\n", id);
            exit(-1);
        }
    }

    // 4. Write output
    write_ppm(pth_args[0].scaled_image, argv[2]);

    free_resources(scaled_image, contour_map, grid, step_x, pth_args);


    return 0;
}
