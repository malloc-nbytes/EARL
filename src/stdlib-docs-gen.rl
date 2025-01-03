# MIT License

# Copyright (c) 2023 malloc-nbytes

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

### This file is used to generate the documentation for the StdLib
### section of the EARL-language-reference.org/html. The parsing
### that happens is not human-friendly and can easily break!

#############################################################################
### RULES
#############################################################################
### There are two types of comments that this file looks for:
###   1. `###` sequence comment
###   2. `#--` docs comment
### To begin parsing a certain section, you start with the sequence comment.
### To parse the information on the section, you start with a docs comment.
### The available sequence sections are:
###   * Variable
###   * Function
###   * Enum
###   * Class
###   * Method
###   * Member
###   * End
###   * IGNORE
###  You end the section with a sequence comment `End`.
###  Variables are expected to have the following:
###    * Name
###    * Type
###    * Description
###  Functions are expected to have the following:
###    * Name
###    * Parameter(s)
###    * Returns
###    * Description
### Enums are expected to have the following:
###    * Name
###    * Parameter(s)
###    * Description
### Classes are expected to have the following:
###    * Name
###    * Parameter(s)
###    * Description
### Methods are expected to have the following:
###    * Name
###    * Parameter(s)
###    * Returns
###    * Description
### Members are expected to have the following:
###    * Name
###    * Type
###    * Description
### If a module is still being worked on and you do not want to add
### it to the docs yet, add `### IGNORE` at the top of the file and
### this script will ignore it.
### See files in std/ for examples on how to use these comments.
### Because the docs are generated using Emacs Org Mode, this script
### will check to see if Emacs is installed. If it is, it will export
### the EARL-language-reference.org and README.org to .html files.
### If Emacs is not found, it is up to the user to download Emacs
### or get someone else to convert it.

module Main

import "std/system.rl";
import "std/utils.rl";
import "std/io.rl";
import "std/datatypes/str.rl";
import "std/script.rl";

enum State {
    None,
    Variables,
    Functions,
    Methods,
    Members,
    Classes,
    Enums,
}

fn replace_backticks(@ref desc) {
    let backtick = -1;
    for i in 0 to len(desc) {
        let c = desc[i];
        if c == '`' {
            if backtick != -1 {
                desc[backtick] = '=';
                desc[i] = '=';
                backtick = -1;
            }
            else {
                backtick = i;
            }
        }
    }
}

class Parameter [name, ty] {
    @pub let name, ty = (name, ty);

    @pub fn tostr() {
        return this.name + ": " + this.ty;
    }
}

class Function [] {
    @pub let name, returns, description = ("", "", "");
    @pub let parameters = [];

    @pub fn add_name(name) {
        this.name = name;
    }

    @pub fn add_param(param) {
        this.parameters.append(param);
    }

    @pub fn add_returns(returns) {
        this.returns = returns;
    }

    @pub fn add_description(desc) {
        this.description = description;
    }

    @pub fn addto_description(s) {
        this.description += s;
    }

    @pub fn dump() {
        print(this.name, "(");
        for i in 0 to len(this.parameters) {
            print(this.parameters[i].tostr());
            if i != len(this.parameters)-1 {
                print(", ");
            }
        }
        println(") -> ", this.returns);
        println("└──", this.description, "\n");
    }

    @pub fn to_org_mode() {
        let s = "#+begin_example\n";
        s += this.name + "(";
        for i in 0 to len(this.parameters) {
            s += this.parameters[i].name + ": " + this.parameters[i].ty;
            if i != len(this.parameters)-1 { s += ", "; }
        }
        s += ") -> " + this.returns + "\n";
        s += "#+end_example\n\n";
        replace_backticks(this.description);
        s += this.description + "\n";
        return s;
    }

    @pub fn to_jsx() {
        let s = "";
        s += "                        <EARLCodeSnippet code={`";
        s += this.name + "(";

        for i in 0 to len(this.parameters) {
            s += this.parameters[i].name + ": " + this.parameters[i].ty;
            if i != len(this.parameters)-1 {
                s += ", ";
            }
        }
        s += ") -> " + this.returns + "`} language={'armasm'} />\n";

        s += "                        <EARLInfoIndent>\n";
        foreach desc in this.description.split("\n") {
            s += "                            <EARLInfo text='" + desc + "' />\n";
        }
        s += "                        </EARLInfoIndent>\n";
        return s;
    }
}

class Class [] {
    @pub let name = "";
    @pub let description = "";
    @pub let params = [];
    @pub let methods = [];
    @pub let members = [];

