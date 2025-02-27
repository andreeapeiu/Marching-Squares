// Copyright Peiu Andreea - 324CA refacere PCLP1
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIE(assertion, message) \
	do { \
		if (assertion) { \
			fprintf(stderr, "Error: %s\n", message); \
			exit(EXIT_FAILURE); \
		} \
	} while (0)

#define NMAX 100

typedef struct {
	int r, g, b;
} color_rgb;

typedef struct {
	char magic_word[3];
	int width;
	int height;
	int loaded; // 0 or 1
	int max_val;
	int **binary; // for grid
	int has_grid;
	color_rgb **colors;
} image;

void dealloc_color(color_rgb **v, int n)
{
	if (!v)
		return;
	int i;
	for (i = 0; i < n; i++)
		if (v[i])
			free(v[i]);
	free(v);
}

void dealloc_matrix(int **v, int n)
{
	if (!v)
		return;
	for (int i = 0; i < n; i++)
		if (v[i])
			free(v[i]);
	free(v);
	v = NULL;
}

void dealloc_color1(color_rgb ***v, int n)
{
	int i;
	for (i = 0; i < n; i++)
		free(*v[i]);
	free(*v);
}

void dealloc_matrix1(int ***v, int n)
{
	for (int i = 0; i < n; i++)
		free(*v[i]);
	free(*v);
}

void exit_program(image *img)
{
	// Eliberez toate resursele programului
	if (img->loaded != 1) {
		printf("No image loaded\n");
		return;
	}

	if (img->loaded == 1) {
		dealloc_color(img->colors, img->height);
		dealloc_matrix(img->binary, img->height / 4 + 1);

		img->loaded = 0;
		img->height = 0;
		img->width = 0;

		printf("Gigel a terminat\n");
	}
	free(img);
}

int **alloc_matrix(int l, int c)
{
	// Aloc matricea binary
	int **v = (int **)calloc(l, sizeof(int *));
	DIE(!v, "Failed to allocate memory\n");

	for (int i = 0; i < l; i++) {
		v[i] = (int *)calloc(c, sizeof(int));
		DIE(!v[i], "Failed to allocate memory\n");
	}
	return v;
}

color_rgb **alloc_color(int l, int c)
{
	// Aloc matricea r, g, b
	color_rgb **v = (color_rgb **)malloc(l * sizeof(color_rgb *));
	DIE(!v, "Failed to allocate memory\n");

	for (int i = 0; i < l; i++) {
		v[i] = (color_rgb *)malloc(c * sizeof(color_rgb));
		DIE(!v[i], "Failed to allocate memory\n");
	}
	return v;
}

void free_image_memory(image *img)
{
	for (int k = 0; k < img->height; k++) {
		free(img->colors[k]);
	}
	free(img->colors);
	free(img);
}

int validate_pixel(color_rgb pixel, int max_val)
{
	return (pixel.r >= 0 && pixel.r <= max_val &&
			pixel.g >= 0 && pixel.g <= max_val &&
			pixel.b >= 0 && pixel.b <= max_val);
}

void read(image *img, int *it_worked)
{
	scanf("%s", img->magic_word);

	if (strcmp(img->magic_word, "P3") != 0) {
		printf("Eroare: trebuie sa fie P3\n");
		*it_worked = 0;
		free(img);
		return;
	}

	scanf("%d %d", &img->width, &img->height);
	scanf("%d", &img->max_val);

	img->colors = alloc_color(img->height, img->width);

	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			if (scanf("%d %d %d",
					  &img->colors[i][j].r,
					  &img->colors[i][j].g,
					  &img->colors[i][j].b) != 3) {
				printf("Eroare: eroare citire pixeli\n");
				free_image_memory(img);
				*it_worked = 0;
				return;
			}

			if (!validate_pixel(img->colors[i][j], img->max_val)) {
				printf("Eroare: valoare pixel necorespunzatoare\n");
				free_image_memory(img);
				*it_worked = 0;
				return;
			}
		}
	}

	*it_worked = 1;
	img->loaded = 1;

	printf("Imagine citita cu succes [%d-%d]\n", img->height, img->width);
}

void write(image *img)
{
	if (img->loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	// Afisez matricea de pixeli in format PPM
	if (img->loaded == 1) {
		printf("P3\n");
		printf("%d %d\n", img->width, img->height);
		printf("%d\n", img->max_val);
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				printf("%d %d %d\n", img->colors[i][j].r,
					   img->colors[i][j].g, img->colors[i][j].b);
			}
		}
	}
}

void resize(image *img)
{
	if (img->loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	int resize_factor = 4;

	int new_width = img->width * resize_factor;
	int new_height = img->height * resize_factor;

	color_rgb **new_colors = alloc_color(new_height, new_width);

	// Multiplic fiecare pixel de 4 ori
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			color_rgb pixel = img->colors[i][j];
			int start_row = i * resize_factor;
			int start_col = j * resize_factor;

			for (int k = 0; k < resize_factor; k++) {
				for (int l = 0; l < resize_factor; l++) {
					new_colors[start_row + k][start_col + l] = pixel;
				}
			}
		}
	}

	dealloc_color(img->colors, img->height);

	img->colors = new_colors;
	img->width = new_width;
	img->height = new_height;

	printf("Imagine redimensionata cu succes [%d-%d]\n",
		   img->height, img->width);
}

