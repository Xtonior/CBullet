import re
import sys
from pathlib import Path

HEADER = """#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "bullet_types.h"

// Structs
{STRUCT_DECLS}

// --- AUTO-GENERATED ---

{FUNCTION_DECLS}

// --- END ---

#ifdef __cplusplus
}
#endif
"""

def extract_struct_decls(code: str):
    # Удалим комментарии
    clean = re.sub(r'//.*?$|/\*.*?\*/', '', code, flags=re.MULTILINE | re.DOTALL)

    struct_names = set()

    # Найти struct Foo { ... };
    struct_defs = re.findall(r'\bstruct\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\{[^}]*?\};', clean, re.DOTALL)
    struct_names.update(struct_defs)

    # Найти typedef struct Foo Foo;
    typedef_direct = re.findall(r'\btypedef\s+struct\s+([a-zA-Z_][a-zA-Z0-9_]*)\s+\1\s*;', clean)
    struct_names.update(typedef_direct)

    # Найти typedef struct Foo { ... } Foo;
    typedef_with_body = re.findall(
        r'\btypedef\s+struct\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\{[^}]*?\}\s*([a-zA-Z_][a-zA-Z0-9_]*)\s*;',
        clean, re.DOTALL)
    for internal, external in typedef_with_body:
        struct_names.add(external)

    # Найти typedef struct Foo Bar;
    typedef_alias = re.findall(r'\btypedef\s+struct\s+([a-zA-Z_][a-zA-Z0-9_]*)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*;', clean)
    for internal, external in typedef_alias:
        struct_names.add(external)

    return '\n'.join(f'typedef struct {name} {name};' for name in sorted(struct_names))

def extract_function_decls(code: str):
    clean = re.sub(r'//.*?$|/\*.*?\*/', '', code, flags=re.DOTALL | re.MULTILINE)
    
    extern_blocks = re.findall(r'extern\s+"C"\s*\{([\s\S]*?)^\}', clean, re.MULTILINE)

    decls = []
    for block in extern_blocks:
        funcs = re.findall(
            r'([a-zA-Z_][a-zA-Z0-9_ \*\n]*?)\s+(\*?\s*[a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*\{',
            block
        )
        for ret_type, name, args in funcs:
            ret_type = ' '.join(ret_type.strip().split())

            name = name.strip()
            if name.startswith('*'):
                ret_type += '*'
                name = name[1:].strip()

            ret_type = ret_type.replace(' *', '*').replace('* ', '*')
            args = ' '.join(args.strip().split())
            decl = f"{ret_type} {name}({args});"
            decls.append(decl)

    return '\n'.join(decls)

def main():
    if len(sys.argv) != 3:
        print("Usage: python gen_header.py bullet_wrapper.cpp bullet_wrapper.h")
        return

    cpp_path = Path(sys.argv[1])
    h_path = Path(sys.argv[2])

    cpp_code = cpp_path.read_text()
    function_decls = extract_function_decls(cpp_code)
    struct_decls = extract_struct_decls(cpp_code)

    header = HEADER.replace("{STRUCT_DECLS}", struct_decls)
    header = header.replace("{FUNCTION_DECLS}", function_decls)

    h_path.write_text(header)
    print(f"Generated header: {h_path}")

if __name__ == "__main__":
    main()
