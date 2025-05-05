module Main

import "std/datatypes/list.rl";

# Change the grid size to whatever you want.
# However it tends to slow down quite a
# bit at 50x50.
let grid, ROWS, COLS = ([], 10, 10);

@world fn show_grid() {
    $"clear";
    foreach @ref col in grid {
        println(col);
    }
}

@world fn create_grid() {
    foreach i in 0..ROWS {
        grid.append(List::preset(0, COLS));
    }
}

@world fn set_live_cells(@ref @const live_cells) {
    foreach cell in live_cells {
        let x, y = cell;
        if x < 0 || x >= ROWS {
            panic(f"x: {x} is out of range of length {ROWS}");
        }
        if y < 0 || y >= COLS {
            panic(f"y: {y} is out of range of length {COLS}");
        }
        grid[x][y] = 1;
    }
}

@world fn get_neighbors(i, j) {
    let neighbors = [];

    let offsets = [
        (-1, -1), (-1,  0),
        (-1,  1), ( 0, -1),
        ( 0,  1), ( 1, -1),
        ( 1,  0), ( 1,  1)
    ];

    foreach off in offsets {
        let dx, dy = off;
        let ni, nj = (i + dx, j + dy);

        if ni >= 0 && ni < ROWS && nj >= 0 && nj < COLS {
            neighbors.append(grid[ni][nj]);
        }
    }

    return neighbors;
}

fn fold(acc, @ref @const lst, func) {
    foreach elem in lst {
        acc = func(acc, elem);
    }
    return acc;
}

@world fn run() {
    let new_grid = List::preset(List::preset(0, COLS), ROWS);

    fn count_live_neighbors(i, j) {
        let neighbors = get_neighbors(i, j);
        return fold(0, neighbors, |acc, cell| { return acc + cell; });
    }

    foreach i in 0..ROWS {
        foreach j in 0..COLS {
            let live_neighbors = count_live_neighbors(i, j);

            if grid[i][j] == 1 {
                if live_neighbors == 2 || live_neighbors == 3 {
                    new_grid[i][j] = 1;
                }
                else {
                    new_grid[i][j] = 0;
                }
            }
            else {
                if live_neighbors == 3 {
                    new_grid[i][j] = 1;
                }
                else {
                    new_grid[i][j] = 0;
                }
            }
        }
    }

    grid = new_grid;
    show_grid();
}

fn main() {
    create_grid();

    # Change live cells as you see fit. This creates a glider.
    let live_cells = [(1, 2), (2, 3), (3, 1), (3, 2), (3, 3)];
    set_live_cells(live_cells);

    while true {
        run();
    }
}

main();
