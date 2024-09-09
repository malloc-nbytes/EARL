module Main

fn twosum_optimized(lst, t) {
    # Create an empty dictionary that holds
    # keys of type int.
    let found = Dict(int);

    for i in 0 to len(lst) {
        let compliment = t - lst[i];

        # Dictionaries will return `none` if not
        # found, and `none` results to false if
        # used in a boolean context. If the item
        # is found, it will return `some(value)`
        # which results to true in a boolean context.
        if found[compliment] {
            return some((found[compliment].unwrap(), i));
        }

        # Insert the pair into the dictionary.
        found.insert(lst[i], i);
    }

    return none;
}

fn twosum_brute_force(lst, t) {
    for i in 0 to len(lst)-1 {
        for j in i to len(lst) {
            if lst[i] + lst[j] == t {
                # Returning `some(tuple<int, int>)` to keep
                # this function's return types consistent.
                return some((i, j));
            }
        }
    }

    return none;
}

let lst, t = (1..=5, 4);

let result1 = twosum_brute_force(lst, t);
let result2 = twosum_optimized(lst, t);

if result1 {
    println(f"found {t} in twosum_brute_force at: ", result1.unwrap());
}
else {
    println(f"could not find {t} in twosum_brute_force");
}

if result2 {
    println(f"found {t} in twosum_optimized at: ", result2.unwrap());
}
else {
    println(f"could not find {t} in twosum_optimized");
}
