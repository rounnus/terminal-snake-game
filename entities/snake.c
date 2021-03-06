#include <string.h>

#include <snake/snake.h>
#include <refresh.h>
#include <equations.h>
#include <terminal/keystrokes_codes.h>
#include <terminal/terminal.h>
#include <mem.h>

static int dx = 1;
static int dy = 0;
static char head = SNAKE_HEAD_RIGHT;
static g_snake_piece **snake_pieces = NULL;
static s_food         *snake_food   = NULL;
static size_t          snake_size   = 1;

static inline int is_food_eaten() {
    double distance = calculate_distance(snake_pieces[0]->s_x, snake_pieces[0]->s_y,
                                          snake_food->f_x, snake_food->f_y);

    if (distance < 5)
        return 1;
    else
        return 0;
}

static int move_pieces() {

    g_snake_piece prev_piece_point;
    g_snake_piece tmp_piece_point;

    g_snake_piece *snake_head_point = snake_pieces[0];

    // Save the current head location.
    prev_piece_point.s_x = snake_head_point->s_x;
    prev_piece_point.s_y = snake_head_point->s_y;
    // Clear the previous symbol in screen.
    GO_TO(prev_piece_point.s_x, prev_piece_point.s_y);
    printf(" ");
    // move the head.
    snake_head_point->s_x += dx;
    snake_head_point->s_y += dy;
    snake_head_point->s_symbol = head;

    double distance;

    for (int curr_piece = 1; curr_piece < snake_size; curr_piece++) {
        // Clear the screen.
        GO_TO(snake_pieces[curr_piece]->s_x, snake_pieces[curr_piece]->s_y);
        printf(" ");

        // Save the current location for the next piece.
        tmp_piece_point.s_x = snake_pieces[curr_piece]->s_x;
        tmp_piece_point.s_y = snake_pieces[curr_piece]->s_y;

        // Check if the head hit a piece of tail.
        if (tmp_piece_point.s_x != 0 && tmp_piece_point.s_y != 0) {
            distance = calculate_distance(
                    snake_head_point->s_x,
                    snake_head_point->s_y,
                    tmp_piece_point.s_x,
                    tmp_piece_point.s_y
                    );

            if (distance < 1) {
                return 0;
            }
        }

        // Change the location.
        snake_pieces[curr_piece]->s_x = prev_piece_point.s_x;
        snake_pieces[curr_piece]->s_y = prev_piece_point.s_y;

        // Switch the values of tmp and prev_piece_point.

        prev_piece_point.s_x = tmp_piece_point.s_x;
        prev_piece_point.s_y = tmp_piece_point.s_y;

    }

    return 1;
}

void increase_snake_size() {
    // increase the array.

    REALLOCATE_MEM(snake_pieces, sizeof(g_snake_piece *) * snake_size);

    // configure the new piece.
    g_snake_piece *new_piece = NULL;
    ALLOCATE_MEM(new_piece, 1, sizeof(g_snake_piece));
    new_piece->s_x = 0;
    new_piece->s_y = 0;
    new_piece->s_symbol = SNAKE_TAIL;

    // Set the new piece into the array.
    snake_pieces[snake_size - 1] = new_piece;
}

static int is_snake_hit_wall() {
    struct winsize terminal_dimension = get_terminal_dimensions();

    int snake_current_x = snake_pieces[0]->s_x;
    int snake_current_y = snake_pieces[0]->s_y;

    if (snake_current_x == -1 || snake_current_x > terminal_dimension.ws_col ||
        snake_current_y == -1 || snake_current_y > terminal_dimension.ws_row) {

        return 1;
    }

    else
        return 0;
}

int move_snake(int direction) {
    if      (direction == UPP_ARROW)   dy = -1, dx =  0, head = SNAKE_HEAD_UP;
    else if (direction == DOWN_ARROW)  dy =  1, dx =  0, head = SNAKE_HEAD_DOWN;
    else if (direction == RIGHT_ARROW) dy =  0, dx =  1, head = SNAKE_HEAD_RIGHT;
    else if (direction == LEFT_ARROW)  dy =  0, dx = -1, head = SNAKE_HEAD_LEFT;

    if (!move_pieces())      return 0;
    if (is_snake_hit_wall()) return 0;
    //
    if (is_food_eaten()) {
        change_position_of(snake_food);
        snake_size = snake_size + 1;
        increase_snake_size();
    }


    refresh_game(snake_pieces, snake_size, snake_food);
    return 1;
}

void initialize() {
    struct winsize terminal_dimensions = get_terminal_dimensions();

    // Allocate space for each entity.
    ALLOCATE_MEM(snake_pieces, 1, sizeof(g_snake_piece *));
    ALLOCATE_MEM(snake_food, 1, sizeof(s_food));

    // Initial values.
    // snake head
    ALLOCATE_MEM(snake_pieces[0], 1, sizeof(g_snake_piece));
    snake_pieces[0]->s_symbol = SNAKE_HEAD_RIGHT;

    // Centralize the snake.
    snake_pieces[0]->s_x = (terminal_dimensions.ws_col - 1)/2;
    snake_pieces[0]->s_y = terminal_dimensions.ws_row / 2;

    // Food.
    snake_food->f_symbol = FOOD_SYMBOL;
    change_position_of(snake_food);
}

void destruct() {

    for (int piece = 0; piece < snake_size; piece++)
        free(snake_pieces[piece]);

    free(snake_pieces);
    free(snake_food);
}
