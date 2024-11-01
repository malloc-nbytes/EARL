module Main

import "std/matrix.rl"; as M
import "std/list.rl"; as L
import "std/math.rl";
import "std/system.rl"; as sys
import "std/script.rl"; as scr

@const let HAS_AUDIO = scr::program_exists("play");

if !HAS_AUDIO {
    println("program `play` from package `sox` is not found, will not play audio");
}

let WIDTH, HEIGHT = (15, 15);
@const let COLORS =     ["🟨", "🟫", "🟧", "🟪", "🟦", "🟥", "⬛"];
@const let FACE_TYPES = ["🤓", "💩", "🎃", "😈", "🥶", "🥵", "🔥"];

@const let SEED = init_seed(datetime().raw());

let kill_guys = true;
let audio_file = "";
let guys = 3;
let winner = "";
let dead = [];
let balls = [];
let board = M::T([], 0, 0);
let faces = M::T([], 0, 0);
let screen = "";
let speed_min = 0.1;
let speed_max = 0.5;
let sleep_time = 10000;

@world fn make_ball() {
    balls.append([float(random()%(WIDTH-3))+2,
                  float(random()%(HEIGHT-3))+2,
                  float(random()%314)/100*2,
                  (float(random()%100)/100)*(speed_max-speed_min)+speed_min,
                  -1,
                  -1,
                  random()%(len(FACE_TYPES)-1)]);
}

@world fn update_balls() {
    faces = M::from2d(L::preset(L::preset(-1, WIDTH), HEIGHT));
    let x = 0;
    let y = 1;
    let dir = 2;
    let speed = 3;
    let prevx = 4;
    let prevy = 5;
    let face = 6;
    foreach @ref ball in balls {
        if ball[speed] == 0 {
            continue;
        }
        ball[x] += cos(ball[dir]) * ball[speed];
        ball[y] += sin(ball[dir]) * ball[speed];
        if ball[x] < 0.5 || ball[x] > WIDTH-0.5 {
            ball[dir] = Math::PI - ball[dir];
        }
        if ball[y] < 0.5 || ball[y] > HEIGHT-0.5 {
            ball[dir] *= -1;
        }
        if ball[x] > WIDTH+0.5 {
            ball[x] = float(random()%(WIDTH-3))+2;
        }
        if ball[y] > HEIGHT+0.5 {
            ball[y] = float(random()%(HEIGHT-3))+2;
        }
        let inbounds = !(ball[x] < 0 || int(ball[x]) >= WIDTH || ball[y] < 0 || int(ball[y]) >= HEIGHT);
        if inbounds {
            if faces.at(int(ball[x]), int(ball[y])) != -1 && kill_guys {
                let tmp = ball[face];
                dead.append(tmp);
                ball[speed] = 0;
                ball[face] = len(FACE_TYPES)-1;
                winner = FACE_TYPES[faces.at(int(ball[x]), int(ball[y]))];
                guys -= 1;
                if WIDTH > 5 {
                    WIDTH -= 1;
                }
                if HEIGHT > 5 {
                    HEIGHT -= 1;
                }
                if HAS_AUDIO && len(audio_file) > 0 {
                    $f"play -q {audio_file} &";
                }
            }
            faces.at(int(ball[x]), int(ball[y])) = ball[face];
            if ((int(ball[x]) != ball[prevx]) || (int(ball[y]) != ball[prevy])) {
                ball[prevx] = int(ball[x]);
                ball[prevy] = int(ball[y]);
                board.at(ball[prevx], ball[prevy]) = ball[face];
            }
        }
    }
}

@world fn show_board() {
    println(screen);
    screen = "";
    for i in 0 to WIDTH {
        for j in 0 to HEIGHT {
            if faces.at(i, j) != -1 {
                screen += FACE_TYPES[faces.at(i, j)];
            }
            else {
                screen += COLORS[board.at(i, j)%len(COLORS)];
            }
        }
        screen += "\n";
    }
}

@world fn main() {
    println("Note: your terminal must be able to display emojis for this program to work");

    guys = int(input("how many opponents? [DO NOT SET TO > 30 WHEN USING AN AUDIO FILE!] "));
    speed_min = float(input("min speed? [0.0 - 1.0] "));
    speed_max = float(input("min speed? [0.0 - 1.0] "));
    WIDTH = int(input("width? [rec. 25] "));
    HEIGHT = int(input("height? [rec. 25] "));
    audio_file = input("path to audio file? [leave blank for none] ");
    kill_guys = bool(input("enable eliminations? [true|false] "));

    if WIDTH != HEIGHT {
        panic("width and height must match");
    }

    sleep_time = int(input("sleep time? [rec. 10000] "));

    board = M::from2d(L::preset(L::preset(len(COLORS)-1, WIDTH), HEIGHT));
    faces = M::from2d(L::preset(L::preset(-1, WIDTH), HEIGHT));

    foreach i in 0..guys {
        make_ball();
    }
    loop {
        $"clear";
        if guys <= 1 {
            println(f"winner: {winner}");
        }
        if kill_guys {
            print("dead: ");
            foreach d in dead {
                print(FACE_TYPES[d]);
            }
            print("\n");
        }
        show_board();
        update_balls();
        sleep(sleep_time);
    }
}

main();

