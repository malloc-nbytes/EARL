module Main

import "std/containers/interval-tree.rl";

let t = IntervalTree::T([(12, 15)]);

println(t.overlapping(13, 14));