    @pub fn add_name(name) {
        this.name = name;
    }

    @pub fn add_param(param) {
        this.params.append(param);
    }

    @pub fn addto_description(s) {
        this.description += s;
    }

    @pub fn add_method(m) {
        this.methods.append(m);
    }

    @pub fn add_member(m) {
        this.members.append(m);
    }

    @pub fn dump() {
        print(this.name, "[");
        for i in 0 to len(this.params) {
            print(this.params[i].tostr());
            if i != len(this.params)-1 {
                print(", ");
            }
        }
        println("]");
        println("└──", this.description, "\n");
    }

    @pub fn to_org_mode() {
        let s = "#+begin_quote\n#+begin_example\n";
        s += this.name + " [";
        for i in 0 to len(this.params) {
            s += this.params[i].name + ": " + this.params[i].ty;
            if i != len(this.params)-1 { s += ", "; }
        }
        s += "]\n";
        s += "#+end_example\n";
        replace_backticks(this.description);
        s += this.description + "\n#+end_quote\n";

        if len(this.members) != 0 {
            s += "**** *=" + this.name + "= Members*:\n\n";
        }

        foreach m in this.members {
            s += "#+begin_quote\n";
            s += m.to_org_mode() + "\n";
            s += "#+end_quote\n";
        }

        if len(this.methods) != 0 {
            s += "\n**** *=" + this.name + "= Implements*\n\n";
        }
        foreach m in this.methods {
            s += "#+begin_quote\n";
            s += m.to_org_mode() + "\n";
            s += "#+end_quote\n";
        }

        return s;
    }

    @pub fn to_jsx() {
        let s = "";
        s += "                        <EARLCodeSnippet code={`";
        s += this.name + "[";
        for i in 0 to len(this.params) {
            s += this.params[i].name + ": " + this.params[i].ty;
            if i != len(this.params)-1 { s += ", "; }
        }
        s += "]`} language={'armasm'} />\n";

        s += "                        <EARLInfoIndent>\n";
        foreach desc in this.description.split("\n") {
            s += "                            <EARLInfo text='" + desc + "' />\n";
        }
        s += "                        </EARLInfoIndent>\n";


        foreach m in this.members { unimplemented(__FILE__, ":", __FUNC__, ": class members"); }

        s += "                    </>\n";
        s += "                ),\n";

        if len(this.methods) != 0 {
            s += "                subsections: [\n";
            s += "                    {\n";
            s += "                        title: \""+ this.name +" Implements\",\n";
            s += "                        content: (\n";
            s += "                            <>\n";
            foreach m in this.methods {
                s += m.to_jsx();
            }
            s += "                            </>\n";
            s += "                        ),\n";
            s += "                    },\n";
            s += "                ],\n";
        }

        s += "            },\n";

        return s;
    }
}

class Variable [] {
    @pub let name = "";
    @pub let ty = "";
    @pub let description = "";

    @pub fn dump() {
        println(this.name, ": ", this.ty);
        println("└──", this.description, "\n");
    }

    @pub fn add_name(name) {
        this.name = name;
    }

    @pub fn add_type(ty) {
        this.ty = ty;
    }

    @pub fn addto_description(s) {
        this.description += s;
    }

    @pub fn to_org_mode() {
        let s = "#+begin_example\n";
        s += this.name + ": " + this.ty + "\n";
        s += "#+end_example\n\n";
        replace_backticks(this.description);
        s += this.description + "\n";
        return s;
    }

    @pub fn to_jsx() {
        let s = "";
        s += "                        <EARLCodeSnippet code={`";
        s += this.name + ": " + this.ty + "`} language={'armasm'} />\n";
        s += "                        <EARLInfoIndent>\n";
        foreach desc in this.description.split("\n") {
            s += "                            <EARLInfo text=\"" + desc + "\" />\n";
        }
        s += "                        </EARLInfoIndent>\n";
        return s;
    }
}

class Enum [] {
    @pub let name = "";
    @pub let items = [];
    @pub let description = "";

    @pub fn add_name(name) {
        this.name = name;
    }

    @pub fn add_param(item) {
        this.items.append(item);
    }

    @pub fn addto_description(s) {
        this.description += s;
    }

    @pub fn dump() {
        println(this.name, " {");
        for i in 0 to len(this.items) {
            print("    ", this.items[i].tostr());
            if i != len(this.items)-1 {
                print(",");
            }
            println();
        }
        println("}");
        println("└──", this.description, "\n");
    }

