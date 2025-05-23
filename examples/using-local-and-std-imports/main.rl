module Main

# Import from the stdlib
import "std/datatypes/list.rl";

# Import locally
import "other-file.rl"; as local

#--- Local import examples
# We can access these resources because
# they are labeled with the @pub attribute.
println(OtherFile::other_file_var1);
println(local::other_file_var2);

let s = OtherFile::sum(1, 2);
println(s);

#--- StdLib examples
let lst = List::preset(0, 10);
println(lst);

lst = [1,5,2,8,9];
List::quicksort(lst, List::DEFAULT_INT_ASCEND_QUICKSORT);
println(lst);