// Helper pentru calcularea mediei pixelilor
int **calculate_pixel_averages(image *img)
{
	int **medii = alloc_matrix(img->height, img->width);
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			medii[i][j] = (img->colors[i][j].r + img->colors[i][j].g +
						   img->colors[i][j].b) / 3;
		}
	}
	return medii;
}

// Verific si adaug vecini
void add_neighbor(int **medii, int x, int y, int *sum, int *count,
				  int height, int width)
{
	if (x >= 0 && x < height && y >= 0 && y < width) {
		*sum += medii[x][y];
		(*count)++;
	}
}

int **calculate_nodes(int **medii, image *img, int step, int M, int N)
{
	int **noduri = alloc_matrix(M + 1, N + 1);

	for (int i = 0; i <= M; i++) {
		for (int j = 0; j <= N; j++) {
			int sum = 0, count = 0;

			// Pozitia curenta
			add_neighbor(medii, i * step, j * step, &sum,
						 &count, img->height, img->width);

			// Vecinii diagonali
			add_neighbor(medii, i * step - 1, j * step - 1, &sum,
						 &count, img->height, img->width); // Nord-Vest
			add_neighbor(medii, i * step - 1, j * step + 1, &sum,
						 &count, img->height, img->width); // Nord-Est
			add_neighbor(medii, i * step + 1, j * step - 1, &sum,
						 &count, img->height, img->width); // Sud-Vest
			add_neighbor(medii, i * step + 1, j * step + 1, &sum,
						 &count, img->height, img->width); // Sud-Est

			if (count)
				noduri[i][j] = sum / count;
			else
				noduri[i][j] = 0;
		}
	}
	return noduri;
}

// Helper pentru generarea matricei binare
int **generate_binary_matrix(int **noduri, int M, int N)
{
	int **binary = alloc_matrix(M + 1, N + 1);

	for (int i = 0; i <= M; i++)
		for (int j = 0; j <= N; j++)
			// Compar cu threshold-ul 200
			if (noduri[i][j] <= 200)
				binary[i][j] = 1;
			else
				binary[i][j] = 0;
	return binary;
}

void grid(image *img)
{
	if (img->loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	int step = 4;
	int N = img->width / step;
	int M = img->height / step;

	// Calculez media aritmetica a pixelilor
	int **medii = calculate_pixel_averages(img);
	// Calculez media vecinilor
	int **noduri = calculate_nodes(medii, img, step, M, N);
	// Umplu matricea binara cu 0 sau 1
	img->binary = generate_binary_matrix(noduri, M, N);

	printf("Grid calculat cu succes [%d-%d]\n", M, N);

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++)
			printf("%d ", img->binary[i][j]);
		printf("\n");
	}

	dealloc_matrix(medii, img->height);
	dealloc_matrix(noduri, M + 1);
}

void grid_for_march(image *img)
{
	if (img->loaded == 0) {
		printf("No image loaded\n");
		return;
	}

	int step = 4;
	int N = img->width / step;
	int M = img->height / step;

	int **medii = calculate_pixel_averages(img);
	int **noduri = calculate_nodes(medii, img, step, M, N);
	img->binary = generate_binary_matrix(noduri, M, N);

	dealloc_matrix(medii, img->height);
	dealloc_matrix(noduri, M + 1);
}

void initialize_configurations(int contours[16][4][4])
{
	// Configuratiile pentru contururi
	int tmp[16][4][4] = {
		// Toate inactive
		{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},

		// SV activ
		{{0, 0, 0, 0}, {0, 0, 0, 0}, {255, 0, 0, 0}, {180, 255, 0, 0}},

		// SE activ
		{{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 255}, {0, 0, 255, 180}},

		// SV și SE active
		{{0, 0, 0, 0}, {0, 0, 0, 0}, {255, 255, 255, 255},
		 {180, 180, 180, 180}},

		// NE activ
		{{0, 0, 255, 180}, {0, 0, 0, 255}, {0, 0, 0, 0}, {0, 0, 0, 0}},

		// NE SV active
		{{0, 0, 255, 180}, {0, 0, 0, 255}, {255, 0, 0, 0}, {180, 255, 0, 0}},

		// NE SE active
		{{0, 0, 255, 180}, {0, 0, 255, 180}, {0, 0, 255, 180},
		 {0, 0, 255, 180}},

		// NE SV SE active
		{{0, 255, 180, 180}, {255, 180, 180, 180}, {180, 180, 180, 180},
		 {180, 180, 180, 180}},

		// NV activ
		{{180, 255, 0, 0}, {255, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},

		// NV SV active
		{{180, 255, 0, 0}, {180, 255, 0, 0}, {180, 255, 0, 0},
		 {180, 255, 0, 0}},

		// NV SE active
		{{180, 255, 0, 0}, {255, 0, 0, 0}, {0, 0, 0, 255}, {0, 0, 255, 180}},

		// NV, SV SE
		{{180, 180, 255, 0}, {180, 180, 180, 255}, {180, 180, 180, 180},
		 {180, 180, 180, 180}},

		// NV NE active
		{{180, 180, 180, 180}, {255, 255, 255, 255}, {0, 0, 0, 0},
		 {0, 0, 0, 0}},

		// NV, NE SV active
		{{180, 180, 180, 180}, {180, 180, 180, 180}, {180, 180, 180, 255},
		 {180, 180, 255, 0}},

		// NV, NE SE active
		{{180, 180, 180, 180}, {180, 180, 180, 180}, {255, 180, 180, 180},
		 {0, 255, 180, 180}},

		// Toate active
		{{255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255},
		 {255, 255, 255, 255}}
	};

	// Copiez configuratiile in matricea `contours`
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				contours[i][j][k] = tmp[i][j][k];
}

void init_contur(int contur[16][4][4])
{
	// Initializeaza contururile
	initialize_configurations(contur);

	// Afisez configuratiile
	for (int i = 0; i < 16; i++) {
		printf("P3\n");
		printf("4 4\n");
		printf("255\n");
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				int value = contur[i][j][k];
				printf("%d %d %d\n", value, value, value);
			}
		}
	}
}