    @pub fn to_org_mode() {
        let s = "#+begin_example\n";
        s += this.name + " {\n";

        for i in 0 to len(this.items) {
            let di = this.items[i];
            s += "    " + di.name + " -> " + di.ty;
            if i != len(this.items)-1 {
                s += ",";
            }
            s += "\n";
        }

        s += "}\n";
        s += "#+end_example\n\n";
        replace_backticks(this.description);
        s += this.description + "\n";

        return s;
    }

    @pub fn to_jsx() {
        let s = "";
        s += "                        <EARLCodeSnippet code={`";
        s += this.name + " {\n";

        for i in 0 to len(this.items) {
            let item = this.items[i];
            s += "    " + item.name + " -> " + item.ty;
            if i != len(this.items)-1 {
                s += ",";
            }
            s += "\n";
        }

        s += "}`} language={'armasm'} />\n";
        s += "                        <EARLInfoIndent>\n";
        foreach desc in this.description.split("\n") {
            s += "                            <EARLInfo text='" + desc + "' />\n";
        }
        s += "                        </EARLInfoIndent>\n";
        return s;
    }
}

class Entry [import_, mod, classes, functions, enums, variables] {
    @pub let import_ = import_;
    @pub let mod = mod;
    @pub let classes = classes;
    @pub let functions = functions;
    @pub let enums = enums;
    @pub let variables = variables;

    fn functions_to_org_mode() {
        let result = "";
        foreach f in this.functions {
            let s = "#+begin_quote\n";
            s += f.to_org_mode();
            s += "#+end_quote\n";
            result += s + "\n";
        }
        return result;
    }

    fn enums_to_org_mode() {
        let result = "";
        foreach e in this.enums {
            let s = "#+begin_quote\n";
            s += e.to_org_mode();
            s += "#+end_quote\n";
            result += s + "\n";
        }
        return result;
    }

    fn classes_to_org_mode() {
        let result = "";
        foreach c in this.classes {
            # Quotes are handled in the classes.
            let s = "*** *=" + c.name + "=*\n";
            s += c.to_org_mode();
            result += s + "\n";
        }
        return result;
    }

    fn variables_to_org_mode() {
        let result = "";

        foreach v in this.variables {
            let s = "#+begin_quote\n";
            s += v.to_org_mode();
            s += "#+end_quote\n";
            result += s + "\n";
        }

        return result;
    }

    @pub fn to_jsx() {
        let s = "    {\n";
        s += "        title: \"" + this.mod + "\",\n";
        s += "        content: (\n";
        s += "            <>\n";
        s += "                <EARLInfo text='Import: `\"" + this.import_ + "\"`' />\n";
        s += "                <EARLInfo text='Module: `" + this.mod + "`' />\n";
        s += "            </>\n";
        s += "        ),\n";
        s += "        subsections: [\n";
        s += "            {\n";
        s += "                title: \"Variable List\",\n";
        s += "                content: (\n";
        s += "                    <>\n";

        if len(this.variables) == 0 {
            s += "                        <EARLInfo text='None' />\n";
        }
        else {
            foreach @const @ref v in this.variables {
                s += v.to_jsx();
            }
        }

        s += "                    </>\n";
        s += "                ),\n";
        s += "            },\n";
        s += "            {\n";
        s += "                title: \"Enum List\",\n";
        s += "                content: (\n";
        s += "                    <>\n";
        if len(this.enums) == 0 {
            s += "                        <EARLInfo text='None' />\n";
        }
        else {
            foreach @const @ref e in this.enums {
                s += e.to_jsx();
            }
        }
        s += "                    </>\n";
        s += "                ),\n";
        s += "            },\n";

        s += "            {\n";
        s += "                title: \"Function List\",\n";
        s += "                content: (\n";
        s += "                    <>\n";
        if len(this.functions) == 0 {
            s += "                        <EARLInfo text='None' />\n";
        }
        else {
            foreach @const @ref f in this.functions {
                s += f.to_jsx();
            }
        }
        s += "                    </>\n";
        s += "                ),\n";
        s += "            },\n";

        s += "            {\n";
        s += "                title: \"Class List\",\n";
        s += "                content: (\n";
        s += "                    <>\n";
        if len(this.classes) == 0 {
            s += "                        <EARLInfo text='None' />\n";
            # The class will close the section because it may
            # have other subsections!
            s += "                    </>\n";
            s += "                ),\n";
            s += "            },\n";
        }
        else {
            foreach @const @ref c in this.classes {
                s += c.to_jsx();
            }
        }

        s += "        ],\n";
        s += "    },\n";

        return s;
    }

