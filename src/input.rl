module Main

fn f() {
    for i in 0 to 10 {
        if i == 5 {
            (1..=10).map(|k|{k;});
        }
    }
    0;
}

println(f());
