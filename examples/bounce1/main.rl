module Main

import "std/system.rl";
import "std/list.rl";
import "std/math.rl";

class Matrix [init, r, c] {
    let lst = init;
    let rows, cols = (r, c);

    @pub fn index(i, j) {
        assert(i < this.rows && i >= 0 && j < this.cols && j >= 0);
        return lst[i*this.cols+j];
    }
}

let speed = 1;
let x, y = (3., 3.);
let dx, dy = (.77*speed/100, 1.*speed/100);
let width, height = (20, 20);
let not_draws_per_sec = 1;
let current_not_draws_per_sec = 0;

let faces = [".", "O","X"];
let current_face = 0;
let emoji_screen = "";

let board = Matrix(List::preset(0, height*width), height, width);
let int_y = int(y);
let int_x = int(x);

@world fn draw() {
    let _ = System::cmd("clear");
    println(emoji_screen);
    emoji_screen = "";

    for i in 0 to height {
        for j in 0 to width {
            emoji_screen += faces[board.index(i, j)%len(faces)];
        }
        emoji_screen += '\n';
    }
}

loop {
    sleep(900);

    x += dx;
    y += dy;

    let bounce = false;

    if x <= 0 || x >= width {
        dx = -dx;
        bounce = true;
    }
    if y <= 0 || y >= height {
        dy = -dy;
        bounce = true;
    }

    if bounce {
        x += dx;
        y += dy;
    }

    if int_y != int(y) || int_x != int(x) {
        board.index(int(y), int(x)) += 1;
        current_not_draws_per_sec += 1;
        if current_not_draws_per_sec == not_draws_per_sec {
            current_not_draws_per_sec = 0;
            draw();
        }
    }

    int_y = int(y);
    int_x = int(x);
}

