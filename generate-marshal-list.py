#!/usr/bin/env python3
import sys
import re

def main():
    if len(sys.argv) < 4:
        print("Usage: generate-marshal-list.py <prefix> <output_file> <source_files...>", file=sys.stderr)
        sys.exit(1)
    
    prefix = sys.argv[1]
    output_file = sys.argv[2]
    source_files = sys.argv[3:]
    
    # Pattern to match e.g. eel_marshal_VOID__INT or caja_marshal_VOID__INT
    pattern = re.compile(rf'{prefix}_marshal_([A-Z0-9]+__[A-Z0-9_]+)')
    
    results = set()
    for filepath in source_files:
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                for line in f:
                    for match in pattern.finditer(line):
                        results.add(match.group(1))
        except Exception as e:
            print(f"Warning: could not read {filepath}: {e}", file=sys.stderr)
            
    # Sort and format
    formatted = []
    for r in sorted(results):
        parts = r.split('__')
        if len(parts) == 2:
            line = parts[0].replace('_', ',') + ':' + parts[1].replace('_', ',')
            formatted.append(line)
            
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write('\n'.join(formatted) + '\n')

if __name__ == '__main__':
    main()
