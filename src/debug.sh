# To use this file, run:
#   bash --rcfile debug

ARGS=""

# source ~/.bashrc

echo -e "\033[5;41mEntering debug environment. Use \`exit\` or \`C-d\` to exit.\033[0m"

alias build='g++ -Iinclude/ -Wreturn-type -O2 -DDEBUG -std=c++17 -g -o earl-debug-build *.cpp primitives/*.cpp member-intrinsics/*.cpp grammar/*.cpp include/scope.hpp'
alias run='gdb --args ./earl-debug-build $ARGS'
alias done='source ~/.bashrc'
export PS1="(DEBUG EARL) [ "
LS_COLORS='' ; export LS_COLORS

echo -e "\033[5;41mBuilding earl-debug-build\033[0m"
build

printf "\033[5;41mArguments: \033[0m"
read ARGS
