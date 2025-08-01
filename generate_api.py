import re
import sys
from pathlib import Path

HEADER = """#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BulletWorld BulletWorld;
typedef struct BulletBody  BulletBody;
typedef struct BulletShape BulletShape;

// --- AUTO-GENERATED ---

{FUNCTION_DECLS}

// --- END ---

#ifdef __cplusplus
}
#endif
"""

def extract_function_decls(code: str):
    # объединяем все строки, удаляя комментарии и лишние пробелы
    clean = re.sub(r'//.*?$|/\*.*?\*/', '', code, flags=re.DOTALL | re.MULTILINE)
    
    # поддержка переноса строки после extern "C"
    extern_blocks = re.findall(r'extern\s+"C"\s*\{([\s\S]*?)^\}', clean, re.MULTILINE)

    decls = []
    for block in extern_blocks:
        funcs = re.findall(
            r'([a-zA-Z_][a-zA-Z0-9_ \*\n]*?)\s+(\*?\s*[a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*\{',
            block
    )
    for ret_type, name, args in funcs:
        ret_type = ' '.join(ret_type.strip().split())
    
        # переносим * из имени в тип, если нужно
        name = name.strip()
        if name.startswith('*'):
            ret_type += '*'
            name = name[1:].strip()
    
        ret_type = ret_type.replace(' *', '*').replace('* ', '*')
        args = ' '.join(args.strip().split())
        decl = f"{ret_type} {name}({args});"
        decls.append(decl)

    return "\n".join(decls)

def main():
    if len(sys.argv) != 3:
        print("Usage: python gen_header.py bullet_wrapper.cpp bullet_wrapper.h")
        return

    cpp_path = Path(sys.argv[1])
    h_path = Path(sys.argv[2])

    cpp_code = cpp_path.read_text()
    function_decls = extract_function_decls(cpp_code)

    header = HEADER.replace("{FUNCTION_DECLS}", function_decls)
    h_path.write_text(header)
    print(f"Generated header: {h_path}")

if __name__ == "__main__":
    main()
