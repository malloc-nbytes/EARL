module Main

@pub fn is_only_ignorable(@const @ref s: str) {
    foreach c in s {
        if (c != ' ' && c != '\n' && c != '\r' && c != '\t') {
            return false;
        }
    }
    return true;
}

println(is_only_ignorable(""));
