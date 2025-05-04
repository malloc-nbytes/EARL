module Main

let a, b, c = (1, 2, 3);

persist(f"{__FILE__}/{a}", a);
persist(f"{__FILE__}/{b}", b);
persist(f"{__FILE__}/{c}", c);
