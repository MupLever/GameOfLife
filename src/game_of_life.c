#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#define WIDTH 25
#define LENGTH 80
#define MAX_SPEED 5

static struct termios stored_settings;
void start_game(int *matrix_cell);
void update(int *matrix_cell, int *matrix_cell_temp);
void nulling_matrix(int *matrix);
void draw_game(const int *matrix_cell, int speed);
unsigned short int is_able_born(const int *matrix, int x, int y);
unsigned short int is_able_dead(const int *matrix, int x, int y);
void is_end_of_x(int *x);
void is_end_of_y(int *y);
void set_keypress(void);
void start_ask(int *matrix);
void set_matrix_from_file(int file_num, int *matrix);

int main() {
    char code;
    int matrix_cell[WIDTH * LENGTH];
    nulling_matrix(matrix_cell);
    set_keypress();
    start_ask(matrix_cell);
    if (scanf("%c", &code) == 1 && code == '\n') {
        int speed = 4, flag = 0, matrix_cell_temp[WIDTH * LENGTH];
        nulling_matrix(matrix_cell_temp);
        fd_set rfds;
        struct timeval tv;
        while (flag == 0) {
            printf("\e[1;1H\e[2J");
            sleep(MAX_SPEED - speed);
            draw_game(matrix_cell, speed);
            FD_ZERO(&rfds);
            FD_SET(0, &rfds);
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            int retval = select(2, &rfds, NULL, NULL, &tv);
            if (retval) {
                code = getc(stdin);
            } else {
                code = ' ';
            }
            usleep(60000);
            if (code == 'q' || code == 'Q')
                flag = 1;
            else if ((code == 'a' || code == 'A') && (MAX_SPEED - speed > 0))
                speed += 1;
            else if ((code == 'z' || code == 'Z') && (MAX_SPEED - speed < 5))
                speed -= 1;
            update(matrix_cell, matrix_cell_temp);
        }
    }
    printf("\e[1;1H\e[2J");
    printf("Goodbye! Have a nice day! :-)\n");
    return 0;
}

void start_game(int *matrix_cell) {
    matrix_cell[1 * LENGTH + 3] = 1;
    matrix_cell[2 * LENGTH + 1] = 1;
    matrix_cell[2 * LENGTH + 3] = 1;
    matrix_cell[3 * LENGTH + 2] = 1;
    matrix_cell[3 * LENGTH + 3] = 1;

    matrix_cell[10 * LENGTH + 2] = 1;
    matrix_cell[11 * LENGTH + 2] = 1;
    matrix_cell[12 * LENGTH + 2] = 1;
}
void nulling_matrix(int *matrix) {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < LENGTH; j++) {
            matrix[i * LENGTH + j] = 0;
        }
    }
}
void draw_game(const int *matrix, int speed) {
    printf("Input Q if want to end the game of life\n");
    printf("SPEED: %d\n", speed);
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < LENGTH - 1; j++) {
            if (matrix[i * LENGTH + j] == 1) {
                printf("o");
            } else {
                printf(" ");
            }
        }
        printf(" \n");
    }
}

void update(int *matrix_cell, int *matrix_cell_temp) {
    int i, j;
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < LENGTH; j++) {
            if (matrix_cell[i * LENGTH + j] == 0 && is_able_born(matrix_cell, i, j)) {
                matrix_cell_temp[i * LENGTH + j] = 1;
            }
            if (matrix_cell[i * LENGTH + j] == 1) {
                if (is_able_dead(matrix_cell, i, j)) {
                    matrix_cell_temp[i * LENGTH + j] = 0;
                } else {
                    matrix_cell_temp[i * LENGTH + j] = 1;
                }
            }
        }
    }
    nulling_matrix(matrix_cell);
    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < LENGTH; j++) {
            matrix_cell[i * LENGTH + j] = matrix_cell_temp[i * LENGTH + j];
        }
    }
    nulling_matrix(matrix_cell_temp);
}

unsigned short int is_able_born(const int *matrix, int x, int y) {
    unsigned short int result = 0, sum = 0;
    int i = x - 1;
    for (int count1 = 0; count1 < 3; count1++) {
        is_end_of_x(&i);
        int j = y - 1;
        for (int count2 = 0; count2 < 3; count2++) {
            is_end_of_y(&j);
            sum += matrix[i * LENGTH + j];
            j++;
        }
        i++;
    }
    if (sum == 3) {
        result = 1;
    }
    return result;
}

unsigned short int is_able_dead(const int *matrix, int x, int y) {
    unsigned short int result = 0, sum = 0;
    int i = x - 1;
    for (int count1 = 0; count1 < 3; count1++) {
        is_end_of_x(&i);
        int j = y - 1;
        for (int count2 = 0; count2 < 3; count2++) {
            is_end_of_y(&j);
            sum += matrix[i * LENGTH + j];
            j++;
        }
        i++;
    }
    sum -= 1;
    if (sum < 2 || sum > 3) {
        result = 1;
    }
    return result;
}

void is_end_of_x(int *x) {
    if (*x < 0) {
        *x += WIDTH;
    } else if (*x >= WIDTH) {
        *x -= WIDTH;
    }
}

void is_end_of_y(int *y) {
    if (*y < 0) {
        *y += LENGTH;
    } else if (*y >= LENGTH) {
        *y -= LENGTH;
    }
}

void set_keypress(void) {
  struct termios new_settings;

  tcgetattr(0, &stored_settings);

  new_settings = stored_settings;

  new_settings.c_lflag &= (~ICANON);
  new_settings.c_lflag &= (~ECHO);
  new_settings.c_cc[VTIME] = 0;
  new_settings.c_cc[VMIN] = 1;

  tcsetattr(0, TCSANOW, &new_settings);
  return;
}

void set_matrix_from_file(int file_num, int *matrix) {
    FILE *fp;
    if (file_num == 1) {
        fp = fopen("Place1.txt", "r");
    } else if (file_num == 2) {
        fp = fopen("Place2.txt", "r");
    } else if (file_num == 3) {
        fp = fopen("Place3.txt", "r");
    } else if (file_num == 4) {
        fp = fopen("Place4.txt", "r");
    } else {
        fp = fopen("Place5.txt", "r");
    }
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < LENGTH; j++) {
            char symb = fgetc(fp);
            if (symb != '\n') {
                matrix[i * LENGTH + j] = (int)symb - 48;
            }
        }
    }
    printf("Input Enter to start the game of life\n");
}

void start_ask(int *matrix) {
    printf("Do you want to input a start file? (y/n)\n");
    char yn, endl;
    int flag = 1;
    int numb;
    while (flag && scanf("%c%c", &yn, &endl) == 2 && endl == '\n') {
        if (yn == 'y') {
            printf("Choose the file:\n");
            printf("1. Random\n");
            printf("2. Gun!!!\n");
            printf("3. Dvourer\n");
            printf("4. Puffer\n");
            printf("5. Whirligig\n");
            while (scanf("%d%c", &numb, &endl) != 2 || endl != '\n' || numb < 1 || numb > 5) {
                printf("Please, try again!\n");
            }
            set_matrix_from_file(numb, matrix);
            flag = 0;
        } else if (yn == 'n') {
            flag = 0;
            start_game(matrix);
            printf("Input Enter to start the game of life\n");
        } else {
            printf("Wrong button, please, try again!\n");
        }
    }
}
