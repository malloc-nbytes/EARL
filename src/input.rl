let x = 0;

while x < 10 {
    println(x);
    let y = 0;
    while y < 10 {
        println("  ", y);
        y += 1;
    }
    x += 1;
}