    @pub fn to_org_mode() {
        let s = "** " + this.mod + "\n\n";
        s += "*** *Import*: =\""+ this.import_ +"\"=\n\n";
        s += "*** *Module*: =" + this.mod + "=\n\n";

        s += "*** *Variable List*:";
        if len(this.variables) == 0 {
            s += " /None/\n\n";
        }
        else {
            s += "\n" + variables_to_org_mode();
        }

        s += "*** *Enum List*:";
        if len(this.enums) == 0 {
            s += " /None/\n\n";
        }
        else {
            s += "\n" + enums_to_org_mode();
        }

        s += "*** *Function List*:";
        if len(this.functions) == 0 {
            s += " /None/\n\n";
        }
        else {
            s += "\n" + functions_to_org_mode();
        }

        s += "*** *Class List*:";
        if len(this.classes) == 0 {
            s += " /None/\n\n";
        }
        else {
            s += "\n" + classes_to_org_mode();
        }

        return s;
    }
}

fn commit(
    @ref state,
    @ref cur_item,
    @const @ref variables,
    @const @ref functions,
    @const @ref enums,
    @const @ref classes,
    @const @ref methods,
    @const @ref members,
    @ref cur_class,
    mod) {

    assert(cur_item);

    if state == State.Variables {
        print("(Variable) ", f"{mod}::");
        cur_item.unwrap().dump();
        variables.append(cur_item.unwrap());
        state = State.None;
        cur_item = none;
    }
    else if state == State.Functions {
        print("(Function) ", f"{mod}::");
        cur_item.unwrap().dump();
        functions.append(cur_item.unwrap());
        state = State.None;
        cur_item = none;
    }
    else if state == State.Enums {
        print("(Enum) ", f"{mod}::");
        cur_item.unwrap().dump();
        enums.append(cur_item.unwrap());
        state = State.None;
        cur_item = none;
    }
    else if state == State.Classes {
        print("(Class) ", f"{mod}::");
        cur_item.unwrap().dump();
        foreach m in methods { cur_item.unwrap().add_method(m); }
        foreach m in members { cur_item.unwrap().add_member(m); }
        classes.append(cur_item.unwrap());
        state = State.None;
        cur_item = none;
    }
    else if state == State.Methods {
        print("(Method) ", f"{mod}::", cur_class.unwrap().name, ".");
        cur_item.unwrap().dump();
        methods.append(cur_item.unwrap());
        state = State.Classes;
        cur_item = cur_class;
    }
    else if state == State.Members {
        print("(Member) ", f"{mod}::", cur_item.unwrap().name, ".");
        cur_member.unwrap().dump();
        members.append(cur_item.unwrap());
        state = State.Classes;
        cur_item = cur_class;
    }
}

fn index(lines, fp) {
    let state = State.None;
    let mod = "";
    let variables, functions, enums, classes = (
        [],
        [],
        [],
        [],
    );
    let methods, members = ([], []);
    let history = "";
    let cur_item = none;
    let cur_class = none;
    let desc_newline = false;

    let i = 0;
    while i < len(lines) {
        let line = lines[i];
        Str::trim(line);

        let parts = line.split(" ").filter(|s|{return s != "";});
        if parts[0] == "module" {
            if mod != "" {
                panic("duplicate module statements");
            }
            mod = parts[1];
        }
        else if parts[0] == "###" {
            if parts[1] == "IGNORE" {
                return none;
            }
            else if parts[1] == "Variable" {
                cur_item = some(Variable());
                state = State.Variables;
            }
            else if parts[1] == "Function" {
                cur_item = some(Function());
                state = State.Functions;
            }
            else if parts[1] == "Enum" {
                cur_item = some(Enum());
                state = State.Enums;
            }
            else if parts[1] == "Class" {
                cur_item = some(Class());
                state = State.Classes;
            }
            else if parts[1] == "Method" {
                cur_class = cur_item;
                cur_item = some(Function());
                state = State.Methods;
            }
            else if parts[1] == "Member" {
                cur_class = cur_item;
                cur_item = some(Variable());
                state = State.Members;
            }
            else if parts[1] == "End" {
                commit(state, cur_item, variables, functions, enums, classes, methods, members, cur_class, mod);
            }
            else {
                panic("unknown section: ", parts[1]);
            }
        }
        else if parts[0] == "#--" {
            let right = parts[2:].fold(|s, acc|{return acc+' '+s;}, "");
            assert(cur_item);
            match parts[1] {
                "Name:" -> {
                    desc_newline = false;
                    Str::trim(right);
                    cur_item.unwrap().add_name(right);
                }
                "Type:" -> {
                    desc_newline = false;
                    Str::trim(right);
                    cur_item.unwrap().add_type(right);
                }
                "Parameter:" -> {
                    desc_newline = false;
                    let param_parts = right.split(":");
                    let name = param_parts[0];
                    let ty = "";
                    if len(param_parts) > 2 {
                        ty = param_parts[1:].fold(|s, acc|{return acc+':'+s;}, "");
                    }
                    else {
                        ty = param_parts[1];
                    }
                    Str::trim(name);
                    Str::trim(ty);
                    cur_item.unwrap().add_param(Parameter(name, ty));
                }
                "Returns:" -> {
                    desc_newline = false;
                    Str::trim(right);
                    cur_item.unwrap().add_returns(right);
                }
                "Description:" -> {
                    cur_item.unwrap().addto_description(right);
                    if len(right) > 0 {
                        desc_newline = true;
                    }
                }
                _ -> {
                    let s = "";
                    if len(parts) < 2 {
                        s = "";
                    }
                    else if desc_newline {
                        s = '\n'+parts[1]+right;
                    }
                    else {
                        s = parts[1]+right;
                        desc_newline = true;
                    }
                    cur_item.unwrap().addto_description(s);
                }
            }
        }

        i += 1;
    }

    if mod == "" {
        panic(f"file `{fp}` is missing a module statement");
    }

    return some((Entry(fp, mod, classes, functions, enums, variables), mod));
}