void apply_contour_to_cell(color_rgb **new_colors, int contur[16][4][4],
						   int config, int cell_size, int i, int j, image *img)
{
	for (int k = 0; k < cell_size; k++) {
		for (int l = 0; l < cell_size; l++) {
			int x = i * cell_size + k;
			int y = j * cell_size + l;
			if (x < img->height && y < img->width) {
				// Copiez culoarea din contur
				int value = contur[config][k][l];
				new_colors[x][y].r = value;
				new_colors[x][y].g = value;
				new_colors[x][y].b = value;
			}
		}
	}
}

void process_cell_grid(image *img, color_rgb **new_colors,
					   int contur[16][4][4], int cell_size)
{
	int N = img->width / cell_size;
	int M = img->height / cell_size;

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			// Shiftez bitii pentru a obtine configuratia
			int config = (img->binary[i][j] << 3) |
						 (img->binary[i][j + 1] << 2) |
						 (img->binary[i + 1][j + 1] << 1) |
						 img->binary[i + 1][j];
			apply_contour_to_cell(new_colors, contur, config,
								  cell_size, i, j, img);
		}
	}
}

void replace_image_colors(image *img, color_rgb **new_colors)
{
	for (int i = 0; i < img->height; i++)
		for (int j = 0; j < img->width; j++) {
			img->colors[i][j].r = new_colors[i][j].r;
			img->colors[i][j].g = new_colors[i][j].g;
			img->colors[i][j].b = new_colors[i][j].b;
		}
}

void marching_squares(image *img, int contur[16][4][4])
{
	if (img->has_grid == 0)
		grid_for_march(img);

	initialize_configurations(contur);

	color_rgb **new_colors = alloc_color(img->height, img->width);

	process_cell_grid(img, new_colors, contur, 4);
	replace_image_colors(img, new_colors);
	dealloc_color(new_colors, img->height);
	printf("Marching Squares aplicat cu succes [%d-%d]\n",
		   img->height, img->width);
}

int main(void)
{
	char comanda[NMAX], copy_command[NMAX];
	int contur[16][4][4] = {0};
	int ok = 1, check_function, check_command = 0;
	int it_worked = 0, read_check = 0;
	image *img = (image *)calloc(1, sizeof(image));
	DIE(!img, "Failed to allocate memory\n");

	img->has_grid = 0;
	img->loaded = 0;
	while (ok) {
		scanf("%s", comanda);
		strcpy(copy_command, comanda);
		check_function = 0;
		char *p = strtok(comanda, "\n ");

		if (strcmp(p, "READ") == 0) {
			if (read_check) {
				dealloc_color(img->colors, img->height);
				dealloc_matrix(img->binary, img->height / 4 + 1);
				img->loaded = 0;
				img->height = 0;
				img->width = 0;
				img->has_grid = 0;
			}
			read(img, &it_worked);
			read_check = 1;
			check_function = it_worked;
			check_command = 1;
		}

		if (strstr(comanda, "WRITE")) {
			write(img);
			check_function = 1;
			check_command = 1;
		}

		if (strcmp(p, "EXIT") == 0) {
			exit_program(img);
			ok = 0;

			check_function = 1;
			check_command = 1;
		}

		if (strstr(p, "RESIZE")) {
			resize(img);
			check_function = 1;
			check_command = 1;
		}

		if (strstr(comanda, "INIT_CONTUR")) {
			init_contur(contur);
			check_function = 1;
			check_command = 1;
		}

		if (strstr(comanda, "MARCH")) {
			marching_squares(img, contur);
			check_function = 1;
			check_command = 1;
		}

		if (strstr(comanda, "GRID")) {
			grid(img);
			img->has_grid = 1;
			check_function = 1;
			check_command = 1;
		}

		if (check_function == 0)
			return 0;

		if (check_command == 0)
			printf("Invalid command\n");
	}
	return 0;
}