fn ready_input(fp) {
    let lines = IO::file_to_str(fp)
        .split("\n")
        .filter(|k|{return k != "";});
    return lines;
}

fn iterdir(path) {
    if System::isdir(path) {
        let files = [];
        foreach f in System::ls(path) {
            files += iterdir(f);
        }
        return files;
    }
    return [path];
}

fn main(convert_mode: str) {
    # let stdlib = System::ls("std/");
    let stdlib = iterdir("std/");
    let autogen = "";
    let docsfporg = "../EARL-language-reference.org";
    let modules = [];
    let entries = [];

    foreach f in stdlib {
        println("############################################################");
        println(f"# Indexing {f}");
        println("############################################################\n");
        let lines = ready_input(f);
        let res = index(lines, f);
        if res {
            let entry, mod = res.unwrap();
            if convert_mode == "org" {
                autogen += entry.to_org_mode();
            }
            modules.append(mod);
            entries.append(entry);
        }
        else {
            println(f"file {f} has IGNORE tag, ignoring...");
        }
    }

    if convert_mode == "jsx" {
        let jsx = "DO NOT MODIFY THIS FILE! IT IS AUTOGENERATED BY\n";
        jsx += "    https://github.com/malloc-nbytes/EARL/blob/main/src/stdlib-docs-gen.rl\n";
        jsx = "import EARLInfo from \"../components/EARLInfo\";\n";
        jsx += "import EARLInfoIndent from \"../components/EARLInfoIndent\";\n";
        jsx += "import EARLCodeSnippet from \"../components/EARLCodeSnippet\";\n";
        jsx += "export const stdLibModules = [\n";
        foreach e in entries {
            jsx += e.to_jsx();
        }
        jsx += "];\n";

        let jsx_output = "StdLibModules.jsx";
        let f = open(jsx_output, "w");
        f.write(jsx);
        f.close();
        println(f"Wrote to {jsx_output}");
    }
    else if convert_mode == "org" {
        let orig_content = IO::file_to_str(docsfp).split("# BEGIN_AUTOGEN")[0] + "# BEGIN_AUTOGEN\n";
        let f = open(docsfp, "w");
        let content = orig_content+autogen;
        f.write(content);
        f.close();

        if Script::program_exists("emacs") {
            println("Found Emacs, converting EARL-language-reference.org to HTML");
            $"rm ../EARL-language-reference.html";
            $"rm ../README.html";
            $"emacs --batch ../EARL-language-reference.org -f org-html-export-to-html";
            $"emacs --batch ../README.org -f org-html-export-to-html";
        }
        else {
            println("Emacs is not available on the system, cannot convert EARL-language-reference.org to HTML, ignoring...");
        }
    }
    println(f"The following modules");
    println(modules);
}

let mode = "";
if len(argv()) != 2 || (argv()[1] != "org" && argv()[1] != "jsx") {
    panic("Usage: earl ", argv()[0], " -- <org|jsx>");
}
mode = argv()[1];

main(mode);
